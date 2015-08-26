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

#include "Component.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! Moves the scene object with WASD control scheme
	class WasdInput2D : public SceneComponent {
	public:

								OverrideComponent( WasdInput2D, SceneComponent )

								//! Constructs WasdInput2D instance.
								WasdInput2D( f32 speed = 1.0f, platform::Key::Mapping left = platform::Key::Total, platform::Key::Mapping right = platform::Key::Total, platform::Key::Mapping up = platform::Key::Total, platform::Key::Mapping down = platform::Key::Total )
									: m_speed( speed ), m_left( left ), m_right( right ), m_up( up ), m_down( down ) {}

		//! Returns the movement speed.
		f32						speed( void ) const;

		//! Returns left key mapping.
		platform::Key::Mapping	left( void ) const;

		//! Return right key mapping.
		platform::Key::Mapping	right( void ) const;

		//! Returns up key mapping.
		platform::Key::Mapping	up( void ) const;

		//! Returns down key mapping.
		platform::Key::Mapping	down( void ) const;

	private:

		f32						m_speed;	//!< Movement speed.
		platform::Key::Mapping	m_left;		//!< Move object left key mapping
		platform::Key::Mapping	m_right;	//!< Move object right key mapping
		platform::Key::Mapping	m_up;		//!< Move object up key mapping
		platform::Key::Mapping	m_down;		//!< Move object down key mapping
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Input_H__    */