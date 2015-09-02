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

#include "SceneSystem.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! Asset system ensures that all needed assets are loaded into the memory.
	class AssetSystem : public Ecs::EntitySystem {
	public:

							//! Constructs the AssetSystem instance.
							AssetSystem( Ecs::Entities& entities, renderer::HalPtr hal );

	private:

		//! Checks if an entity contains any components that contain assets and queues them for loading.
		virtual void		entityAdded( const Ecs::EntityPtr& entity );

		//! Tracks the asset loading.
		virtual void		update( u32 currentTime, f32 dt );

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

		renderer::HalPtr	m_hal;		//! Rendering HAL.
		List<AssetWPtr>		m_queue;	//!< Asset loading queue.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_AssetSystem_H__    */