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

#ifndef __DC_Scene_AssetSystem_H__
#define __DC_Scene_AssetSystem_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Asset system ensures that all needed assets are loaded into the memory.
	class AssetSystem : public Ecs::EntitySystem {
	public:

							//! Constructs the AssetSystem instance.
							AssetSystem( AssetBundleWPtr assets );

	private:

		//! Checks if an entity contains any components that contain assets and queues them for loading.
		virtual void		entityAdded( const Ecs::Entity& entity ) DC_DECL_OVERRIDE;

		//! Tracks the asset loading.
		virtual void		update( u32 currentTime, f32 dt ) DC_DECL_OVERRIDE;

		//! Opens asset data stream.
		Io::StreamPtr		openFileStream( AssetWPtr asset ) const;

		//! Queues static mesh assets.
		void				queueStaticMesh( StaticMesh* mesh );

		//! Queues sprite assets.
		void				queueSprite( Sprite* sprite );

		//! Queues mesh asset for loading.
		void				queueMeshAsset( MeshPtr mesh );

		//! Queues material asset for loading.
		void				queueMaterialAsset( MaterialPtr material );

		//! Queues an asset for loading.
		void				queueAsset( AssetWPtr asset );

	private:

		//! Asset loaders factory type.
		typedef AbstractFactory<AssetLoader, AssetFormat>	AssetLoaders;

		AssetBundleWPtr		m_assets;	//!< Parent asset bundle.
		List<AssetWPtr>		m_queue;	//!< Asset loading queue.
		AssetLoaders		m_loaders;	//!< Available asset loaders.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_AssetSystem_H__    */