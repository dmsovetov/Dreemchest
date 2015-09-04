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

#include "WireframeRenderPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** WireframeRenderPass::WireframeRenderPass
WireframeRenderPass::WireframeRenderPass( Ecs::Entities& entities, const Rendering& rendering )
	: StaticMeshRenderPass( entities, rendering )
{
}

// ** WireframeRenderPass::render
void WireframeRenderPass::render( const Ecs::EntityPtr& camera, u32 currentTime, f32 dt, const Matrix4& viewProjection )
{
	m_settings = camera->get<RenderWireframeMeshes>();
	StaticMeshRenderPass::render( camera, currentTime, dt, viewProjection );
}

// ** WireframeRenderPass::begin
bool WireframeRenderPass::begin( u32 currentTime )
{
	if( !StaticMeshRenderPass::begin( currentTime ) ) {
		return false;
	}

	// Set wireframe polygon mode
	m_rendering.m_hal->setPolygonMode( Renderer::PolygonWire );

	// Set the default shader
	m_rvm.setDefaultShader( m_rendering.m_shaders->shaderById( ShaderCache::ConstantColor ) );

	// Set the additive blending
	m_rvm.setDefaultBlending( Renderer::BlendOne, Renderer::BlendOne );

	// Set the depth testing function
	m_rvm.setDefaultDepthFunction( Renderer::LessEqual );

	return true;
}

// ** WireframeRenderPass::end
void WireframeRenderPass::end( void )
{
	// Flush emitted render operations
	StaticMeshRenderPass::end();

	// Set fill polygon mode
	m_rendering.m_hal->setPolygonMode( Renderer::PolygonFill );
}

} // namespace Scene

DC_END_DREEMCHEST