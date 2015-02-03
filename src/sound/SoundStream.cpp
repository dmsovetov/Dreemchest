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

#include "SoundStream.h"

namespace dreemchest {

namespace sound {

// ---------------------------------------- StandardStreamOpener ---------------------------------------- //

// ** StandardStreamOpener::open
ISoundStream* StandardStreamOpener::open( const char* uri )
{
    return StandardSoundStream::open( uri );
}

// ---------------------------------------- StandardSoundStream ----------------------------------------- //

// ** StandardSoundStream::StandardSoundStream
StandardSoundStream::StandardSoundStream( FILE* file ) : m_file( file )
{
    u32 savedPosition = position();
    setPosition( 0, SeekEnd );
    m_length = position();
    setPosition( savedPosition, SeekSet );
}

StandardSoundStream::~StandardSoundStream( void )
{
    fclose( m_file );
}

// ** StandardSoundStream::open
ISoundStream* StandardSoundStream::open( const char* uri )
{
    FILE* file = fopen( uri, "rb" );
    return file ? DC_NEW StandardSoundStream( file ) : NULL;
}

// ** StandardSoundStream::release
void StandardSoundStream::release( void )
{
    delete this;
}

// ** StandardSoundStream::length
u32 StandardSoundStream::length( void ) const
{
    return m_length;
}

// ** StandardSoundStream::loadToRam
ISoundStream* StandardSoundStream::loadToRam( void ) const
{
    DC_NOT_IMPLEMENTED;
    return NULL;
}

// ** StandardSoundStream::read
u32 StandardSoundStream::read( void* buffer, u32 size )
{
    DC_BREAK_IF( buffer == NULL );
    DC_BREAK_IF( size <= 0 );

    return fread( buffer, 1, size, m_file );
}

// ** StandardSoundStream::setPosition
void StandardSoundStream::setPosition( u32 offset, SeekOrigin origin )
{
    u32 result = 0;

    switch( origin ) {
    case SeekSet: result = fseek( m_file, offset, SEEK_SET ); break;
    case SeekCur: result = fseek( m_file, offset, SEEK_CUR ); break;
    case SeekEnd: result = fseek( m_file, offset, SEEK_END ); break;
    }
    
    DC_BREAK_IF( result != 0 );
}

// ** StandardSoundStream::position
u32 StandardSoundStream::position( void ) const
{
    return ftell( m_file );
}

} // namespace sound

} // namespace dreemchest