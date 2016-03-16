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

#include "Renderable.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ----------------------------------------------------------------- Renderable ----------------------------------------------------------------- //

// ** Renderable::Renderable
Renderable::Renderable( void )
    : m_primitiveType( Renderer::PrimTriangles )
{

}

// ** Renderable::chunkCount
s32 Renderable::chunkCount( void ) const
{
    return static_cast<s32>( m_chunks.size() );
}

// ** Renderable::primitiveType
Renderer::PrimitiveType Renderable::primitiveType( void ) const
{
    return m_primitiveType;
}

// ** Renderable::setPrimitiveType
void Renderable::setPrimitiveType( Renderer::PrimitiveType value )
{
    m_primitiveType = value;
}

// ** Renderable::vertexBuffer
Renderer::VertexBufferWPtr Renderable::vertexBuffer( s32 chunk ) const
{
    DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
    return m_chunks[chunk].vertexBuffer;
}

// ** Renderable::setVertexBuffer
void Renderable::setVertexBuffer( s32 chunk, Renderer::VertexBufferPtr value )
{
    if( chunk >= chunkCount() ) {
        m_chunks.resize( chunk + 1 );
    }

    m_chunks[chunk].vertexBuffer = value;
}

// ** Renderable::indexBuffer
Renderer::IndexBufferWPtr Renderable::indexBuffer( s32 chunk ) const
{
    DC_ABORT_IF( chunk < 0 || chunk >= chunkCount(), "index is out of range" );
    return m_chunks[chunk].indexBuffer;
}

// ** Renderable::setIndexBuffer
void Renderable::setIndexBuffer( s32 chunk, Renderer::IndexBufferPtr value )
{
    if( chunk >= chunkCount() ) {
        m_chunks.resize( chunk + 1 );
    }

    m_chunks[chunk].indexBuffer = value;
}

// ----------------------------------------------------------------- Shader ----------------------------------------------------------------- //

// ** Shader::vertex
const String& Shader::vertex( void ) const
{
    return m_vertex;
}

// ** Shader::setVertex
void Shader::setVertex( const String& value )
{
    m_vertex = value;
}

// ** Shader::fragment
const String& Shader::fragment( void ) const
{
    return m_fragment;
}

// ** Shader::setFragment
void Shader::setFragment( const String& value )
{
    m_fragment = value;
}

// ** Shader::addFeature
void Shader::addFeature( u32 mask, const String& name )
{
	Feature feature;
	feature.mask = mask;
	feature.name = name;
	m_features.push_back( feature );
}

// ** Shader::featureCount
s32 Shader::featureCount( void ) const
{
    return static_cast<s32>( m_features.size() );
}

// ** Shader::feature
const Shader::Feature& Shader::feature( s32 index ) const
{
    DC_ABORT_IF( index < 0 || index >= featureCount(), "index is out of range" );
    return m_features[index];
}

// ** Shader::permutation
const Shader::Permutation& Shader::permutation( Renderer::HalWPtr hal, u32 features ) const
{
    // First lookup already compiled permutations
    Permutations::iterator i = m_permutations.find( features );

    // If found just return a reference to it.
    if( i != m_permutations.end() ) {
        return i->second;
    }

    // Compile the permutation.
    m_permutations[features] = Permutation();
    compile( hal, m_permutations[features], features );

    return m_permutations[features];
}

