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

#include "PipelineFeatureLayout.h"
#include "VertexBufferLayout.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
    //! An offset of sampler features.
    enum
    {
        SamplerFeaturesOffset = MaxVertexAttributes
    };
    
    //! An offset of constant buffer features.
    enum
    {
        CBufferFeaturesOffset = SamplerFeaturesOffset + State::MaxTextureSamplers
    };
    
    //! An offset of user-defined shader features.
    enum
    {
        UserDefinedFeaturesOffset = CBufferFeaturesOffset + State::MaxConstantBuffers
    };
    
    //! A total number of available user-defined feature bits.
    enum
    {
        MaxUserDefinedFeatureBits = 64 - UserDefinedFeaturesOffset
    };
    
// ------------------------------------------------------------------ PipelineFeature -------------------------------------------------------------------- //
    
// ** PipelineFeature::constantBuffer
PipelineFeatures PipelineFeature::constantBuffer(u8 index)
{
    return static_cast<PipelineFeatures>(1) << (CBufferFeaturesOffset + index);
}
    
// ** PipelineFeature::vertexAttribute
PipelineFeatures PipelineFeature::vertexAttribute(VertexAttribute vertexAttribute)
{
    s32 bit = vertexAttribute - 1; // Skip VertexPosition attribute
    return static_cast<PipelineFeatures>(1) << bit;
}

// ** PipelineFeature::sampler
PipelineFeatures PipelineFeature::sampler(u8 index)
{
    return static_cast<PipelineFeatures>(1) << (SamplerFeaturesOffset + index);
}
    
// ** PipelineFeature::user
PipelineFeatures PipelineFeature::user(PipelineFeatures userDefined)
{
    return userDefined << UserDefinedFeaturesOffset;
}

// ---------------------------------------------------------------- PipelineFeatureLayout ---------------------------------------------------------------- //
    
// ** PipelineFeatureLayout::PipelineFeatureLayout
PipelineFeatureLayout::PipelineFeatureLayout( void )
    : m_mask(0)
{
    
}
    
// ** PipelineFeatureLayout::mask
PipelineFeatures PipelineFeatureLayout::mask( void ) const
{
    return m_mask;
}

// ** PipelineFeatureLayout::elementCount
s32 PipelineFeatureLayout::elementCount( void ) const
{
    return static_cast<s32>(m_elements.size());
}

// ** PipelineFeatureLayout::elementAt
const PipelineFeatureLayout::Element& PipelineFeatureLayout::elementAt(s32 index) const
{
    NIMBLE_ABORT_IF(index < 0 || index >= elementCount(), "index is out of range");
    return m_elements[index];
}
    
// ** PipelineFeatureLayout::addFeature
void PipelineFeatureLayout::addFeature(const String& name, PipelineFeatures mask)
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
