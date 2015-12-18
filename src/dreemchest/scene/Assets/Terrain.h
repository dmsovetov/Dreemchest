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

#ifndef __DC_Scene_Terrain_H__
#define __DC_Scene_Terrain_H__

#include "Assets.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Heightmap wrapping class.
	class Heightmap {
	public:

		typedef u16				Type;	//!< Single heightmap pixel type.
		typedef Array<Type>		Buffer;	//!< Heightmap buffer.

		//! Base class to declare custom heightmap generators.
		class Generator : public RefCounted {
		public:

			//! Calculates the height at specified point.
			virtual Type		calculate( u32 x, u32 z ) = 0;
		};

								//! Constructs the Heightmap instance.
								Heightmap( u32 size );

		//! Returns heightmap size.
		u32						size( void ) const;

		//! Returns height at specified coordinate.
		Type					height( u32 x, u32 z ) const;

		//! Sets height at specified coordinate.
		void					setHeight( u32 x, u32 z, Type value );

		//! Constructs the heightmap from a callback.
		void					set( StrongPtr<Generator> generator );

		//! Fills the heightmap with a constant height value.
		class ConstantHeight : public Generator {
		public:

								//! Constructs ConstantHeight instance.
								ConstantHeight( Type value = 0 )
									: m_value( value ) {}

			//! Returns the constant terrain height.
			virtual Type		calculate( u32 x, u32 z ) DC_DECL_OVERRIDE { return m_value; }

		private:

			Type				m_value;	//!< Constant height value.
		};

		//! Fills the heightmap with a noise.
		class Noise : public Generator {
		public:

								//! Constructs Noise instance.
								Noise( Type min = 0, Type max = ~0 )
									: m_min( min ), m_max( max ) {}

			//! Returns the random terrain height.
			virtual Type		calculate( u32 x, u32 z ) DC_DECL_OVERRIDE { return randomValue( m_min, m_max ); }

		private:

			Type				m_min;		//!< Minimum height value.
			Type				m_max;		//!< Maximum height value.
		};

	private:

		u32						m_size;		//!< Heightmap size.
		Buffer					m_buffer;	//!< Actual heightmap buffer.
	};

	//! Heightmap base terrain.
	class Terrain : public Asset {
	friend class AssetBundle;
	public:

		static s32				kChunkSize;	//!< Single terrain chunk size.
		static s32				kMaxSize;	//!< The maximum terrain size.

		//! Terrain vertex struct.
		struct Vertex {
			Vec3				position;	//!< Vertex position.
			Vec3				normal;		//!< Vertex normal.
			Vec2				uv;			//!< Vertex UV coordinate.
		};

		typedef Array<Vertex>	VertexBuffer;	//!< Terrain vertex buffer type.
		typedef Array<u16>		IndexBuffer;	//!< Terrain index buffer type.

								ClassEnableTypeInfoSuper( Terrain, Asset )

								//! Constructs Terrain instance.
								Terrain( AssetBundle* bundle = NULL, const String& uuid = String(), const String& name = String(), u32 size = 0 );

		//! Returns terrain size.
		u32						size( void ) const;

		//! Returns terrain chunk count.
		u32						chunkCount( void ) const;

		//! Returns terrain chunk vertex buffer.
		VertexBuffer			chunkVertexBuffer( u32 x, u32 z ) const;

		//! Returns terrain chunk index buffer.
		IndexBuffer				chunkIndexBuffer( void ) const;

		//! Creates the chunk mesh.
		MeshPtr					createChunkMesh( u32 x, u32 z ) const;

		//! Creates the mesh for whole terrain.
		MeshPtr					createMesh( void ) const;

	private:

		//! Adds the mesh node constructed from chunk buffers.
		void					setMeshChunk( MeshWPtr mesh, u32 chunk, const VertexBuffer& vertices, const IndexBuffer& indices, u32 x = 0, u32 z = 0 ) const;

	private:

		Heightmap				m_heightmap;	//!< Terrain heightmap.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Terrain_H__    */