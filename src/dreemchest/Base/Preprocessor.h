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

// ** Code to a string
#define CODE( ... ) #__VA_ARGS__

// ** Bit shifting
#define BIT( bit )                ( 1 << bit )

// ** Memory management
#define DC_RELEASE( object )        if( object != NULL ) { object->release(); object = NULL; }
#define DC_DELETE( pointer )        delete pointer;  pointer = NULL;
#define DC_DELETE_ARRAY( pointer )  delete[]pointer; pointer = NULL;

// !!Deprecated
#define DC_RELEASE_DEPRECATED( object ) if( object != NULL ) { object->Release(); object = NULL; }

// ** Function binding
#define dcThisMethod( method )              CLOSURE( this, &method )
#define dcObjectMethod( object, method )    CLOSURE( object, &method )
#define dcStaticFunction( function )        SCLOSURE( &function )

// ** Smart ptrs
#define        DC_DECLARE_PTRS( type, name )								\
                typedef WeakPtr<class type>			dc##name##Weak;			\
                typedef StrongPtr<class type>		dc##name##Strong;

#define dcDeclarePtrs( T )							\
				typedef StrongPtr<class T>	T##Ptr;	\
				typedef WeakPtr<class T>	T##WPtr;

#define dcDeclareNamedPtrs( T, name )					\
				typedef StrongPtr<class T>	name##Ptr;	\
				typedef WeakPtr<class T>	name##WPtr;

// ** Random
#define		RANDOM_SCALAR( min, max ) ((rand() / float( RAND_MAX ) * ( (max) - (min) )) + (min))
#define		RANDOM_COLOR( min, max ) (Rgb(	RANDOM_SCALAR( (min).r, (max).r ),	\
											RANDOM_SCALAR( (min).g, (max).g ),  \
											RANDOM_SCALAR( (min).b, (max).b ) ))

// ** new
#if defined( DC_DEVELOPER_BUILD ) && defined( DC_EXPORTS )
    #if DC_TRACK_MEMORY
        #define DC_NEW            new( __FILE__, __LINE__ )
    #else
        #define    DC_NEW            new
    #endif
#else
    #define    DC_NEW            new
#endif

// ** Debugging
#if defined( _DEBUG ) || defined( DC_PLATFORM_ANDROID ) || ((defined( DC_PLATFORM_IOS ) || defined( DC_PLATFORM_MACOS )) && !defined( NDEBUG ))
    #define DC_DEBUG
#endif

#ifdef NDEBUG
    #undef DC_DEBUG
#endif

#define DC_NOT_IMPLEMENTED              \
            DC_BREAK_IF( true );        \
            Exception::Error( "%s : not implemented\n", __FUNCTION__ );

#define DC_CHECK_IMPL( ... )											\
            if( m_impl == NULL ) {												\
                log::warn( "%s : not implemented\n", __FUNCTION__ );	\
                return __VA_ARGS__;										\
            }

#define DC_DECLARE_IS( type, name, value )                                  \
        public:                                                             \
            virtual type* is##name( void ) { return value; }                \
            virtual const type* is##name( void ) const { return value; }

#define DC_LOG_THIS            Message( "%s : object of type '%s' (%x)\n", __FUNCTION__, GetClassName(), this );

#if defined( DC_DEBUG )
    #if defined( DC_PLATFORM_WINDOWS )
        #if defined( _WIN64 )
            #include <crtdbg.h>
            #define DC_BREAK_IF( condition ) if( condition ) { _CrtDbgBreak(); }
        #else
            #define DC_BREAK_IF( condition ) if( condition ) { __asm { int 3 } }
        #endif

        #define DC_CHECK_MEMORY DC_BREAK_IF( _CrtCheckMemory() == 0 )
    #elif defined( DC_PLATFORM_IOS ) || defined( DC_PLATFORM_MACOS )
        #if defined __arm__ || defined __thumb__
            #define DC_BREAK_IF( condition ) if( condition ) { asm( "trap" ); }
        #elif defined(__i386__) || defined(__x86_64__)
            #define DC_BREAK_IF( condition ) if( condition ) { asm( "int $0x3" ); }
        #else
            #define DC_BREAK_IF( condition ) assert( !(condition) );
        #endif

        #define DC_CHECK_MEMORY
    #elif defined( DC_PLATFORM_ANDROID )
        #define DC_BREAK_IF( condition )        \
            if( condition )  {                    \
                char buffer[1024];                \
                sprintf( buffer, "Debug break at %s, line %d\nExpression: %s\n", __FILE__, __LINE__, #condition );    \
                __android_log_print( ANDROID_LOG_WARN, "DreemchestNative", buffer );    \
            }

        #define DC_CHECK_MEMORY
    #elif defined( DC_PLATFORM_LINUX )
        #if defined(__i386__) || defined(__x86_64__)
            #define DC_BREAK_IF( condition ) if( condition ) { asm("int $3"); }
        #else
            #include <cassert>
            #define DC_BREAK_IF( condition ) assert( !(condition) );
        #endif

        #define DC_CHECK_MEMORY
    #elif defined( DC_PLATFORM_FLASH )
        #define DC_BREAK_IF( condition )        \
            if( condition )  {                    \
                static char buffer[1024];        \
                sprintf( buffer, "Debug break at %s, line %d\nExpression: %s\n", __FILE__, __LINE__, #condition );    \
                printf( buffer );            \
            }

        #define DC_CHECK_MEMORY
    #endif

    #define DC_DEBUG_ONLY( ... )		__VA_ARGS__
    #define DC_ASSERT( condition )		DC_BREAK_IF( !condition )

    #if !defined( DC_PLATFORM_ANDROID )
        #define DC_ENSURE_TYPE( object, type )    DC_BREAK_IF( dynamic_cast<const type*>( object ) == NULL )
    #else
        #define DC_ENSURE_TYPE( object, type )
    #endif

    #define DC_BEGIN_MEASURE( id, ctx )    \
        float __time##id = ctx->iTime->GetMilliseconds( true );

    #define DC_END_MEASURE( id, message, ctx ) \
        ctx->iLog->Message( "%s in %2.2fms\n", message, ctx->iTime->GetMilliseconds( true ) - __time##id );
#else
    #define DC_BREAK_IF( condition )
    #define DC_CHECK_MEMORY
    #define DC_DEBUG_ONLY( ... )
    #define DC_ENSURE_TYPE( object, type )
#endif

#define DC_BREAK DC_BREAK_IF( true )

#endif        /*    !__Base_Preprocessor_H__    */
