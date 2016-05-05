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

#ifndef DREEMCHEST_H
#define DREEMCHEST_H

#define dcInterface

#ifdef DC_BUILD_ENGINE
	#ifndef DC_NAMESPACE
		#define DC_NAMESPACE    dreemchest
	#endif
#endif

#ifdef DC_NAMESPACE
    #define DC_USE_DREEMCHEST    using namespace DC_NAMESPACE;
	#define DC_DREEMCHEST_NS	 DC_NAMESPACE::
    #define DC_BEGIN_DREEMCHEST  namespace DC_NAMESPACE {
    #define DC_END_DREEMCHEST    }
#else
    #define DC_USE_DREEMCHEST
	#define DC_DREEMCHEST_NS
    #define DC_BEGIN_DREEMCHEST
    #define DC_END_DREEMCHEST
#endif

#include <Nimble.h>

#include "Deprecated.h"

//! Wrap the Nimble logger tag definition
#define DREEMCHEST_LOGGER_TAG( tag ) NIMBLE_LOGGER_TAG( tag )

#define LogDebug( prefix, message, ... )     Log::debug( NIMBLE_LOGGER_CONTEXT, prefix, message, __VA_ARGS__ )
#define LogVerbose( prefix, message, ... )   Log::verbose( NIMBLE_LOGGER_CONTEXT, prefix, message, __VA_ARGS__ )
#define LogMilestone( prefix, message, ... ) Log::milestone( NIMBLE_LOGGER_CONTEXT, prefix, message, __VA_ARGS__ )
#define LogWarning( prefix, message, ... )   Log::warn( NIMBLE_LOGGER_CONTEXT, prefix, message, __VA_ARGS__ )
#define LogError( prefix, message, ... )     Log::error( NIMBLE_LOGGER_CONTEXT, prefix, message, __VA_ARGS__ )
#define LogFatal( prefix, message, ... )     Log::fatal( NIMBLE_LOGGER_CONTEXT_FULL, prefix, message, __VA_ARGS__ )
#define LogInternal( prefix, message, ... )  Log::internalError( NIMBLE_LOGGER_CONTEXT_FULL, prefix, message, __VA_ARGS__ )

#ifdef HAVE_JSON
	#include <json/json.h>
#endif	/*	HAVE_JSON	*/

DC_BEGIN_DREEMCHEST

    namespace Platform {
    
        //! Returns current time in milliseconds.
	    extern u32 currentTime( void );

    } // namespace Platform

    namespace Io {
    
        dcDeclarePtrs( Stream )

    } // namespace Io

    namespace Assets {
    
        NIMBLE_LOGGER_TAG( Assets )

        class Assets;
        class Asset;
        class AbstractAssetCache;

        //! Opaque 32 bit handle.
        typedef OpaqueHandle<12, 20> Index;

        //! Asset index hasher.
        typedef OpaqueHandleHasher<Index> IndexHasher;

        //! Forward declaration of an AssetDataHandle type.
        template<typename TAsset> class DataHandle;

        //! Forward declaration of an asset WriteLock type.
        template<typename TAsset> class WriteLock;

        //! Asset identifier type.
        typedef String AssetId;

        //! Asset type identifier type.
        typedef u16 TypeId;

        //! Set of assets.
        typedef Set<class Handle> AssetSet;

        //! List of assets.
        typedef List<class Handle> AssetList;

        // Unique ptr for asset source.
        typedef AutoPtr<class AbstractSource> SourceUPtr;

        //! Loading queue unique pointer type.
        typedef AutoPtr<class LoadingQueue> LoadingQueueUPtr;

        dcDeclarePtrs( Assets )

    } // namespace Assets

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include <Network/Network.h>
	#include <Io/Io.h>
    #include <Assets/Assets.h>
	#include <Event/Event.h>
	#include <Threads/Threads.h>
    #include <Reflection/Reflection.h>
	#include <Ecs/Ecs.h>
	#include <Platform/Platform.h>
	#include <Threads/Threads.h>
	#include <Scene/Scene.h>
	#include <Renderer/Renderer.h>
	#include <Sound/Sound.h>
	#include <Fx/Fx.h>
#endif

#endif  /*  !defined( DREEMCHEST_H )    */
