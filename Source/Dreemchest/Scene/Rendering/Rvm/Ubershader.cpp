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

#include "Ubershader.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Ubershader::Ubershader
Ubershader::Ubershader( void )
    : m_supportedFeatures( 0 )
{
}

// ** Ubershader::addInclude
void Ubershader::addInclude( const String& value )
{
    m_includes.push_back( value );
}

// ** Ubershader::vertex
const String& Ubershader::vertex( void ) const
{
    return m_vertex;
}

// ** Ubershader::setVertex
void Ubershader::setVertex( const String& value )
{
    m_vertex = value;
}

// ** Ubershader::fragment
const String& Ubershader::fragment( void ) const
{
    return m_fragment;
}

// ** Ubershader::setFragment
void Ubershader::setFragment( const String& value )
{
    m_fragment = value;
}

// ** Ubershader::addFeature
void Ubershader::addFeature( Bitmask mask, const String& name )
{
	Feature feature;
	feature.mask = mask;
	feature.name = name;
	m_features.push_back( feature );
    m_supportedFeatures = m_supportedFeatures | mask;
}

// ** Ubershader::featureCount
s32 Ubershader::featureCount( void ) const
{
    return static_cast<s32>( m_features.size() );
}

// ** Ubershader::feature
const Ubershader::Feature& Ubershader::feature( s32 index ) const
{
    DC_ABORT_IF( index < 0 || index >= featureCount(), "index is out of range" );
    return m_features[index];
}

// ** Ubershader::supportedFeatures
Ubershader::Bitmask Ubershader::supportedFeatures( void ) const
{
    return m_supportedFeatures;
}

// ** Ubershader::permutation
const Renderer::ShaderPtr& Ubershader::permutation( Renderer::HalWPtr hal, Bitmask features ) const
{
    // First cancel all features that are not supported by a shader
    //features = features & m_supportedFeatures;

    // Now lookup a permutation cache
    Permutations::const_iterator i = m_permutations.find( features );

    if( i != m_permutations.end() ) {
        return i->second;
    }

	// Generate macro definitions from features
	String macro = "";

	for( u32 i = 0, n = featureCount(); i < n; i++ ) {
        const Feature& feature = this->feature( i );

		if( feature.mask & features ) {
			macro += "#define " + feature.name + "\n";
		}
	}

    LogVerbose( "shader", "compiling permutation %s\n", macro.c_str() );

    // Includes
    for( s32 i = 0, n = static_cast<s32>( m_includes.size() ); i < n; i++ ) {
        macro += m_includes[i];
    }

	// Compile the shader
	Renderer::ShaderPtr compiled = hal->createShader( (macro + vertex()).c_str(), (macro + fragment()).c_str() );
    DC_BREAK_IF( !compiled.valid() );

    m_permutations[features] = compiled;

    return m_permutations[features];
}

} // namespace Scene

DC_END_DREEMCHEST