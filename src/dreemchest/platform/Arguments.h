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

#ifndef __DC_Platform_Arguments_H__
#define __DC_Platform_Arguments_H__

#include "Platform.h"

DC_BEGIN_DREEMCHEST

namespace Platform {

	//! Application arguments parser.
	class Arguments {
	public:

						//! Constructs Arguments instance.
						Arguments( s8** argv, u32 argc );

		//! Returns integer value.
		s32				integer( const String& name, s32 defaultValue = 0 ) const;

		//! Returns the float value.
		f32				number( const String& name, f32 defaultValue = 0.0f ) const;

		//! Returns string value.
		const String&	string( const String& name, const String& defaultValue = "" ) const;

		//! Returns true if argument with a specified name was passed to an application.
		bool			has( const String& name ) const;

	private:

		//! Container type to store parsed arguments.
		typedef Map<String, String> ArgumentValues;

		ArgumentValues	m_values;	//!< Parsed argument values.
	};

} // namespace Platform

DC_END_DREEMCHEST

#endif /*   !defined( __DC_Platform_Arguments_H__ )   */
