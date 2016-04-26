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

#include "../RenderScene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Rendering context.
    class RenderingContext : public RefCounted {
    friend class Rvm;
    public:

        //! Returns a parent rendering HAL instance.
        Renderer::HalWPtr                       hal( void ) const;

        //! Queues an input layout instance for creation and returns it's index.
        RenderResource                          requestInputLayout( const VertexFormat& vertexFormat );

        //! Queues a vertex buffer instance for creation and returns it's index.
        RenderResource                          requestVertexBuffer( const void* data, s32 size );

        //! Queues an index buffer instance for creation and returns it's index.
        RenderResource                          requestIndexBuffer( const void* data, s32 size );

        //! Queues a constant buffer instance for creation and returns it's index.
        RenderResource                          requestConstantBuffer( const void* data, s32 size, const Renderer::ConstantBufferLayout* layout );

        //! Queues a texture instance for creation and returns it's index.
        RenderResource                          requestTexture( const void* data, s32 width, s32 height, s32 channels );

        //! Creates a shader from a file.
        UbershaderPtr                           createShader( const String& fileName ) const;

        //! Interns a render target and returns it's integer identifier.
        s32                                     internRenderTarget( RenderTargetPtr renderTarget );

        //! Returns a render target by an index.
        const RenderTargetPtr&                  renderTarget( s32 identifier ) const;

        //! Returns a vertex buffer by an index.
        const Renderer::VertexBufferPtr&        vertexBuffer( RenderResource identifier ) const;

        //! Returns an index buffer by an index.
        const Renderer::IndexBufferPtr&         indexBuffer( RenderResource identifier ) const;

        //! Returns a constant buffer by an index.
        const Renderer::ConstantBufferPtr&      constantBuffer( RenderResource identifier ) const;

        //! Returns an input layout by an index.
        const Renderer::InputLayoutPtr&         inputLayout( RenderResource identifier ) const;

        //! Returns a texture by an index.
        const Renderer::TexturePtr&             texture( s32 identifier ) const;
       
        //! Interns a shader and returns it's integer identifier.
        s32                                     internShader( UbershaderPtr shader );
        
        //! Returns a shader by an index.
        const UbershaderPtr&                    shader( s32 identifier ) const;

        //! Creates a RenderingContext instance.
        static RenderingContextPtr              create( Renderer::HalWPtr hal );

    private:

        //! Forward declaration of a ResourceConstructor type.
        struct ResourceConstructor;

                                                //! Constructs a RenderingContext instance.
                                                RenderingContext( Renderer::HalWPtr hal );

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

    private:

        //! A resource item that should be created.
        struct ResourceConstructor {
            //! Resource type being constructed
            enum Type {
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

            Type                type;       //!< Resource constructor type.
            RenderResource      id;         //!< Handle to a resource being constructed.
            union {
                struct {
                    u8          format;     //!< Vertex format used by an input layout constructor.
                } inputLayout;

                struct {
                    const void* data;       //!< Data that should be uploaded to a buffer after construction.
                    s32         size;       //!< A buffer size.
                    const void* userData;   //!< Used by a constant buffer constructor.
                } buffer;

                struct {
                    const void* data;       //!< A texture data that should be uploaded after construction.
                    u16         width;      //!< A texture width.
                    u16         height;     //!< A texture height.
                    u8          channels;   //!< A texture format.
                } texture;
            };
        };

        //! Container type to store a list of resource constructors that should be run before rendering a frame.
        typedef List<ResourceConstructor>       ResourceConstructors;

        Renderer::HalWPtr                       m_hal;                      //!< A rendering HAL to be used.
        ResourceConstructors                    m_resourceConstructors;     //!< A list of resources to be constructed.
        FixedArray<Renderer::VertexBufferPtr>   m_vertexBufferPool;         //!< Allocated vertex buffers.
        FixedArray<Renderer::IndexBufferPtr>    m_indexBufferPool;          //!< Allocated index buffers.
        FixedArray<Renderer::ConstantBufferPtr> m_constantBufferPool;       //!< Allocated constant buffers.
        FixedArray<Renderer::InputLayoutPtr>    m_inputLayoutPool;          //!< Allocated input layouts.
        FixedArray<Renderer::TexturePtr>        m_texturePool;              //!< Allocated textures.

        IndexCache<RenderTargetPtr>             m_renderTargets;            //!< Interned render targets.
        IndexCache<UbershaderPtr>               m_shaders;                  //!< Interned shaders.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingContext_H__    */