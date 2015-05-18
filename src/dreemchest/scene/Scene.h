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

#include <renderer/Renderer.h>
#include <renderer/Hal.h>

DC_BEGIN_DREEMCHEST

namespace scene {

	DECLARE_LOG( log )

	class Renderer;
	class Mesh;
	class Material;
	class SceneObject;
	class Component;
		class Transform;
		class MeshRenderer;
		class Camera;

	typedef StrongPtr<class Scene>		ScenePtr;
	typedef StrongPtr<SceneObject>		SceneObjectPtr;
	typedef WeakPtr<SceneObject>		SceneObjectWPtr;
	typedef StrongPtr<Component>		ComponentPtr;
	typedef StrongPtr<Transform>		TransformPtr;
	typedef WeakPtr<Transform>			TransformWPtr;
	typedef StrongPtr<Camera>			CameraPtr;
	typedef WeakPtr<Camera>				CameraWPtr;
	typedef StrongPtr<MeshRenderer>		MeshRendererPtr;
	typedef StrongPtr<Mesh>				MeshPtr;
	typedef StrongPtr<Material>			MaterialPtr;
	typedef StrongPtr<Renderer>			RendererPtr;

	//! Container type to store a list of objects.
	typedef List<SceneObjectPtr>	SceneObjects;

	//! The root class for a scene subsystem.
	class Scene : public RefCounted {
	public:

		//! Adds a new scene object to scene.
		void							add( const SceneObjectPtr& sceneObject );

		//! Performs a scene update.
		void							update( f32 dt );

		//! Renders a scene.
		void							render( f32 aspect );

		//! Sets a scene renderer.
		void							setRenderer( const RendererPtr& value );

        //! Returns active camera.
        const CameraPtr&                camera( void ) const;

		//! Returns a scene object list.
		const SceneObjects&				sceneObjects( void ) const;

		//! Creates a new scene.
		static ScenePtr					create( void );

	private:

										//! Constructs a Scene instance.
										Scene( void );

	private:

		//! Scene object list.
		SceneObjects					m_sceneObjects;

		//! Main scene camera.
		CameraPtr						m_camera;

		//! Default scene renderer.
		RendererPtr						m_renderer;
	};

} // namespace scene

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "SceneObject.h"
	#include "Component.h"
	#include "Mesh.h"
	#include "Renderer.h"
	#include "Material.h"
#endif

#endif    /*    !__DC_Scene_H__    */