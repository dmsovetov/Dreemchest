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

#include "AssetSource.h"
#include "Asset.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

// ** Asset::Asset
Asset::Asset( void )
    : m_state( Unloaded )
    , m_cache( NULL )
{
}

// ** Asset::Asset
Asset::Asset( const TypeId& type, AbstractAssetCache* cache, const Index& data, const AssetId& uniqueId, SourceUPtr source )
    : m_source( source )
    , m_type( type )
    , m_uniqueId( uniqueId )
    , m_state( Unloaded )
    , m_data( data )
    , m_cache( cache )
{
    memset( &m_timestamp, 0, sizeof( Timestamp ) );
}

// ** Asset::type
const TypeId& Asset::type( void ) const
{
    return m_type;
}

// ** Asset::state
Asset::State Asset::state( void ) const
{
    return m_state;
}

// ** Asset::uniqueId
const AssetId& Asset::uniqueId( void ) const
{
    return m_uniqueId;
}

// ** Asset::name
const String& Asset::name( void ) const
{
    return m_name;
}

// ** Asset::setName
void Asset::setName( const String& value )
{
    m_name = value;
}

// ** Asset::source
AbstractSource& Asset::source( void ) const
{
    return *m_source.get();
}

// ** Asset::switchToState
void Asset::switchToState( State value )
{
    NIMBLE_BREAK_IF( m_state == value );
    m_state = value;
}

// ** Asset::dataIndex
const Index& Asset::dataIndex( void ) const
{
    return m_data;
}

// ** Asset::timestamp
const Asset::Timestamp& Asset::timestamp( void ) const
{
    return m_timestamp;
}

// ** Asset::isUpToDate
bool Asset::isUpToDate( void ) const
{
    // Unloaded assets are always up-to-date
    if( !isLoaded() ) {
        return true;
    }

    // If an asset source was modified later that this asset was constructed, then an asset is up-to-date
    if( m_source->lastModified() <= m_timestamp.constructed ) {
        return true;
    }

    return false;
}

} // namespace Assets

DC_END_DREEMCHEST
