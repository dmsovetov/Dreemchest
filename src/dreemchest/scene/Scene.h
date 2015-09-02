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

#ifndef __DC_Scene_H__
#define __DC_Scene_H__

#include "../Dreemchest.h"

#ifndef FOO_INCLUDED
    #include <Foo/Foo.h>
#endif

#include <platform/Platform.h>
#include <platform/Input.h>
#include <platform/Window.h>

#include <renderer/Renderer.h>
#include <renderer/Hal.h>
#include <renderer/Renderer2D.h>

#include <Ecs/Entity.h>
#include <Ecs/Component.h>
#include <Ecs/EntitySystem.h>

#include <Io/DiskFileSystem.h>

DC_BEGIN_DREEMCHEST

namespace scene {

	DECLARE_LOG( log )

	class Scene;
	class SceneObject;
	class View;
	class AssetBundle;
	class Asset;
		class Image;
		class Mesh;
		class Material;
	class Component;
		class Transform;
		class StaticMesh;
		class Sprite;
		class Camera;
		class Rotator2D;

	typedef StrongPtr<Scene>			ScenePtr;
	typedef WeakPtr<Scene>				SceneWPtr;
	typedef StrongPtr<AssetBundle>		AssetBundlePtr;
	typedef WeakPtr<AssetBundle>		AssetBundleWPtr;
	typedef StrongPtr<SceneObject>		SceneObjectPtr;
	typedef WeakPtr<SceneObject>		SceneObjectWPtr;

	dcDeclarePtrs( View )
	dcDeclarePtrs( Transform )
	dcDeclarePtrs( Camera )
	dcDeclarePtrs( RenderPassBase )
	dcDeclarePtrs( Asset )
	dcDeclarePtrs( Image )
	dcDeclarePtrs( Mesh )
	dcDeclarePtrs( Material )

	//! Supported image formats.
	enum ImageFormat {
		  ImageRaw = 0
		, TotalImageFormats
	};

	//! The root class for a scene subsystem.
	class Scene : public RefCounted {
	public:

		//! Performs a scene update.
		void							update( f32 dt );

		//! Renders a scene.
		void							render( void );

		//! Creates a new scene object instance.
		SceneObjectPtr					createSceneObject( void );

		//! Returns the scene systems.
		ecs::Systems&					systems( void );

		//! Returns the scene rendering systems.
		ecs::Systems&					renderingSystems( void );

		//! Creates a new scene.
		static ScenePtr					create( void );

	private:

										//! Constructs a Scene instance.
										Scene( void );

	private:

		//! Scene entities container.
		ecs::Entities					m_entities;

		//! Entity update systems.
		ecs::Systems					m_systems;

		//! Entity rendering systems.
		ecs::Systems					m_renderingSystems;

		//! All cameras that reside in scene.
		ecs::FamilyPtr					m_cameras;

		//! Next scene object id.
		ecs::EntityId					m_nextEntityId;
	};

} // namespace scene

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "SceneObject.h"
	#include "Components/Rendering.h"
	#include "Components/Transform.h"
	#include "Components/Physics.h"
	#include "Systems/TransformSystems.h"
	#include "Systems/Physics2D.h"
	#include "Systems/UnlitMeshRenderer.h"
	#include "Systems/SpriteRenderer.h"
	#include "Systems/DebugSpriteTransformRenderer.h"
	#include "Systems/Input2DSystems.h"
	#include "Systems/RenderSystem.h"
	#include "Systems/AssetSystem.h"
	#include "Assets/Mesh.h"
	#include "Assets/Material.h"
	#include "Assets/Image.h"
#endif

#endif    /*    !__DC_Scene_H__    */