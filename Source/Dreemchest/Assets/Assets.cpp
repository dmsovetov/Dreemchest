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
#include "AssetSource.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

// ---------------------------------------------- Asset ---------------------------------------------- //

// ** Asset::Asset
Asset::Asset( void ) : m_state( Unloaded )
{
}

// ** Asset::Asset
Asset::Asset( Type type, const AssetId& uniqueId, SourceUPtr source )
    : m_source( source )
    , m_type( type )
    , m_uniqueId( uniqueId )
    , m_state( Unloaded )
    , m_lastConstructed( 0 )
    , m_lastModified( 0 )
    , m_lastUsed( 0 )
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

// ** Asset::source
Source* Asset::source( void ) const
{
    return m_source.get();
}

// ** Asset::switchToState
void Asset::switchToState( State value )
{
    DC_BREAK_IF( m_state == value );
    m_state = value;
}

// ** Asset::setData
void Asset::setData( Index value )
{
    m_data = value;
}

// ** Asset::data
Index Asset::data( void ) const
{
    return m_data;
}

// ** Asset::hasData
bool Asset::hasData( void ) const
{
    return data().isValid();
}

// ** Asset::lastModified
u32 Asset::lastModified( void ) const
{
    return m_lastModified;
}

// ** Asset::lastUsed
u32 Asset::lastUsed( void ) const
{
    return m_lastUsed;
}

// ** Asset::lastConstructed
u32 Asset::lastConstructed( void ) const
{
    return m_lastConstructed;
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
Handle Assets::addAsset( const Type& type, const AssetId& uniqueId, SourceUPtr source )
{
    DC_BREAK_IF( m_indexById.find( uniqueId ) != m_indexById.end() );

    // First reserve the slot for an asset data.
    Index index = m_assets.add( Asset( type, uniqueId, source ) );

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
    if( !asset->hasData() ) {
        asset->setData( reserveAssetData( asset->type() ) );
    }

    // Get the asset format
    Source* source = asset->source();
    DC_ABORT_IF( source == NULL, "invalid asset source" );

    // Parse asset
    bool result = source->parse( *this, asset );

    // Switch to a Loaded or Error state
    asset->switchToState( result ? Asset::Loaded : Asset::Error );

    // Update the last constructed time stamp
    asset->m_lastConstructed = Time::current();

    return result;
}

// ** Assets::update
void Assets::update( f32 dt )
{
    // Check outdated assets
    for( s32 i = 0, n = m_assets.size(); i < n; i++ ) {
        // Get an asset by index
        Asset& asset = m_assets.dataAt( i );

        // Skip unloaded assets
        if( asset.state() != Asset::Loaded ) {
            continue;
        }

        // Compare the asset source last modified timestamp with last constructed one
        const Source* source = asset.source();
        DC_ABORT_IF( source == NULL, "asset has no valid asset source" );

        // Skip up-to-date assets
        if( source->lastModified() <= asset.lastModified() ) {
            continue;
        }

        // Mark this asset as unloaded
        asset.switchToState( Asset::Unloaded );

        // Queue asset for reloading
        Handle handle( this, m_assets.handleAt( i ) );
        LogVerbose( "cache", "reloading '%s'\n", asset.name().c_str() );

        queueForLoading( handle );
    }

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