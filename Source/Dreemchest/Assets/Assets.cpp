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

#include "Asset.h"
#include "AssetHandle.h"
#include "AssetSource.h"
DC_BEGIN_DREEMCHEST

namespace Assets {

// ** Assets::Assets
Assets::Assets( void )
{
    m_loadingQueue = DC_NEW LoadingQueue( *this, INT_MAX );
}

// ** Assets::Assets
Assets::~Assets( void )
{
    for( AssetCaches::iterator i = m_cache.begin(), end = m_cache.end(); i != end; ++i ) {
        delete i->second;
    }
}

// ** Assets::addAsset
Handle Assets::addAsset( const TypeId& type, const AssetId& uniqueId, SourceUPtr source )
{
    NIMBLE_BREAK_IF( m_indexById.find( uniqueId ) != m_indexById.end() );

    // Find an asset cache for this type of asset
    AbstractAssetCache& cache = findAssetCache( type );

    // Setup a new asset
    Asset asset( type, &cache, cache.reserve(), uniqueId, source );

    // First reserve the slot for an asset data.
    Index index = m_assets.add( asset );

    // Now register unique id associated with this asset slot.
    m_indexById[uniqueId] = index;

    LogDebug( "asset", "%s %s added (%d assets of a same type, %d assets total)\n", assetTypeName( type ).c_str(), uniqueId.c_str(), cache.size(), m_assets.size() );

    // Construct an asset handle.
    return Handle( this, index );
}

// ** Assets::removeAsset
bool Assets::removeAsset( const AssetId& id )
{
    // Find index by id.
    AssetIndexById::iterator i = m_indexById.find( id );
    NIMBLE_BREAK_IF( i == m_indexById.end() );

    // Store the slot before and remove the handle mapping.
    Index index = i->second;
    m_indexById.erase( i );

    // Output log message
    const Asset& asset = m_assets.get( index );
    AbstractAssetCache& cache = findAssetCache( asset.type() );
    LogDebug( "asset", "%s %s removed (%d assets of a same type, %d assets total)\n", assetTypeName( asset.type() ).c_str(), id.c_str(), cache.size(), m_assets.size() );

    // Now release an asset data
    return m_assets.remove( index );
}

// ** Assets::totalBytesUsed
s32 Assets::totalBytesUsed( void ) const
{
    s32 result = 0;

    for( AssetCaches::const_iterator i = m_cache.begin(), end = m_cache.end(); i != end; ++i ) {
        result += i->second->allocatedBytes();
    }

    return result;
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

// ** Assets::typeFromName
TypeId Assets::typeFromName( const String& value ) const
{
    Map<String, TypeId>::const_iterator i = m_nameToType.find( value );
    
    if( i == m_nameToType.end() ) {
        NIMBLE_BREAK;
        return 0;
    }

    return i->second;
}

// ** Assets::assetTypeName
String Assets::assetTypeName( const TypeId& type ) const
{
    Map<TypeId, String>::const_iterator i = m_typeToName.find( type );
    
    if( i == m_typeToName.end() ) {
        NIMBLE_BREAK;
        return "";
    }

    return i->second;
}

// ** Assets::releaseWriteLock
void Assets::releaseWriteLock( const Handle& asset )
{
    asset->m_timestamp.modified = Platform::currentTime();
}

// ** Assets::queueLoaded
void Assets::queueLoaded( const Handle& asset )
{
    m_loadedAssets.push_back( asset );
}

// ** Assets::queueUnloaded
void Assets::queueUnloaded( const Handle& asset )
{
    m_unloadedAssets.push_back( asset );
}

// ** Assets::findAssetCache
AbstractAssetCache& Assets::findAssetCache( const TypeId& type ) const
{
    AssetCaches::iterator i = m_cache.find( type );

    if( i != m_cache.end() ) {
        return *i->second;
    }

    NIMBLE_ABORT_IF( true, "unknown asset type" );
    return *reinterpret_cast<AbstractAssetCache*>( NULL );
}

// ** Assets::update
void Assets::update( f32 dt )
{
    // Save current time
    m_currentTime = Platform::currentTime();

    // Check outdated assets
    for( s32 i = 0, n = m_assets.size(); i < n; i++ ) {
        // Get an asset by index
        Asset& asset = m_assets.dataAt( i );

        // Skip up-to-date assets
        if( asset.isUpToDate() ) {
            continue;
        }

        LogVerbose( "cache", "asset '%s' was changed\n", asset.name().c_str() );

        // Create an asset handle
        Handle assetHandle = createHandle( asset );

        // Unload an asset
        forceUnload( assetHandle );

        // Queue asset for reloading
        m_loadingQueue->queue( assetHandle );
    }

    // Update the loading queue
    m_loadingQueue->update();

    // Notify listeners about asset state changes
    for( AssetList::iterator i = m_unloadedAssets.begin(), end = m_unloadedAssets.end(); i != end; ++i ) {
        notify<Unloaded>( *this, i->asset() );
    }
    m_unloadedAssets.clear();

    for( AssetList::iterator i = m_loadedAssets.begin(), end = m_loadedAssets.end(); i != end; ++i ) {
        notify<Loaded>( *this, i->asset() );
    }
    m_loadedAssets.clear();
}

// ** Assets::createHandle
Handle Assets::createHandle( const Asset& asset ) const
{
    return findAsset( asset.uniqueId() );
}

// ** Assets::forceLoad
bool Assets::forceLoad( const Handle& asset )
{
    return m_loadingQueue->loadToCache( asset );
}

// ** Assets::forceUnload
void Assets::forceUnload( Handle asset )
{
    NIMBLE_BREAK_IF( !asset->isLoaded(), "an asset was not loaded" );

    // Mark this asset as unloaded
    asset->switchToState( Asset::Unloaded );

    // Queue an asset for notification
    m_unloadedAssets.push_front( asset );

    LogVerbose( "cache", "asset '%s' unloaded\n", asset->name().c_str() );
}

} // namespace Assets

DC_END_DREEMCHEST
