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

#include "UserInput.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------------ Direction ------------------------------------------------ //

// ** Direction::Direction
Direction::Direction( const Vec3& direction ) : m_direction( direction )
{

}

// ** Direction::get
Vec3 Direction::get( void ) const
{
	return m_direction;
}

// ------------------------------------------ DirectionFromKeyboard ------------------------------------------ //

// ** DirectionFromKeyboard::DirectionFromKeyboard
DirectionFromKeyboard::DirectionFromKeyboard( Platform::Key::Mapping left, Platform::Key::Mapping right, Platform::Key::Mapping up, Platform::Key::Mapping down )
	: m_left( left ), m_right( right ), m_up( up ), m_down( down )
{
	m_input = Platform::Input::sharedInstance();
}

// ** DirectionFromKeyboard::get
Vec3 DirectionFromKeyboard::get( void ) const
{
	// Resulting direction
	Vec3 direction( 0.0f, 0.0f, 0.0f );

	// Get the movement direction
		 if( m_input->keyDown( m_left ) )  direction.x = -1.0f;
	else if( m_input->keyDown( m_right ) ) direction.x =  1.0f;

		 if( m_input->keyDown( m_up ) )    direction.y =  1.0f;
	else if( m_input->keyDown( m_down ) )  direction.y = -1.0f;

	// Normalize the direction
	direction.normalize();

	return direction;
}

// ----------------------------------------------- Identifier ------------------------------------------------ //

// ** Identifier::name
const String& Identifier::name( void ) const
{
	return m_name;
}

// ** Identifier::setName
void Identifier::setName( const String& value )
{
	m_name = value;
}

// --------------------------------------------- MoveInDirection --------------------------------------------- //

// ** MoveInDirection::speed
f32 MoveInDirection::speed( void ) const
{
	return m_speed;
}

// ** MoveByInput::axes
MoveInDirection::Axes MoveInDirection::axes( void ) const
{
	return m_axes;
}

// ** MoveInDirection::direction
Vec3 MoveInDirection::direction( void ) const
{
	return m_direction->get();
}

} // namespace Scene

DC_END_DREEMCHEST