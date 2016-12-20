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
    
// ------------------------------------------------------------------ PipelineFeature -------------------------------------------------------------------- //
    
// ** PipelineState::PipelineState
PipelineState::PipelineState( void )
    : m_stateBlockFeatures(0)
    , m_features(0)
    , m_featureLayout(NULL)
    , m_changes(~0)
{
    
}
    
// ** PipelineState::features
PipelineFeatures PipelineState::features( void ) const
{
    return m_features;
}

// ** PipelineState::mask
PipelineFeatures PipelineState::mask( void ) const
{
    return m_featureLayout ? m_featureLayout->mask() : 0;
}

// ** PipelineState::setProgram
void PipelineState::setProgram(Program value)
{
    if (value == m_program)
    {
        return;
    }
    
    m_program = value;
    m_changes = m_changes | ProgramChanged;
}
    
// ** PipelineState::program
Program PipelineState::program( void ) const
{
    return m_program;
}
    
// ** PipelineState::setFeatureLayout
void PipelineState::setFeatureLayout(const PipelineFeatureLayout* value)
{
    if (value == m_featureLayout)
    {
        return;
    }
    
    m_featureLayout = value;
    m_changes = m_changes | FeatureLayoutChanged;
}
    
// ** PipelineState::featureLayout
const PipelineFeatureLayout* PipelineState::featureLayout( void ) const
{
    return m_featureLayout;
}
    
// ** PipelineState::activateVertexAttributes
void PipelineState::activateVertexAttributes(PipelineFeatures features)
{
    m_stateBlockFeatures = m_stateBlockFeatures | features;
}

// ** PipelineState::activateSampler
void PipelineState::activateSampler(u8 index)
{
    NIMBLE_BREAK_IF(index >= State::MaxTextureSamplers, "sampler index is out of range");
    m_stateBlockFeatures = m_stateBlockFeatures | PipelineFeature::sampler(index);
}

// ** PipelineState::activateConstantBuffer
void PipelineState::activateConstantBuffer(u8 index)
{
    NIMBLE_BREAK_IF(index >= State::MaxConstantBuffers, "constant buffer index is out of range");
    m_stateBlockFeatures = m_stateBlockFeatures | PipelineFeature::constantBuffer(index);
}

// ** PipelineState::activateUserFeatures
void PipelineState::activateUserFeatures(PipelineFeatures features)
{
    m_stateBlockFeatures = m_stateBlockFeatures | PipelineFeature::user(features);
}

// ** PipelineState::beginStateBlock
void PipelineState::beginStateBlock( void )
{
    m_stateBlockFeatures = 0;
}

// ** PipelineState::endStateBlock
void PipelineState::endStateBlock( void )
{
    // Mask activated features with a feature layout mask to reject those features that won't be used by a shader program.
    PipelineFeatures features = m_stateBlockFeatures & mask();
    
    // Do we have any changes?
    if (m_features == features)
    {
        return;
    }
    
    // Save this feature bitmask and record changes in a bitmask.
    m_features = features;
    m_changes  = m_changes | FeaturesChanged;
}
    
// ** PipelineState::changes
u8 PipelineState::changes( void ) const
{
    return m_changes;
}
    
// ** PipelineState::acceptChanges
void PipelineState::acceptChanges( void )
{
    m_changes = 0;
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
