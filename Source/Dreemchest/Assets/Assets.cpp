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

#include "Assets.h"

#include "AssetHandle.h"
#include "AssetFormat.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

// ---------------------------------------------- Asset ---------------------------------------------- //

// ** Asset::Asset
Asset::Asset( void ) : m_state( Unloaded )
{
}

// ** Asset::Asset
Asset::Asset( Type type, const AssetId& uniqueId, FormatUPtr format )
    : m_format( format ), m_type( type ), m_uniqueId( uniqueId ), m_state( Unloaded )
{
}

// ** Asset::type
const Type& Asset::type( void ) const
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

// ** Asset::format
Format* Asset::format( void ) const
{
    return m_format.get();
}

// ** Asset::switchToState
void Asset::switchToState( State value )
{
    DC_BREAK_IF( m_state == value );
    m_state = value;
}

// ** Asset::setCache
void Asset::setCache( Index value )
{
    m_cache = value;
}

// --------------------------------------------- Assets --------------------------------------------- //

// ** Assets::Assets
Assets::Assets( void )
{
}

// ** Assets::Assets
Assets::~Assets( void )
{
    for( AssetCaches::iterator i = m_cache.begin(), end = m_cache.end(); i != end; ++i ) {
        delete i->second;
    }
}

// ** Assets::addAsset
Handle Assets::addAsset( const Type& type, const AssetId& uniqueId, FormatUPtr format )
{
    DC_BREAK_IF( m_indexById.find( uniqueId ) != m_indexById.end() );

    // First reserve the slot for an asset data.
    Index index = m_assets.add( Asset( type, uniqueId, format ) );

    // Now register unique id associated with this asset slot.
    m_indexById[uniqueId] = index;

    // Construct an asset handle.
    return Handle( this, index );
}

// ** Assets::removeAsset
bool Assets::removeAsset( const AssetId& id )
{
    // Find index by id.
    AssetIndexById::iterator i = m_indexById.find( id );
    DC_BREAK_IF( i == m_indexById.end() );

    // Store the slot before and remove the handle mapping.
    Index index = i->second;
    m_indexById.erase( i );

    // Now release an asset data
    return m_assets.remove( index );
}

// ** Assets::findAsset
Handle Assets::findAsset( const AssetId& id ) const
{
    AssetIndexById::const_iterator i = m_indexById.find( id );

    if( i == m_indexById.end() ) {
        return Handle();
    }

    return Handle( const_cast<Assets*>( this ), i->second );
}

// ** Assets::assetAtIndex
const Asset& Assets::assetAtIndex( Index index ) const
{
    return m_assets.get( index );
}

// ** Assets::isIndexValid
bool Assets::isIndexValid( Index index ) const
{
    return m_assets.has( index );
}

// ** Assets::releaseWriteLock
void Assets::releaseWriteLock( const Handle& asset )
{
    asset->m_lastModified = Platform::currentTime();
}

// ** Assets::loadAssetToCache
bool Assets::loadAssetToCache( Handle asset )
{
    if( asset->state() != Asset::Unloaded ) {
        return true;
    }

    // Perform loading
    LogVerbose( "cache", "loading '%s'...\n", asset->name().c_str() );

    // Switch to Loading state
    asset->switchToState( Asset::Loading );

    // Reserve asset data before loading
    asset->setCache( reserveAssetData( asset->type() ) );

    // Get the asset format
    Format* format = asset->format();
    DC_BREAK_IF( !format );

    // Parse asset
    bool result = format->parse( *this, asset );

    // Switch to a Loaded or Error state
    asset->switchToState( result ? Asset::Loaded : Asset::Error );

    return result;
}

// ** Assets::update
void Assets::update( f32 dt )
{
    // Process the loading queue
    while( !m_loadingQueue.empty() ) {
        // Get the first asset in loading queue
        Handle asset = *m_loadingQueue.begin();
        m_loadingQueue.pop_front();

        // Load an asset to cache
        loadAssetToCache( asset );
    }
}

// ** Assets::queue
void Assets::queueForLoading( const Handle& asset ) const
{
    // Make sure this asset should be loaded
    if( asset->state() != Asset::Unloaded ) {
        return;
    }

    // First check if this asset is already in queue
    AssetList::const_iterator i = std::find( m_loadingQueue.begin(), m_loadingQueue.end(), asset );

    // Just skip if it was found
    if( i != m_loadingQueue.end() ) {
        return;
    }

    m_loadingQueue.push_back( asset );
    LogVerbose( "cache", "asset '%s' is queued for loading\n", asset->name().c_str() );
}

// ** Assets::reserveAssetData
Index Assets::reserveAssetData( const Type& type )
{
    AssetCaches::iterator i = m_cache.find( type );

    if( i != m_cache.end() ) {
        return i->second->reserve();
    }

    return Index();
}

} // namespace Assets

DC_END_DREEMCHEST