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

#ifndef __DC_Platform_Window_H__
#define __DC_Platform_Window_H__

#include "Platform.h"

DC_BEGIN_DREEMCHEST

namespace Platform {

    //! A platform-specific window implementation interface.
    class IWindow {
    public:

        virtual                 ~IWindow( void ) {}

		//! Closes a window.
		virtual void			close( void )				= 0;

        //! Returns window width.
        virtual u32             width( void ) const         = 0;

        //! Returns window height.
        virtual u32             height( void ) const        = 0;

		//! Converts the global mouse coordinates to a local ones.
		virtual void			mapCursorToWindow( s32& x, s32& y ) const = 0;

        //! Returns a window caption.
        virtual String          caption( void ) const       = 0;

        //! Sets a window caption.
        virtual void            setCaption( const String& value )   = 0;

        //! Returns a platform-specific window handle.
        virtual void*           handle( void ) const        = 0;

        //! Sets window implemetation owner.
        virtual void            setOwner( Window* value )   = 0;

        //! Returns a window implementation owner.
        virtual Window*         owner( void ) const         = 0;
    };

    //! A platform-specific work with windows.
    class Window : public InjectEventEmitter<RefCounted> {
    public:

        virtual                 ~Window( void );

        //! Creates and shows a new Window instance.
        static Window*          create( u32 width, u32 height );

		//! Closes a window and releases a memory.
		void					release( void );

        //! Returns a window caption.
        String                  caption( void ) const;
        
        //! Sets a window caption.
        void                    setCaption( const String& value );

        //! Returns window width.
        u32                     width( void ) const;

        //! Returns window height.
        u32                     height( void ) const;

		//! Returns an aspect ratio of a window.
		f32						aspectRatio( void ) const;

		//! Converts the global mouse coordinates to a local ones.
		void					mapCursorToWindow( s32& x, s32& y ) const;


        //! Returns a platform-specific window handle.
        /*!
         On Windows it returns a HWND handle.
         On MacOS it returns a NSWindow pointer.
         */
        void*                   handle( void ) const;

        //! Notifies window about a frame update.
        void                    notifyUpdate( void );

        //! Notifies window that mouse was pressed.
        void                    notifyMouseDown( u32 x, u32 y, s32 touchId = -1 );

        //! Notifies window that mouse was released.
        void                    notifyMouseUp( u32 x, u32 y, s32 touchId = -1 );

        //! Notifies window that mouse was moved.
        void                    notifyMouseMove( u32 sx, u32 sy, u32 ex, u32 ey, s32 touchId = -1 );

        //! Notifies window that key was pressed.
        void                    notifyKeyDown( Key key );

        //! Notifies window that key was released.
        void                    notifyKeyUp( Key key );

		//! Base class for all window events.
		struct Event {
						//! Constructs Event instance.
						Event( WindowWPtr window )
							: window( window ) {}
			WindowWPtr	window;	//!< Weak pointer to a window that emitted this event.
		};

		//! This event is emitted each frame.
		struct Update : public Event {
						//! Constructs Update instance.
						Update( WindowWPtr window )
							: Event( window ) {}
		};

		//! Base class for all touch events.
		struct TouchEvent : public Event {
						//! Constructs TouchEvent instance.
						TouchEvent( WindowWPtr window, s32 x, s32 y, s32 id = -1 )
							: Event( window ), x( x ), y( y ), id( id ) {}

			s32			x;	//!< Touch X window coordinate.
			s32			y;	//!< Touch Y window coordinate.
			s32			id;	//!< Touch identifier.
		};

		//! This event is emitted when user touches the window.
		struct TouchBegan : public TouchEvent {
						//! Constructs TouchBegan instance.
						TouchBegan( WindowWPtr window, s32 x, s32 y, s32 id = -1 )
							: TouchEvent( window, x, y, id ) {}
		};

		//! This event is emitted when user's touch was ended.
		struct TouchEnded : public TouchEvent {
						//! Constructs TouchEnded instance.
						TouchEnded( WindowWPtr window, s32 x, s32 y, s32 id = -1 )
							: TouchEvent( window, x, y, id ) {}
		};

		//! This event is emitted when user's touch was moved.
		struct TouchMoved : public TouchEvent {
						//! Constructs TouchMoved instance.
						TouchMoved( WindowWPtr window, s32 x, s32 y, s32 id = -1 )
							: TouchEvent( window, x, y, id ) {}
		};

	#if defined( DC_PLATFORM_WINDOWS ) || defined( DC_PLATFORM_MACOS )
		//! This event is emitted when user presses the key.
		struct KeyPressed : public Event {
						//! Constructs KeyPressed instance.
						KeyPressed( WindowWPtr window, Key key )
							: Event( window ), key( key ) {}

			Key			key;	//!< Pressed key.	
		};

		//! This event is emitted when user releases the key.
		struct KeyReleased : public Event {
						//! Constructs KeyReleased instance.
						KeyReleased( WindowWPtr window, Key key )
							: Event( window ), key( key ) {}

			Key			key;	//!< Released key.	
		};
	#endif	/*	DC_PLATFORM_WINDOWS || DC_PLATFORM_MACOS	*/

    private:

                                //! Constructs a new Window instance.
                                Window( IWindow* impl );

    private:

        //! Platform-spefific implementation.
        IWindow*                m_impl;
    };

} // namespace Platform

DC_END_DREEMCHEST

#endif  /*  !defined(__DC_Platform_Window_H__)  */
