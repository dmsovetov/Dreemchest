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

IMPLEMENT_LOGGER( log )

// ---------------------------------------------- Asset ---------------------------------------------- //

// ** Asset::Asset
Asset::Asset( void ) : m_state( Unloaded )
{
}

// ** Asset::Asset
Asset::Asset( AssetType type, const AssetId& uniqueId, AbstractAssetFormat* format )
    : m_format( format ), m_type( type ), m_uniqueId( uniqueId ), m_state( Unloaded )
{
}

// ** Asset::type
const AssetType& Asset::type( void ) const
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
AbstractAssetFormat* Asset::format( void ) const
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
void Asset::setCache( SlotIndex32 value )
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
AssetHandle Assets::addAsset( const AssetType& type, const AssetId& uniqueId, AbstractAssetFormat* format )
{
    DC_BREAK_IF( m_slotById.find( uniqueId ) != m_slotById.end() );

    // First reserve the slot for an asset data.
    SlotIndex32 slot = m_assets.add( Asset( type, uniqueId, format ) );

    // Now register unique id associated with this asset slot.
    m_slotById[uniqueId] = slot;

    // Construct an asset handle.
    return AssetHandle( this, slot );
}

// ** Assets::removeAsset
bool Assets::removeAsset( const AssetId& id )
{
    // Find handle by id.
    AssetSlotsById::iterator i = m_slotById.find( id );
    DC_BREAK_IF( i == m_slotById.end() );

    // Store the slot before and remove the handle mapping.
    SlotIndex32 slot = i->second;
    m_slotById.erase( i );

    // Now release an asset data
    return m_assets.remove( slot );
}

// ** Assets::findAsset
AssetHandle Assets::findAsset( const AssetId& id ) const
{
    AssetSlotsById::const_iterator i = m_slotById.find( id );

    if( i == m_slotById.end() ) {
        return AssetHandle();
    }

    return AssetHandle( const_cast<Assets*>( this ), i->second );
}

// ** Assets::assetAtSlot
const Asset& Assets::assetAtSlot( SlotIndex32 slot ) const
{
    return m_assets.get( slot );
}

// ** Assets::isValidSlot
bool Assets::isValidSlot( SlotIndex32 slot ) const
{
    return m_assets.has( slot );
}

// ** Assets::releaseWriteLock
void Assets::releaseWriteLock( const AssetHandle& asset )
{
    asset->m_lastModified = Platform::currentTime();
}

// ** Assets::loadAssetToCache
bool Assets::loadAssetToCache( AssetHandle asset )
{
    if( asset->state() != Asset::Unloaded ) {
        return true;
    }

    // Perform loading
    log::verbose( "Loading '%s'...\n", asset->name().c_str() );

    // Switch to Loading state
    asset->switchToState( Asset::Loading );

    // Reserve asset data before loading
    asset->setCache( reserveAssetData( asset->type() ) );

    // Get the asset format
    AbstractAssetFormat* format = asset->format();
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
        AssetHandle asset = *m_loadingQueue.begin();
        m_loadingQueue.pop_front();

        // Load an asset to cache
        loadAssetToCache( asset );
    }
}

// ** Assets::queue
void Assets::queueForLoading( const AssetHandle& asset ) const
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
    log::verbose( "Asset '%s' is queued for loading\n", asset->name().c_str() );
}

// ** Assets::reserveAssetData
SlotIndex32 Assets::reserveAssetData( const AssetType& type )
{
    AssetCaches::iterator i = m_cache.find( type );

    if( i != m_cache.end() ) {
        return i->second->reserve();
    }

    return SlotIndex32();
}

} // namespace Assets

DC_END_DREEMCHEST