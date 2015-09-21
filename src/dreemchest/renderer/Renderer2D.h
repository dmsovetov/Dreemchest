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

#ifndef __DC_Renderer_Renderer2D_H__
#define __DC_Renderer_Renderer2D_H__

#include "Renderer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer {

	//! Renderer2D is used for displaying 2D vector graphics.
	class Renderer2D : public RefCounted {
	public:

		//! Render vertex.
		struct Vertex {
			f32					x, y, z;		//!< Vertex coordinate.
			u8					r, g, b, a;		//!< Vertex color.
			f32					u, v;			//!< Vertex texture coordinates.

								//! Construct Vertex instance.
								Vertex( void )
									: x( 0.0f ), y( 0.0f ), z( 0.0f ) {}
		};

		//! Begins the 2D rendering.
		void					begin( const Matrix4& value, Compare depthTest = Always, BlendFactor src = BlendSrcAlpha, BlendFactor dst = BlendInvSrcAlpha );

		void					end( void );

		//! Renders a fullscreen quad.
		void					fullscreenQuad( const Texture2DPtr& texture, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

		//! Renders an oriented quad.
		void                    orientedQuad( const Texture2DPtr& texture, f32 x, f32 y, f32 w, f32 h, const Vec2& up, const Vec2& side, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

		//! Renders a single point.
		void					point( f32 x, f32 y, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

		//! Renders the line segment.
		void					line( f32 x1, f32 y1, f32 x2, f32 y2, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

		//! Renders the line segment in 3D space.
		void					line( const Vec3& start, const Vec3& end, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

		//! Renders bounding box in 3D space.
		void					wireBox( const Bounds& bounds, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

		//! Renders a wire sphere in 3D space.
		void					wireSphere( const Vec3& center, f32 radius, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

		//! Creates a new instance of Renderer2D.
		static Renderer2DPtr	create( const HalPtr& hal, u32 maxVertexBufferSize = 1024 );

	private:

								//! Constructs the Renderer2D instance.
								Renderer2D( const HalPtr& hal, u32 maxVertexBufferSize );

		//! Renders the accumulated mesh vertices and resets the vertex buffer.
		void					flush( void );

		//! Pushes a new set of vertices to a buffer.
		void					emitVertices( PrimitiveType primitiveType, const Texture2DPtr& texture, const Vertex* vertices, u32 count );

		//! Returns the total number of indices used for rendering a primitive set.
		static u32				indexCountForPrimitives( PrimitiveType primitiveType, u32 count );

	private:

		//! Available shader types.
		enum ShaderType {
			ShaderDefault,		//!< Renders primitives with just a color.
			ShaderTextured,		//!< Renders primitives with color & texture.
			TotalShaderTypes,	//!< Total shader types.
		};

		//! Current render state.
		struct State {
			PrimitiveType		primitiveType;		//!< Active primitive type.
			Texture2DPtr		texture;			//!< Active texture.
			u32					nVertices;			//!< Total number of emitted vertices.
			u32					nIndices;			//!< Total number of emitted indices.

								//! Constructs the State instance.
								State( void )
									: primitiveType( TotalPrimitiveTypes ), nVertices( 0 ), nIndices( 0 ) {}

								//! Constructs the State instance.
								State( PrimitiveType primitiveType, const Texture2DPtr& texture )
									: primitiveType( primitiveType ), texture( texture ), nVertices( 0 ), nIndices( 0 ) {}

			//! Compares two states.
			bool				operator != ( const State& other ) const { return primitiveType != other.primitiveType || texture != other.texture; }
		};

		HalPtr					m_hal;									//!< Rendering HAL.
		Matrix4					m_viewProjection;						//!< The view-projection matrix.
		VertexBufferPtr			m_vertexBuffer;							//!< This vertex buffer stores a generated 2D mesh.
		Vertex*					m_vertices;								//!< Locked vertex buffer.
		IndexBufferPtr			m_indexBuffers[TotalPrimitiveTypes];	//!< Index buffers for each primitive type.
		VertexDeclarationPtr	m_vertexFormat[TotalPrimitiveTypes];	//!< Available vertex formats.
		ShaderPtr				m_shaders[TotalShaderTypes];			//!< Loaded shaders.
		State					m_state;								//!< Current renderer state.
		Stack<Matrix4>			m_transform;							//!< Transform matrix stack.
		u32						m_maxVertexBufferSize;					//!< The maximum size of a vertex buffer.
		u32						m_maxIndexBufferSize;					//!< The maximum size of an index buffer.
	};

} // namespace Renderer

DC_END_DREEMCHEST

#endif /*	!__DC_Renderer_Renderer2D_H__	*/