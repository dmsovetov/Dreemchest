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

#ifndef __DC_Assets_AssetLoadingQueue_H__
#define __DC_Assets_AssetLoadingQueue_H__

#include "../Dreemchest.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

    //! Asset loading queue performs loading of assets.
    class LoadingQueue {
    friend class Assets;
    public:

                        //! Constructs LoadingQueue instance.
                        LoadingQueue( Assets& assets, s32 maxAssetsToLoad = 1 );

        //! Updates loading queue.
        void            update( void );

        //! Adds an asset to a queue.
        void            queue( Handle asset );

        //! Returns the maximum number of assets that can be loaded in a single frame.
        s32             maxAssetsToLoad( void ) const;

        //! Sets the maximum number of assets that can be loaded in a single frame.
        void            setMaxAssetsToLoad( s32 value );

    private:

        //! Loads a single asset to a cache.
        bool            loadToCache( Handle asset );

    private:

        Assets&         m_assets;           //!< Parent asset manager.
        s32             m_maxAssetsToLoad;  //!< Maximum number of assets that can be loaded in a single frame.
        AssetList       m_queue;            //!< All queued assets are put to this list.
    };

} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_AssetLoadingQueue_H__    */