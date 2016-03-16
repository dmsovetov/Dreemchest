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

#ifndef __DC_Scene_RenderingContext_H__
#define __DC_Scene_RenderingContext_H__

#include "../Scene.h"
#include "../Assets/Mesh.h"
#include "../Assets/Material.h"
#include "../Assets/Image.h"
#include "Assets/Renderable.h"
#include "Assets/RenderAssetSources.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Render asset cache.
    template<typename TAsset, typename TRenderAsset, typename TSource>
    class RenderAssetCache {
    public:

        //! Alias the render asset handle type.
        typedef Assets::GenericHandle<TRenderAsset> RenderAssetHandle;

        //! Container type to store render asset handles.
        typedef Array<RenderAssetHandle>    Container;

        //! Requests the render asset index by an asset handle.
        s32                                 request( Assets::Assets& assets, RenderingContextWPtr context, const Assets::GenericHandle<TAsset>& asset, CString postfix );

        //! Returns the render asset handles.
        const Container&                    handles( void ) const;

        //! Returns the total number of assets stored in cache.
        s32                                 count( void ) const;

    private:

        //! Container type to map from an asset to a render asset index.
    #ifdef DC_CPP11_DISABLED
        typedef Map<RenderAssetHandle, s32> IndexByAsset;
    #else
        //! Asset handle hash predicate
        struct Hash {
            //! Just returns the handle index converted to a size_t
            size_t operator()( RenderAssetHandle const & key ) const {
                return key.index();
            }
        };

        typedef std::unordered_map<RenderAssetHandle, s32, Hash> IndexByAsset;
    #endif

        IndexByAsset                        m_indexByAsset;
        Container                           m_handles;
    };

    // ** RenderAssetCache::count
    template<typename TAsset, typename TRenderAsset, typename TSource>
    s32 RenderAssetCache<TAsset, TRenderAsset, TSource>::count( void ) const
    {
        return static_cast<s32>( handles().size() );
    }

    // ** RenderAssetCache::handles
    template<typename TAsset, typename TRenderAsset, typename TSource>
    const typename RenderAssetCache<TAsset, TRenderAsset, TSource>::Container& RenderAssetCache<TAsset, TRenderAsset, TSource>::handles( void ) const
    {
        return m_handles;
    }

    // ** RenderAssetCache::request
    template<typename TAsset, typename TRenderAsset, typename TSource>
    s32 RenderAssetCache<TAsset, TRenderAsset, TSource>::request( Assets::Assets& assets, RenderingContextWPtr context, const Assets::GenericHandle<TAsset>& asset, CString postfix )
    {
        // First lookup the render asset by an asset handle
        IndexByAsset::iterator i = m_indexByAsset.find( asset );

        // Next render index to be used
        s32 index = static_cast<s32>( m_handles.size() );

        // Found the renderable
        if( i != m_indexByAsset.end() ) {
            // Ensure that mesh handle matches the stored one
            if( i->first.index() == asset.index() ) {
                return i->second;
            }

            // Save this renderable asset index
            index = i->second;

            // Renderable is outdated - remove it
            m_indexByAsset.erase( i );
        }

        // Resize an array of renderable assets
        if( index >= static_cast<s32>( m_handles.size() ) ) {
            m_handles.resize( index + 1 );
        }

        // Read lock the asset to queue it for loading
        asset.readLock();

        // Create a render asset instance
        RenderAssetHandle handle = assets.add<TRenderAsset>( asset.uniqueId() + "." + postfix, DC_NEW TSource( asset, context ) );
        DC_ABORT_IF( !handle.isValid(), "failed to create render asset" );
        handle.asset().setName( asset.asset().name() + "." + postfix );

        // Save this handle
        m_indexByAsset[asset] = index;
        m_handles[index]      = handle;

        return index;
    }

	//! Rendering context.
	class RenderingContext : public RefCounted {
	public:

        //! Begins rendering with this context.
        void                                begin( void );

        //! Ends rendering with this context.
        void                                end( void );

        //! Returns the parent rendering HAL instance.
        Renderer::HalWPtr                   hal( void ) const;

        //! Returns parent scene instance.
        SceneWPtr                           scene( void ) const;

        //! Returns renderable index for a specified mesh asset.
        s32                                 requestRenderable( const MeshHandle& handle );

        //! Returns technique index for a specified material asset.
        s32                                 requestTechnique( const MaterialHandle& handle );

        //! Returns texture index for a specified image asset.
        s32                                 requestTexture( const ImageHandle& handle );

        //! Returns shader source index for a specified handle.
        s32                                 requestShaderSource( const ShaderSourceHandle& handle );

        //! Returns renderable handle by index.
        const RenderableHandle&             renderableByIndex( s32 index ) const;

        //! Returns technique handle by index.
        const TechniqueHandle&              techniqueByIndex( s32 index ) const;

        //! Returns texture handle by index.
        const TextureHandle&                textureByIndex( s32 index ) const;

        //! Returns shader source handle by index.
        const ShaderSourceHandle&           shaderByIndex( u32 index ) const;

        //! Creates a new shader instance.
        ShaderSourceHandle                  createShaderSource( const String& identifier, const String& fileName );

        //! Creates a shader program with a specified feature set.
        const ProgramHandle&                requestProgram( u32 source, u32 features );

        //! Returns the command buffer instance.
        Commands&                           commands( void );

		//! Adds a new render system to the scene.
		template<typename TRenderSystem>
		void							    addRenderSystem( void );

		//! Creates new rendering context.
		static RenderingContextPtr			create( Assets::Assets& assets, Renderer::HalWPtr hal, SceneWPtr scene );

	private:

											//! Constructs the RenderingContext instance.
											RenderingContext( Assets::Assets& assets, Renderer::HalWPtr hal, SceneWPtr scene );

	private:

        //! Renderable asset cache.
        typedef RenderAssetCache<Mesh, Renderable, RenderableMeshSource> RenderableCache;

        //! Technique asset cache.
        typedef RenderAssetCache<Material, Technique, TechniqueMaterialSource> TechniqueCache;

        //! Texture asset cache.
        typedef RenderAssetCache<Image, Texture, TextureImageSource> TextureCache;

        //! Container type to store all created shader permutations.
    #ifdef DC_CPP11_DISABLED
        typedef Map<u64, ProgramHandle> Permutations;
        typedef Map<ShaderSourceHandle, u32> ShaderSourceIndices;
    #else
        struct Hash {
            size_t operator()( ShaderSourceHandle const & key ) const { return key.index(); }
        };

        typedef std::unordered_map<u64, ProgramHandle> Permutations;
        typedef std::unordered_map<ShaderSourceHandle, u32, Hash> ShaderSourceIndices;
    #endif

        Renderer::HalWPtr                   m_hal;              //!< Parent HAL instance.
        RvmUPtr                             m_rvm;              //!< Internal rvm instance.
        CommandsUPtr                        m_commands;         //!< Internal rvm command buffer.
        SceneWPtr                           m_scene;            //!< Parent scene instance.
        Assets::Assets&                     m_assets;           //!< Asset manager used to create render assets.
        Array<RenderSystemUPtr>	            m_renderSystems;    //!< Entity render systems.

        RenderableCache                     m_renderables;
        TechniqueCache                      m_techniques;
        TextureCache                        m_textures;

        Permutations                        m_permutations;         //!< All created shader permutations.
        ShaderSourceIndices                 m_shaderSourceIndices;  //!< Maps from a shader source to it's index.
        Array<ShaderSourceHandle>           m_shaderSources;
	};

	// ** RenderingContext::addRenderSystem
	template<typename TRenderSystem>
	void RenderingContext::addRenderSystem( void )
	{
		m_renderSystems.push_back( DC_NEW TRenderSystem( *this ) );
	}

    // ** RenderingContext::requestRenderable
    NIMBLE_INLINE s32 RenderingContext::requestRenderable( const MeshHandle& handle )
    {
        s32 idx = m_renderables.request( m_assets, this, handle, "renderable" ) + 1;
        renderableByIndex( idx ).readLock();
        return idx;
    }

    // ** RenderingContext::requestTechnique
    NIMBLE_INLINE s32 RenderingContext::requestTechnique( const MaterialHandle& handle )
    {
        s32 idx = m_techniques.request( m_assets, this, handle, "technique" ) + 1;
        techniqueByIndex( idx ).readLock();
        return idx;
    }

    // ** RenderingContext::requestTexture
    NIMBLE_INLINE s32 RenderingContext::requestTexture( const ImageHandle& handle )
    {
        s32 idx = m_textures.request( m_assets, this, handle, "texture" ) + 1;
        textureByIndex( idx ).readLock();
        return idx;
    }

    // ** RenderingContext::renderableByIndex
    NIMBLE_INLINE const RenderableHandle& RenderingContext::renderableByIndex( s32 index ) const
    {
        DC_ABORT_IF( index <= 0 || index > m_renderables.count(), "index is out of range" );
        return m_renderables.handles()[index - 1];
    }

    // ** RenderingContext::techniqueByIndex
    NIMBLE_INLINE const TechniqueHandle& RenderingContext::techniqueByIndex( s32 index ) const
    {
        DC_ABORT_IF( index <= 0 || index > m_techniques.count(), "index is out of range" );
        return m_techniques.handles()[index - 1];
    }

    // ** RenderingContext::textureByIndex
    NIMBLE_INLINE const TextureHandle& RenderingContext::textureByIndex( s32 index ) const
    {
        DC_ABORT_IF( index <= 0 || index > m_textures.count(), "index is out of range" );
        return m_textures.handles()[index - 1];
    }

    // ** RenderingContext::shaderByIndex
    NIMBLE_INLINE const ShaderSourceHandle& RenderingContext::shaderByIndex( u32 index ) const
    {
        DC_BREAK_IF( index <= 0 || index > static_cast<u32>( m_shaderSources.size() ), "index is out of range" );
        return m_shaderSources[index - 1];
    }

    // ** RenderingContext::requestShaderSource
    NIMBLE_INLINE s32 RenderingContext::requestShaderSource( const ShaderSourceHandle& handle )
    {
        handle.readLock();

        ShaderSourceIndices::const_iterator i = m_shaderSourceIndices.find( handle );

        if( i != m_shaderSourceIndices.end() ) {
            DC_BREAK_IF( i->first.index() != handle.index(), "outdated shader source item" );
            return i->second + 1;
        }

        u32 count = static_cast<u32>( m_shaderSources.size() );
        u32 index = count;

        // Resize an array of renderable assets
        if( index >= count ) {
            m_shaderSources.resize( index + 1 );
        }

        // Save this handle
        m_shaderSourceIndices[handle] = index;
        m_shaderSources[index]        = handle;

        return index + 1;
    }

    // ** RenderingContext::requestProgram
    NIMBLE_INLINE const ProgramHandle& RenderingContext::requestProgram( u32 source, u32 features )
    {
        DC_BREAK_IF( source == 0, "invalid shader source" );

        // Create the permutation hash key
        u64 hash = static_cast<u64>( source ) << 32 | features;

        // First lookup the permutation cache
        Permutations::const_iterator i = m_permutations.find( hash );

        // Found - return a program
        if( i != m_permutations.end() ) {
            return i->second;
        }

        // Create new permutation
        const ShaderSourceHandle& shader = shaderByIndex( source );
        DC_BREAK_IF( !shader.isValid(), "invalid shader source" );

        // Create new program asset
        ProgramHandle program = m_assets.add<Program>( shader.asset().uniqueId() + ".program." + toString( features ), DC_NEW ProgramShaderSource( shader, this ) );
        program.asset().setName( shader.asset().uniqueId() + ".program." + toString( features ) );
        m_permutations[hash] = program;

        return m_permutations[hash];
    }

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingContext_H__    */