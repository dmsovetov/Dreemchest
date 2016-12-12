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

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** DebugRenderSystem::DebugRenderSystem
DebugRenderSystem::DebugRenderSystem( RenderingContext& context, RenderScene& renderScene )
    : RenderSystem( context, renderScene )
    , m_staticMeshes( context, renderScene )
    , m_lights( context, renderScene )
    , m_cameras( context, renderScene )
{
    // Create a sprite shader
    m_debugShader = m_context.requestShader("../../Source/Dreemchest/Scene/Rendering/Shaders/Default.shader");
}

// ** DebugRenderSystem::emitRenderOperations
void DebugRenderSystem::emitRenderOperations( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const DebugRenderer& debugRenderer )
{
    // Push a shader sprite rendering state
    StateScope state = stateStack.newScope();
    //state->bindProgram( m_context.internShader( m_debugShader ) );
    state->bindProgram( m_debugShader );
    state->setBlend( Renderer::BlendSrcAlpha, Renderer::BlendInvSrcAlpha );

    // Static mesh bounding boxes
    m_staticMeshes.renderWithColor( frame, commands, stateStack, debugRenderer.staticMeshColor() );

    // Lights bounding boxes
    m_lights.renderWithColor( frame, commands, stateStack, debugRenderer.lightColor() );

    // Cameras bounding boxes
    m_cameras.renderWithColor( frame, commands, stateStack, debugRenderer.cameraColor() );
}

} // namespace Scene

DC_END_DREEMCHEST
