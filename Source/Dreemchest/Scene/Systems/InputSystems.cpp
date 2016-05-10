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

// ** InputSystemBase::InputSystemBase
InputSystemBase::InputSystemBase( Scene& scene, const Ecs::Aspect& cameraAspect, const Ecs::Aspect& entityAspect )
    : m_scene( scene )
{
    m_cameras  = m_scene.ecs()->requestIndex( "InputSystemBase.cameras", cameraAspect );
    m_entities = m_scene.ecs()->requestIndex( "InputSystemBase.entities", entityAspect );
}

// ** InputSystemBase::~InputSystemBase
InputSystemBase::~InputSystemBase( void )
{
}

// ** InputSystemBase::update
void InputSystemBase::update( void )
{
    // Dispatch input events recorded by all camera viewports
    Ecs::EntitySet& cameras = m_cameras->entities();

    for( Ecs::EntitySet::const_iterator i = cameras.begin(), end = cameras.end(); i != end; ++i ) {
        // Get a camera component and associated viewport
        Camera&   camera   = *(*i)->get<Camera>();
        Viewport& viewport = *(*i)->get<Viewport>();

        // Get a camera effective viewport rect
        Rect cameraViewportRect = viewport.denormalize( camera.ndc() );

        // Process each recorded event
        for( s32 j = 0, n = viewport.eventCount(); j < n; j++ ) {
            // Get an event by index
            const InputEvent& e = viewport.eventAt( j );

            // Skip an event if it's outside of an effective viewport rect
            if( !cameraViewportRect.contains( e.touchEvent.x, e.touchEvent.y ) ) {
                continue;
            }

            // Now dispatch this event to all entities
            dispatchEvent( e );
        }

        // Clear all queued events
        viewport.clearEvents();
    }
}

// ** InputSystemBase::dispatchEvent
void InputSystemBase::dispatchEvent( const InputEvent& e )
{
    // Dispatch this event to all active entities
    Ecs::EntitySet& entities = m_entities->entities();

    for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
        switch( e.type ) {
        case InputEvent::TouchBeganEvent:   touchBegan( *i->get(), e.flags, e.touchEvent );
                                            break;
        case InputEvent::TouchMovedEvent:   touchMoved( *i->get(), e.flags, e.touchEvent );
                                            break;
        case InputEvent::TouchEndedEvent:   touchEnded( *i->get(), e.flags, e.touchEvent );
                                            break;
        }
    }
}

} // namespace Scene

DC_END_DREEMCHEST