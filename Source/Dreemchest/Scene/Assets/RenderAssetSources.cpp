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
#include "Mesh.h"
#include "Image.h"
#include "Material.h"
#include "Renderable.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ----------------------------------------------------------------------- RenderableMeshSource ----------------------------------------------------------------------- //

// ** RenderableMeshSource::RenderableMeshSource
RenderableMeshSource::RenderableMeshSource( MeshHandle mesh, Renderer::HalWPtr hal )
    : AssetSource( mesh ), m_hal( hal )
{
}

// ** RenderableMeshSource::constructFromAsset
bool RenderableMeshSource::constructFromAsset( const Mesh& mesh, Assets::Assets& assets, Renderable& renderable )
{
    DC_BREAK_IF( mesh.chunkCount() == 0, "constructing renderable from an empty mesh" );

    // Create vertex declaration
    Renderer::VertexDeclarationPtr vertexFormat = m_hal->createVertexDeclaration( "P3:N:T0:T1" );

    // Add all mesh chunks to renderable
    for( s32 i = 0, n = mesh.chunkCount(); i < n; i++ ) {
        // Create new renderable
	    const Mesh::VertexBuffer& vertices = mesh.vertexBuffer( i );
	    const Mesh::IndexBuffer&  indices  = mesh.indexBuffer( i );

	    u32 vertexCount = ( u32 )vertices.size();
	    u32 indexCount = ( u32 )indices.size();

	    // Create GPU buffers.
	    Renderer::VertexBufferPtr vertexBuffer = m_hal->createVertexBuffer( vertexFormat, vertexCount );
	    Renderer::IndexBufferPtr  indexBuffer  = m_hal->createIndexBuffer( indexCount );

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
TextureImageSource::TextureImageSource( ImageHandle image, Renderer::HalWPtr hal )
    : AssetSource( image ), m_hal( hal )
{
}

// ** TextureImageSource::constructFromAsset
bool TextureImageSource::constructFromAsset( const Image& image, Assets::Assets& assets, Texture& texture )
{
    // Upload this image to a GPU texture
	Renderer::Texture2DPtr instance = m_hal->createTexture2D( image.width(), image.height(), image.bytesPerPixel() == 3 ? Renderer::PixelRgb8 : Renderer::PixelRgba8 );
	instance->setData( 0, &image.mipLevel( 0 )[0] );

    // Set texture asset
    texture.setTexture( instance );

    // Output log message
    LogVerbose( "renderingContext", "%dx%d %s texture constructed from image '%s'.\n", image.width(), image.height(), image.bytesPerPixel() == 3 ? "RGB8" : "RGBA8", m_asset.name().c_str() );

    return true;
}

// ----------------------------------------------------------------------- TechniqueMaterialSource ----------------------------------------------------------------------- //

// ** TechniqueMaterialSource::TechniqueMaterialSource
TechniqueMaterialSource::TechniqueMaterialSource( MaterialHandle material, Renderer::HalWPtr hal )
    : AssetSource( material ), m_hal( hal )
{
}

// ** TechniqueMaterialSource::constructFromAsset
bool TechniqueMaterialSource::constructFromAsset( const Material& material, Assets::Assets& assets, Technique& technique )
{
	CString vertex = NIMBLE_STRINGIFY(
			uniform mat4 u_vp, u_transform;

			void main() {
				gl_Position = u_vp * u_transform * gl_Vertex;
			}
		);
	CString fragment = NIMBLE_STRINGIFY(
			uniform vec4 u_color;
            uniform vec4 u_clr0;

			void main() {
				gl_FragColor = u_color * u_clr0;
			}
		);

    // Create the technique shader instance
    Renderer::ShaderPtr shader = m_hal->createShader( vertex, fragment );

    // Known shader input names
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

    NIMBLE_STATIC_ASSERT( (sizeof( inputs ) / sizeof( inputs[0] )) == Technique::TotalInputs, "missing shader input names" );

    // Locate all shader inputs
    for( s32 i = 0; i < Technique::TotalInputs; i++ ) {
        // Find the input location
        u32 location = shader->findUniformLocation( inputs[i] );

        if( !location ) {
            continue;
        }

        // Save the location index
        technique.setInputLocation( static_cast<Technique::Input>( i ), location );
        LogDebug( "technique", "%s is bound to %d for material %s\n", inputs[i], location, m_asset.asset().name().c_str() );
    }

    // Set technique shader
    technique.setShader( shader );

    // Set technique colors
    for( s32 i = 0; i < Material::TotalMaterialLayers; i++ ) {
        technique.setColor( i, material.color( static_cast<Material::Layer>( i ) ) );
    }

    return true;
}

} // namespace Scene

DC_END_DREEMCHEST