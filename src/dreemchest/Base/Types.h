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

#ifndef __DC_TYPES_H__
#define __DC_TYPES_H__

#define DC_TYPES_INCLUDED

#include <string>
#include  <vector>
#include <map>
#include <list>
#include <set>
#include <algorithm>
#include <stack>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sstream>

DC_BEGIN_DREEMCHEST

	template<typename T>
	class Array : public std::vector<T> {
	public:

		//! Constructs an empty Array instance.
		Array( void ) {}

		//! Constructs an Array instance with a single item inside.
		Array( const T& value ) { push_back( value ); }

		//! Appends a new value to an array.
		Array& operator << ( const T& value ) { push_back( value ); }
	};

	template<typename T>
	class Stack : public std::stack<T> {};

	template<typename T>
	class List : public std::list<T> {};

    template< typename T, typename Compare = std::less<T> >
    class Set : public std::set<T, Compare> {};

    template<typename K, typename V, typename P = std::less<K> >
    class Map : public std::map<K, V, P> {};

	struct Void {};

    typedef double                  f64;
    typedef float                   f32;
    typedef char                    s8;
    typedef signed long long        s64;
    typedef signed int              s32;
    typedef unsigned long long      u64;
    typedef unsigned int            u32;
    typedef short                   s16;
    typedef unsigned short          u16;
    typedef unsigned char           u8;
    typedef const char*             CString;
    typedef StringHash::type        strhash;
    typedef std::string             String;
    typedef Array<String>			StringArray;
	typedef List<String>			StringList;
	typedef Array<class Guid>		GuidArray;

	template<typename T>
	class Hash : public std::map<strhash, T> {};

	//! Returns random item from list
	template<typename T>
	const T& randomItem( const List<T>& items )
	{
		u32 idx = rand() % items.size();

		typename List<T>::const_iterator i = items.begin();

		while( idx-- ) {
			++i;
		}

		return *i;
	}

	//! Returns random item from set
	template<typename T>
	const T& randomItem( const Set<T>& items )
	{
		u32 idx = rand() % items.size();

		typename Set<T>::const_iterator i = items.begin();

		while( idx-- ) {
			++i;
		}

		return *i;
	}

	//! Returns random item from array
	template<typename T>
	const T& randomItem( const Array<T>& items )
	{
		return items[rand() % items.size()];
	}

    //! Helper struct to hold a numeric range.
    struct Range {
                Range( f32 min = 0.0f, f32 max = 0.0f )
                    : min( min ), max( max ) {}

        f32     min;
        f32     max;
    };

	//! Convert value to a string.
	template< typename T>
	String toString( const T& n )
    {
        std::ostringstream stm;
        stm << n ;
        return stm.str();
    }

DC_END_DREEMCHEST

#endif        /*    !__DC_TYPES_H__    */