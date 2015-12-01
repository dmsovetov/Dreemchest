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

								ClassEnableTypeInfoSuper( Terrain, Asset )

		//! Returns terrain size.
		u32						size( void ) const;

		//! Returns terrain chunk count.
		u32						chunkCount( void ) const;

		//! Returns terrain height at specified vertex.
		u16						height( u32 x, u32 z ) const;

		//! Returns terrain chunk vertex buffer.
		Array<Vertex>			chunkVertexBuffer( u32 x, u32 z ) const;

		//! Returns terrain chunk index buffer.
		Array<u16>				chunkIndexBuffer( void ) const;

	private:

								//! Constructs Terrain instance.
								Terrain( AssetBundle* bundle, const String& uuid, const String& name, u32 size );

	private:

		u32						m_size;			//!< Terrain size.
		Array<u16>				m_heightmap;	//!< Terrain heightmap.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Terrain_H__    */