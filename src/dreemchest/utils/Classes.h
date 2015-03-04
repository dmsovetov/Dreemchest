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

#ifndef __DC_Utils_Classes_H__
#define __DC_Utils_Classes_H__

#include "StringHash.h"

DC_BEGIN_DREEMCHEST

//! Macro definition to disable copy constructor.
#define ClassDisableCopying( T )            \
    private:                                \
        T( const T& );                      \
        T& operator = ( const T& )

//! Macro definition to disable heap allocation of this class.
#define ClassDisableHeapAlloc()             \
    private:								\
        void *operator new( size_t size );	\
        void *operator new[]( size_t size );

//! Macro definition to embed a type id into the class
#define ClassEnableTypeId( name )                                           \
    public:                                                                 \
        static TypeIdx typeIdx( void ) { return TypeIndex<name>::idx(); }   \
        static TypeId typeId( void ) {                                      \
            static StringHash hash( #name ); return hash;                   \
        }

#define ClassEnableTypeInfo( name )                                                 \
    ClassEnableTypeId( name )                                                       \
    public:                                                                         \
        template<typename T> bool is( void ) const { return is( T::typeId() ); }    \
        virtual bool is( const TypeId& id ) const {                                 \
            return id == typeId();                                                  \
        }

//! Macro definition to enable type casts & type id.
#define ClassEnableTypeInfoSuper( name, super )                                     \
    ClassEnableTypeId( name )                                                       \
    public:                                                                         \
        template<typename T> bool is( void ) const { return is( T::typeId() ); }    \
        virtual bool is( const TypeId& id ) const {                                 \
            if( id == typeId() ) return true; else return super::is( id );          \
        }

//! Type for class index.
typedef unsigned short  TypeIdx;

//! Type for type id.
typedef unsigned int    TypeId;

//! TypeIndexGenerator is a base class for TypeIndex and used to store a global class counter.
class TypeIndexGenerator {
protected:

    static TypeIdx  generateNextIdx( void ) { static TypeIdx nextTypeIdx = 1; return nextTypeIdx++; }
};

//! TypeIndex class helps to generate a unique integer index for any class.
/*!
 Note: TypeIndex doesn't guaranty that a same index is generated for a same class all the time.
 */
template<typename T>
class TypeIndex : public TypeIndexGenerator {
public:

    //! Generates a type index.
    static TypeIdx      idx( void ) { static TypeIdx idx = generateNextIdx(); return idx; }
};

//! Does a type cast.
template<typename T, typename S>
inline T* castTo( S* ptr ) {
    return ptr->is( T::typeId() ) ? static_cast<T*>( ptr ) : 0;
}

DC_END_DREEMCHEST

#endif  /*      !defined( __DC_Utils_Classes_H__ )   */
