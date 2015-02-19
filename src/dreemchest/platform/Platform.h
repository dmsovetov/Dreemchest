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

#ifndef __DC_Platform_Platform_H__
#define __DC_Platform_Platform_H__

#ifndef DC_UTILS_INCLUDED
    #include <utils/Utils.h>
#endif

DC_BEGIN_DREEMCHEST

namespace platform {

    DECLARE_LOG( log )

    // ** enum Key
    //! Available key mappings.
    struct Key {
		enum Mapping {
			_0, _1, _2, _3, _4, _5, _6, _7, _8, _9,
			Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
			A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
			F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
			Escape, Tab, Capital, Shift, Control, Win, Alt, Space,
			Return, Back,
			Pause, Snapshot, Scroll,
			PgDown, PgUp, Home, End, Insert, Delete,
			Up, Down, Left, Right,
			Numlock, Divide, Multiply, Subtract, Add, Decimal,
			RButton, LButton, MButton,

			Total
		} m_mapping;

		Key( Mapping mapping ) : m_mapping( mapping ) {}
    };

    class Application;
    class Window;
    class WindowDelegate;

} // namespace platform

    DC_DECLARE_PTRS( platform::WindowDelegate, WindowDelegate )

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
    #include "Window.h"
    #include "Application.h"
#endif

#endif  /*  !defined( __DC_Platform_Platform_H__ )  */
