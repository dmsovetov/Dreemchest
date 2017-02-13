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

#include "AssetLoadingQueue.h"
#include "Asset.h"
#include "AssetHandle.h"
#include "AssetSource.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

// ** LoadingQueue::LoadingQueue
LoadingQueue::LoadingQueue( Assets& assets, s32 maxAssetsToLoad )
    : m_assets( assets )
    , m_maxAssetsToLoad( maxAssetsToLoad )
{
    NIMBLE_BREAK_IF( maxAssetsToLoad <= 0, "maximum number of assets to load is expected to be a positive number" );
}

// ** LoadingQueue::update
void LoadingQueue::update( void )
{
    // The maximum number of assets that can be loaded in a single frame
    s32 maximumAssetsToLoad = m_maxAssetsToLoad;

    // Process the loading queue
    while( !m_queue.empty() && maximumAssetsToLoad-- ) {
        // Get the first asset in loading queue
        Handle handle = *m_queue.begin();
        m_queue.pop_front();

        // Load an asset to cache
        loadToCache( handle );
    }
}

// ** LoadingQueue::loadToCache
bool LoadingQueue::loadToCache( Handle asset )
{
    if( asset->state() == Asset::Unloaded ) {
        LogDebug( "loadingQueue", "forcing asset '%s' to be loaded\n", asset->name().c_str() );
        asset->switchToState( Asset::WaitingForLoading );
    }

    if( asset->state() != Asset::WaitingForLoading ) {
        LogWarning( "loadingQueue", "asset '%s' is waiting for loading\n", asset->name().c_str() );
        return true;
    }

    // Perform loading
    LogVerbose( "loadingQueue", "loading '%s'...\n", asset->name().c_str() );

    // Switch to Loading state
    asset->switchToState( Asset::Loading );

    // Get the asset format
    AbstractSource& source = asset->source();

    // Parse asset
    bool result = source.construct( m_assets, asset );

    if( !result ) {
        LogWarning( "loadingQueue", "'%s' was failed to load\n", asset->name().c_str() );
    }

    // Switch to a Loaded or Error state
    asset->switchToState( result ? Asset::Loaded : Asset::Error );

    // Update the last constructed time stamp
    asset->m_timestamp.constructed = Time::current();

    // Notify listeners about an asset loading completion
    if( result ) {
        m_assets.queueLoaded( asset );
    }

    // Output the log message
    LogDebug( "loadingQueue", "%s loaded\n", asset->name().c_str() );

    return result;
}

// ** LoadingQueue::queue
void LoadingQueue::queue( Handle asset )
{
    // Make sure this asset should be loaded
    if( asset->state() != Asset::Unloaded ) {
        return;
    }

    // First check if this asset is already in queue
    AssetList::const_iterator i = std::find( m_queue.begin(), m_queue.end(), asset );

    // Just skip if it was found
    if( i != m_queue.end() ) {
        return;
    }

    // Push an asset to a loading queue
    m_queue.push_back( asset );
    LogVerbose( "loadingQueue", "asset '%s' is queued for loading\n", asset->name().c_str() );

    // Swith asset state
    asset->switchToState( Asset::WaitingForLoading );
}

// ** LoadingQueue::maxAssetsToLoad
s32 LoadingQueue::maxAssetsToLoad( void ) const
{
    return m_maxAssetsToLoad;
}

// ** LoadingQueue::setMaxAssetsToLoad
void LoadingQueue::setMaxAssetsToLoad( s32 value )
{
    NIMBLE_BREAK_IF( value <= 0, "maximum number of assets to load is expected to be a positive number" );
    m_maxAssetsToLoad = value;
}

} // namespace Assets

DC_END_DREEMCHEST
