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

#include "BasicPasses.h"
#include "../ParticleRendering.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

#if DEV_DEPRECATED_SCENE_RENDERER

// ** AmbientPass::setup
void AmbientPass::setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection )
{
	// Disable additive rendering in ambient pass
	rvm.setRasterization( RenderAdditive, Rvm::SkipRasterization );

	// Set the additive blending
	rvm.setRegister( Rvm::ConstantColor, Vec4( 0.2f, 0.2f, 0.2f ) );
}

// ** ParticleSystemsPass::render
void ParticleSystemsPass::render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const Particles& particles, const Transform& transform )
{
	Renderer::Renderer2DPtr renderer = context->renderer();

	if( !m_renderer.valid() ) {
		m_renderer = Fx::ParticleRenderer::create( Fx::RenderQuads, DC_NEW ParticleRenderingInterface( renderer ) );
	}

	Fx::ParticleSystemInstanceWPtr instance = particles.instance();

	for( s32 i = 0, ne = instance->emitterCount(); i < ne; i++ ) {
		Fx::EmitterInstanceWPtr emitter = instance->emitter( i );

		for( s32 j = 0, np = emitter->particlesCount(); j < np; j++ ) {
			Fx::ParticlesInstanceWPtr particles = emitter->particles( j );
			m_renderer->render( particles->material(), &particles->items(), particles->aliveCount() );
		}
	}
}

#endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */

} // namespace Scene

DC_END_DREEMCHEST