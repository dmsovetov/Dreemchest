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

#ifndef __Base_Classes_H__
#define __Base_Classes_H__

#include "StringHash.h"

DC_BEGIN_DREEMCHEST

//! Macro definition to disable copy constructor.
#define ClassDisableCopying( T )            \
    private:                                \
        T( const T& );                      \
        T& operator = ( const T& );

//! Macro definition to disable heap allocation of this class.
#define ClassDisableHeapAlloc()             \
    private:								\
        void *operator new( size_t size );	\
        void *operator new[]( size_t size );

//! Macro definition to embed a type id into the class
#define ClassEnableTypeId( T )														\
    public:																			\
		virtual TypeId  typeId( void ) const  { return TypeInfo<T>::id(); }			\
		virtual TypeIdx typeIndex( void ) const  { return TypeIndex<T>::idx(); }	\
		virtual CString typeName( void ) const { return TypeInfo<T>::name(); }		

//! Macro definition for declaring an implementation interface.
#define BeginPrivateInterface( T )								\
	namespace impl {											\
		typedef AutoPtr<class T##Private> T##PrivatePtr;		\
		class T##Private : public RefCounted {					\
		protected:												\
			T*			m_parent;								\
		public:													\
            void  setParent( T* parent ) { m_parent = parent; } \
			virtual		~T##Private( void ) {}

#define EndPrivateInterface										\
		};														\
	} // namespace impl

#define InterfaceMethod( m ) virtual m = 0;

#define UsePrivateInterface( T ) private: impl::T##PrivatePtr	m_impl;

//! Macro definition that enabled a type casting for class.
#define ClassEnableTypeInfo( name )                                                             \
    ClassEnableTypeId( name )                                                                   \
    public:                                                                                     \
        template<typename Type> bool is( void ) const { return is( TypeInfo<Type>::id() ); }	\
        virtual bool is( const TypeId& id ) const {                                             \
            return id == TypeInfo<name>::id();                                                  \
        }

//! Macro definition for enabling instance cloning
#define ClassEnableCloning( T )	\
	virtual T* clone( void ) const { return new T; }

//! Macro definition to enable type casts & type id.
#define ClassEnableTypeInfoSuper( name, super )                                                 \
    ClassEnableTypeId( name )                                                                   \
    public:                                                                                     \
        template<typename Type> bool is( void ) const { return is( TypeInfo<Type>::id() ); }	\
        virtual bool is( const TypeId& id ) const {                                             \
            if( id == TypeInfo<name>::id() ) return true; else return super::is( id );          \
        }

//! Macro definitions for template function types
#define TemplateFunctionTypes1 typename Arg0
#define TemplateFunctionTypes2 typename Arg0, typename Arg1
#define TemplateFunctionTypes3 typename Arg0, typename Arg1, typename Arg2
#define TemplateFunctionTypes4 typename Arg0, typename Arg1, typename Arg2, typename Arg3
#define TemplateFunctionTypes5 typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4
#define TemplateFunctionTypes6 typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5
#define TemplateFunctionTypes7 typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6

//! Macro definitions for template function types
#define TemplateFunctionArgs1 const Arg0& arg0
#define TemplateFunctionArgs2 const Arg0& arg0, const Arg1& arg1
#define TemplateFunctionArgs3 const Arg0& arg0, const Arg1& arg1, const Arg2& arg2
#define TemplateFunctionArgs4 const Arg0& arg0, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3
#define TemplateFunctionArgs5 const Arg0& arg0, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4
#define TemplateFunctionArgs6 const Arg0& arg0, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4, const Arg5& arg5
#define TemplateFunctionArgs7 const Arg0& arg0, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4, const Arg5& arg5, const Arg6& arg6

//! Type for class index.
typedef unsigned short  TypeIdx;

//! Type for type id.
typedef unsigned int    TypeId;

//! Type for global type indices.
struct GlobalTypeIndex {};

//! TypeIndexGenerator is a base class for TypeIndex and used to store a global class counter.
template<typename T>
class TypeIndexGenerator {
protected:

    static TypeIdx  generateNextIdx( void ) { static TypeIdx nextTypeIdx = 1; return nextTypeIdx++; }
};

//! TypeIndex class helps to generate a unique integer index for any class.
/*!
 Note: TypeIndex doesn't guaranty that a same index is generated for a same class all the time.
 */
template<typename T>
class TypeIndex : public TypeIndexGenerator<GlobalTypeIndex> {
public:

    //! Generates a type index.
    static TypeIdx      idx( void ) { static TypeIdx idx = TypeIndexGenerator::generateNextIdx(); return idx; }
};

