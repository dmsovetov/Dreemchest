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

#include "Input.h"

DC_BEGIN_DREEMCHEST

namespace platform {

//! Platform-specific keyboard constructor.
extern IKeyboard* createKeyboard( void );

// ** Keyboard::s_keyboard
Keyboard* Keyboard::s_keyboard = NULL;

// ** Keyboard::Keyboard
Keyboard::Keyboard( IKeyboard* impl ) : m_impl( impl )
{
    DC_BREAK_IF( s_keyboard != NULL );
    if( !m_impl ) log::warn( "Keyboard::Keyboard : keyboard interface is not implemented on current platform\n" );
    s_keyboard = this;
}

Keyboard::~Keyboard( void )
{
    DC_DELETE( m_impl )
}

// ** Keyboard::sharedInstance
Keyboard* Keyboard::sharedInstance( void )
{
	if( s_keyboard == NULL ) {
		s_keyboard = Keyboard::create();
	}

    return s_keyboard;
}

// ** Keyboard::create
Keyboard* Keyboard::create( void )
{
#if defined( DC_PLATFORM )
    if( IKeyboard* impl = createKeyboard() ) {
        return DC_NEW Keyboard( impl );
    }

    return NULL;
#endif

    return DC_NEW Keyboard( NULL );
}

// ** Keyboard::keyDown
bool Keyboard::keyDown( const Key& key )
{
    DC_CHECK_IMPL( false );
    return m_impl->keyDown( key );
}

} // namespace platform

DC_END_DREEMCHEST