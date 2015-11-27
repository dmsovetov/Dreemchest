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

#ifndef __DC_Scene_BasicPasses_H__
#define __DC_Scene_BasicPasses_H__

#include "../RenderPass.h"
#include "../Emitters/StaticMeshRopEmitter.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Renders unlit textured scene.
	class UnlitPass : public RenderPassBase {
	public:

								//! Constructs UnlitPass instance.
								UnlitPass( Ecs::EcsWPtr ecs )
									{ addEmitter( DC_NEW StaticMeshRopEmitter( ecs, StaticMeshRopEmitter::Unlit ) ); }
	};


	//! Renders a constant color ambient
	class AmbientPass : public RenderPassBase {
	public:

								//! Constructs AmbientPass instance.
								AmbientPass( Ecs::EcsWPtr ecs )
									{ addEmitter( DC_NEW StaticMeshRopEmitter( ecs, StaticMeshRopEmitter::DiffuseTexture | StaticMeshRopEmitter::Shader | StaticMeshRopEmitter::RenderingMode, Material::Ambient ) ); }

	protected:

		//! Setups the RVM for rendering ambient.
		virtual void			setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection );
	};

	//! Renders particle systems.
	class ParticleSystemsPass : public RenderPass<Particles> {
	public:

								//! Constructs ParticleSystemsPass instance.
								ParticleSystemsPass( Ecs::EcsWPtr ecs )
									: RenderPass( ecs, "ParticleSystemsPass" ) {}

	protected:

		//! Renders the particle system.
		virtual void			render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const Particles& particles, const Transform& transform );

	private:

		Fx::ParticleRendererPtr	m_renderer;	//!< Particle renderer instance.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_BasicPasses_H__    */