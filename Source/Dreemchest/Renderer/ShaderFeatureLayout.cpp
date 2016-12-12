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

#include "ShaderFeatureLayout.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
// ------------------------------------------------------------------- ShaderFeature --------------------------------------------------------------------- //
    
// ** ShaderFeature::constantBuffer
PipelineFeatures ShaderFeature::constantBuffer(ConstantBufferFeatures index)
{
    return static_cast<PipelineFeatures>(1) << (CBufferFeaturesOffset + index);
}
    
// ----------------------------------------------------------------- ShaderFeatureLayout ----------------------------------------------------------------- //
    
// ** ShaderFeatureLayout::ShaderFeatureLayout
ShaderFeatureLayout::ShaderFeatureLayout( void )
    : m_mask(0)
{
    
}
    
// ** ShaderFeatureLayout::mask
PipelineFeatures ShaderFeatureLayout::mask( void ) const
{
    return m_mask;
}

// ** ShaderFeatureLayout::elementCount
s32 ShaderFeatureLayout::elementCount( void ) const
{
    return static_cast<s32>(m_elements.size());
}

// ** ShaderFeatureLayout::elementAt
const ShaderFeatureLayout::Element& ShaderFeatureLayout::elementAt(s32 index) const
{
    NIMBLE_ABORT_IF(index < 0 || index >= elementCount(), "index is out of range");
    return m_elements[index];
}
    
// ** ShaderFeatureLayout::addFeature
void ShaderFeatureLayout::addFeature(const String& name, PipelineFeatures mask)
{
    u32 offset = 0;
    
    for( offset = 0; offset < sizeof( Bitset ) * 8; offset++ )
    {
        if( (static_cast<u64>( 1 ) << offset & mask) != 0 )
        {
            break;
        }
    }
    
    Element feature;
    feature.mask   = mask;
    feature.name   = name;
    feature.offset = offset;
    m_elements.push_back( feature );
    m_mask = m_mask | mask;
}

} // namespace Renderer

DC_END_DREEMCHEST
