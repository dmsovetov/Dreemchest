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

#include "EmscriptenWindow.h"

DC_BEGIN_DREEMCHEST

namespace Platform
{

// ** createWindow
IWindow* createWindow( u32 width, u32 height )
{
    EmscriptenWindow* window = DC_NEW EmscriptenWindow;

    //if( window->create( width, height ) ) {
    //    return window;
    //}

    delete window;
    return NULL;
}

// ** EmscriptenWindow::EmscriptenWindow
EmscriptenWindow::EmscriptenWindow( void )
{

}

// ** EmscriptenWindow::~EmscriptenWindow
EmscriptenWindow::~EmscriptenWindow( void )
{

}

// ** EmscriptenWindow::close
void EmscriptenWindow::close( void )
{

}

// ** EmscriptenWindow::setOwner
void EmscriptenWindow::setOwner( Window* value )
{
}

// ** EmscriptenWindow::owner
Window* EmscriptenWindow::owner( void ) const
{
    return NULL;
}

// ** EmscriptenWindow::width
u32 EmscriptenWindow::width( void ) const
{
    return 0;
}

// ** EmscriptenWindow::height
u32 EmscriptenWindow::height( void ) const
{
    return 0;
}

// ** EmscriptenWindow::mapCursorToWindow
void EmscriptenWindow::mapCursorToWindow( s32& x, s32& y ) const
{
}

// ** EmscriptenWindow::setCaption
void EmscriptenWindow::setCaption( const String& value )
{
}

// ** EmscriptenWindow::caption
String EmscriptenWindow::caption( void ) const
{
	return "";
}

// ** EmscriptenWindow::handle
void* EmscriptenWindow::handle( void ) const
{
    return NULL;
}

} // namespace Platform

DC_END_DREEMCHEST
