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

#include "StreamedRenderPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** StreamedRenderPassBase::StreamedRenderPassBase
StreamedRenderPassBase::StreamedRenderPassBase( RenderingContext& context, RenderScene& renderScene, s32 maxVerticesInBatch )
    : RenderPassBase( context, renderScene )
    , m_maxVerticesInBatch( maxVerticesInBatch )
{
    // Request a vertex buffer used for rendering
    m_vertexBuffer = m_context.requestVertexBuffer( NULL, m_maxVerticesInBatch * VertexFormat( VertexFormat::Position | VertexFormat::Color | VertexFormat::Uv0 ).vertexSize() );
}

// ** StreamedRenderPassBase::end
void StreamedRenderPassBase::end( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack )
{
    flush( commands, stateStack );
}

// ** StreamedRenderPassBase::beginBatch
void StreamedRenderPassBase::beginBatch( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, VertexFormat vertexFormat, Renderer::PrimitiveType primitive, s32 capacity )
{
    // Flush an active batch
    if( m_activeBatch.primitive != primitive || m_activeBatch.vertexFormat != vertexFormat ) {
        flush( commands, stateStack );
    }

    // Calculate a batch capacity
    s32 actualCapacity = capacity ? capacity : m_maxVerticesInBatch;

    // And start a new one
    m_activeBatch.size          = 0;
    m_activeBatch.capacity      = actualCapacity;
    m_activeBatch.primitive     = primitive;
    m_activeBatch.vertexFormat  = vertexFormat;
    m_activeBatch.stream        = frame.allocate( vertexFormat.vertexSize() * actualCapacity );
}

// ** StreamedRenderPassBase::restartBatch
void StreamedRenderPassBase::restartBatch( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, VertexFormat vertexFormat, Renderer::PrimitiveType primitive )
{
    // Save an active batch state
    ActiveBatch activeBatch = m_activeBatch;

    // Flush an active batch
    flush( commands, stateStack );

    // And start a new one with same parameters
    beginBatch( frame, commands, stateStack, vertexFormat, primitive, activeBatch.capacity );
}

// ** StreamedRenderPassBase::flush
void StreamedRenderPassBase::flush( CommandBuffer& commands, StateStack& stateStack )
{
    // Nothing to render - just skip
    if( m_activeBatch.size == 0 ) {
        return;
    }

    // Push a render state
    StateScope state = stateStack.newScope();
    state->bindVertexBuffer( m_vertexBuffer );
    state->bindInputLayout( m_context.requestInputLayout( m_activeBatch.vertexFormat ) );

    // Upload vertex data to a GPU buffer and emit a draw primitives command
    commands.uploadVertexBuffer( m_vertexBuffer, m_activeBatch.stream, m_activeBatch.size * m_activeBatch.vertexFormat.vertexSize() );
    commands.drawPrimitives( 0, m_activeBatch.primitive, 0, m_activeBatch.size, stateStack );

    // Reset an active batch state
    m_activeBatch = ActiveBatch();
}

// ** StreamedRenderPassBase::hasEnoughSpace
bool StreamedRenderPassBase::hasEnoughSpace( s32 additionalVertices ) const
{
    return (m_activeBatch.size + additionalVertices) <= m_activeBatch.capacity;
}

// ** StreamedRenderPassBase::emitFrustum
void StreamedRenderPassBase::emitFrustum( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, f32 fov, f32 aspect, f32 near, f32 far, const Matrix4& transform, const Rgba* color )
{
    // Calculate a tangents from a FOV and aspect ratio
    f32 tanHalfVFOV = tanf( radians( fov * 0.5f ) );
    f32 tanHalfHFOV = tanf( radians( fov * aspect * 0.5f ) );

    // Calculate near and far planes size
    f32 nh = tanHalfVFOV * near;
    f32 nw = tanHalfHFOV * near;
    f32 fh = tanHalfVFOV * far;
    f32 fw = tanHalfHFOV * far;

    // Construct a world space frustum vertices
    Vec3 worldSpaceVertices[] = {
          // Near plane
          transform * Vec4( -nw, -nh, -near )
        , transform * Vec4(  nw, -nh, -near )
        , transform * Vec4(  nw,  nh, -near )
        , transform * Vec4( -nw,  nh, -near )

          // Far plane
        , transform * Vec4( -fw, -fh, -far )
        , transform * Vec4(  fw, -fh, -far )
        , transform * Vec4(  fw,  fh, -far )
        , transform * Vec4( -fw,  fh, -far )

          // An origin point
        , transform * Vec4( 0.0f, 0.0f, 0.0f )
    };

    emitWireBounds( frame, commands, stateStack, worldSpaceVertices, color );

    // Create a transparent color
    Rgba transparent = color ? color->transparent( 0.25f ) : Rgba( 1.0f, 1.0f, 1.0f ).transparent( 0.25f );

    // Emit edges that are behind the near plane
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[0], &transparent );
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[1], &transparent );
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[2], &transparent );
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[3], &transparent );
}

