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

#include "Storage.h"
#include "BinaryStorage.h"
#include "Serializable.h"

#ifdef HAVE_JSONCPP
    #include "JsonStorage.h"
#endif

DC_BEGIN_DREEMCHEST

namespace io {

// ** Storage::Storage
Storage::Storage( StorageType type, const StreamPtr& stream ) : m_impl( NULL )
{
    if( stream == NULL ) {
        log::warn( "Storage::Storage : invalid stream\n" );
        return;
    }

    switch( type ) {
    case StorageBinary:
            m_impl = new BinaryStorage( stream );
            break;
    case StorageJson:
        #ifdef HAVE_JSONCPP
            m_impl = new JsonStorage( stream );
        #else
            log::warn( "Storage::Storage : JSON storage is not available\n" );
        #endif
            break;

    case StorageXml:
            log::warn( "Storage::Storage : XML storage is not available\n" );
            break;

    case StorageYaml:
            log::warn( "Storage::Storage : Yaml storage is not available\n" );
            break;
    }
}

// ** Storage::Storage
Storage::~Storage( void )
{
    delete m_impl;
}

// ** Storage::operator bool
Storage::operator bool( void ) const
{
    return m_impl != NULL;
}

// ** Storage::pushObjectRead
void Storage::pushObjectRead( CString key ) const
{
    m_impl->pushObjectRead( key );
}

// ** Storage::popObjectRead
void Storage::popObjectRead( void ) const
{
    m_impl->popObjectRead();
}

// ** Storage::pushObjectWrite
void Storage::pushObjectWrite( CString key )
{
    m_impl->pushObjectWrite( key );
}

// ** Storage::popObjectWrite
void Storage::popObjectWrite( void )
{
    m_impl->popObjectWrite();
}

// ** Storage::write
void Storage::write( CString key, const Serializable& value )
{
    value.write( *this, key );
}

// ** Storage::read
void Storage::read( CString key, Serializable& value ) const
{
    value.read( *this, key );
}

// ** Storage::pushArrayWrite
void Storage::pushArrayWrite( CString key, u32 size )
{
    m_impl->pushArrayWrite( key, size );
}

// ** Storage::popArrayWrite
void Storage::popArrayWrite( void )
{
    m_impl->popArrayWrite();
}

// ** Storage::pushItemWrite
void Storage::pushItemWrite( u32 index )
{
    m_impl->pushItemWrite( index );
}

// ** Storage::popItemWrite
void Storage::popItemWrite( void )
{
    m_impl->popItemWrite();
}

// ** Storage::pushArrayRead
u32 Storage::pushArrayRead( CString key ) const
{
    return m_impl->pushArrayRead( key );
}

// ** Storage::popArrayRead
void Storage::popArrayRead( void ) const
{
    m_impl->popArrayRead();
}

} // namespace io

DC_END_DREEMCHEST
