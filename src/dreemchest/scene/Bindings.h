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

#ifndef __DC_Scene_Bindings_H__
#define __DC_Scene_Bindings_H__

#include "Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Constant Vec3 binding, can be subclassed to implement a different behaviour.
	class Vec3Binding : public RefCounted {
	public:

								//! Constructs the Vec3Binding instance
								Vec3Binding( const Vec3& value = Vec3( 0.0f, 0.0f, 0.0f ) );

		//! Returns the value.
		virtual Vec3			get( void ) const;

	protected:

		Vec3					m_value;	//!< The direction value.
	};

	//! Calculates the movement direction vector from the keyboard input.
	class Vec3FromKeyboard : public Vec3Binding {
	public:

								//! Constructs the Vec3FromKeyboard instance.
								Vec3FromKeyboard( Platform::Key::Mapping left = Platform::Key::Total, Platform::Key::Mapping right = Platform::Key::Total, Platform::Key::Mapping up = Platform::Key::Total, Platform::Key::Mapping down = Platform::Key::Total );

		//! Calculates the direction.
		virtual Vec3			get( void ) const DC_DECL_OVERRIDE;

	private:

		Platform::Input*		m_input;		//!< The input used.
		Platform::Key::Mapping	m_left;			//!< Left key mapping
		Platform::Key::Mapping	m_right;		//!< Right key mapping
		Platform::Key::Mapping	m_up;			//!< Up key mapping
		Platform::Key::Mapping	m_down;			//!< Down key mapping
	};

	//! Calculates the rotation values from mouse movement.
	class Vec3FromMouse : public Vec3Binding {
	public:

								//! Constructs Vec3FromMouse instance.
								Vec3FromMouse( f32 scaleFactor = 1.0f );

		//! Calculates the rotation values.
		virtual Vec3			get( void ) const DC_DECL_OVERRIDE;

	private:

		Platform::Input*		m_input;		//!< The input used.
		f32						m_scaleFactor;	//!< The output scaling factor.
		mutable Vec2			m_lastPosition;	//!< Last cursor position.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Bindings_H__    */