// ** StreamedRenderPassBase::emitVertices
void StreamedRenderPassBase::emitVertices( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, Renderer::PrimitiveType primitive, const Vec3* positions, const Vec2* uv, const Rgba* colors, s32 count )
{
    // Get an input layout for these vertices
    u8 vertexFormat = VertexFormat::Position;
    if( uv ) vertexFormat     = vertexFormat | VertexFormat::Uv0;
    if( colors ) vertexFormat = vertexFormat | VertexFormat::Color;

    // Ensure that we have a space for these vertices
    if( !hasEnoughSpace( count ) || m_activeBatch.vertexFormat != vertexFormat ) {
        restartBatch( frame, commands, stateStack, vertexFormat, primitive );
    }

    // Write each vertex to an output stream
    for( s32 i = 0; i < count; i++ ) {
        m_activeBatch.vertexFormat.setVertexAttribute( VertexFormat::Position, positions[i], m_activeBatch.stream, m_activeBatch.size + i );
        if( colors ) {
            m_activeBatch.vertexFormat.setVertexAttribute( VertexFormat::Color, colors[i].toInteger(), m_activeBatch.stream, m_activeBatch.size + i );
        }
        if( uv ) {
            m_activeBatch.vertexFormat.setVertexAttribute( VertexFormat::Uv0, uv[i], m_activeBatch.stream, m_activeBatch.size + i );
        }
    }

    // Increase a batch size
    m_activeBatch.size += count;
}

// ** StreamedRenderPassBase::emitLine
void StreamedRenderPassBase::emitLine( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Vec3& start, const Vec3& end, const Rgba* color )
{
    Rgba vertexColor = color ? *color : Rgba( 1.0f, 1.0f, 1.0f, 1.0 );
    Vec3 positions[] = { start, end   };
    Rgba colors[]    = { vertexColor, vertexColor };
    emitVertices( frame, commands, stateStack, Renderer::PrimLines, positions, NULL, color ? colors : NULL, 2 );
}

// ** StreamedRenderPassBase::emitWireBounds
void StreamedRenderPassBase::emitWireBounds( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Bounds& bounds, const Rgba* color )
{
    // Get a bounding box position
    f32 x = bounds.min().x;
    f32 y = bounds.min().y;
    f32 z = bounds.min().z;

    // Get a bounding box extents
    f32 w = bounds.width();
    f32 h = bounds.height();
    f32 d = bounds.depth();

    // Construct a vertex buffer for a bounding box
    Vec3 worldSpaceVertices[] = {
          // Bottom side
          { x,      y, z     }
        , { x + w,  y, z     }
        , { x + w,  y, z + d }
        , { x,      y, z + d }

          // Top side
        , { x,      y + h, z     }
        , { x + w,  y + h, z     }
        , { x + w,  y + h, z + d }
        , { x,      y + h, z + d }
    };

    emitWireBounds( frame, commands, stateStack, worldSpaceVertices, color );
}

// ** StreamedRenderPassBase::emitWireBounds
void StreamedRenderPassBase::emitWireBounds( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Vec3 vertices[8], const Rgba* color )
{
    for( s32 i = 0; i < 4; i++ ) {
        emitLine( frame, commands, stateStack, vertices[i], vertices[(i + 1) % 4], color );
    }
    for( s32 i = 0; i < 4; i++ ) {
        emitLine( frame, commands, stateStack, vertices[i + 4], vertices[(i + 1) % 4 + 4], color );
    }
    for( s32 i = 0; i < 4; i++ ) {
        emitLine( frame, commands, stateStack, vertices[i], vertices[i + 4], color );
    }
}

// ** StreamedRenderPassBase::emitBasis
void StreamedRenderPassBase::emitBasis( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Matrix4& transform )
{
    // Calculate a transformed basis origin
    Vec3 origin = transform * Vec3::zero();
    
    // Construct basis colors
    Rgba red( 1.0f, 0.0f, 0.0f );
    Rgba green( 0.0f, 1.0f, 0.0f );
    Rgba blue( 0.0f, 0.0f, 1.0f );

    // Emit a line for each basis vector
    emitLine( frame, commands, stateStack, origin, Vec3( transform * Vec3::axisX() ), &red );
    emitLine( frame, commands, stateStack, origin, Vec3( transform * Vec3::axisY() ), &green );
    emitLine( frame, commands, stateStack, origin, Vec3( transform * Vec3::axisZ() ), &blue );
}

// ** StreamedRenderPassBase::emitRect
void StreamedRenderPassBase::emitRect( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Vec3* positions, const Vec2* uv, const Rgba* colors )
{
    u16  indices[] = { 0, 1, 2, 0, 2, 3 };
    Vec3 trianglePositions[3];
    Rgba triangleColors[3];
    Vec2 triangleUv[3];

    for( s32 j = 0; j < 2; j++ ) {
        for( s32 i = 0; i < 3; i++ ) {
            trianglePositions[i] = positions[indices[i + j * 3]];
            triangleColors[i]    = colors ? colors[indices[i + j * 3]] : Rgba();
            triangleUv[i]        = uv ? uv[indices[i + j * 3]] : Vec2();
        }
        emitVertices( frame, commands, stateStack, Renderer::PrimTriangles, trianglePositions, uv ? triangleUv : NULL, colors ? triangleColors : NULL, 3 );
    }
}

// ** StreamedRenderPassBase::emitRect
void StreamedRenderPassBase::emitRect( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Vec3& point, const Vec3& u, const Vec3& v, const Rgba* color )
{
    Rgba vertexColor = color ? *color : Rgba();

    Vec3 vertices[] = {
          point - u - v
        , point + u - v
        , point + u + v
        , point - u + v
    };
    Rgba colors[] = {
          vertexColor
        , vertexColor
        , vertexColor
        , vertexColor
    };
    Vec2 uv[] = {
          { 0.0f, 0.0f }
        , { 1.0f, 0.0f }
        , { 1.0f, 1.0f }
        , { 0.0f, 1.0f }
    };

    emitRect( frame, commands, stateStack, vertices, uv, color ? colors : NULL );
}

} // namespace Scene

DC_END_DREEMCHEST
