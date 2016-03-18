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

#ifndef __Base_Preprocessor_H__
#define __Base_Preprocessor_H__

// ** Function binding
#define dcThisMethod( method )              CLOSURE( this, (&method) )
#define dcObjectMethod( object, method )    CLOSURE( object, (&method) )
#define dcStaticFunction( function )        SCLOSURE( (&function) )

// ** Smart ptrs
#define        DC_DECLARE_PTRS( type, name )								\
                typedef DC_DREEMCHEST_NS WeakPtr<class type>    dc##name##Weak;			\
                typedef DC_DREEMCHEST_NS StrongPtr<class type>  dc##name##Strong;

#define dcDeclarePtrs( T )							\
				typedef DC_DREEMCHEST_NS StrongPtr<class T>	T##Ptr;	\
				typedef DC_DREEMCHEST_NS WeakPtr<class T>	T##WPtr;

#define dcDeclareNamedPtrs( T, name )					\
				typedef DC_DREEMCHEST_NS StrongPtr<class T>	name##Ptr;	\
				typedef DC_DREEMCHEST_NS WeakPtr<class T>	name##WPtr;

// ** Random
#define		RANDOM_SCALAR( min, max ) ((rand() / float( RAND_MAX ) * ( (max) - (min) )) + (min))
#define		RANDOM_COLOR( min, max ) (Rgb(	RANDOM_SCALAR( (min).r, (max).r ),	\
											RANDOM_SCALAR( (min).g, (max).g ),  \
											RANDOM_SCALAR( (min).b, (max).b ) ))

#define DC_CHECK_IMPL( ... )											\
            if( m_impl == NULL ) {												\
                LogWarning( "%s : not implemented\n", __FUNCTION__ );	\
                return __VA_ARGS__;										\
            }

#define DC_DECLARE_IS( type, name, value )                                  \
        public:                                                             \
            virtual type* is##name( void ) { return value; }                \
            virtual const type* is##name( void ) const { return value; }

#endif        /*    !__Base_Preprocessor_H__    */
