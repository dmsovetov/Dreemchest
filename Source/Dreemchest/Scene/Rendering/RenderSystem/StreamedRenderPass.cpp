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
    , m_vertexFormat( VertexFormat::Position | VertexFormat::Color )
{
	// Request an input layout for a 2D vertex format
	m_inputLayout  = m_context.requestInputLayout( m_vertexFormat );

	// Request a vertex buffer used for rendering
	m_vertexBuffer = m_context.requestVertexBuffer( NULL, m_maxVerticesInBatch * m_vertexFormat.vertexSize() );
}

// ** StreamedRenderPassBase::end
void StreamedRenderPassBase::end( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack )
{
    flush( commands, stateStack );
}

// ** StreamedRenderPassBase::beginBatch
void StreamedRenderPassBase::beginBatch( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, Renderer::PrimitiveType primitive, s32 capacity )
{
    // Flush an active batch
    if( m_activeBatch.primitive != primitive ) {
        flush( commands, stateStack );
    }

    // Calculate a batch capacity
    s32 actualCapacity = capacity ? capacity : m_maxVerticesInBatch;

    // And start a new one
    m_activeBatch.size      = 0;
    m_activeBatch.capacity  = actualCapacity;
    m_activeBatch.primitive = primitive;
    m_activeBatch.stream    = frame.allocate( m_vertexFormat.vertexSize() * actualCapacity );
}

// ** StreamedRenderPassBase::restartBatch
void StreamedRenderPassBase::restartBatch( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, Renderer::PrimitiveType primitive )
{
    // Save an active batch state
    ActiveBatch activeBatch = m_activeBatch;

    // Flush an active batch
    flush( commands, stateStack );

    // And start a new one with same parameters
    beginBatch( frame, commands, stateStack, primitive, activeBatch.capacity );
}

// ** StreamedRenderPassBase::flush
void StreamedRenderPassBase::flush( RenderCommandBuffer& commands, RenderStateStack& stateStack )
{
    // Nothing to render - just skip
    if( m_activeBatch.size == 0 ) {
        return;
    }

    // Push a render state
    StateScope state = stateStack.newScope();
	state->bindVertexBuffer( m_vertexBuffer );
	state->bindInputLayout( m_inputLayout );

	// Upload vertex data to a GPU buffer and emit a draw primitives command
	commands.uploadVertexBuffer( m_vertexBuffer, m_activeBatch.stream, m_activeBatch.size * m_vertexFormat.vertexSize() );
	commands.drawPrimitives( 0, Renderer::PrimLines, stateStack.states(), 0, m_activeBatch.size );

    // Reset an active batch state
    m_activeBatch = ActiveBatch();
}

// ** StreamedRenderPassBase::hasEnoughSpace
bool StreamedRenderPassBase::hasEnoughSpace( s32 additionalVertices ) const
{
    return (m_activeBatch.size + additionalVertices) <= m_activeBatch.capacity;
}

// ** StreamedRenderPassBase::emitFrustum
void StreamedRenderPassBase::emitFrustum( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, f32 fov, f32 aspect, f32 near, f32 far, const Matrix4& transform, const Rgba& color )
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

    // Emit a near plane
    emitLine( frame, commands, stateStack, worldSpaceVertices[0], worldSpaceVertices[1], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[1], worldSpaceVertices[2], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[2], worldSpaceVertices[3], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[3], worldSpaceVertices[0], color );

    // Emit a far plane
    emitLine( frame, commands, stateStack, worldSpaceVertices[4], worldSpaceVertices[5], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[5], worldSpaceVertices[6], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[6], worldSpaceVertices[7], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[7], worldSpaceVertices[4], color );

    // Emit edges that are behind the near plane
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[0], color.transparent( 0.25f ) );
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[1], color.transparent( 0.25f ) );
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[2], color.transparent( 0.25f ) );
    emitLine( frame, commands, stateStack, worldSpaceVertices[8], worldSpaceVertices[3], color.transparent( 0.25f ) );

    // Emit edges that are infront of the near plane
    emitLine( frame, commands, stateStack, worldSpaceVertices[0], worldSpaceVertices[4], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[1], worldSpaceVertices[5], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[2], worldSpaceVertices[6], color );
    emitLine( frame, commands, stateStack, worldSpaceVertices[3], worldSpaceVertices[7], color );
}

