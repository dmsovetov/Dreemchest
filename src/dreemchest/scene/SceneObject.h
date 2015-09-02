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

#ifndef __DC_Scene_SceneObject_H__
#define __DC_Scene_SceneObject_H__

#include "Scene.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! Scene object class is a component holder.
	class SceneObject : public Ecs::Entity {
	EmbedUserData
	friend class Scene;
	public:

								OverrideEntity( SceneObject, Entity )

		//! Returns the parent scene
		const SceneWPtr&		scene( void ) const;

	private:

								//! Constructs a SceneObject instance.
								SceneObject( Scene* scene, Ecs::Entities& entities, const Ecs::EntityId& id = Ecs::EntityId() );

	private:

		//! Parent scene
		SceneWPtr				m_scene;
	};

	// ** SceneObject::SceneObject
	inline SceneObject::SceneObject( Scene* scene, Ecs::Entities& entities, const Ecs::EntityId& id ) : Entity( entities, id ), m_scene( scene )
	{
	}

	// ** SceneObject::scene
	inline const SceneWPtr& SceneObject::scene( void ) const
	{
		return m_scene;
	}

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_SceneObject_H__    */