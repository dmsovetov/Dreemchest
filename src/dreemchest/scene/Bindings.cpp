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

#include "Bindings.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------------ Vec3Binding ------------------------------------------------ //

// ** Vec3Binding::Vec3Binding
Vec3Binding::Vec3Binding( const Vec3& value ) : m_value( value )
{

}

// ** Vec3Binding::get
Vec3 Vec3Binding::get( void ) const
{
	return m_value;
}

// ** Vec3Binding::set
void Vec3Binding::set( const Vec3& value )
{
	m_value = value;
}

// ---------------------------------------------- Vec3FromKeyboard --------------------------------------------- //

// ** Vec3FromKeyboard::Vec3FromKeyboard
Vec3FromKeyboard::Vec3FromKeyboard( Platform::Key::Mapping left, Platform::Key::Mapping right, Platform::Key::Mapping up, Platform::Key::Mapping down )
	: m_left( left ), m_right( right ), m_up( up ), m_down( down )
{
	m_input = Platform::Input::sharedInstance();
}

// ** Vec3FromKeyboard::get
Vec3 Vec3FromKeyboard::get( void ) const
{
	// Resulting direction
	Vec3 direction( 0.0f, 0.0f, 0.0f );

	// Get the movement direction
		 if( m_input->keyDown( m_left ) )  direction.x = -1.0f;
	else if( m_input->keyDown( m_right ) ) direction.x =  1.0f;

		 if( m_input->keyDown( m_up ) )    direction.z = -1.0f;
	else if( m_input->keyDown( m_down ) )  direction.z =  1.0f;

	// Normalize the direction
	direction.normalize();

	return direction;
}

// ---------------------------------------------- Vec3FromMouse --------------------------------------------- //

// ** Vec3FromMouse::Vec3FromMouse
Vec3FromMouse::Vec3FromMouse( f32 scaleFactor ) : m_scaleFactor( scaleFactor ), m_lastPosition( -1.0f, -1.0f )
{
	m_input = Platform::Input::sharedInstance();
}

// ** Vec3FromMouse::get
Vec3 Vec3FromMouse::get( void ) const
{
	// Get the mouse cursor
	Vec2 pos = Vec2( m_input->mouseX(), m_input->mouseY() );

	// This is a first time the direction is requested
	if( m_lastPosition.x == -1 || m_lastPosition.y == -1 ) {
		m_lastPosition = pos;
		return Vec3( 0.0f, 0.0f, 0.0f );
	}

	// Calculate deltas
	f32 dx = pos.x - m_lastPosition.x;
	f32 dy = pos.y - m_lastPosition.y;

	// Set last used position
	m_lastPosition = pos;

	return Vec3( -dy, -dx, 0.0f ) * m_scaleFactor;
}

} // namespace Scene

DC_END_DREEMCHEST