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

#ifndef __DC_Scene_Mesh_H__
#define __DC_Scene_Mesh_H__

#include "Assets.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Mesh data container.
	class Mesh : public Asset {
	friend class RenderingContext;
	public:

		//! Mesh vertex.
		struct Vertex {
			enum { MaxTexCoords = 2 };
			Vec3				position;			//!< Vertex position.
			Vec3				normal;				//!< Vertex normal.
			Vec2				uv[MaxTexCoords];	//!< Texture coordinates.
		};

		typedef Array<Vertex>	VertexBuffer;		//!< Mesh vertex buffer type.
		typedef Array<u16>		IndexBuffer;		//!< Mesh index buffer type.

								ClassEnableTypeInfoSuper( Mesh, Asset )

								//! Constructs Mesh instance.
								Mesh( void );

		//! Returns mesh bounds.
		const Bounds&			bounds( void ) const;

		//! Returns the total number of mesh chunks.
		s32						chunkCount( void ) const;

		//! Sets the total number of mesh chunks.
		void					setChunkCount( s32 value );

		//! Returns vertex buffer for a specified mesh chunk.
		const VertexBuffer&		vertexBuffer( s32 chunk ) const;

		//! Sets chunk vertex buffer.
		void					setVertexBuffer( s32 chunk, const VertexBuffer& value );

		//! Returns index buffer for a specified mesh chunk.
		const IndexBuffer&		indexBuffer( s32 chunk ) const;

		//! Sets chunk index buffer.
		void					setIndexBuffer( s32 chunk, const IndexBuffer& value );

		//! Updates mesh bounds.
		void					updateBounds( void );

		//! Returns internal rendering id for a mesh node.
		const RenderingAssetId&	chunkId( s32 chunk ) const;

		//! Creates an empty mesh instance.
		static MeshPtr			create( void );

	private:

		//! Sets internal rendering id for a mesh node.
		void					setId( s32 chunk, const RenderingAssetId& value );

	private:

		//! Internal mesh chunk.
		struct Chunk {
			VertexBuffer		vertices;	//!< Mesh node vertices.
			IndexBuffer			indices;	//!< Mesh node indices.
			RenderingAssetId	id;			//!< Internal rendering id.
		};

		Bounds					m_bounds;	//!< Bounding box of a mesh.
		Array<Chunk>			m_chunks;	//!< Mesh chunks.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Mesh_H__    */