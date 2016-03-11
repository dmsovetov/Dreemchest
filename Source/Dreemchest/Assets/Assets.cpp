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
Asset::Asset( void )
    : m_state( Unloaded )
    , m_cache( NULL )
{
}

// ** Asset::Asset
Asset::Asset( Type type, void* cache, const AssetId& uniqueId, SourceUPtr source )
    : m_source( source )
    , m_type( type )
    , m_uniqueId( uniqueId )
    , m_state( Unloaded )
    , m_cache( cache )
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
AbstractSource* Asset::source( void ) const
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

    // Find an asset cache for this type of asset
    AbstractAssetCache* cache = findAssetCache( type );
    DC_BREAK_IF( !cache, "no cache for this type of asset" );

    // First reserve the slot for an asset data.
    Index index = m_assets.add( Asset( type, cache, uniqueId, source ) );


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

// ** Assets::typeFromName
Type Assets::typeFromName( const String& value ) const
{
    Map<String, Type>::const_iterator i = m_nameToType.find( value );
    
    if( i == m_nameToType.end() ) {
        DC_BREAK;
        return Type();
    }

    return i->second;
}

// ** Assets::assetTypeName
String Assets::assetTypeName( const Type& type ) const
{
    Map<Type, String>::const_iterator i = m_typeToName.find( type );
    
    if( i == m_typeToName.end() ) {
        DC_BREAK;
        return "";
    }

    return i->second;
}

// ** Assets::releaseWriteLock
void Assets::releaseWriteLock( const Handle& asset )
{
    asset->m_lastModified = Platform::currentTime();
}

// ** Assets::findAssetCache
Assets::AbstractAssetCache* Assets::findAssetCache( const Type& type ) const
{
    AssetCaches::iterator i = m_cache.find( type );

    if( i != m_cache.end() ) {
        return i->second;
    }

    return NULL;
}

// ** Assets::loadAssetToCache
bool Assets::loadAssetToCache( Asset& asset )
{
    if( asset.state() != Asset::Unloaded ) {
        return true;
    }

    // Perform loading
    LogVerbose( "cache", "loading '%s'...\n", asset->name().c_str() );

    // Switch to Loading state
    asset.switchToState( Asset::Loading );

    // Reserve asset data before loading
    if( !asset.hasData() ) {
        asset.setData( reserveAssetData( asset.type() ) );
    }

    // Get the asset format
    AbstractSource* source = asset.source();
    DC_ABORT_IF( source == NULL, "invalid asset source" );

    // Parse asset
    Handle handle = createHandle( asset );
    bool   result = source->construct( *this, handle );

    // Switch to a Loaded or Error state
    asset.switchToState( result ? Asset::Loaded : Asset::Error );

    // Update the last constructed time stamp
    asset.m_lastConstructed = Time::current();

    return result;
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

        // Skip unloaded assets
        if( asset.state() != Asset::Loaded ) {
            continue;
        }

        // Compare the asset source last modified timestamp with last constructed one
        const AbstractSource* source = asset.source();
        DC_ABORT_IF( source == NULL, "asset has no valid asset source" );

        // Skip up-to-date assets
        if( source->lastModified() <= asset.lastModified() ) {
            continue;
        }

        // Mark this asset as unloaded
        asset.switchToState( Asset::Unloaded );

        // Queue asset for reloading
        LogVerbose( "cache", "reloading '%s'\n", asset.name().c_str() );
        queueForLoading( asset );
    }

    // Process the loading queue
    while( !m_loadingQueue.empty() ) {
        // Get the first asset in loading queue
        Handle handle = *m_loadingQueue.begin();
        m_loadingQueue.pop_front();

        // Load an asset to cache
        loadAssetToCache( handle.asset() );
    }
}

// ** Assets::queueForLoading
void Assets::queueForLoading( const Asset& asset ) const
{
    // Make sure this asset should be loaded
    if( asset.state() != Asset::Unloaded ) {
        return;
    }

    // Get an asset handle by a unique id
    AssetIndexById::const_iterator j = m_indexById.find( asset.uniqueId() );
    DC_ABORT_IF( j == m_indexById.end(), "unknown asset id" );

    // Construct asset handle
    Handle handle( const_cast<Assets*>( this ), j->second );

    // First check if this asset is already in queue
    AssetList::const_iterator i = std::find( m_loadingQueue.begin(), m_loadingQueue.end(), handle );

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

// ** Assets::createHandle
Handle Assets::createHandle( const Asset& asset ) const
{
    return findAsset( asset.uniqueId() );
}

} // namespace Assets

DC_END_DREEMCHEST