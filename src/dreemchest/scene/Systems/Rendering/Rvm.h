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

#ifndef __DC_Scene_Rvm_H__
#define __DC_Scene_Rvm_H__

#include "RenderSystem.h"
#include "../../Assets/Material.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Renderer virtual machine.
	class Rvm {
	public:

		//! Rendering operation emitted by scene object processing.
		struct Command {
			u32							shader;										//!< Shader to be used for rendering.
			Renderer::VertexBuffer*		vertexBuffer;								//!< Vertex buffer to be used for rendering.
			Renderer::IndexBuffer*		indexBuffer;								//!< Index buffer to be used for rendering.
			Renderer::Texture*			textures[Material::TotalMaterialLayers];	//!< Texture to be used for rendering.
			const Rgba*					colors[Material::TotalMaterialLayers];		//!< Material colors.
			Matrix4						transform;									//!< Transformation matrix.
		};

										//! Constructs the Rvm
										Rvm( u32 maxCommands );

		//! Clears the emitted commands.
		void							clear( void );

		//! Flushes all accumulated commands to a framebuffer.
		void							flush( Renderer::HalPtr hal );

		//! Emits a new render operation.
		Command*						emit( void );

		//! Sets the global shader constant.
		void							setUniformMatrix( CString name, const Matrix4& value );

		//! Returns the view-projection matrix.
		const Matrix4&					viewProjection( void ) const;

		//! Sets the view-projection matrix.
		void							setViewProjection( const Matrix4& value );

		//! Sets the id to shader mapping.
		void							setShader( u32 id, const Renderer::ShaderPtr& shader );

		//! Sets the default shader.
		void							setDefaultShader( const Renderer::ShaderPtr& value );

		//! Sets the default blending.
		void							setDefaultBlending( Renderer::BlendFactor src, Renderer::BlendFactor dst );

		//! Sets the default deth testing function.
		void							setDefaultDepthFunction( Renderer::Compare value );

		//! Sets the default cull face side.
		void							setDefaultCullFace( Renderer::TriangleFace value );

		//! Sorts render operations by shader used, then by texture, then by mesh data.
		static bool						sortByShaderTextureMesh( const Command* a, const Command* b );

	private:

		//! Sets the shader used by a command.
		void							setShader( Renderer::HalPtr hal, Renderer::Shader* shader );

		//! Sets the per-instance shader uniforms.
		void							setShaderUniforms( Renderer::Shader* shader, const Command* cmd );

		//! Returns the shader by id.
		Renderer::Shader*				findShaderById( u32 id ) const;

	private:

		//! Render operations list container
		typedef List<const Command*>	EmittedCommands;

		static CString					s_samplersUniformNames[];	//!< Samplers uniform names.
		static CString					s_colorsUniformNames[];		//!< Colors uniform names.
		static CString					s_vpUniformName;			//!< View-projection matrix uniform name.
		static CString					s_transformUniformName;		//!< Model transform uniform name.

		ArrayAllocator<Command>			m_allocator;				//!< Array allocator used to allocated render operations.
		EmittedCommands					m_commands;					//!< Emitted rendering commands.
		Matrix4							m_viewProjection;			//!< The view projection matrix.
		Array<Renderer::ShaderPtr>		m_shaders;					//!< Array of available shaders.

		Renderer::ShaderPtr				m_defaultShader;			//!< The default shader to use.
		Renderer::BlendFactor			m_defaultSrcBlending;		//!< The default source blend factor.
		Renderer::BlendFactor			m_defaultDstBlending;		//!< The default dest blend factor.
		Renderer::Compare				m_defaultDepthFunction;		//!< The default blend function.
		Renderer::TriangleFace			m_defaultCullFace;			//!< The default cull face side.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rvm_H__    */