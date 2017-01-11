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
    
// ------------------------------------------------------------------ PipelineState -------------------------------------------------------------------- //
    
// ** PipelineState::PipelineState
PipelineState::PipelineState( void )
    : m_program(0)
    , m_featureLayout(NULL)
    , m_inputLayout(NULL)
    , m_vertexBuffer(0)
    , m_indexBuffer(0)
    , m_colorMask(0)
{
    setBlending(BlendDisabled, BlendDisabled);
    setDepthState(LessEqual, true);
    setCullFace(TriangleFaceBack);
    setStencilFunction(CompareDisabled, 0, ~0);
    setStencilOp(StencilKeep, StencilKeep, StencilKeep);
    setColorMask(~0);
    setRasterization(PolygonFill);
    setAlphaTest(CompareDisabled, 0);
    setPolygonOffset(0.0f, 0.0f);
    
    memset(m_texture, 0, sizeof(m_texture));
    memset(m_constantBuffer, 0, sizeof(m_constantBuffer));
}

// ** PipelineState::mask
PipelineFeatures PipelineState::mask( void ) const
{
    return m_featureLayout ? m_featureLayout->mask() : 0;
}

void PipelineState::setCullFace(TriangleFace value)
{
    m_cullFace = value;
}

TriangleFace PipelineState::cullFace() const
{
    return m_cullFace;
}

// ** PipelineState::setProgram
void PipelineState::setProgram(ResourceId value)
{
    if (value == m_program)
    {
        return;
    }
    
    m_program = value;
}
    
// ** PipelineState::program
ResourceId PipelineState::program( void ) const
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
}
    
// ** PipelineState::featureLayout
const PipelineFeatureLayout* PipelineState::featureLayout( void ) const
{
    return m_featureLayout;
}
    
// ** PipelineState::setInputLayout
void PipelineState::setInputLayout(const VertexBufferLayout* value)
{
    m_inputLayout = value;
}
    
// ** PipelineState::inputLayout
const VertexBufferLayout* PipelineState::inputLayout() const
{
    return m_inputLayout;
}

// ** PipelineState::setTexture
void PipelineState::setTexture(ResourceId id, u8 index)
{
    NIMBLE_BREAK_IF(index >= State::MaxTextureSamplers, "sampler index is out of range");
    m_texture[index] = id;
}
    
// ** PipelineState::texture
ResourceId PipelineState::texture(u8 index) const
{
    NIMBLE_BREAK_IF(index >= State::MaxTextureSamplers, "sampler index is out of range");
    return m_texture[index];
}

// ** PipelineState::setConstantBuffer
void PipelineState::setConstantBuffer(ResourceId id, u8 index)
{
    NIMBLE_BREAK_IF(index >= State::MaxConstantBuffers, "constant buffer index is out of range");
    m_constantBuffer[index] = id;
}
    
// ** PipelineState::texture
ResourceId PipelineState::constantBuffer(u8 index) const
{
    NIMBLE_BREAK_IF(index >= State::MaxConstantBuffers, "constant buffer index is out of range");
    return m_constantBuffer[index];
}
    
// ** PipelineState::setVertexBuffer
void PipelineState::setVertexBuffer(ResourceId id)
{
    m_vertexBuffer = id;
}
    
// ** PipelineState::vertexBuffer
ResourceId PipelineState::vertexBuffer() const
{
    return m_vertexBuffer;
}

// ** PipelineState::setIndexBuffer
void PipelineState::setIndexBuffer(ResourceId id)
{
    m_indexBuffer = id;
}
    
// ** PipelineState::indexBuffer
ResourceId PipelineState::indexBuffer() const
{
    return m_indexBuffer;
}
    
// ** PipelineState::setDepthState
void PipelineState::setDepthState(Compare function, bool depthWrite)
{
    m_depthState.function   = function;
    m_depthState.depthWrite = depthWrite;
}

// ** PipelineState::depthTestFunction
Compare PipelineState::depthTestFunction() const
{
    return m_depthState.function;
}

