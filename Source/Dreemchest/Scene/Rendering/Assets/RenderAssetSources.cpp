/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#include "RenderAssetSources.h"
#include "Renderable.h"
#include "../RenderingContext.h"
#include "../../Assets/Mesh.h"
#include "../../Assets/Image.h"
#include "../../Assets/Material.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ----------------------------------------------------------------------- RenderableMeshSource ----------------------------------------------------------------------- //

// ** RenderableMeshSource::RenderableMeshSource
RenderableMeshSource::RenderableMeshSource( MeshHandle mesh, RenderingContextWPtr context )
    : AssetSource( mesh ), m_context( context )
{
}

// ** RenderableMeshSource::constructFromAsset
bool RenderableMeshSource::constructFromAsset( const Mesh& mesh, Assets::Assets& assets, Renderable& renderable )
{
    DC_BREAK_IF( mesh.chunkCount() == 0, "constructing renderable from an empty mesh" );

    // Get the rendering HAL instance
    Renderer::HalWPtr hal = m_context->hal();

    // Create vertex declaration
    Renderer::VertexDeclarationPtr vertexFormat = hal->createVertexDeclaration( "P3:N:T0:T1" );

    // Add all mesh chunks to renderable
    for( s32 i = 0, n = mesh.chunkCount(); i < n; i++ ) {
        // Create new renderable
	    const Mesh::VertexBuffer& vertices = mesh.vertexBuffer( i );
	    const Mesh::IndexBuffer&  indices  = mesh.indexBuffer( i );

	    u32 vertexCount = ( u32 )vertices.size();
	    u32 indexCount = ( u32 )indices.size();

	    // Create GPU buffers.
	    Renderer::VertexBufferPtr vertexBuffer = hal->createVertexBuffer( vertexFormat, vertexCount );
	    Renderer::IndexBufferPtr  indexBuffer  = hal->createIndexBuffer( indexCount );

	    // Upload the vertex data
	    Mesh::Vertex* vertex = vertexBuffer->lock<Mesh::Vertex>();

	    for( u32 j = 0; j < vertexCount; j++ ) {
		    vertex->position = vertices[j].position;
		    vertex->normal = vertices[j].normal;

		    for( u32 k = 0; k < Mesh::Vertex::MaxTexCoords; k++ ) {
			    vertex->uv[k] = vertices[j].uv[k];
		    }
		
		    vertex++;	
	    }

	    vertexBuffer->unlock();

	    // Upload the index data
	    u16* index = indexBuffer->lock();
	    memcpy( index, &indices[0], indices.size() * sizeof( u16 ) );
	    indexBuffer->unlock();

	    // Save vertex & index buffers inside renderable
        renderable.setVertexBuffer( i, vertexBuffer );
        renderable.setIndexBuffer( i, indexBuffer );

        // Output log message
        LogDebug( "renderingContext", "renderable with %d vertices and %d indices constructed from mesh '%s'.\n", vertices.size(), indices.size(), m_asset.name().c_str() ); 
    }

    return true;
}

// ----------------------------------------------------------------------- TextureImageSource ----------------------------------------------------------------------- //

// ** TextureImageSource::TextureImageSource
TextureImageSource::TextureImageSource( ImageHandle image, RenderingContextWPtr context )
    : AssetSource( image ), m_context( context )
{
}

// ** TextureImageSource::constructFromAsset
bool TextureImageSource::constructFromAsset( const Image& image, Assets::Assets& assets, Texture& texture )
{
    // Upload this image to a GPU texture
	Renderer::Texture2DPtr instance = m_context->hal()->createTexture2D( image.width(), image.height(), image.bytesPerPixel() == 3 ? Renderer::PixelRgb8 : Renderer::PixelRgba8 );
	instance->setData( 0, &image.mipLevel( 0 )[0] );

    // Set texture asset
    texture.setTexture( instance );

    // Output log message
    LogVerbose( "renderingContext", "%dx%d %s texture constructed from image '%s'.\n", image.width(), image.height(), image.bytesPerPixel() == 3 ? "RGB8" : "RGBA8", m_asset.name().c_str() );

    return true;
}

// ----------------------------------------------------------------------- ProgramShaderSource ----------------------------------------------------------------------- //

// ** ProgramShaderSource::ProgramShaderSource
ProgramShaderSource::ProgramShaderSource( ShaderHandle shader, RenderingContextWPtr context )
    : AssetSource( shader ), m_context( context )
{
}

