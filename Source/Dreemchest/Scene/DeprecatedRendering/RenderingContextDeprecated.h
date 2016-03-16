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

#ifndef __DC_Scene_RenderingContextDeprecated_H__
#define __DC_Scene_RenderingContextDeprecated_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

#if DEV_DEPRECATED_SCENE_RENDERER

	//! Rendering context.
	class RenderingContextDeprecated : public RefCounted {
	public:

        //! Renderable data structure.
        struct Renderable {
            Renderer::PrimitiveType         primitiveType;  //!< Renderable primitive type.
            Renderer::VertexBufferPtr       vertexBuffer;   //!< GPU-side vertex buffer constructed from a mesh asset.
            Renderer::IndexBufferPtr        indexBuffer;    //!< Index buffer constructed from a mesh asset.
        };

		//! Returns RVM.
		RvmPtr								rvm( void ) const;

		//! Returns shader cache.
		ShaderCachePtr						shaders( void ) const;

		//! Returns hal.
		Renderer::HalPtr					hal( void ) const;

		//! Returns renderer.
		Renderer::Renderer2DPtr				renderer( void ) const;

        //! Returns the renderable index for a specified mesh asset.
        s32                                 requestRenderable( const MeshHandle& mesh, s32 chunk );

        //! Returns texture for a specified image asset.
        Renderer::TexturePtr                requestTexture( const ImageHandle& image );

		//! Creates new rendering context.
		static RenderingContextPtr			create( const Renderer::HalPtr& hal );

	private:

											//! Constructs the RenderingContextDeprecated instance.
											RenderingContextDeprecated( RvmPtr rvm, ShaderCachePtr shaders, Renderer::HalPtr hal, Renderer::Renderer2DPtr renderer );

	private:

        //! Container type to store mapping from chunk handle to renderable index.
        typedef Map<u32, u32> MeshToRenderable;

        //! Container type to store mapping from image handle to texture.
        typedef Map<u32, Renderer::TexturePtr> ImageToTexture;

		RvmPtr								m_rvm;			    //!< Rendering virtual machine.
		ShaderCachePtr						m_shaders;		    //!< Shaders cache.
		Renderer::HalPtr					m_hal;			    //!< Rendering HAL.
		Renderer::Renderer2DPtr				m_renderer;		    //!< Rendering interface.
        Array<Renderable>                   m_renderables;      //!< Array of renderable data.
        ImageToTexture                      m_textureByImage;   //!< Maps from image handle to GPU texture.
        MeshToRenderable                    m_renderableByMesh; //!< Maps from mesh handle to a renderable index.
	};

#endif DEV_DEPRECATED_SCENE_RENDERER

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderingContextDeprecated_H__    */