// ** PipelineState::depthWrite
bool PipelineState::depthWrite() const
{
    return m_depthState.depthWrite;
}
    
// ** PipelineState::setBlending
void PipelineState::setBlending(BlendFactor src, BlendFactor dst)
{
    m_blend.source = src;
    m_blend.dest   = dst;
}
    
// ** PipelineState::sourceBlendFactor
BlendFactor PipelineState::sourceBlendFactor() const
{
    return m_blend.source;
}

// ** PipelineState::destBlendFactor
BlendFactor PipelineState::destBlendFactor() const
{
    return m_blend.dest;
}

// ** PipelineState::setColorMask
void PipelineState::setColorMask(u8 value)
{
    m_colorMask = value;
}

// ** PipelineState::colorMask
u8 PipelineState::colorMask() const
{
    return m_colorMask;
}

// ** PipelineState::setStencilFunction
void PipelineState::setStencilFunction(Compare function, u8 ref, u8 mask)
{
    m_stencil.function = function;
    m_stencil.ref = ref;
    m_stencil.mask = mask;
}

// ** PipelineState::stencilFunction
Compare PipelineState::stencilFunction() const
{
    return m_stencil.function;
}
    
// ** PipelineState::stencilRef
u8 PipelineState::stencilRef() const
{
    return m_stencil.ref;
}

// ** PipelineState::stencilMask
u8 PipelineState::stencilMask() const
{
    return m_stencil.mask;
}

// ** PipelineState::setStencilOp
void PipelineState::setStencilOp(StencilAction sfail, StencilAction dfail, StencilAction dspass)
{
    m_stencilOp.sfail = sfail;
    m_stencilOp.dfail = dfail;
    m_stencilOp.dspass = dspass;
}

// ** PipelineState::stencilStencilFail
StencilAction PipelineState::stencilStencilFail() const
{
    return m_stencilOp.sfail;
}

// ** PipelineState::stencilDepthFail
StencilAction PipelineState::stencilDepthFail() const
{
    return m_stencilOp.dfail;
}

// ** PipelineState::stencilPass
StencilAction PipelineState::stencilPass() const
{
    return m_stencilOp.dspass;
}

// ** PipelineState::setRasterization
void PipelineState::setRasterization(PolygonMode value)
{
    m_rasterization = value;
}

// ** PipelineState::rasterization
PolygonMode PipelineState::rasterization() const
{
    return m_rasterization;
}
    
// ** PipelineState::setAlphaTest
void PipelineState::setAlphaTest(Compare function, u8 ref)
{
    m_alphaTest.function = function;
    m_alphaTest.ref = ref;
}

// ** PipelineState::alphaTestFunction
Compare PipelineState::alphaTestFunction() const
{
    return m_alphaTest.function;
}
    
// ** PipelineState::alphaTestRef
u8 PipelineState::alphaTestRef() const
{
    return m_alphaTest.ref;
}

// ** PipelineState::setPolygonOffset
void PipelineState::setPolygonOffset(f32 factor, f32 units)
{
    m_polygonOffset.factor = factor;
    m_polygonOffset.units = units;
}

// ** PipelineState::polygonOffsetFactor
f32 PipelineState::polygonOffsetFactor() const
{
    return m_polygonOffset.factor;
}

// ** PipelineState::polygonOffsetUnits
f32 PipelineState::polygonOffsetUnits() const
{
    return m_polygonOffset.units;
}

// ** PipelineState::resetConstantBuffer
void PipelineState::resetConstantBuffer(ResourceId id)
{
    for (s32 i = 0; i < State::MaxConstantBuffers; i++)
    {
        if (m_constantBuffer[i] == id)
        {
            m_constantBuffer[i] = 0;
        }
    }
}
    
// ** PipelineState::resetProgram
void PipelineState::resetProgram(ResourceId id)
{
    if (m_program == id)
    {
        m_program = 0;
    }
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
