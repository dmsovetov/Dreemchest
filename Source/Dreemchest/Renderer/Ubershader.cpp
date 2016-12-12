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
#include "Hal.h"

DC_BEGIN_DREEMCHEST

namespace Renderer {

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

#if DEV_DEPRECATED_HAL
// ** Ubershader::permutation
const ShaderPtr& Ubershader::permutation( Renderer::HalWPtr hal, PipelineFeatures features, const PipelineFeatureLayout* featureLayout ) const
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
    String debug = "";

    for( u32 i = 0, n = featureLayout->elementCount(); i < n; i++ ) {
        const PipelineFeatureLayout::Element& element = featureLayout->elementAt(i);

        if( element.mask & features )
        {
            macro += "#define " + element.name + " " + toString( (element.mask & features) >> element.offset ) + "\n";
            if( debug.length() ) debug += ", ";
            debug += element.name;
        }
    }

    LogVerbose( "shader", "compiling permutation %s\n", debug.empty() ? "" : ("(" + debug + ")").c_str() );

    // Includes
    for( s32 i = 0, n = static_cast<s32>( m_includes.size() ); i < n; i++ ) {
        macro += m_includes[i];
    }

    // Compile the shader
    ShaderPtr compiled = hal->createShader( (macro + vertex()).c_str(), (macro + fragment()).c_str() );
    NIMBLE_BREAK_IF( !compiled.valid() );

    m_permutations[features] = compiled;

    return m_permutations[features];
}
#endif  /*  #if DEV_DEPRECATED_HAL  */

} // namespace Renderer

DC_END_DREEMCHEST
