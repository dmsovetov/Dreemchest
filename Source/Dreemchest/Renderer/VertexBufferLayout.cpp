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

#include "VertexBufferLayout.h"
#include "PipelineFeatureLayout.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
// ** VertexBufferLayout::VertexBufferLayout
VertexBufferLayout::VertexBufferLayout( s32 vertexSize )
    : m_features( 0 )
    , m_vertexSize( vertexSize )
{
}
    
// ** VertexBufferLayout::operator []
const VertexBufferLayout::Element& VertexBufferLayout::operator [](s32 index) const
{
    NIMBLE_ABORT_IF(index < 0 || index >= MaxVertexAttributes && "index is out of range");
    return m_attributes[index];
}

// ** VertexBufferLayout::attributeLocation
void VertexBufferLayout::attributeLocation(VertexAttribute attribute, s32 count, s32 offset)
{
    m_attributes[attribute].count  = count;
    m_attributes[attribute].offset = offset;
    
    m_features = m_features | PipelineFeature::vertexAttribute(attribute);
}

// ** VertexBufferLayout::features
PipelineFeatures VertexBufferLayout::features( void ) const
{
    return m_features;
}

// ** VertexBufferLayout::vertexSize
s32 VertexBufferLayout::vertexSize( void ) const
{
    return m_vertexSize;
}

// ** VertexBufferLayout::position
const VertexBufferLayout::Element& VertexBufferLayout::position( void ) const
{
    return m_attributes[VertexPosition];
}

// ** VertexBufferLayout::color
const VertexBufferLayout::Element& VertexBufferLayout::color( void ) const
{
    return m_attributes[VertexColor];
}

// ** VertexBufferLayout::normal
const VertexBufferLayout::Element& VertexBufferLayout::normal( void ) const
{
    return m_attributes[VertexNormal];
}

// ** VertexBufferLayout::uv
const VertexBufferLayout::Element& VertexBufferLayout::uv( u32 sampler ) const
{
    return m_attributes[VertexTexCoord0 + sampler];
}

// ** VertexBufferLayout::pointSize
const VertexBufferLayout::Element& VertexBufferLayout::pointSize( void ) const
{
    return m_attributes[VertexPointSize];
}
    
} // namespace Renderer

DC_END_DREEMCHEST
