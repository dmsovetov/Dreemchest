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

DC_BEGIN_DREEMCHEST

//! Macro definition to embed a type id into the class
#define ClassEnableTypeId( T )                                                        \
    public:                                                                            \
        virtual TypeId  typeId( void ) const  { return TypeInfo<T>::id(); }            \
        virtual TypeIdx typeIndex( void ) const  { return TypeIndex<T>::idx(); }    \
        virtual CString typeName( void ) const { return TypeInfo<T>::name(); }        

//! Macro definition for declaring an implementation interface.
#define BeginPrivateInterface( T )                                \
    namespace impl {                                            \
        typedef AutoPtr<class T##Private> T##PrivatePtr;        \
        class T##Private : public RefCounted {                    \
        protected:                                                \
            T*            m_parent;                                \
        public:                                                    \
            void  setParent( T* parent ) { m_parent = parent; } \
            virtual        ~T##Private( void ) {}

#define EndPrivateInterface                                        \
        };                                                        \
    } // namespace impl

#define InterfaceMethod( m ) virtual m = 0;

#define UsePrivateInterface( T ) private: impl::T##PrivatePtr    m_impl;

//! Macro definition that enabled a type casting for class.
#define ClassEnableTypeInfo( name )                                                             \
    ClassEnableTypeId( name )                                                                   \
    public:                                                                                     \
        template<typename Type> bool is( void ) const { return is( TypeInfo<Type>::id() ); }    \
        virtual bool is( const TypeId& id ) const {                                             \
            return id == TypeInfo<name>::id();                                                  \
        }

//! Macro definition for enabling instance cloning
#define ClassEnableCloning( T )    \
    virtual T* clone( void ) const { return new T; }

//! Macro definition to enable type casts & type id.
#define ClassEnableTypeInfoSuper( name, super )                                                 \
    ClassEnableTypeId( name )                                                                   \
    public:                                                                                     \
        template<typename Type> bool is( void ) const { return is( TypeInfo<Type>::id() ); }    \
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

DC_END_DREEMCHEST

#endif  /*      !defined( __Base_Classes_H__ )   */
