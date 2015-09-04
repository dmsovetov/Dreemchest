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

namespace Scene {

	DECLARE_LOG( log )

	class Scene;
	//class SceneObject;
	class View;
	class AssetBundle;
	class Asset;
		class Image;
		class Mesh;
		class Material;
//	class Component;
		class Transform;
		class StaticMesh;
		class Sprite;
		class Camera;
		class Rotor;

	// Alias the Ecs::Entity type
	typedef Ecs::Entity				SceneObject;
	typedef StrongPtr<SceneObject>	SceneObjectPtr;
	typedef WeakPtr<SceneObject>	SceneObjectWPtr;

	// Alias the Ecs::Component type
	typedef Ecs::Component			Component;

	dcDeclarePtrs( Direction )

	//dcDeclarePtrs( SceneObject )
	dcDeclarePtrs( Scene )
	dcDeclarePtrs( View )
	dcDeclarePtrs( Transform )
	dcDeclarePtrs( Camera )
	dcDeclarePtrs( RenderPassBase )
	dcDeclarePtrs( ShaderCache )
	dcDeclarePtrs( AssetBundle )
	dcDeclarePtrs( AssetLoader )
	dcDeclarePtrs( Asset )
	dcDeclarePtrs( AssetData )
	dcDeclarePtrs( Image )
	dcDeclarePtrs( Mesh )
	dcDeclarePtrs( Material )

	dcDeclarePtrs( AssetTexture )
	dcDeclarePtrs( AssetMesh )

	dcDeclarePtrs( Box2DPhysics )
	dcDeclarePtrs( RigidBody2D )

	//! Container type to store scene objects.
	typedef List<SceneObjectPtr> SceneObjectsList;

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
		Ecs::Systems&					systems( void );

		//! Returns the scene rendering systems.
		Ecs::Systems&					renderingSystems( void );

		//! Creates a new scene.
		static ScenePtr					create( void );

	private:

										//! Constructs a Scene instance.
										Scene( void );

	private:

		//! Scene entities container.
		Ecs::Entities					m_entities;

		//! Entity update systems.
		Ecs::Systems					m_systems;

		//! Entity rendering systems.
		Ecs::Systems					m_renderingSystems;

		//! All cameras that reside in scene.
		Ecs::FamilyPtr					m_cameras;

		//! Next scene object id.
		Ecs::EntityId					m_nextEntityId;
	};

} // namespace Scene

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "Components/Rendering.h"
	#include "Components/Transform.h"
	#include "Components/Physics.h"
	#include "Assets/Mesh.h"
	#include "Assets/Material.h"
	#include "Assets/Image.h"
	#include "Systems/TransformSystems.h"
	#include "Systems/Physics2D.h"
	#include "Systems/Input2DSystems.h"
	#include "Systems/AssetSystem.h"

	#include "Systems/Rendering/ShaderCache.h"
	#include "Systems/Rendering/Renderers.h"
#endif

#endif    /*    !__DC_Scene_H__    */