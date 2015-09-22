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

#ifndef __Base_UserData_H__
#define __Base_UserData_H__

#include "Classes.h"

DC_BEGIN_DREEMCHEST

	//! A helper class for holding generic user data inside objects.
	class UserData {
	public:

		//! Sets a user data of a specified type.
		template<typename T> void set( const T* userData );

		//! Returns a pointer to a user data with specified type.
		template<typename T> const T* get( void ) const;
		template<typename T> T*		  get( void );

		//! Returns true if user data holds a pointer of specified type.
		template<typename T> bool has( void ) const;

		//! Removes attached user data of specified type.
		template<typename T> void remove( void );
	
	private:

		//! Container type to store a user data pointers.
		typedef Map<TypeIdx, const void*> UserDataHolder;

		//! Actual user data attached.
		UserDataHolder	m_userData;
	};

	// ** UserData::set
	template<typename T>
	inline void UserData::set( const T* userData )
	{
		m_userData[TypeIndex<T>::idx()] = reinterpret_cast<const void*>( userData );
	}

	// ** UserData::get
	template<typename T>
	inline const T* UserData::get( void ) const
	{
		UserDataHolder::const_iterator i = m_userData.find( TypeIndex<T>::idx() );
		return i != m_userData.end() ? reinterpret_cast<const T*>( i->second ) : NULL;
	}

	// ** UserData::get
	template<typename T>
	inline T* UserData::get( void )
	{
		UserDataHolder::iterator i = m_userData.find( TypeIndex<T>::idx() );
		return i != m_userData.end() ? const_cast<T*>( reinterpret_cast<const T*>( i->second ) ) : NULL;
	}

	// ** UserData::has
	template<typename T>
	inline bool UserData::has( void ) const
	{
		return m_userData.count( TypeIndex<T>::idx() ) != 0;
	}

	// ** UserData::remove
	template<typename T>
	inline void UserData::remove( void )
	{
		UserDataHolder::iterator i = m_userData.find( TypeIndex<T>::idx() );

		if( i != m_userData.end() ) {
			m_userData.erase( i );
		}
	}

//! Macro definition for embedding user data into class declarations.
#define EmbedUserData																				\
	public:																							\
		template<typename T> const T* userData( void ) const { return m_userData.get<T>();	}		\
		template<typename T> T*		  userData( void ) { return m_userData.get<T>();	}			\
		template<typename T> void     setUserData( const T* value ) { m_userData.set<T>( value ); }	\
		template<typename T> bool     hasUserData( void ) const { return m_userData.has<T>(); }		\
	private:																						\
		UserData	m_userData;

DC_END_DREEMCHEST

#endif	/*	!__Base_UserData_H__	*/
