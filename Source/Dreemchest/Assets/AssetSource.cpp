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

#include "Asset.h"
#include "AssetSource.h"
#include "AssetHandle.h"

#include <Io/DiskFileSystem.h>
#include <Io/Streams/Stream.h>

DC_BEGIN_DREEMCHEST

namespace Assets {

// ---------------------------------------------- NullSource ---------------------------------------------- //

// ** NullSource::construct
bool NullSource::construct( Assets& assets, Handle asset )
{
    return true;
}

// ** NullSource::lastModified
u32 NullSource::lastModified( void ) const
{
    return 0;
}

// ------------------------------------------ AbstractFileSource ------------------------------------------ //

// ** AbstractFileSource::AbstractFileSource
AbstractFileSource::AbstractFileSource( void )
    : m_lastModified( 0 )
{
}

// ** AbstractFileSource::construct
bool AbstractFileSource::construct( Assets& assets, Handle asset )
{
    Io::StreamPtr stream = Io::DiskFileSystem::open( m_fileName );

    if( !stream.valid() ) {
        return false;
    }

    bool result = constructFromStream( stream, assets, asset );
    return result;
}

// ** AbstractFileSource::lastModified
u32 AbstractFileSource::lastModified( void ) const
{
    return m_lastModified;
}

// ** AbstractFileSource::setLastModified
void AbstractFileSource::setLastModified( u32 value )
{
    m_lastModified = value;
}

// ** AbstractFileSource::fileName
const String& AbstractFileSource::fileName( void ) const
{
    return m_fileName;
}

// ** AbstractFileSource::fileName
void AbstractFileSource::setFileName( const String& value )
{
    m_fileName = value;
}

} // namespace Assets

DC_END_DREEMCHEST