// ** Shader::compile
bool Shader::compile( Renderer::HalWPtr hal, Permutation& permutation, u32 features ) const
{
    DC_BREAK_IF( m_vertex.empty() || m_fragment.empty(), "the shader source is empty" );

    permutation.shader = hal->createShader( m_vertex.c_str(), m_fragment.c_str() );

	// Generate macro definitions from features
	String macro = "";

	for( u32 i = 0, n = featureCount(); i < n; i++ ) {
        const Feature& feature = this->feature( i );

		if( feature.mask & features ) {
			macro += "#define " + feature.name + "\n";
		}
	}

    LogVerbose( "shader", "compiling permutation %s\n", macro.c_str() );

	// Compile the shader
	permutation.shader = hal->createShader( (macro + m_vertex).c_str(), (macro + m_fragment).c_str() );
    DC_BREAK_IF( !permutation.shader.valid() );

    // Failed to compile - return false
    if( !permutation.shader.valid() ) {
        return false;
    }

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

    NIMBLE_STATIC_ASSERT( (sizeof( inputs ) / sizeof( inputs[0] )) == TotalInputs, "missing program input names" );

    // Locate all shader inputs
    for( s32 i = 0; i < TotalInputs; i++ ) {
        // Find the input location
        permutation.locations[i] = permutation.shader->findUniformLocation( inputs[i] );
    }

	return true;
}

#if DEV_DEPRECATED_PROGRAM
// ------------------------------------------------------------ Program ------------------------------------------------------------ //

// ** Program::Program
Program::Program( void )
{
    memset( m_locations, 0, sizeof( m_locations ) );
}

// ** Program::inputLocation
u32 Program::inputLocation( Input input ) const
{
    return m_locations[input];
}

// ** Program::setInputLocation
void Program::setInputLocation( Input input, u32 value )
{
    m_locations[input] = value;
}

// ** Program::features
u32 Program::features( void ) const
{
    return m_features;
}

// ** Program::setFeatures
void Program::setFeatures( u32 value )
{
    m_features = value;
}

// ** Program::shader
Renderer::ShaderWPtr Program::shader( void ) const
{
    return m_shader;
}

// ** Program::setShader
void Program::setShader( Renderer::ShaderPtr value )
{
    m_shader = value;
}
#endif

// ----------------------------------------------------------------- Texture ----------------------------------------------------------------- //

// ** Texture::texture
Renderer::TextureWPtr Texture::texture( void ) const
{
    return m_texture;
}

// ** Texture::setTexture
void Texture::setTexture( Renderer::TexturePtr value )
{
    m_texture = value;
}

// ----------------------------------------------------------------- Technique ----------------------------------------------------------------- //

// ** Technique::Technique
Technique::Technique( void )
    : m_features( 0 )
    , m_lightingModel( LightingModelUnlit )
{
}

// ** Technique::textureCount
s32 Technique::textureCount( void ) const
{
    return static_cast<s32>( m_textures.size() );
}

// ** Technique::texture
TextureHandle Technique::texture( s32 index ) const
{
    DC_ABORT_IF( index < 0 || index >= textureCount(), "index is out of range" );
    return m_textures[index];
}

// ** Technique::setTexture
void Technique::setTexture( s32 index, TextureHandle value )
{
    if( index >= textureCount() ) {
        m_textures.resize( index + 1 );
    }

    m_textures[index] = value;
}

// ** Technique::features
u32 Technique::features( void ) const
{
    return m_features;
}

// ** Technique::setFeatures
void Technique::setFeatures( u32 value )
{
    m_features = value;
}

// ** Technique::lightingModel
LightingModel Technique::lightingModel( void ) const
{
    return m_lightingModel;
}

// ** Technique::setLightingModel
void Technique::setLightingModel( LightingModel value )
{
    m_lightingModel = value;
}

// ** Technique::colorCount
s32 Technique::colorCount( void ) const
{
    return static_cast<s32>( m_colors.size() );
}

// ** Technique::color
const Rgba& Technique::color( s32 index ) const
{
    DC_ABORT_IF( index < 0 || index >= colorCount(), "index is out of range" );
    return m_colors[index];
}

// ** Technique::setColor
void Technique::setColor( s32 index, const Rgba& value )
{
    if( index >= colorCount() ) {
        m_colors.resize( index + 1 );
    }

    m_colors[index] = value;
}

} // namespace Scene

DC_END_DREEMCHEST