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

#ifndef     __DC_Platform_Window_H__
#define     __DC_Platform_Window_H__

#include    "Platform.h"

DC_BEGIN_DREEMCHEST

namespace platform {

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

    //! WindiwDelegate is used to handle events raised by windows.
    class WindowDelegate : public RefCounted {
    public:

        virtual                 ~WindowDelegate( void ) {}

        //! Handles window frame update.
        virtual void            handleUpdate( Window* window ) {}

		//! Handles a window close.
		virtual void			handleClosed( Window* window ) {}

        //! Handles mouse pressed event.
        virtual void            handleMouseDown( Window* window, u32 x, u32 y, int touchId = -1 ) {}

        //! Handles mouse released event.
        virtual void            handleMouseUp( Window* window, u32 x, u32 y, int touchId = -1 ) {}

        //! Handles mouse moved event.
        virtual void            handleMouseMove( Window* window, u32 sx, u32 sy, u32 ex, u32 ey, int touchId = -1 ) {}

        //! Handles key pressed event.
        virtual void            handleKeyDown( Window* window, Key key ) {}

        //! Handles key released event.
        virtual void            handleKeyUp( Window* window, Key key ) {}
    };

    //! A platform-specific work with windows.
    class Window {
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

        //! Sets a window event delegate.
        void                    setDelegate( WindowDelegate* delegate );

        //! Returns window width.
        u32                     width( void ) const;

        //! Returns window height.
        u32                     height( void ) const;

        //! Returns a platform-specific window handle.
        /*!
         On Windows it returns a HWND handle.
         On MacOS it returns a NSWindow pointer.
         */
        void*                   handle( void ) const;

        //! Notifies window about a frame update.
        void                    notifyUpdate( void );

        //! Notifies window that mouse was pressed.
        void                    notifyMouseDown( u32 x, u32 y, int touchId = -1 );

        //! Notifies window that mouse was released.
        void                    notifyMouseUp( u32 x, u32 y, int touchId = -1 );

        //! Notifies window that mouse was moved.
        void                    notifyMouseMove( u32 sx, u32 sy, u32 ex, u32 ey, int touchId = -1 );

        //! Notifies window that key was pressed.
        void                    notifyKeyDown( Key key );

        //! Notifies window that key was released.
        void                    notifyKeyUp( Key key );

    private:

                                //! Constructs a new Window instance.
                                Window( IWindow* impl );

    private:

        //! Platform-spefific implementation.
        IWindow*                m_impl;

        //! Event delegate.
        dcWindowDelegateStrong  m_delegate;
    };

} // namespace platform

DC_END_DREEMCHEST

#endif  /*  !defined(__DC_Platform_Window_H__)  */