//! GroupedTypeIndex is used for generating a separate sets of indices.
template<typename T, typename U>
class GroupedTypeIndex : public TypeIndexGenerator<U> {
public:

	//! Generates a type index.
	static TypeIdx      idx( void ) { static TypeIdx idx = TypeIndexGenerator<U>::generateNextIdx(); return idx; }
};

//! TypeInfo class helps to generate a universaly unique id & extract a type name.
template<typename T>
class TypeInfo {
public:

	//! Returns a qualified class name (with namespace).
	static CString qualifiedName( void )
	{
		static String value = parseQualifiedName();
		return value.c_str();
	}

	//! Returns a class name without a namespace.
	static CString name( void )
	{
		static String value = parseName();
		return value.c_str();
	}

	//! Returns a unique class id.
	static TypeId id( void )
	{
		static StringHash value = StringHash( name() );
		return value;
	}

private:

	//! Parses a class name from __FUNCTION__ macro
	static String parseName( void )
	{
		String name = parseQualifiedName();

		u32 start = name.find_last_of( ":" );
		if( start != String::npos ) {
			name = name.substr( start + 1 );
		}

		return name;
	}

	//! Parses a full class name from __FUNCTION__ macro
	static String parseQualifiedName( void )
	{
    #if __GNUC__
        String name = __PRETTY_FUNCTION__;

		// ** Parse a GCC __PRETTY_FUNCTION__
		u32 eq = name.find_first_of( "=" );

		if( eq != String::npos ) {
			eq += 2;
			u32 end = name.find_first_of( ";", eq );
			return name.substr( eq, end - eq );
		}
    #else
		String name = __FUNCTION__;
    #endif

		// ** Get the template argument
		u32 start = name.find_first_of( "<" );
		u32 end   = name.find_first_of( ">" );
		name = name.substr( start + 1, end - start - 1 ); 

		// ** Remove the 'struct ' from the beginning.
		start = name.find_first_of( " " );
		if( start != String::npos ) {
			name = name.substr( start + 1 );
		}

		return name;
	}
};

//! Does a type cast.
template<typename T, typename S>
inline T* castTo( S* ptr ) {
    return ptr->is( TypeInfo<T>::id() ) ? static_cast<T*>( ptr ) : 0;
}

//! Does a type cast.
template<typename T, typename S>
inline const T* castTo( const S* ptr ) {
    return ptr->is( TypeInfo<T>::id() ) ? static_cast<const T*>( ptr ) : 0;
}

//! Statically check if a class D is derived from B.
/*!
	http://stackoverflow.com/questions/2910979/how-does-is-base-of-work
*/
template<typename Base, typename Derived>
struct IsBaseOf
{
	typedef char (&yes)[1];
	typedef char (&no)[2];

	template <typename HostBase, typename HostDerived>
	struct Host
	{
	  operator HostBase*() const;
	  operator HostDerived*();
	};

	template <typename T> 
	static yes check(Derived*, T);
	static no check(Base*, int);

	static const bool Value = sizeof(check(Host<Base, Derived>(), int())) == sizeof(yes);
};

//! Enabled if a static condition is true.
template <bool, class T = void> 
struct EnableIf 
{
};
 
template <class T> 
struct EnableIf<true, T> 
{ 
	typedef T Type; 
};

//! Provides the member typedef Type which is the type pointed to by T, or, if T is not a pointer, then type is the same as T.
template<typename T>
struct RemovePointer
{
    typedef T Type;

	//! Returns an input argument as a reference.
	static const T& cast( const T& value )
	{
		return value;
	}
};

template<typename T>
struct RemovePointer<T*>
{
    typedef typename RemovePointer<T>::Type Type;

	//! Returns an input argument as a reference.
	static const Type& cast( const T* value )
	{
		return *value;
	}
};

// Tupple index builder
// http://habrahabr.ru/post/101430/

template<int ... Idxs>
struct IndexesTuple  
{ 
};

template<int Num, typename Tp = IndexesTuple<>> 
struct IndexTupleBuilder;

template<int Num, int ... Idxs>  
struct IndexTupleBuilder<Num, IndexesTuple<Idxs ...>> : IndexTupleBuilder<Num - 1, IndexesTuple<Idxs ..., sizeof ... (Idxs)>>  
{ 
};

template<int ... Idxs> 
struct IndexTupleBuilder<0, IndexesTuple<Idxs ...>> 
{ 
	typedef IndexesTuple<Idxs...> Indexes; 
};

DC_END_DREEMCHEST

#endif  /*      !defined( __Base_Classes_H__ )   */
