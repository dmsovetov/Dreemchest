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

#ifndef __DC_Scene_Component_Input_H__
#define __DC_Scene_Component_Input_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Constant direction, can be subclassed to implement a different direction behaviour.
	class Direction : public RefCounted {
	public:

								//! Constructs the Direction instance
								Direction( const Vec3& direction = Vec3( 0.0f, 0.0f, 0.0f ) );
		virtual					~Direction( void ) {}

		//! Returns the direction.
		virtual Vec3			get( void ) const;

	protected:

		Vec3					m_direction;	//!< The direction value.
	};

	//! Calculates the direction vector from the keyboard input.
	class DirectionFromKeyboard : public Direction {
	public:

								//! Constructs the DirectionFromKeyboard instance.
								DirectionFromKeyboard( Platform::Key::Mapping left = Platform::Key::Total, Platform::Key::Mapping right = Platform::Key::Total, Platform::Key::Mapping up = Platform::Key::Total, Platform::Key::Mapping down = Platform::Key::Total );

		//! Calculates the direction.
		virtual Vec3			get( void ) const;

	private:

		Platform::Input*		m_input;		//!< The input used.
		Platform::Key::Mapping	m_left;			//!< Left key mapping
		Platform::Key::Mapping	m_right;		//!< Right key mapping
		Platform::Key::Mapping	m_up;			//!< Up key mapping
		Platform::Key::Mapping	m_down;			//!< Down key mapping
	};

	//! Moves the scene object along the input direction.
	class MoveInDirection : public Component {
	public:

		//! Movement axes.
		enum Axes {
			  XY				//!< Move along the XY axes.
			, XZ				//!< Move along the XZ axes.
		};

								OverrideComponent( MoveInDirection, Component )

								//! Constructs MoveInDirection instance.
								MoveInDirection( Axes axes = XY, f32 speed = 1.0f, const DirectionPtr& direction = DirectionPtr() )
									: m_axes( axes ), m_speed( speed ), m_direction( direction ) {}

		//! Returns the movement speed.
		f32						speed( void ) const;

		//! Movement axes.
		Axes					axes( void ) const;

		//! Returns the movement direction.
		Vec3					direction( void ) const;

	private:

		Axes					m_axes;			//!< Movement is performed along this axes.
		f32						m_speed;		//!< Movement speed.
		DirectionPtr			m_direction;	//!< Movement direction.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Input_H__    */