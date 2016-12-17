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

#include "GenericRenderPasses.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** AmbientPass::AmbientPass
AmbientPass::AmbientPass( RenderingContext& context, RenderScene& renderScene )
    : RenderPassBase( context, renderScene )
{
    // Create an ambient lighting shader instance
    m_shader = m_context.deprecatedRequestShader( "../../Source/Dreemchest/Scene/Rendering/Shaders/Ambient.shader" );
}

// ** AmbientPass::render
void AmbientPass::render( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack )
{
    StateScope pass = stateStack.newScope();
    pass->bindProgram( m_shader );
    pass->enableFeatures( ShaderEmissionColor | ShaderAmbientColor );

    RenderPassBase::emitStaticMeshes( m_renderScene.staticMeshes(), frame, commands, stateStack );
    RenderPassBase::emitPointClouds( m_renderScene.pointClouds(), frame, commands, stateStack );
}

// ------------------------------------------------------- ShadowPass ------------------------------------------------------- //

// ** ShadowPass::ShadowPass
ShadowPass::ShadowPass( RenderingContext& context, RenderScene& renderScene )
    : RenderPassBase( context, renderScene )
{
    // Create a shadowmap shader
    m_shader = m_context.deprecatedRequestShader( "../../Source/Dreemchest/Scene/Rendering/Shaders/Shadow.shader" );

    // Create a shadow parameters constant buffer
    m_cbuffer = m_context.deprecatedRequestConstantBuffer( NULL, sizeof( RenderScene::CBuffer::Shadow ), RenderScene::CBuffer::Shadow::Layout );
}

// ** ShadowPass::cbuffer
ConstantBuffer_ ShadowPass::cbuffer( void ) const
{
    return m_cbuffer;
}

// ** ShadowPass::render
TransientRenderTarget ShadowPass::render( RenderFrame& frame, CommandBuffer& commands, StateStack& stateStack, const RenderScene::CBuffer::Shadow& parameters )
{
    // Acquire a shadow render target
    s32 dimensions  = static_cast<s32>( 1.0f / parameters.invSize );
    TransientRenderTarget renderTarget = commands.acquireRenderTarget( dimensions, dimensions, Renderer::PixelD24X8 );

    // Render scene from a light's point of view
    CommandBuffer& cmd = commands.renderToTarget( frame, renderTarget );
    cmd.clear( Rgba( 1.0f, 1.0f, 1.0f, 1.0f ), ~0 );

    // Update a shadow constant buffer
    cmd.uploadConstantBuffer( m_cbuffer, frame.internBuffer( &parameters, sizeof parameters ), sizeof parameters );

    // Push a shadow pass scope
    StateScope state = stateStack.newScope();
    state->bindConstantBuffer( m_cbuffer, Constants::Shadow );
    state->bindProgram( m_shader );
    state->setCullFace( Renderer::TriangleFaceFront );

    // Render all static meshes to a target
    RenderPassBase::emitStaticMeshes( m_renderScene.staticMeshes(), frame, cmd, stateStack );

    return renderTarget;
}

} // namespace Scene

DC_END_DREEMCHEST
