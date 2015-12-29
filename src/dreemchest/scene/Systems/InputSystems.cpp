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

#include "InputSystems.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** TouchSystem::TouchSystem
TouchSystem::TouchSystem( const String& name, const Ecs::Aspect& aspect, ViewportWPtr viewport ) : EntitySystem( name, aspect ), m_viewport( viewport )
{
	DC_BREAK_IF( !viewport.valid() );

	m_viewport->subscribe<Viewport::TouchBegan>( dcThisMethod( TouchSystem::handleTouchBegan ) );
	m_viewport->subscribe<Viewport::TouchEnded>( dcThisMethod( TouchSystem::handleTouchEnded ) );
	m_viewport->subscribe<Viewport::TouchMoved>( dcThisMethod( TouchSystem::handleTouchMoved ) );
}

TouchSystem::~TouchSystem( void )
{
	if( m_viewport.valid() ) {
		m_viewport->unsubscribe<Viewport::TouchBegan>( dcThisMethod( TouchSystem::handleTouchBegan ) );
		m_viewport->unsubscribe<Viewport::TouchEnded>( dcThisMethod( TouchSystem::handleTouchEnded ) );
		m_viewport->unsubscribe<Viewport::TouchMoved>( dcThisMethod( TouchSystem::handleTouchMoved ) );
	}
}

// ** TouchSystem::handleTouchBegan
void TouchSystem::handleTouchBegan( const Viewport::TouchBegan& e )
{
	Ecs::EntitySet& entities = m_index->entities();

	for( Ecs::EntitySet::iterator i = entities.begin(), n = entities.end(); i != n; ) {
		Ecs::Entity& entity = *(i++)->get();
		touchBeganEvent( const_cast<Viewport::TouchBegan&>( e ), entity );
	}
}

// ** TouchSystem::handleTouchEnded
void TouchSystem::handleTouchEnded( const Viewport::TouchEnded& e )
{
	Ecs::EntitySet& entities = m_index->entities();

	for( Ecs::EntitySet::iterator i = entities.begin(), n = entities.end(); i != n; ) {
		Ecs::Entity& entity = *(i++)->get();
		touchEndedEvent( const_cast<Viewport::TouchEnded&>( e ), entity );
	}
}

// ** TouchSystem::handleTouchMoved
void TouchSystem::handleTouchMoved( const Viewport::TouchMoved& e )
{
	Ecs::EntitySet& entities = m_index->entities();

	for( Ecs::EntitySet::iterator i = entities.begin(), n = entities.end(); i != n; ) {
		Ecs::Entity& entity = *(i++)->get();
		touchMovedEvent( const_cast<Viewport::TouchMoved&>( e ), entity );
	}
}

} // namespace Scene

DC_END_DREEMCHEST