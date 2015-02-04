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

    // ** class IWindow
    //! A platform-specific window implementation interface.
    class IWindow {
    public:

        virtual             ~IWindow( void ) {}

        //! Returns window width.
        virtual u32         width( void ) const         = 0;

        //! Returns window height.
        virtual u32         height( void ) const        = 0;

        //! Returns a window caption.
        virtual String      caption( void ) const       = 0;

        //! Sets a window caption.
        virtual void        setCaption( const String& value )   = 0;
    };

    // ** class Window
    //! A platform-specific work with windows.
    class Window {
    public:

        virtual             ~Window( void );

        //! Creates and shows a new Window instance.
        static Window*      create( u32 width, u32 height );

        //! Returns a window caption.
        String              caption( void ) const;
        
        //! Sets a window caption.
        void                setCaption( const String& value );

    private:

                            //! Constructs a new Window instance.
                            Window( IWindow* impl );

    private:

        IWindow*            m_impl;
    };

} // namespace platform

DC_END_DREEMCHEST

#endif  /*  !defined(__DC_Platform_Window_H__)  */
