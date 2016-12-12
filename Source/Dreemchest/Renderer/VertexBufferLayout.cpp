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

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
// ** VertexBufferLayout::VertexBufferLayout
VertexBufferLayout::VertexBufferLayout( s32 vertexSize )
    : m_features( 0 )
    , m_vertexSize( vertexSize )
{
}

// ** VertexBufferLayout::attributeLocation
void VertexBufferLayout::attributeLocation( Attribute attribute, s32 count, s32 offset )
{
    static PipelineFeatures kAttributeMask[TotalAttributes] = {
        0,
        BIT( 0 ),
        BIT( 1 ),
        BIT( 2 ),
        BIT( 3 ),
        BIT( 4 ),
        BIT( 5 ),
        BIT( 6 ),
        BIT( 7 ),
        BIT( 8 ),
        BIT( 9 ),
        BIT( 10 ),
        BIT( 11 ),
        BIT( 12 ),
    };
    
    m_attributes[attribute].count  = count;
    m_attributes[attribute].offset = offset;
    
    m_features = m_features | kAttributeMask[attribute];
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
    return m_attributes[Position];
}

// ** VertexBufferLayout::color
const VertexBufferLayout::Element& VertexBufferLayout::color( void ) const
{
    return m_attributes[Color];
}

// ** VertexBufferLayout::normal
const VertexBufferLayout::Element& VertexBufferLayout::normal( void ) const
{
    return m_attributes[Normal];
}

// ** VertexBufferLayout::uv
const VertexBufferLayout::Element& VertexBufferLayout::uv( u32 sampler ) const
{
    return m_attributes[Uv0 + sampler];
}

// ** VertexBufferLayout::pointSize
const VertexBufferLayout::Element& VertexBufferLayout::pointSize( void ) const
{
    return m_attributes[PointSize];
}
    
} // namespace Renderer

DC_END_DREEMCHEST
