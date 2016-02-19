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

namespace Platform {

//! Platform-specific inpit constructor.
extern IInput* createInput( void );

// ** Input::s_input
Input* Input::s_input = NULL;

// ** Input::Input
Input::Input( IInput* impl ) : m_impl( impl )
{
    DC_BREAK_IF( s_input != NULL );
    if( !m_impl ) LogWarning( "input", "not implemented on current platform\n" );
    s_input = this;

	memset( m_isKeyDown, 0, sizeof( m_isKeyDown ) );
}

Input::~Input( void )
{
    delete m_impl;
}

// ** Input::sharedInstance
Input* Input::sharedInstance( void )
{
	if( s_input == NULL ) {
		s_input = Input::create();
	}

    return s_input;
}

// ** Input::create
Input* Input::create( void )
{
#if defined( DC_PLATFORM_WINDOWS ) || defined( DC_PLATFORM_OSX )
    if( IInput* impl = createInput() ) {
        return DC_NEW Input( impl );
    }
#endif

    return NULL;
}

// ** Input::keyDown
bool Input::keyDown( const Key& key )
{
    DC_CHECK_IMPL( false );
    return m_impl->keyDown( key );
}

// ** Input::keyPressed
bool Input::keyPressed( const Key& key )
{
    DC_CHECK_IMPL( false );
    
	if( keyDown( key ) ) {
		if( !m_isKeyDown[key] ) {
			m_isKeyDown[key] = true;
			return true;
		}

		return false;
	} else {
		m_isKeyDown[key] = false;
	}

	return false;
}

// ** Input::mouseX
s32 Input::mouseX( void ) const
{
    DC_CHECK_IMPL( 0 );
    return m_impl->mouseX();
}

// ** Input::mouseY
s32 Input::mouseY( void ) const
{
    DC_CHECK_IMPL( 0 );
    return m_impl->mouseY();
}

// ** Input::mouseY
void Input::setMouse( s32 x, s32 y )
{
    DC_CHECK_IMPL();
    return m_impl->setMouse( x, y );
}

} // namespace Platform

DC_END_DREEMCHEST