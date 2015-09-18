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

#include "../Scene.h"
#include "../assets/Material.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Renderer virtual machine.
	class Rvm : public RefCounted {
	public:

		//! Rendering operation emitted by scene object processing.
		struct Command {
			f32							distance;									//!< Normalized distance to camera.
			u8							mode;										//!< Rendering mode.
			Renderer::Shader*			shader;										//!< Shader to be used for rendering.
			Renderer::VertexBuffer*		vertexBuffer;								//!< Vertex buffer to be used for rendering.
			Renderer::IndexBuffer*		indexBuffer;								//!< Index buffer to be used for rendering.
			Renderer::Texture*			textures[Material::TotalMaterialLayers];	//!< Texture to be used for rendering.
			const Rgba*					colors[Material::TotalMaterialLayers];		//!< Material colors.
			Matrix4						transform;									//!< Transformation matrix.
		};

		//! Rasterization options used by a render mode.
		struct RasterizationOptions {
			bool						m_enabled;		//!< Skips rasterization when this flag is not set.
			Renderer::BlendFactor		m_blend[2];		//!< Used blend factors.
			Renderer::Compare			m_alphaTest;	//!< Alpha test function used.
			f32							m_alphaRef;		//!< Reference value for an alpha test.
			bool						m_depthMask;	//!< Is writing to a depth buffer enabled or not.
			Renderer::Compare			m_depthTest;	//!< Depth test function used.

			//! Returns true if the rendering mode will not be skipped.
			operator bool( void ) const { return m_enabled; }

			//! Returns rasterization options with modified depth mask.
			RasterizationOptions		overrideDepthMask( bool value ) const
			{
				RasterizationOptions result = *this;
				result.m_depthMask = value;
				return result;
			}

			//! Returns rasterization options with modified blending.
			RasterizationOptions		overrideBlending( Renderer::BlendFactor src, Renderer::BlendFactor dst ) const
			{
				RasterizationOptions result = *this;
				result.m_blend[0] = src;
				result.m_blend[1] = dst;
				return result;
			}
		};

		static const RasterizationOptions SkipRasterization;					//!< Skip rendering.
		static const RasterizationOptions OpaqueRasterization;					//!< Rasterization options for opaque rendering.
		static const RasterizationOptions CutoutRasterization;					//!< Rasterization options for cutout rendering.
		static const RasterizationOptions TranslucentRasterization;				//!< Rasterization options for translucent rendering.
		static const RasterizationOptions AdditiveRasterization;				//!< Rasterization options for additive rendering.

		//! Available stats counters.
		enum Counter {
			  TextureSwitches
			, ShaderSwitches
			, VertexBufferSwitches
			, DrawIndexed
			, Triangles
			, Vertices
			, RopAllocatedMemory
			, TotalCounters
		};

		//! Available constant registers.
		enum Register {
			  ConstantColor
			, LightPosition
			, LightColor
			, TotalRegisters
		};

										//! Constructs the Rvm
										Rvm( Renderer::HalPtr hal, u32 maxCommands );

		//! Resets internal states to default.
		void							reset( void );

		//! Clears emitted commands.
		void							clear( void );

		//! Flushes all accumulated commands to a framebuffer.
		void							flush( void );

		//! Returns the specified counter value.
		s32								counter( Counter counter ) const;

		//! Resets all counters.
		void							resetCounters( void );

		//! Emits a new render operation.
		Command*						emit( void );

		//! Returns rendering HAL.
		Renderer::HalPtr				hal( void ) const;

		//! Sets rasterization state for specified rendering mode.
		void							setRasterization( RenderingMode mode, const RasterizationOptions& value );

		//! Sets rasterization state for specified rendering mode mask.
		void							setRasterization( u32 mask, const RasterizationOptions& value );

		//! Sets the global shader constant.
		void							setUniformMatrix( CString name, const Matrix4& value );

		//! Sets the global shader constant.
		void							setRegister( Register id, const Vec4& value );

		//! Returns true if the specified rendering mode is enabled.
		bool							willRender( RenderingMode mode ) const;

		//! Returns the view-projection matrix.
		const Matrix4&					viewProjection( void ) const;

		//! Sets the view-projection matrix.
		void							setViewProjection( const Matrix4& value );

		//! Sets the id to shader mapping.
		void							setShader( u32 id, const Renderer::ShaderPtr& shader );

		//! Sets the default shader.
		void							setDefaultShader( const Renderer::ShaderPtr& value );

		//! Sets the default cull face side.
		void							setDefaultCullFace( Renderer::TriangleFace value );

		//! Sets the default polygon mode.
		void							setDefaultPolygonMode( Renderer::PolygonMode value );

		//! Sorts render operations by shader used, then by texture, then by mesh data.
		static bool						sortByShaderTextureMesh( const Command* a, const Command* b );

	private:

		//! Sets the shader used by a command.
		void							setShader( Renderer::Shader* shader );

		//! Sets the rendering mode used by a command.
		void							setRenderingMode( u8 value );

		//! Sets the per-instance shader uniforms.
		void							setShaderUniforms( Renderer::Shader* shader, const Command* cmd );

		//! Returns the shader by id.
		Renderer::Shader*				findShaderById( u32 id ) const;

		//! Increases the counter value.
		void							increase( Counter counter, u32 amount = 1 );

	private:

		//! Render operations list container
		typedef List<const Command*>	EmittedCommands;

		static CString					s_samplersUniformNames[];			//!< Samplers uniform names.
		static CString					s_colorsUniformNames[];				//!< Colors uniform names.
		static CString					s_vpUniformName;					//!< View-projection matrix uniform name.
		static CString					s_transformUniformName;				//!< Model transform uniform name.

		Renderer::HalPtr				m_hal;								//!< HAL to be used for rendering.
		ArrayAllocator<Command>			m_allocator;						//!< Array allocator used to allocated render operations.
		EmittedCommands					m_commands;							//!< Emitted rendering commands.
		Matrix4							m_viewProjection;					//!< The view projection matrix.
		Array<Renderer::ShaderPtr>		m_shaders;							//!< Array of available shaders.
		s32								m_counters[TotalCounters];			//!< Stats counters.
		Vec4							m_registers[TotalRegisters];		//!< Constant registers.
		RasterizationOptions			m_rasterization[TotalRenderModes];	//!< Render mode rasterization options.

		Renderer::ShaderPtr				m_defaultShader;					//!< The default shader to use.
		Renderer::TriangleFace			m_defaultCullFace;					//!< The default cull face side.
		Renderer::PolygonMode			m_defaultPolygonMode;				//!< The default polygon mode.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rvm_H__    */