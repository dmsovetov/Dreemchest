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

#ifndef __Base_Logger_H__
#define __Base_Logger_H__

#pragma warning( disable : 4996 )

#ifndef DC_PLATFORM_ANDROID
	#define PRINTF( ... ) printf( __VA_ARGS__ )
#else
	#include <android/log.h>
	#define PRINTF( ... ) __android_log_print(ANDROID_LOG_INFO, "Log", __VA_ARGS__)
#endif

#define LOGGER_FORMAT( fmt )                                                \
    char	buffer[MaxMessageLength];                                       \
    va_list ap;                                                             \
    va_start(ap, fmt);                                                      \
    vsnprintf(buffer, MaxMessageLength, fmt, ap);                           \
    va_end(ap);

#define DECLARE_LOG( name )                                                 \
    class name {                                                            \
    public:                                                                 \
        enum LogType { Verbose, Normal, Warning, Error };                   \
        enum { MaxMessageLength = 4096 };                                   \
                                                                            \
        typedef void ( *LoggerCallback )( int type, const char* message );  \
                                                                            \
        static void setStandardHandler( void ) { s_callback = standardLogger; } \
		static void setLogLevel( LogType level ) { s_logLevel = level; } \
        static void registerCallback( LoggerCallback callback ) { s_callback = callback; }  \
        static void verbose( const char* fmt, ... ) { if( s_callback && Verbose >= s_logLevel ) { LOGGER_FORMAT( fmt ); s_callback( Verbose, buffer ); } }   \
        static void msg( const char* fmt, ... ) { if( s_callback && Normal >= s_logLevel ) { LOGGER_FORMAT( fmt ); s_callback( Normal, buffer ); } }   \
        static void warn( const char* fmt, ... ) { if( s_callback && Warning >= s_logLevel ) { LOGGER_FORMAT( fmt ); s_callback( Warning, buffer ); } }   \
        static void error( const char* fmt, ... ) { if( s_callback && Error >= s_logLevel ) { LOGGER_FORMAT( fmt ); s_callback( Error, buffer ); } }   \
                                                                            \
    private:                                                                \
        static void standardLogger( int type, const char* message ) { PRINTF( "%s", message ); } \
        static LoggerCallback s_callback;                                   \
		static LogType s_logLevel;											\
    };

#define IMPLEMENT_LOGGER( name )					\
	name::LoggerCallback name::s_callback = NULL;	\
	name::LogType name::s_logLevel = name::Verbose;

#endif  /*  !defined( __Base_Logger_H__ ) */
