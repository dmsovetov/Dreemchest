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

#include "JsonLoader.h"

DC_BEGIN_DREEMCHEST

namespace Io {

#if DEV_DEPRECATED_SERIALIZATION

#ifdef HAVE_JSON

// ** JsonLoaderBase::load
bool JsonLoaderBase::load( const String& json )
{
	Json::Reader reader;

	// Parse the JSON string
	if( !reader.parse( json, m_json ) ) {
		return false;
	}

	// Read objects from JSON
	for( Json::ValueIterator i = m_json.begin(), end = m_json.end(); i != end; ++i ) {
		// Get the instance type.
		String type = i->get( "class", "" ).asString();

		// Construct object
		constructObject( type, *i );
/*
		// Read the scene object.
		if( type == "SceneObject" ) {
			requestSceneObject( i.key().asString() );
			continue;
		}

		// Read the component
		Ecs::ComponentPtr component = requestComponent( i.key().asString() );

		// Get the scene object to attach the component to.
		Ecs::EntityPtr entity = requestSceneObject( i->get( "sceneObject", Json::Value() ).asString() );

		// Attach the component.
		entity->attachComponent( component.get() );*/
	}

	return true;
}

// ** JsonLoaderBase::constructObject
bool JsonLoaderBase::constructObject( const String& name, const Json::Value& value )
{
	Loaders::const_iterator i = m_loaders.find( name );

	if( i == m_loaders.end() ) {
		return false;
	}

	return i->second( value );
}

// ** JsonLoaderBase::registerLoader
void JsonLoaderBase::registerLoader( const String& name, const Loader& loader )
{
	m_loaders[name] = loader;
}

// ** JsonLoaderBase::readVec3
Vec3 JsonLoaderBase::readVec3( const Json::Value& value )
{
	return Vec3( value[0].asFloat(), value[1].asFloat(), value[2].asFloat() );
}

// ** JsonLoaderBase::readRect
Rect JsonLoaderBase::readRect( const Json::Value& value )
{
	return Rect( value[0].asFloat(), value[1].asFloat(), value[2].asFloat(), value[3].asFloat() );
}

// ** JsonLoaderBase::readRgba
Rgba JsonLoaderBase::readRgba( const Json::Value& value )
{
	return Rgba( value[0].asFloat(), value[1].asFloat(), value[2].asFloat(), value[3].asFloat() );
}

// ** JsonLoaderBase::readRgb
Rgb JsonLoaderBase::readRgb( const Json::Value& value )
{
	return Rgb( value[0].asFloat(), value[1].asFloat(), value[2].asFloat() );
}

// ** JsonLoaderBase::readQuat
Quat JsonLoaderBase::readQuat( const Json::Value& value )
{
	return Quat( value[0].asFloat(), value[1].asFloat(), value[2].asFloat(), value[3].asFloat() );
}

// ** JsonLoaderBase::readQuat
Array<f32> JsonLoaderBase::readFloats( const Json::Value& value )
{
	Array<f32> result;

	for( s32 i = 0, n = value.size(); i < n; i++ ) {
		result.push_back( value[i].asFloat() );
	}

	return result;
}

#endif	/*	HAVE_JSON	*/

#endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

} // namespace Io

DC_END_DREEMCHEST