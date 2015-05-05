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
#include "Renderer.h"
#include "Component.h"

#include <platform/Input.h>

DC_BEGIN_DREEMCHEST

namespace scene {

IMPLEMENT_LOGGER( log )

// ** Scene::Scene
Scene::Scene( void )
{
	m_camera = CameraPtr( DC_NEW Camera( NULL ) );
}

// ** Scene::setRenderer
void Scene::setRenderer( const RendererPtr& value )
{
	m_renderer = value;
}

// ** Scene::update
void Scene::update( f32 dt )
{
	platform::Input* input = platform::Input::sharedInstance();

	if( !input->keyDown( platform::Key::RButton ) ) {
		return;
	}

	float speed = 0.001f;

	if( input->keyDown( platform::Key::Shift ) ) speed *= 5.0f;
	if( input->keyDown( platform::Key::Space ) ) speed *= 50.0f;

	if( input->keyDown( platform::Key::W ) ) m_camera->move(  speed * dt );
	if( input->keyDown( platform::Key::S ) ) m_camera->move( -speed * dt );

	if( input->keyDown( platform::Key::A ) ) m_camera->strafe( -speed * dt );
	if( input->keyDown( platform::Key::D ) ) m_camera->strafe(  speed * dt );
		
	static s32 prevMouseX = -1;
	static s32 prevMouseY = -1;

	s32 x = input->mouseX();
	s32 y = input->mouseY();

	if( prevMouseX >= 0 && prevMouseY >= 0 ) {
		f32 dx = (x - prevMouseX) * 0.1f;
		f32 dy = (y - prevMouseY) * 0.1f;

		m_camera->pitch( -dy );
		m_camera->yaw( -dx );

		input->setMouse( prevMouseX, prevMouseY );
	} else {
		prevMouseX = x;
		prevMouseY = y;
	}
}

// ** Scene::render
void Scene::render( f32 aspect )
{
	if( m_renderer == RendererPtr() ) {
		log::warn( "Scene::render : no scene renderer set.\n" );
		return;
	}

	m_renderer->render( m_camera->view(), m_camera->proj( aspect ), this );
}

// ** Scene::add
void Scene::add( const SceneObjectPtr& sceneObject )
{
	m_sceneObjects.push_back( sceneObject );
}

// ** Scene::sceneObjects
const SceneObjects& Scene::sceneObjects( void ) const
{
	return m_sceneObjects;
}

// ** Scene::create
ScenePtr Scene::create( void )
{
	return ScenePtr( DC_NEW Scene );
}

} // namespace scene

DC_END_DREEMCHEST