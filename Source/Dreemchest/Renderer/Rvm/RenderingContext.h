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

#ifndef __DC_Renderer_RenderingContext_H__
#define __DC_Renderer_RenderingContext_H__

#include "../Renderer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! Rendering context.
    class RenderingContext : public RefCounted
    {
    friend class Rvm;
    public:

    #if DEV_DEPRECATED_HAL
        //! Returns a parent rendering HAL instance.
        HalWPtr                                 hal( void ) const;
    #endif  /*  #if DEV_DEPRECATED_HAL  */

        //! Queues an input layout instance for creation and returns it's index.
        RenderId                                requestInputLayout( const VertexFormat& vertexFormat );

        //! Queues a vertex buffer instance for creation and returns it's index.
        RenderId                                requestVertexBuffer( const void* data, s32 size );

        //! Queues an index buffer instance for creation and returns it's index.
        RenderId                                requestIndexBuffer( const void* data, s32 size );

        //! Queues a constant buffer instance for creation and returns it's index.
        RenderId                                requestConstantBuffer( const void* data, s32 size, const ConstantBufferLayout* layout );

        //! Queues a texture instance for creation and returns it's index.
        RenderId                                requestTexture( const void* data, s32 width, s32 height, PixelFormat format );

        //! Creates a shader from a file.
        UbershaderPtr                           createShader( const String& fileName ) const;

    #if DEV_DEPRECATED_HAL
        //! Returns a vertex buffer by an index.
        const VertexBufferPtr&                  vertexBuffer( RenderId identifier ) const;

        //! Returns an index buffer by an index.
        const IndexBufferPtr&                   indexBuffer( RenderId identifier ) const;

        //! Returns a constant buffer by an index.
        const ConstantBufferPtr&                constantBuffer( RenderId identifier ) const;

        //! Returns an input layout by an index.
        const InputLayoutPtr&                   inputLayout( RenderId identifier ) const;

        //! Returns a texture by an index.
        const TexturePtr&                       texture( s32 identifier ) const;
        
        //! Returns a shader by an index.
        const UbershaderPtr&                    shader( s32 identifier ) const;
    #endif  /*  #if DEV_DEPRECATED_HAL  */
        
        //! Interns a shader and returns it's integer identifier.
        s32                                     internShader( UbershaderPtr shader );

    #if DEV_DEPRECATED_HAL
        //! Creates a RenderingContext instance.
        static RenderingContextPtr              create( Renderer::HalWPtr hal );
    #else
        //! Creates a RenderingContext instance.
        static RenderingContextPtr              create();
    #endif  /*  #if DEV_DEPRECATED_HAL  */

    private:

        //! Forward declaration of a ResourceConstructor type.
        struct ResourceConstructor;

                                            #if DEV_DEPRECATED_HAL
                                                //! Constructs a RenderingContext instance.
                                                RenderingContext( HalWPtr hal );
                                            #else
                                                //! Constructs a RenderingContext instance.
                                                RenderingContext();
                                            #endif  /*  #if DEV_DEPRECATED_HAL  */

        //! Constructs all resources before rendering a frame.
        void                                    constructResources( void );

        //! Constructs an input layout.
        void                                    constructInputLayout( const ResourceConstructor& constructor );

        //! Constructs a vertex buffer.
        void                                    constructVertexBuffer( const ResourceConstructor& constructor );

        //! Constructs an index buffer.
        void                                    constructIndexBuffer( const ResourceConstructor& constructor );

        //! Constructs a constant buffer.
        void                                    constructConstantBuffer( const ResourceConstructor& constructor );

        //! Constructs a texture.
        void                                    constructTexture( const ResourceConstructor& constructor );

        //! Acquires an intermediate render target.
        RenderId                                acquireRenderTarget( u16 width, u16 height, PixelFormat format );

        //! Releases an intermediate render target.
        void                                    releaseRenderTarget( RenderId id );

    #if DEV_DEPRECATED_HAL
        //! Returns an intermediate render target.
        RenderTargetWPtr                        intermediateRenderTarget( RenderId id ) const;
    #endif  /*  #if DEV_DEPRECATED_HAL  */

    private:

        //! A resource item that should be created.
        struct ResourceConstructor
        {
            //! Resource type being constructed
            enum Type
            {
                  InputLayout       //!< An input layout will be constructed.
                , VertexBuffer      //!< A vertex buffer will be constructed.
                , IndexBuffer       //!< An index buffer will be constructed.
                , ConstantBuffer    //!< A constant buffer will be constructed.
                , Texture           //!< A texture will be constructed.
                , TotalConstructors //!< A total number of resource constructor types.
            };

            //! A callback function used to construct a resource.
            typedef void ( RenderingContext::*Function )( const ResourceConstructor& );

                                            //! Constructs a ResourceConstructor instance.
                                            ResourceConstructor( Type type = TotalConstructors )
                                                : type( type ) {}

            Type                            type;       //!< Resource constructor type.
            RenderId                        id;         //!< Handle to a resource being constructed.
            
            union
            {
                struct
                {
                    u8                      format;     //!< Vertex format used by an input layout constructor.
                } inputLayout;

                struct
                {
                    const void*             data;       //!< Data that should be uploaded to a buffer after construction.
                    s32                     size;       //!< A buffer size.
                    const void*             userData;   //!< Used by a constant buffer constructor.
                } buffer;

                struct
                {
                    const void*             data;       //!< A texture data that should be uploaded after construction.
                    u16                     width;      //!< A texture width.
                    u16                     height;     //!< A texture height.
                    Renderer::PixelFormat   format;     //!< A texture format.
                } texture;
            };
        };

        //! A maximum number of input layout types
        enum { MaxInputLayouts = 255 };

        //! Container type to store a list of resource constructors that should be run before rendering a frame.
        typedef List<ResourceConstructor>       ResourceConstructors;

    #if DEV_DEPRECATED_HAL
        HalWPtr                                 m_hal;                              //!< A rendering HAL to be used.
    #endif  /*  #if DEV_DEPRECATED_HAL  */
        ResourceConstructors                    m_resourceConstructors;             //!< A list of resources to be constructed.
    #if DEV_DEPRECATED_HAL
        FixedArray<VertexBufferPtr>             m_vertexBufferPool;                 //!< Allocated vertex buffers.
        FixedArray<IndexBufferPtr>              m_indexBufferPool;                  //!< Allocated index buffers.
        FixedArray<ConstantBufferPtr>           m_constantBufferPool;               //!< Allocated constant buffers.
        FixedArray<InputLayoutPtr>              m_inputLayoutPool;                  //!< Allocated input layouts.
        FixedArray<TexturePtr>                  m_texturePool;                      //!< Allocated textures.
        IndexCache<UbershaderPtr>               m_shaders;                          //!< Interned shaders.
    #endif  /*  #if DEV_DEPRECATED_HAL  */
        RenderId                                m_inputLayouts[MaxInputLayouts];    //!< A lookup table for input layout types.

        //! A helper struct that hold info about an intermediate render target.
        struct IntermediateRenderTarget
        {
        #if DEV_DEPRECATED_HAL
            RenderTargetPtr                     renderTarget;                       //!< A GPU render target instance.
        #endif  /*  #if DEV_DEPRECATED_HAL  */
            u16                                 width;                              //!< Render target width.
            u16                                 height;                             //!< Render target height.
            PixelFormat                         format;                             //!< Render target internal format.
            bool                                isFree;                             //!< Indicates that this render target is free.
        };

        Array<IntermediateRenderTarget>         m_renderTargets;            //!< An array of intermediate render targets.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingContext_H__    */
