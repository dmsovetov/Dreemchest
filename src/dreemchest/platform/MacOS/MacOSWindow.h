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

#ifndef __DC_Platform_MacOSWindow_H__
#define __DC_Platform_MacOSWindow_H__

#include "../Window.h"
#include "CocoaWindow.h"

DC_BEGIN_DREEMCHEST

namespace platform {

    // ** class MacOSWindow
    class MacOSWindow : public IWindow {
    public:

                            MacOSWindow( void );
        virtual             ~MacOSWindow( void );

        // ** IWindow
        virtual void        close( void );
        virtual u32         width( void ) const;
        virtual u32         height( void ) const;
        virtual String      caption( void ) const;
        virtual void        setCaption( const String& value );
        virtual void*       handle( void ) const;
        void                setOwner( Window* value );
        Window*             owner( void ) const;

        // ** MacOSWindow
        bool                create( u32 width, u32 height );

    private:

        //! Implementation owner
        Window*             m_owner;

        //! Window width.
        u32                 m_width;

        //! Window height.
        u32                 m_height;

        //! Window handle.
        CocoaWindow*        m_window;
    };

} // namespace platform

DC_END_DREEMCHEST

#endif /*   !defined( __DC_Platform_MacOSWindow_H__ )   */
