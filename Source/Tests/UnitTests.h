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

#include <gtest/gtest.h>
#include <Dreemchest.h>

class TestableObject : public mvvm::GenericObject<TestableObject> {
public:

	template<typename TValue>
	typename WeakPtr<TValue> add( const String& name/*, const TValue& defaultValue = TValue()*/ )
	{
		return GenericObject::add<TValue>( name/*, defaultValue*/ );
	}
};

class UserInfo : public mvvm::GenericObject<UserInfo> {
public:

	UserInfo( void )
	{
		add<mvvm::Text>( "name" );
		add<mvvm::Float>( "level" );
		add<mvvm::Float>( "health" );
	}
};

class Session : public mvvm::GenericObject<Session> {
public:

	Session( void )
	{
		add<mvvm::Text>( "id"/*, "session identifier"*/ );
		add<UserInfo>( "userInfo" );
	}
};

static const String kTestString = "hello world";