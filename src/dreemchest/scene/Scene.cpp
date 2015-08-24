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
#include "Systems/SpriteRenderer.h"
#include "Systems/TransformSystems.h"
#include "Components/Camera.h"

#include <json/json.h>

DC_BEGIN_DREEMCHEST

namespace scene {

IMPLEMENT_LOGGER( log )

// ** Scene::Scene
Scene::Scene( void ) : m_systems( m_entities ), m_nextEntityId( 1 )
{
	m_systems.add<Rotator2DSystem>();
}

// ** Scene::update
void Scene::update( f32 dt )
{
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

// ** Scene::setRenderer
void Scene::setRenderer( renderer::Hal* hal, RendererType renderer )
{
	switch( renderer ) {
	case RendererDefault:	m_renderer = new SpriteRenderer( m_entities, hal );
							break;
	}
}

// ** Scene::render
void Scene::render( const BasicCamera* camera )
{
	if( m_renderer == SpriteRendererPtr() ) {
		log::warn( "Scene::render : no scene renderer set.\n" );
		return;
	}

	m_renderer->setViewProjection( camera->viewProj() );
	m_renderer->update( 0, 0 );
}

// ** Scene::create
ScenePtr Scene::create( void )
{
	return ScenePtr( DC_NEW Scene );
}

} // namespace scene

DC_END_DREEMCHEST