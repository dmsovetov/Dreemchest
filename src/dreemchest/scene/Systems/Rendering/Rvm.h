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

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Renderer virtual machine.
	class Rvm {
	public:

		//! Rendering operation emitted by scene object processing.
		struct Command {
			Renderer::Shader*			shader;			//!< Material shader.
			Renderer::VertexBuffer*		vertexBuffer;	//!< Vertex buffer to be used for rendering.
			Renderer::IndexBuffer*		indexBuffer;	//!< Index buffer to be used for rendering.
			Renderer::Texture*			texture;		//!< Texture to be used for rendering.
			const Rgba*					diffuse;		//!< Diffuse color.
			Matrix4						mvp;			//!< Model/view/projection matrix.
		};

										//! Constructs the Rvm
										Rvm( u32 maxCommands );

		//! Clears the emitted commands.
		void							clear( void );

		//! Flushes all accumulated commands to a framebuffer.
		void							flush( Renderer::HalPtr hal );

		//! Emits a new render operation.
		Command*						emit( void );

				//! Sets the shader for current render operation.
	//	void						setShader( const RenderOp* rop );

		//! Sorts render operations by shader used, then by texture, then by mesh data.
		static bool					sortByShaderTextureMesh( const Command* a, const Command* b );

	private:

		//! Sets the shader used by a command.
		void							setShader( Renderer::HalPtr hal, const Command* cmd );

	private:

		//! Render operations list container
		typedef List<const Command*>	EmittedCommands;

		ArrayAllocator<Command>			m_allocator;	//!< Array allocator used to allocated render operations.
		EmittedCommands					m_commands;		//!< Emitted rendering commands.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rvm_H__    */