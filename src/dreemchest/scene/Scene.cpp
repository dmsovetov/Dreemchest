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

#include "Scene.h"
#include "SceneObject.h"

#include "Components/Rendering.h"

DC_BEGIN_DREEMCHEST

namespace scene {

IMPLEMENT_LOGGER( log )

// ** Scene::Scene
Scene::Scene( void ) : m_systems( m_entities ), m_renderingSystems( m_entities ), m_nextEntityId( 1 )
{
	m_cameras = Ecs::Family::create( m_entities, "Cameras", Ecs::Aspect::all<Camera>() );
}

// ** Scene::update
void Scene::update( f32 dt )
{
	m_entities.notify();
	m_systems.update( 0, dt );
}

// ** Scene::createSceneObject
SceneObjectPtr Scene::createSceneObject( void )
{
	SceneObjectPtr sceneObject( DC_NEW SceneObject( this, m_entities, m_nextEntityId++ ) );

	if( !m_entities.addEntity( sceneObject ) ) {
		return SceneObjectPtr();
	}

	return sceneObject;
}

// ** Scene::render
void Scene::render( void )
{
	// Update all rendering systems
	m_renderingSystems.update( 0, 0.0f );

	// Now reset internal Camera::ClearDisabled flag for all cameras in scene
	const Ecs::EntitySet& cameras = m_cameras->entities();

	for( Ecs::EntitySet::const_iterator i = cameras.begin(), end = cameras.end(); i != end; i++ ) {
		Camera* camera = (*i)->get<Camera>();
		camera->setClearMask( camera->clearMask() & ~Camera::ClearDisabled );
	}
}

// ** Scene::systems
Ecs::Systems& Scene::systems( void )
{
	return m_systems;
}

// ** Scene::renderingSystems
Ecs::Systems& Scene::renderingSystems( void )
{
	return m_renderingSystems;
}

// ** Scene::create
ScenePtr Scene::create( void )
{
	return ScenePtr( DC_NEW Scene );
}

} // namespace scene

DC_END_DREEMCHEST