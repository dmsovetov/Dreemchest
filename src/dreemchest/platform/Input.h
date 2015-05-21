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

#ifndef __DC_Platform_Input_H__
#define __DC_Platform_Input_H__

#include "Platform.h"

DC_BEGIN_DREEMCHEST

namespace platform {

	//! A platform-specific input implementation interface.
	class IInput {
	public:

		virtual                 ~IInput( void ) {}

		//! Returns true if a specified key is down.
		virtual bool            keyDown( const Key& key ) = 0;

		//! Sets the mouse position.
		virtual void			setMouse( s32 x, s32 y ) = 0;

		//! Returns the mouse X position.
		virtual s32				mouseX( void ) const = 0;

		//! Returns the mouse Y position.
		virtual s32				mouseY( void ) const = 0;
	};

	//! Input class performs access to a keyboard and mouse state.
	class Input {
	public:

		virtual                 ~Input( void );

		//! Returns true if a specified key is down.
		/*!
		\param key The key to check.
		*/
		bool                    keyDown( const Key& key );

		//! Returns true once after the key was pressed.
		bool					keyPressed( const Key& key );

		//! Sets the mouse position.
		void					setMouse( s32 x, s32 y );

		//! Returns the mouse X position.
		s32						mouseX( void ) const;

		//! Returns the mouse Y position.
		s32						mouseY( void ) const;

		//! Creates a new Input instance.
		static Input*			create( void );

		//! Returns a shared Input instance.
		static Input*			sharedInstance( void );

	private:

								//! Constructs a new Input instance.
								Input( IInput* impl );

	private:

		//! Shared keyboard instance.
		static Input*			s_input;

		//! Platform specific input implementation.
		IInput*					m_impl;

		//! Key down flags.
		bool					m_isKeyDown[Key::Total];
	};

} // namespace platform

DC_END_DREEMCHEST

#endif /*   !defined( __DC_Platform_Input_H__ )   */