// ** ProgramShaderSource::constructFromAsset
bool ProgramShaderSource::constructFromAsset( const Shader& shader, Assets::Assets& assets, Program& program )
{
	// Generate macro definitions from features
	String macro = "";

	for( u32 i = 0, n = shader.featureCount(); i < n; i++ ) {
        const Shader::Feature& feature = shader.feature( i );

		if( feature.mask & program.features() ) {
			macro += "#define " + feature.name + "\n";
		}
	}

	// Compile the shader
	Renderer::ShaderPtr compiled = m_context->hal()->createShader( (macro + shader.vertex()).c_str(), (macro + shader.fragment()).c_str() );

    // Failed to compile - return false
    if( !compiled.valid() ) {
        return false;
    }

    // Save the compiled shader
    program.setShader( compiled );

    // Known program input names
    CString inputs[] = {
          "u_vp"
        , "u_transform"
        , "u_color"
        , "u_tex0"
        , "u_tex1"
        , "u_tex2"
        , "u_tex3"
        , "u_clr0"
        , "u_clr1"
        , "u_clr2"
        , "u_clr3"
    };

    NIMBLE_STATIC_ASSERT( (sizeof( inputs ) / sizeof( inputs[0] )) == Program::TotalInputs, "missing program input names" );

    // Locate all shader inputs
    for( s32 i = 0; i < Program::TotalInputs; i++ ) {
        // Find the input location
        u32 location = compiled->findUniformLocation( inputs[i] );

        if( !location ) {
            continue;
        }

        // Save the location index
        program.setInputLocation( static_cast<Program::Input>( i ), location );
        LogDebug( "program", "%s is bound to %d for shader %s\n", inputs[i], location, m_asset.asset().name().c_str() );
    }

	return true;
}

// ----------------------------------------------------------------------- TechniqueMaterialSource ----------------------------------------------------------------------- //

// ** TechniqueMaterialSource::TechniqueMaterialSource
TechniqueMaterialSource::TechniqueMaterialSource( MaterialHandle material, RenderingContextWPtr context )
    : AssetSource( material ), m_context( context )
{
}

// ** TechniqueMaterialSource::constructFromAsset
bool TechniqueMaterialSource::constructFromAsset( const Material& material, Assets::Assets& assets, Technique& technique )
{
    // Get the material shader
    ShaderHandle shader = material.shader();
    DC_BREAK_IF( !shader.isLoaded(), "material shader is not loaded" );

    // Set the technique program
    technique.setProgram( m_context->programByIndex( m_context->requestProgram( shader, material.features() ) ) );

    // Set technique colors
    for( s32 i = 0; i < Material::TotalMaterialLayers; i++ ) {
        technique.setColor( i, material.color( static_cast<Material::Layer>( i ) ) );
    }

    return true;
}

// ----------------------------------------------------------------------- ShaderFormatText ----------------------------------------------------------------------- //

// ** ShaderFormatText::ShaderFormatText
ShaderFormatText::ShaderFormatText( const String& fileName )
{
    setFileName( fileName );
}

// ** ShaderFormatText::constructFromStream
bool ShaderFormatText::constructFromStream( Io::StreamPtr stream, Assets::Assets& assets, Shader& shader )
{
	static CString vertexShaderMarker   = "[VertexShader]";
	static CString fragmentShaderMarker = "[FragmentShader]";

	// Read the code from an input stream
	String code;
	code.resize( stream->length() );
	stream->read( &code[0], stream->length() );

	// Extract vertex/fragment shader code blocks
	u32 vertexBegin = code.find( vertexShaderMarker );
	u32 fragmentBegin = code.find( fragmentShaderMarker );

	if( vertexBegin == String::npos && fragmentBegin == String::npos ) {
		return false;
	}

	if( vertexBegin != String::npos ) {
		u32 vertexCodeStart = vertexBegin + strlen( vertexShaderMarker );
		String vertex = code.substr( vertexCodeStart, fragmentBegin > vertexBegin ? fragmentBegin - vertexCodeStart : String::npos );
        shader.setVertex( vertex );
	}

	if( fragmentBegin != String::npos ) {
		u32 fragmentCodeStart = fragmentBegin + strlen( fragmentShaderMarker );
		String fragment = code.substr( fragmentCodeStart, vertexBegin > fragmentBegin ? vertexBegin - fragmentCodeStart : String::npos );
        shader.setFragment( fragment );
	}

	return true;
}

} // namespace Scene

DC_END_DREEMCHEST