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

#ifndef __DC_Platform_WindowsWindow_H__
#define __DC_Platform_WindowsWindow_H__

#include "../Window.h"

#include <windows.h>

DC_BEGIN_DREEMCHEST

namespace platform {

    // ** class WindowsWindow
    class WindowsWindow : public IWindow {
	friend class WindowsApplication;
    public:

                            WindowsWindow( void );
        virtual             ~WindowsWindow( void );

        // ** IWindow
		virtual void		close( void );
        virtual u32         width( void ) const;
        virtual u32         height( void ) const;
		virtual void		mapCursorToWindow( s32& x, s32& y ) const;
        virtual String      caption( void ) const;
        virtual void        setCaption( const String& value );
        virtual void*       handle( void ) const;
        void                setOwner( Window* value );
        Window*             owner( void ) const;

        // ** WindowsWindow
        bool                create( u32 width, u32 height );

	private:

		//! Calculates the window size based on client area size.
		static void			adjustWindowSize( u32 style, u32& width, u32& height );

		//! Window procedure.
		static LRESULT		windowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

		//! Translates a Windows virtual key to Dreemchest key enum.
		static Key			translateKey( u32 key );

    private:

		//! Window registry type
		typedef std::map<HWND, WindowsWindow*>	Windows;

        //! Implementation owner
        Window*             m_owner;

        //! Native window.
        HWND				m_window;

		//! Application instance handle.
		HINSTANCE			m_applicationInstance;

		//! Window class.
		WNDCLASS			m_windowClass;

		//! Window class name.
		String				m_className;

		//! A windows counter to generate a unique class name.
		static u32			s_windowCount;

		//! Window registry.
		static Windows		s_windows;
    };

} // namespace platform

DC_END_DREEMCHEST

#endif /*   !defined( __DC_Platform_iOSWindow_H__ )   */
