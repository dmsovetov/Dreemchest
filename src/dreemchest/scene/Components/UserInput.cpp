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

// --------------------------------------------- MoveAlongAxes --------------------------------------------- //

// ** MoveAlongAxes::speed
f32 MoveAlongAxes::speed( void ) const
{
	return m_speed;
}

// ** MoveAlongAxes::coordinateSystem
u8 MoveAlongAxes::coordinateSystem( void ) const
{
	return m_coordinateSystem;
}

// ** MoveAlongAxes::delta
Vec3 MoveAlongAxes::delta( void ) const
{
	return m_delta->get();
}

// --------------------------------------------- RotateAroundAxes --------------------------------------------- //

// ** RotateAroundAxes::speed
f32 RotateAroundAxes::speed( void ) const
{
	return m_speed;
}

// ** RotateAroundAxes::setSpeed
void RotateAroundAxes::setSpeed( f32 value )
{
	m_speed = value;
}

// ** RotateAroundAxes::coordinateSystem
u8 RotateAroundAxes::coordinateSystem( void ) const
{
	return m_coordinateSystem;
}

// ** RotateAroundAxes::delta
Vec3 RotateAroundAxes::delta( void ) const
{
	return m_delta->get();
}

} // namespace Scene

DC_END_DREEMCHEST