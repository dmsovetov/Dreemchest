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

	//! A platform-specific keyboard implementation interface.
	class IKeyboard {
	public:

		virtual                 ~IKeyboard( void ) {}

		//! Returns true if a specified key is down.
		virtual bool            keyDown( const Key& key ) = 0;
	};

	//! Keyboard class performs access to a keyboard state.
	class Keyboard {
	public:

		virtual                 ~Keyboard( void );

		//! Returns true if a specified key is down.
		/*!
		\param key The key to check.
		*/
		bool                    keyDown( const Key& key );

		//! Notifies an application about a lunch.
		void                    notifyLaunched(void);

		//! Creates a new Keyboard instance.
		static Keyboard*		create(void);

		//! Returns a shared keyboard instance.
		static Keyboard*		sharedInstance(void);

	private:

								//! Constructs a new Keyboard instance.
								Keyboard( IKeyboard* impl );

	private:

		//! Shared keyboard instance.
		static Keyboard*		s_keyboard;

		//! Platform specific keyboard implementation.
		IKeyboard*				m_impl;
	};

} // namespace platform

DC_END_DREEMCHEST

#endif /*   !defined( __DC_Platform_Input_H__ )   */
