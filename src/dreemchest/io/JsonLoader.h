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

#ifndef __DC_Io_JsonLoader_H__
#define __DC_Io_JsonLoader_H__

#include "Io.h"

DC_BEGIN_DREEMCHEST

namespace io {

#ifdef DC_JSON_ENABLED

	//! Base class for all JSON object loaders.
	class JsonLoaderBase {
	public:

		//! Object loader type.
		typedef cClosure<bool(const Json::Value&)>	Loader;

		virtual						~JsonLoaderBase( void ) {}

		//! Registers a new object loader.
		void						registerLoader( const String& name, const Loader& loader );

		//! Loads objects from JSON string.
		bool						load( const String& json );

	protected:

		//! Reads the Vec3 from a JSON object.
		static Vec3					readVec3( const Json::Value& value );

		//! Reads the Rect from a JSON object.
		static Rect					readRect( const Json::Value& value );

		//! Reads the Rgba from JSON object.
		static Rgba					readRgba( const Json::Value& value );

		//! Reads the Rgba from JSON object.
		static Rgb					readRgb( const Json::Value& value );

		//! Reads the Quat from JSON object.
		static Quat					readQuat( const Json::Value& value );

		//! Reads the array of floats from JSON object.
		static Array<f32>			readFloats( const Json::Value& value );

	private:

		//! Constructs an object from JSON object.
		bool						constructObject( const String& name, const Json::Value& value );

	private:

		//! Container type to store loaders by type name.
		typedef Map<String, Loader>	Loaders;

		Loaders						m_loaders;	//!< Object loaders.
		Json::Value					m_json;		//!< Parsed JSON object.
	};

#endif	/*	DC_JSON_ENABLED	*/

} // namespace Io

DC_END_DREEMCHEST

#endif	/*	!__DC_Io_JsonLoader_H__	*/