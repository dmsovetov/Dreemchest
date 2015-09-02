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

#include "Renderer2D.h"
#include "Hal.h"

#define		SET_VERTEX( vtx, _x, _y, color )	\
                vtx.x = ceilf( _x );			\
				vtx.y = ceilf( _y );			\
                vtx.r = u8( color.r * 255 ), vtx.g = u8( color.g * 255 ), vtx.b = u8( color.b * 255 ), vtx.a = u8( color.a * 255 );

#define		SET_VERTEX_UV( vtx, _x, _y, _color, _u, _v )	\
				SET_VERTEX( vtx, _x, _y, _color )			\
				vtx.u = _u;									\
				vtx.v = _v;

DC_BEGIN_DREEMCHEST

namespace renderer {

// ** Renderer2D::Renderer2D
Renderer2D::Renderer2D( const HalPtr& hal, u32 maxVertexBufferSize ) : m_hal( hal ), m_maxVertexBufferSize( maxVertexBufferSize ), m_maxIndexBufferSize( maxVertexBufferSize * 6 )
{
	// Create shaders
	m_shaders[ShaderTextured] = m_hal->createShader(
		CODE(
			uniform mat4 u_mvp;

			varying vec2 v_tex0;
			varying vec4 v_color;

			void main()
			{
				v_tex0		= gl_MultiTexCoord0.xy;
				v_color		= gl_Color;
				gl_Position = u_mvp * gl_Vertex;
			} ),
		CODE(
			uniform sampler2D u_texture;

			varying vec2	  v_tex0;
			varying vec4	  v_color;

			void main()
			{
                vec4 color   = texture2D( u_texture, v_tex0 );
				gl_FragColor = color * v_color;
			} ) );

	m_shaders[ShaderDefault] = m_hal->createShader(
		CODE(
			uniform mat4 u_mvp;

			varying vec4 v_color;

			void main()
			{
				v_color		= gl_Color;
				gl_Position = u_mvp * gl_Vertex;
			} ),
		CODE(
			varying vec4 v_color;

			void main()
			{
				gl_FragColor = v_color;
			} ) );

	// Create vertex declarations
	VertexDeclarationPtr vertexTextured = m_hal->createVertexDeclaration( "P2:C4:T0" );
	VertexDeclarationPtr vertexColored  = m_hal->createVertexDeclaration( "P2:C4", sizeof( Vertex ) );
	DC_BREAK_IF( vertexColored->vertexSize() != sizeof( Vertex ) );

	// Set vertex formats for primitive types
	m_vertexFormat[PrimPoints]    = vertexColored;
	m_vertexFormat[PrimLines]	  = vertexColored;
	m_vertexFormat[PrimTriangles] = vertexTextured;

	// Create a vertex buffer
	m_vertexBuffer = m_hal->createVertexBuffer( vertexTextured, m_maxVertexBufferSize, false );

	// Create index buffers
	IndexBufferPtr idxQuads		  = m_hal->createIndexBuffer( m_maxIndexBufferSize, false );
	IndexBufferPtr idxConsecutive = m_hal->createIndexBuffer( m_maxIndexBufferSize, false );

	// Fill quad index buffer with indices
	u16* indices = idxQuads->lock();

    for( u32 i = 0; i < m_maxIndexBufferSize / 6; i++ ) {
        indices[i * 6 + 0] = 0 + i * 4;
        indices[i * 6 + 1] = 1 + i * 4;
        indices[i * 6 + 2] = 2 + i * 4;

        indices[i * 6 + 3] = 0 + i * 4;
        indices[i * 6 + 4] = 2 + i * 4;
        indices[i * 6 + 5] = 3 + i * 4;
    }

	idxQuads->unlock();

	// Fill consecutive index buffer
	indices = idxConsecutive->lock();
    
    for( u32 i = 0; i < m_maxIndexBufferSize; i++ ) {
        indices[i] = i;
    }

	idxConsecutive->unlock();

	// Set index buffer for each primitive type
//	m_indexBuffers[PrimPoints]    = idxConsecutive;
	m_indexBuffers[PrimTriangles] = idxQuads;

	// Lock the vertex buffer
	m_vertices = reinterpret_cast<Vertex*>( m_vertexBuffer->lock() );
}

// ** Renderer2D::create
Renderer2DPtr Renderer2D::create( const HalPtr& hal, u32 maxVertexBufferSize )
{
	return Renderer2DPtr( DC_NEW Renderer2D( hal, maxVertexBufferSize ) );
}

// ** Renderer2D::begin
void Renderer2D::begin( const Matrix4& value )
{
	// Set the view-projection matrix
	m_viewProjection = value;

	// Disable the depth test for 2d rendering
	m_hal->setDepthTest( true, Always );

	// Enable alpha blending
	m_hal->setBlendFactors( BlendSrcAlpha, BlendInvSrcAlpha );
}

// ** Renderer2D::end
void Renderer2D::end( void )
{
	// Flush the buffer
	flush();

	// Set the default depth test function
	m_hal->setDepthTest( true, Less );

	// Disable alpha blending
	m_hal->setBlendFactors( BlendDisabled, BlendDisabled );
}

// ** Renderer2D::point
void Renderer2D::point( f32 x, f32 y, const Rgba& color )
{
	Vertex vertex;
	SET_VERTEX( vertex, x, y, color );

	emitVertices( PrimPoints, renderer::Texture2DPtr(), &vertex, 1 );
}

// ** Renderer2D::line
void Renderer2D::line( f32 x1, f32 y1, f32 x2, f32 y2, const Rgba& color )
{
	Vertex vertices[2];

	SET_VERTEX( vertices[0], x1, y1, color );
	SET_VERTEX( vertices[1], x2, y2, color );

	emitVertices( PrimLines, renderer::Texture2DPtr(), vertices, 2 );
}

// ** Renderer2D::orientedQuad
void Renderer2D::orientedQuad( const Texture2DPtr& texture, f32 x, f32 y, f32 w, f32 h, const Vec2& up, const Vec2& side, const Rgba& color )
{
    Vertex vertices[4];

    Vec2 s = side * w;
    Vec2 u = up   * h;

    SET_VERTEX_UV( vertices[0], x - s.x + u.x, y - s.y + u.y, color, 0, 0 );
    SET_VERTEX_UV( vertices[1], x + s.x + u.x, y + s.y + u.y, color, 1, 0 );
    SET_VERTEX_UV( vertices[2], x + s.x - u.x, y + s.y - u.y, color, 1, 1 );
    SET_VERTEX_UV( vertices[3], x - s.x - u.x, y - s.y - u.y, color, 0, 1 );

    emitVertices( PrimTriangles, texture, vertices, 4 );
}

// ** Renderer2D::flush
void Renderer2D::flush( void )
{
    if( m_state.nIndices == 0 && m_state.nVertices == 0 ) {
        return;
    }

	// Unlock the vertex buffer before rendering
	m_vertexBuffer->unlock();

	PrimitiveType primitiveType = m_state.primitiveType;

	DC_BREAK_IF( !m_vertexFormat[primitiveType].valid() )
	DC_BREAK_IF( m_state.nIndices && !m_indexBuffers[primitiveType].valid() )

	// Set the shader
	if( m_state.texture.valid() ) {
		ShaderPtr shader = m_shaders[ShaderTextured];
		m_hal->setShader( shader );
		shader->setInt( shader->findUniformLocation( "u_tex0" ), 0 );
		shader->setMatrix( shader->findUniformLocation( "u_mvp" ), m_viewProjection );
	} else {
		ShaderPtr shader = m_shaders[ShaderDefault];
		m_hal->setShader( shader );
		shader->setMatrix( shader->findUniformLocation( "u_mvp" ), m_viewProjection );
	}

	// Set render states
    m_hal->setTexture( 0, m_state.texture.get() );
    m_hal->setVertexBuffer( m_vertexBuffer, m_vertexFormat[primitiveType] );
	if( m_state.nIndices ) {
		m_hal->renderIndexed( primitiveType, m_indexBuffers[primitiveType], 0, m_state.nIndices );
	} else {
		m_hal->renderPrimitives( primitiveType, 0, m_state.nVertices );
	}
	

	// Lock the vertex buffer again
	m_vertices = reinterpret_cast<Vertex*>( m_vertexBuffer->lock() );
	
	// Reset counters
    m_state.nVertices = 0;
	m_state.nIndices  = 0;
}

// ** Renderer2D::emitVertices
void Renderer2D::emitVertices( PrimitiveType primitiveType, const Texture2DPtr& texture, const Vertex* vertices, u32 count )
{
    u32   indexCount = indexCountForPrimitives( primitiveType, count );
	State state		 = State( primitiveType, texture );

	// Flush the mesh if state was changed
	if( state != m_state ) {
		flush();
		m_state = state;
	}

	// Flush the mesh if buffer capacity is reached
    if( (m_state.nVertices + count) >= m_maxVertexBufferSize || (m_state.nIndices + indexCount) >= m_maxIndexBufferSize ) {
        flush();
    }

	// Copy vertices to a buffer
    memcpy( m_vertices + m_state.nVertices, vertices, sizeof( Vertex ) * count );
    m_state.nVertices += count;
    m_state.nIndices  += indexCount;
}

// ** Renderer2D::indexCountForPrimitives
u32 Renderer2D::indexCountForPrimitives( PrimitiveType primType, u32 vertexCount )
{
    switch( primType ) {
    case renderer::PrimTriangles:		return vertexCount + vertexCount / 4 * 2;
    case renderer::PrimTriangleStrip:	return vertexCount;
    default:                            return 0;
    }
    
    return 0;
}

} // namespace renderer

DC_END_DREEMCHEST