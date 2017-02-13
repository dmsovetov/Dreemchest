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

#if DEV_DEPRECATED_HAL && 0

#include    "BatchRenderer.h"
#include    "Hal.h"
#include    "RenderState.h"

#include    "../platform/Engine.h"
#include    "../stage/Shape.h"

#define        SET_VERTEX( vtx, _x, _y, color )    \
                vtx.x = ceilf( _x ) + 0.35f;    \
                vtx.y = ceilf( _y ) + 0.35f;    \
                vtx.r = color.r, vtx.g = color.g, vtx.b = color.b, vtx.a = color.a;

#define        SET_VERTEX_UV( vtx, _x, _y, _color, _u, _v )    \
                SET_VERTEX( vtx, _x, _y, _color )            \
                vtx.u = _u;                                    \
                vtx.v = _v;

DC_BEGIN_DREEMCHEST

namespace Renderer {

// ** BatchRenderer::BatchRenderer
BatchRenderer::BatchRenderer( dcEngine engine, dcHal hal, int maxRenderBufferSize ) : m_engine( engine ), m_hal( hal ), m_immediateTexture( NULL )
{
    for( int i = 0; i < TransformTotal; i++ ) {
        m_transform[i].Identity();
    }

    m_maxVertices        = maxRenderBufferSize;
    m_maxIndices        = m_maxVertices * 6;
    m_immediateTexture    = NULL;
    m_totalIndices        = 0;
    m_totalVertices        = 0;
    m_vertices            = NULL;
    m_indices            = NULL;
    m_mask                = 0;

    initialize();
}

BatchRenderer::~BatchRenderer( void )
{
    DC_RELEASE_N( m_vertexFormat );
    DC_RELEASE_N( m_vertexBuffer );
    DC_RELEASE_N( m_indexBuffer );
    DC_RELEASE_N( m_fanIndexBuffer );

    delete m_rasterState2D;
    delete m_blendState2D;
    delete m_disableColorWrite;
    delete m_beginMask;
    delete m_endMask;
    delete m_disableMask;
}

// ** BatchRenderer::initialize
void BatchRenderer::initialize( void )
{
    // ** Create vertex format
    m_vertexFormat = m_hal->createVertexDeclaration( "P2:T02:C" );
    m_pointFormat  = m_hal->createVertexDeclaration( "P2:S2:C", sizeof( sVertex ) );
    NIMBLE_BREAK_IF( m_vertexFormat->vertexSize() != sizeof( sVertex ) );

    // ** Create buffers
    m_vertexBuffer     = m_hal->createVertexBuffer( m_vertexFormat, m_maxVertices, false );
    m_indexBuffer     = m_hal->createIndexBuffer( m_maxIndices, false );
    m_fanIndexBuffer = m_hal->createIndexBuffer( m_maxIndices, false );

    // ** Lock buffers
    m_indices     = m_indexBuffer->lock();
    m_fanIndices = m_fanIndexBuffer->lock();
    m_vertices     = ( sVertex* )m_vertexBuffer->lock();

    // ** Fill index buffers with data
    for( int i = 0; i < m_maxVertices / 4; i++ ) {
        m_indices[i * 6 + 0] = 0 + i * 4;
        m_indices[i * 6 + 1] = 1 + i * 4;
        m_indices[i * 6 + 2] = 2 + i * 4;

        m_indices[i * 6 + 3] = 0 + i * 4;
        m_indices[i * 6 + 4] = 2 + i * 4;
        m_indices[i * 6 + 5] = 3 + i * 4;
    }
    
    for( int i = 0; i < m_maxIndices; i++ ) {
        m_fanIndices[i] = i;
    }

    // ** 2D render state
    m_rasterState2D = DC_NEW RasterizerState;
    m_rasterState2D->setCullMode( TriangleFaceNone );
    m_rasterState2D->setZWrite( false );

    m_blendState2D = DC_NEW BlendState;
    m_blendState2D->setBlendEnabled( true );
    m_blendState2D->setBlendFunc( BlendSrcAlpha, BlendInvSrcAlpha );

    // ** Mask states
    m_beginMask = DC_NEW DepthStencilState;
    m_beginMask->setStencilEnabled( true );
    m_beginMask->setStencilFunc( Equal, 0, 0xFF );
    m_beginMask->setStencilOp( StencilKeep, StencilKeep, StencilIncSaturate );

    m_endMask = DC_NEW DepthStencilState;
    m_endMask->setStencilFunc( Equal, 0, 0xFF );
    m_endMask->setStencilOp( StencilKeep, StencilKeep, StencilKeep );

    m_disableMask = DC_NEW DepthStencilState;
    m_disableMask->setStencilFunc( Equal, 0, 0xFF );
    m_disableMask->setStencilOp( StencilKeep, StencilKeep, StencilDecSaturate );

    // ** Color write state
    m_disableColorWrite = DC_NEW RasterizerState;
    m_disableColorWrite->setColorWrite( false, false, false, false );

    m_enableColorWrite = DC_NEW RasterizerState;
    m_enableColorWrite->setColorWrite( true, true, true, true );
}

// ** BatchRenderer::beginMask
void BatchRenderer::beginMask( void )
{
    flush();

    if( m_mask == 0 ) {
        m_hal->clear( Black, 1.0f, 0, renderer::ClearStencil );
    }

    m_beginMask->m_stencilValue = m_mask++;
    m_blendState2D->setBlendEnabled( false );
    m_hal->setBlendState( m_blendState2D );
    m_hal->setRasterizerState( m_disableColorWrite );
    m_hal->setDepthStencilState( m_beginMask );
}

// ** BatchRenderer::endMask
void BatchRenderer::endMask( void )
{
    flush();

    m_endMask->setStencilEnabled( true );
    m_endMask->m_stencilValue = m_mask;
    m_blendState2D->setBlendEnabled( true );
    m_hal->setRasterizerState( m_enableColorWrite );
    m_hal->setDepthStencilState( m_endMask );
    m_hal->setBlendState( m_blendState2D );
}

// ** BatchRenderer::disableMask
void BatchRenderer::disableMask( int width, int height )
{
    flush();

    if( --m_mask == 0 ) {
        m_endMask->setStencilEnabled( false );
        m_hal->setDepthStencilState( m_endMask );
        return;
    }

    m_endMask->setStencilEnabled( true );

    m_disableMask->m_stencilMask = m_mask + 1;
    m_hal->setRasterizerState( m_disableColorWrite );
    m_hal->setDepthStencilState( m_disableMask );

    renderRect( 0, 0, width, height );

    endMask();
}

// ** BatchRenderer::pushTransform
void BatchRenderer::pushTransform( eTransform transform )
{
    m_transformStack[transform].push( m_transform[transform] );
}

// ** BatchRenderer::popTransform
void BatchRenderer::popTransform( eTransform transform )
{
    NIMBLE_BREAK_IF( m_transformStack[transform].empty() );

    setTransform( transform, m_transformStack[transform].top() );
    m_transformStack[transform].pop();
}

// ** BatchRenderer::setTransform
void BatchRenderer::setTransform( eTransform transform, const cMatrix4& T, bool multiply )
{
    flush();
    
    cMatrix4& matrix = m_transform[transform];
    matrix = multiply ? matrix * T : T;

    m_hal->setTransform( transform, matrix );
}

// ** BatchRenderer::setBlendMode
void BatchRenderer::setBlendMode( eBlendFactor src, eBlendFactor dst )
{
    if( m_blendState2D->m_src == src && m_blendState2D->m_dst == dst ) {
        return;
    }

    flush();

    m_blendState2D->setBlendFunc( src, dst );
    m_hal->setBlendState( m_blendState2D );
}

// ** BatchRenderer::setColorModulation
void BatchRenderer::setColorModulation( float r, float g, float b, float a )
{
    flush();

    m_color = vec4( r, g, b, a );
    m_hal->setColorModulation( r, g, b, a );
}

// ** BatchRenderer::begin2D
void BatchRenderer::begin2D( int width, int height )
{
    if( width == 0 )  width  = m_engine->GetScreenWidth();
    if( height == 0 ) height = m_engine->GetScreenHeight();

    pushTransform( TransformProjection );
    pushTransform( TransformView );
    pushTransform( TransformModel );

    setTransform( TransformProjection, cMatrix4::Ortho( 0, width, height, 0, -4096, 4096 ) );
    setTransform( TransformView, NULL );
    setTransform( TransformModel, NULL );
    m_hal->setViewport( 0, 0, width, height );

    m_hal->setRasterizerState( m_rasterState2D );
    m_hal->setBlendState( m_blendState2D );
}

// ** BatchRenderer::end2D
void BatchRenderer::end2D( void )
{
    flush();

    popTransform( TransformProjection );
    popTransform( TransformView );
    popTransform( TransformModel );

    m_hal->setViewport( 0, 0, m_engine->GetScreenWidth(), m_engine->GetScreenHeight() );
}

// ** BatchRenderer::flush
void BatchRenderer::flush( void )
{
    if( !m_totalVertices && !m_totalIndices ) {
        return;
    }

    m_hal->setTexture( 0, m_immediateTexture );
    m_hal->setVertexBuffer( m_vertexBuffer, m_immediatePrimitive == PrimPoints ? m_pointFormat : m_vertexFormat );

    if( m_totalIndices ) {
        m_hal->renderIndexed( m_immediatePrimitive, m_immediatePrimitive == PrimTriangles ? m_indexBuffer : m_fanIndexBuffer, 0, m_totalIndices );
    } else {
        m_hal->renderPrimitives( m_immediatePrimitive, 0, m_totalVertices );
    }

    m_totalVertices        = 0;
    m_totalIndices        = 0;
}

// ** BatchRenderer::renderVertices
void BatchRenderer::renderVertices( ePrimitiveType primitiveType, dcTexture2D texture, const sVertex *vertices, int count )
{
    NIMBLE_BREAK_IF( m_vertices == NULL || m_indices == NULL );

    int indexCount = calculateIndexCount( primitiveType, count );
    setRenderState( primitiveType, texture );
    if( (m_totalVertices + count) >= m_maxVertices || (m_totalIndices + indexCount) >= m_maxIndices ) {
        flush();
    }

    memcpy( m_vertices + m_totalVertices, vertices, sizeof( sVertex ) * count );
    m_totalVertices += count;
    m_totalIndices  += indexCount;
}

// ** BatchRenderer::setRenderState
void BatchRenderer::setRenderState( ePrimitiveType primType, dcTexture2D texture )
{
    if( m_immediateTexture == texture && m_immediatePrimitive == primType ) {
        return;
    }

    flush();

    m_immediateTexture   = texture;
    m_immediatePrimitive = primType;
}

// ** BatchRenderer::calculateIndexCount
int BatchRenderer::calculateIndexCount( ePrimitiveType primType, int vertexCount ) const
{
    switch( primType ) {
    case renderer::PrimTriangles:        return vertexCount + vertexCount / 4 * 2;
    case renderer::PrimTriangleStrip:    return vertexCount;
    default:                            return 0;
    }
    
    return 0;
}

// ** BatchRenderer::renderOrientedQuad
void BatchRenderer::renderOrientedQuad( dcTexture2D texture, float x, float y, float w, float h, const vec2& up, const vec2& side, const rgba& color )
{
    sVertex vertices[4];

    vec2 s = side * w;
    vec2 u = up   * w;

    SET_VERTEX_UV( vertices[0], x - s.x + u.x, y - s.y + u.y, color, 0, 0 );
    SET_VERTEX_UV( vertices[1], x + s.x + u.x, y + s.y + u.y, color, 0, 1 );
    SET_VERTEX_UV( vertices[2], x + s.x - u.x, y + s.y - u.y, color, 1, 1 );
    SET_VERTEX_UV( vertices[3], x - s.x - u.x, y - s.y - u.y, color, 1, 0 );

    renderVertices( PrimTriangles, texture, vertices, 4 );
}

// ** BatchRenderer::renderOrientedQuadUV
void BatchRenderer::renderOrientedQuadUV( dcTexture2D texture, float x, float y, float w, float h, const vec2& up, const vec2& side, const vec4& uv, const rgba& color /*= White */ )
{
    sVertex vertices[4];

    vec2 s = side * w;
    vec2 u = up   * w;

    SET_VERTEX_UV( vertices[0], x - s.x + u.x, y - s.y + u.y, color, uv.x, uv.w );
    SET_VERTEX_UV( vertices[1], x + s.x + u.x, y + s.y + u.y, color, uv.z, uv.w );
    SET_VERTEX_UV( vertices[2], x + s.x - u.x, y + s.y - u.y, color, uv.z, uv.y );
    SET_VERTEX_UV( vertices[3], x - s.x - u.x, y - s.y - u.y, color, uv.x, uv.y );

    renderVertices( PrimTriangles, texture, vertices, 4 );
}

// ** BatchRenderer::renderLineStrip
void BatchRenderer::renderLineStrip( const void *positions, const void *colors, int stride, int count, float alpha )
{
    const u8 *position = ( u8* )positions;
    const u8 *color    = ( u8* )colors;
    sVertex     vertices[2];

    for( int i = 0; i < count - 1; i++ ) {
        const vec2& pos     = *( vec2* )position;
        const vec2& nextPos = *( vec2* )(position + stride);
        const vec4& clr     = *( vec4* )color;
        const vec4& nextClr = *( vec4* )(color + stride);

        if( (pos - nextPos).GetLength() <= 0.001f ) {
            continue;
        }

        float ta = 1.0f - float(i) / (count - 1);

        SET_VERTEX( vertices[0], pos.x,     pos.y,     rgba( clr.x,     clr.y,     clr.z,     clr.w * ta * alpha ) );
        SET_VERTEX( vertices[1], nextPos.x, nextPos.y, rgba( nextClr.x, nextClr.y, nextClr.z, nextClr.w * ta * alpha ) );

        renderVertices( PrimLines, NULL, vertices, 2 );

        position += stride;
        color    += stride;
    }
}

// ** BatchRenderer::renderThickLineStrip
void BatchRenderer::renderThickLineStrip( dcTexture2D texture, const void *positions, const void *colors, const void *sizes, int stride, int count, float alpha )
{
    const u8 *position = ( u8* )positions + stride;
    const u8 *color    = ( u8* )colors + stride;
    const u8 *size     = ( u8* )sizes + stride;
    sVertex     vertices[4];

    vec2  sidePrev( 0, 0 );
    float sizePrev = 0.0f;

    for( int i = 1; i < count - 1; i++ ) {
        // ** Points
        const vec2& a  = *( vec2* )(position - stride);
        const vec2& b  = *( vec2* )(position + 0);
        const vec2& c  = *( vec2* )(position + stride);
        const vec4& ca = *( vec4* )(color - stride);
        const vec4& cb = *( vec4* )(color + stride);
        float       sa = *( float* )(size - stride);
        float       sb = *( float* )(size + 0);

        float       ta = 1.0f - float(i - 1) / (count - 1);
        float       tb = 1.0f - float(i)     / (count - 1);

        rgba colorA = rgba( ca.x, ca.y, ca.z, ca.w * alpha * ta );
        rgba colorB = rgba( cb.x, cb.y, cb.z, cb.w * alpha * tb );

        // ** Calculate axis vectors
        vec2  up   = b - a;
        float len  = up.Normalize();
        if( len <= 0.001f ) {
            continue;
        }

        vec2 side = vec2( up.y, -up.x );

        vec2 up2   = vec2::Normalized( c - b );
        vec2 side2 = vec2( up2.y, -up2.x );

        side = vec2::Normalized( side2 + side );

        SET_VERTEX_UV( vertices[0], a.x - side.x     * sb + up.x * len, a.y - side.y     * sb + up.y * len, colorB, 0, (1.0f / count) * (i - 1) );
        SET_VERTEX_UV( vertices[1], a.x + side.x     * sa + up.x * len, a.y + side.y     * sa + up.y * len, colorB, 1, (1.0f / count) * (i - 1) );
        SET_VERTEX_UV( vertices[2], a.x + sidePrev.x * sa,              a.y + sidePrev.y * sa,              colorA, 1, (1.0f / count) * i );
        SET_VERTEX_UV( vertices[3], a.x - sidePrev.x * sb,              a.y - sidePrev.y * sb,              colorA, 0, (1.0f / count) * i );

        renderVertices( PrimTriangles, texture, vertices, 4 );

        position += stride;
        color += stride;
        size += stride;

        sidePrev = side;
        sizePrev = sa;
    }
}

// ** BatchRenderer::renderThickLine
void BatchRenderer::renderThickLine( dcTexture2D texture, float x1, float y1, float x2, float y2, float sizeA, float sizeB, const rgba& colorA, const rgba& colorB )
{
    vec2 up   = vec2( x2, y2 ) - vec2( x1, y1 );
    float len = up.Normalize();
    vec2 side = vec2( up.y, -up.x );
    sVertex vertices[4];

    vec2 s1 = side * sizeA;
    vec2 s2 = side * sizeB;
    vec2 u  = up   * len;

    SET_VERTEX_UV( vertices[0], x1 - s1.x + u.x, y1 - s1.y + u.y,  colorB, 0, 0 );
    SET_VERTEX_UV( vertices[1], x1 + s2.x + u.x, y1 + s2.y + u.y,  colorB, 1, 0 );
    SET_VERTEX_UV( vertices[2], x1 + s2.x,       y1 + s2.y,        colorA, 1, 1 );
    SET_VERTEX_UV( vertices[3], x1 - s1.x,       y1 - s1.y,        colorA, 0, 1 );

    renderVertices( PrimTriangles, texture, vertices, 4 );
}

// ** BatchRenderer::renderLineStrip
void BatchRenderer::renderPointArray( dcTexture2D texture, const void *positions, const void *colors, const void *sizes, int stride, int count )
{
    setRenderState( PrimPoints, texture );
    
    const u8 *position = ( u8* )positions;
    const u8 *color    = ( u8* )colors;
    const u8 *size     = ( u8* )sizes;

    for( int i = 0; i < count; i++ ) {
        if( m_totalVertices >= m_maxVertices ) {
            flush();
        }

        const vec2& xy  = *( vec2* )position;
        const vec4& clr = *( vec4* )color;

        sVertex& vertex = m_vertices[m_totalVertices];

        vertex.x = xy.x;
        vertex.y = xy.y;
        vertex.r = clr.x * 255;
        vertex.g = clr.y * 255;
        vertex.b = clr.z * 255;
        vertex.a = clr.w * 255;
        vertex.u = *( float* )size;

        m_totalVertices += 1;
        
        position += stride;
        color    += stride;
        size     += stride;
    }
}

// ** BatchRenderer::renderPoint
void BatchRenderer::renderPoint( dcTexture2D texture, float x, float y, float size, const rgba& color )
{
    sVertex vertex;
    SET_VERTEX( vertex, x, y, color );
    vertex.u = size;

    renderVertices( PrimPoints, texture, &vertex, 1 );
}

// ** BatchRenderer::renderLine
void BatchRenderer::renderLine( float x1, float y1, float x2, float y2, const rgba& colorA, const rgba& colorB )
{
/*
    static const float kDashSize        = 3.0f;
    static const float kSeparatorSize    = 3.0f;

    // ** Render styled line
    switch( m_lineStyle ) {
        case LineDash:    {
            m_lineStyle        = LineSolid;
            cVector2 start    = cVector2( x1, y1 );
            cVector2 dir    = cVector2( x2, y2 ) - start;
            float     length = dir.Normalize();
            float     count    = length / (kDashSize + kSeparatorSize);
            float     extra  = count - floor( count );

            for( int i = 0, n = int( count ); i < n + 1; i++ ) {
                cVector2 end = start + dir * (kDashSize * ((i == n) ? extra : 1.0f));
                RenderLine( start.x, start.y, end.x, end.y, color );
                start = end + dir * kSeparatorSize;
            }

            m_lineStyle        = LineDash;
        }
            return;
        case LineSolid: break;
        default:        NIMBLE_NOT_IMPLEMENTED;
    }
*/
    // ** Render solid line
    sVertex vertices[2];

    SET_VERTEX( vertices[0], x1, y1, colorA );
    SET_VERTEX( vertices[1], x2, y2, colorB );

    renderVertices( renderer::PrimLines, NULL, vertices, 2 );
}

// ** BatchRenderer::renderBounds
void BatchRenderer::renderBounds( const bounds& bounds, const rgba& color )
{
    renderLine( bounds.left,  bounds.top,    bounds.left,  bounds.bottom, color, color );
    renderLine( bounds.right, bounds.top,    bounds.right, bounds.bottom, color, color );
    renderLine( bounds.left,  bounds.top,    bounds.right, bounds.top,    color, color );
    renderLine( bounds.left,  bounds.bottom, bounds.right, bounds.bottom, color, color );
}

// ** BatchRenderer::renderWireRect
void BatchRenderer::renderWireRect( float x, float y, float w, float h, const rgba& color )
{
    renderLine( x,        y,        x + w,    y,        color, color );
    renderLine( x + w,    y,        x + w,    y + h,    color, color );
    renderLine( x + w,    y + h,    x,    y + h,        color, color );
    renderLine( x,    y + h,        x,    y,            color, color );
}

// ** BatchRenderer::renderShape
void BatchRenderer::renderShape( const cMatrix3& T, const stage::cShape *shape, const rgba& fill, const rgba& stroke )
{
    for( int i = 0, n = ( int )shape->GetTotalShapes(); i < n; i++ ) {
        renderShapePart( T, shape->GetShapePart( i ), false, fill, stroke );
    }
}

// ** BatchRenderer::renderShapePart
void BatchRenderer::renderShapePart( const cMatrix3& T, const stage::sShapePart *shape, bool renderVertices, const rgba& fill, const rgba& stroke )
{
    using namespace stage;

    pushTransform( renderer::TransformModel );
    setTransform( renderer::TransformModel, T, true );

    switch( shape->m_type ) {
    case sShapePart::Circle:    renderCircle( shape->m_x, shape->m_y, shape->m_radius1, fill, stroke );
                                break;

    case sShapePart::Polygon:    renderPolygon( shape->m_x, shape->m_y, shape->m_vertices, shape->m_totalVertices, fill, stroke );
                                if( renderVertices ) {
                                    for( int i = 0; i < shape->m_totalVertices; i++ ) {
                                        const cVector2& v = shape->m_vertices[i];
                                        renderWireRect( shape->m_x + v.x - 1, shape->m_y + v.y - 1, 2, 2, stroke );
                                    }
                                }
                                break;

    case sShapePart::Capsule:    renderCapsule( shape->m_x, shape->m_y, shape->m_center1, shape->m_center2, shape->m_radius1, shape->m_radius2 );
                                break;

    case sShapePart::Rect:        renderRect( shape->m_x - shape->m_radius1 * 0.5f, shape->m_y - shape->m_radius2 * 0.5f, shape->m_radius1, shape->m_radius2, fill, stroke );
                                break;
    }

    popTransform( renderer::TransformModel );
}

// ** BatchRenderer::renderPolygon
void BatchRenderer::renderPolygon( float x, float y, const vec2 *v, int vertexCount, const rgba& fill, const rgba& stroke )
{
    const int kVertexCount = 32;

    NIMBLE_BREAK_IF( vertexCount >= kVertexCount - 1 );
    sVertex vertices[kVertexCount];

    // ** Filled
    for( int i = 0; i < vertexCount; i++ ) {
        SET_VERTEX( vertices[i], v[i].x + x, v[i].y + y, fill );
    }
    SET_VERTEX( vertices[vertexCount], v[0].x + x, v[0].y + y, fill );

    renderVertices( renderer::PrimTriangleFan, NULL, vertices, vertexCount );

    // ** Stroke
    for( int i = 0; i <= vertexCount; i++ ) {
        vertices[i].r = stroke.r, vertices[i].g = stroke.g, vertices[i].b = stroke.b, vertices[i].a = stroke.a;
    }
    renderVertices( renderer::PrimLineStrip, NULL, vertices, vertexCount + 1 );
}

// ** BatchRenderer::renderCapsule
void BatchRenderer::renderCapsule( float x, float y, const vec2& c1, const vec2& c2, float r1, float r2, const rgba& fill, const rgba& stroke )
{
}

// ** BatchRenderer::renderRect
void BatchRenderer::renderRect( float x, float y, float w, float h, const rgba& fill, const rgba& stroke )
{
    sVertex vertices[4];

    x = ceil( x );
    y = ceil( y );
    w = ceil( w );
    h = ceil( h );

    SET_VERTEX( vertices[0], x,        y,        fill );
    SET_VERTEX( vertices[1], x + w, y,        fill );
    SET_VERTEX( vertices[2], x + w, y + h,    fill );
    SET_VERTEX( vertices[3], x,        y + h,    fill );

    renderVertices( renderer::PrimTriangles, NULL, vertices, 4 );
    renderWireRect( x, y, w, h, stroke );
}

// ** BatchRenderer::renderCenteredRect
void BatchRenderer::renderCenteredRect( float x, float y, float w, float h, const rgba& fill, const rgba& stroke )
{
    renderRect( x - w / 2, y - h / 2, w, h, fill, stroke );
}

// ** BatchRenderer::renderCircle
void BatchRenderer::renderCircle( float x, float y, float r, const rgba& fill, const rgba& stroke )
{
    const int kVertexCount = 32;
    sVertex vertices[kVertexCount];

    float    step   = 360.0f / (kVertexCount - 1);
    float    angle  = 0.0f;
    
    // ** Filled
    for( int i = 0; i < kVertexCount; i++ ) {
        SET_VERTEX( vertices[i], sinf( Radians( angle ) ) * r + x, cosf( Radians( angle ) ) * r + y, fill );
        angle += step;
    }
    
    renderVertices( renderer::PrimTriangleFan, NULL, vertices, kVertexCount );
    
    // ** Stroke
    for( int i = 0; i < kVertexCount; i++ ) {
        vertices[i].r = stroke.r, vertices[i].g = stroke.g, vertices[i].b = stroke.b, vertices[i].a = stroke.a;
    }
    renderVertices( renderer::PrimLineStrip, NULL, vertices, kVertexCount );
}

} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  #if DEV_DEPRECATED_HAL  */