// ** StreamedRenderPassBase::emitVertices
void StreamedRenderPassBase::emitVertices( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, Renderer::PrimitiveType primitive, const Vec3* positions, const Rgba* colors, s32 count )
{
    // Ensure that we have a space for these vertices
    if( !hasEnoughSpace( count ) ) {
        restartBatch( frame, commands, stateStack, primitive );
    }

    // Write each vertex to an output stream
    for( s32 i = 0; i < count; i++ ) {
        m_vertexFormat.setVertexAttribute( VertexFormat::Position, positions[i], m_activeBatch.stream, m_activeBatch.size + i );
        m_vertexFormat.setVertexAttribute( VertexFormat::Color, colors[i].toInteger(), m_activeBatch.stream, m_activeBatch.size + i );
    }

    // Increase a batch size
    m_activeBatch.size += count;
}

// ** StreamedRenderPassBase::emitLine
void StreamedRenderPassBase::emitLine( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Vec3& start, const Vec3& end, const Rgba& color )
{
    Vec3 positions[] = { start, end   };
    Rgba colors[]    = { color, color };
    emitVertices( frame, commands, stateStack, Renderer::PrimLines, positions, colors, 2 );
}

// ** StreamedRenderPassBase::emitWireBounds
void StreamedRenderPassBase::emitWireBounds( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Bounds& bounds, const Rgba& color )
{
    // Get a bounding box min and max values
    const Vec3& min = bounds.min();
    const Vec3& max = bounds.max();

    // Construct an extents vectors
    Vec3 x = Vec3( bounds.width(), 0.0f, 0.0f );
    Vec3 y = Vec3( 0.0f, bounds.height(), 0.0f );
    Vec3 z = Vec3( 0.0f, 0.0f, bounds.depth() );

    // Emit a bottom side of a bounding box
    emitLine( frame, commands, stateStack, min        , min + x,     color );
    emitLine( frame, commands, stateStack, min + x    , min + x + z, color );
    emitLine( frame, commands, stateStack, min + x + z, min + z,     color );
    emitLine( frame, commands, stateStack, min + z    , min,         color );

    // Emit a top side of a bounding box
    emitLine( frame, commands, stateStack, min + y        , min + x + y,     color );
    emitLine( frame, commands, stateStack, min + x + y    , min + x + z + y, color );
    emitLine( frame, commands, stateStack, min + x + z + y, min + z + y,     color );
    emitLine( frame, commands, stateStack, min + z + y    , min + y,         color );

    // Emit vertical lines
    emitLine( frame, commands, stateStack, min        , min + y,         color );
    emitLine( frame, commands, stateStack, min + x    , min + x + y,     color );
    emitLine( frame, commands, stateStack, min + x + z, min + x + z + y, color );
    emitLine( frame, commands, stateStack, min + z    , min + z + y,     color );
}

// ** StreamedRenderPassBase::emitWireBounds
void StreamedRenderPassBase::emitWireBounds( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Vec3 vertices[8], const Rgba& color )
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
void StreamedRenderPassBase::emitBasis( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Matrix4& transform )
{
    // Calculate a transformed basis origin
    Vec3 origin = transform * Vec3::zero();

    // Emit a line for each basis vector
    emitLine( frame, commands, stateStack, origin, Vec3( transform * Vec3::axisX() ), Rgba( 1.0f, 0.0f, 0.0f ) );
    emitLine( frame, commands, stateStack, origin, Vec3( transform * Vec3::axisY() ), Rgba( 0.0f, 1.0f, 0.0f ) );
    emitLine( frame, commands, stateStack, origin, Vec3( transform * Vec3::axisZ() ), Rgba( 0.0f, 0.0f, 1.0f ) );
}

} // namespace Scene

DC_END_DREEMCHEST