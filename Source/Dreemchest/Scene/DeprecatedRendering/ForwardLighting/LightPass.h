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

#ifndef __DC_Scene_ForwardLighting_LightPass_H__
#define __DC_Scene_ForwardLighting_LightPass_H__

#include "../RenderPass.h"
#include "../RenderingSystem.h"
#include "../Passes/BasicPasses.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

#if DEV_DEPRECATED_SCENE_RENDERER

	//! Emits render operations for static meshes affected by light sources.
	class ForwardLightPass : public RenderPass<Light> {
	public:

								//! Constructs ForwardLightPass instance.
								ForwardLightPass( Ecs::EcsWPtr ecs );

	protected:

		//! Emits render operations for a single static mesh.
		virtual void			render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const Light& light, const Transform& transform ) DC_DECL_OVERRIDE;

	protected:

		//! Setups the RVM for rendering lit meshes
		virtual void			setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection ) DC_DECL_OVERRIDE;
	};

	//! Emits render operation for additive render mode.
	class AdditiveLightPass : public RenderPassBase {
	public:

								//! Constructs AdditiveLightPass instance.
								AdditiveLightPass( Ecs::EcsWPtr ecs );

	protected:

		//! Setups the RVM for rendering additive layer.
		virtual void			setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection ) DC_DECL_OVERRIDE;
	};

	//! Emits render operations for translucent objects.
	class TranslucentLightPass : public ForwardLightPass {
	public:

								//! Constructs TranslucentLightPass instance.
								TranslucentLightPass( Ecs::EcsWPtr ecs );

	protected:

		//! Setups the RVM for rendering translucent layer.
		virtual void			setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection ) DC_DECL_OVERRIDE;
	};

	//! Forward light rendering system.
	class ForwardLightingRenderer : public RenderingSystem<RenderForwardLit> {
	public:

								ForwardLightingRenderer( Ecs::EcsWPtr ecs )
									: RenderingSystem( ecs, "ForwardLightingRenderer" )
								{
									addPass<AmbientPass>();
									addPass<ForwardLightPass>();
									addPass<TranslucentLightPass>();
									addPass<AdditiveLightPass>();
								}
	};

#endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_ForwardLighting_LightPass_H__    */