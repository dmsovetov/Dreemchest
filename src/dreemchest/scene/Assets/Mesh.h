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

namespace scene {

	//! Mesh data container.
	class Mesh : public Asset {
	friend class AssetBundle;
	public:

								ClassEnableTypeInfoSuper( Mesh, Asset )

		//! Mesh submesh type.
		struct Chunk {
			Renderer::VertexBufferPtr	m_vertexBuffer;	//!< Hardware vertex buffer for this chunk.
			Renderer::IndexBufferPtr	m_indexBuffer;	//!< Hardware index buffer for this chunk.

			//! Returns true if this chunk is valid.
			operator bool( void ) const { return m_vertexBuffer != Renderer::VertexBufferPtr() && m_indexBuffer != Renderer::IndexBufferPtr(); }
		};

		//! Adds a new mesh chunk.
		void					addChunk( const Renderer::VertexBufferPtr& vertexBuffer, const Renderer::IndexBufferPtr& indexBuffer );

		//! Returns the total number of chunks.
		u32						chunkCount( void ) const;

		//! Returns a chunk by index.
		const Chunk&			chunk( u32 index ) const;

		//! Creates a new Mesh instance.
		static MeshPtr			create( const String& name );

		//! Creates a new rectangular Mesh instance.
		static MeshPtr			createRectangular( const String& name, Renderer::Hal* hal, f32 width, f32 height );

	private:

								//! Constructs Mesh instance.
								Mesh( AssetBundle* bundle, const String& name );

	private:

		//! Container type to store mesh chunks.
		typedef Array<Chunk>	Chunks;

		Chunks					m_chunks; //!< Mesh chunks.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Mesh_H__    */