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

#include "DebugRenderSystem.h"
#include "../Passes/DebugRenderPasses.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** DebugRenderSystem::DebugRenderSystem
DebugRenderSystem::DebugRenderSystem( RenderingContext& context, RenderScene& renderScene )
    : RenderSystem( context, renderScene )
{
    m_passes.push_back( DC_NEW DebugCameraPass( context, renderScene ) );
    m_passes.push_back( DC_NEW DebugStaticMeshPass( context, renderScene ) );
    m_passes.push_back( DC_NEW DebugLightPass( context, renderScene ) );

	// Create a sprite shader
	m_debugShader = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Default.shader" );
}

// ** DebugRenderSystem::emitRenderOperations
void DebugRenderSystem::emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const DebugRenderer& debugRenderer )
{
	// Push a shader sprite rendering state
	StateScope state = stateStack.newScope();
	state->bindProgram( m_context.internShader( m_debugShader ) );
    state->setBlend( Renderer::BlendSrcAlpha, Renderer::BlendInvSrcAlpha );

    // Setup passes before rendering
    static_cast<DebugCameraPass*>( m_passes[0].get() )->setColor( debugRenderer.cameraColor() );
    static_cast<DebugStaticMeshPass*>( m_passes[1].get() )->setColor( debugRenderer.staticMeshColor() );
    static_cast<DebugLightPass*>( m_passes[2].get() )->setColor( debugRenderer.lightColor() );

    // Render all nested passes
    RenderSystemBase::emitRenderOperations( frame, commands, stateStack, entity, camera, transform );
}

} // namespace Scene

DC_END_DREEMCHEST