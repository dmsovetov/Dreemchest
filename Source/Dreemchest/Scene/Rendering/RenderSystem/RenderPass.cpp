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

#include "RenderPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderPassBase::RenderPassBase
RenderPassBase::RenderPassBase( RenderingContext& context, RenderScene& renderScene )
    : m_context( context )
    , m_renderScene( renderScene )
{
    // Create a material constant buffer
    m_materialCBuffer = m_context.requestConstantBuffer( NULL, sizeof( RenderScene::CBuffer::Material ), RenderScene::CBuffer::Material::Layout );
}

// ** RenderPassBase::renderWithColor
void RenderPassBase::renderWithColor( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Rgba& color )
{
    // Create a diffuse material constant buffer
    RenderScene::CBuffer::Material material = diffuseMaterial( color );

    // Upload a constant buffer to GPU
    commands.uploadConstantBuffer( m_materialCBuffer, frame.internBuffer( &material, sizeof( material ) ), sizeof( RenderScene::CBuffer::Material ) );

    // Push a material state
    StateScope state = stateStack.newScope();
    state->bindConstantBuffer( m_materialCBuffer, RenderState::MaterialConstants );

    // Emit required render operations
    begin( frame, commands, stateStack );
    emitRenderOperations( frame, commands, stateStack );
    end( frame, commands, stateStack );
}

// ** RenderPassBase::emitStaticMeshes
void RenderPassBase::emitStaticMeshes( const RenderScene::StaticMeshes& staticMeshes, RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, u8 mask )
{
    // Process each mesh entity
    for( s32 i = 0, n = staticMeshes.count(); i < n; i++ ) {
        // Get mesh entity by index
        const RenderScene::StaticMeshNode& mesh = staticMeshes[i];

        // Skip all meshes that do not pass a specified mask
        if( (mesh.mask & mask) == 0 ) {
            continue;
        }

        StateScope materialStates = stateStack.push( mesh.material.states );
        StateScope renderableStates = stateStack.push( mesh.states );

        StateScope instance = stateStack.newScope();
        instance->bindConstantBuffer( mesh.constantBuffer, RenderState::InstanceConstants );

        if( mesh.material.lighting == LightingModel::Unlit ) {
            instance->disableFeatures( ShaderAmbientColor );
        }

        commands.drawIndexed( 0, Renderer::PrimTriangles, stateStack.states(), 0, mesh.count );
    }
}

// ** RenderPassBase::emitPointClouds
void RenderPassBase::emitPointClouds( const RenderScene::PointClouds& pointClouds, RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, u8 mask )
{
    // Process each active point cloud
    for( s32 i = 0, n = pointClouds.count(); i < n; i++ ) {
        // Get a point cloud by index
        const RenderScene::PointCloudNode& pointCloud = pointClouds[i];

        // Skip all meshes that do not pass a specified mask
        if( (pointCloud.mask & mask) == 0 ) {
            continue;
        }

        StateScope materialStates = stateStack.push( pointCloud.material.states );
        StateScope renderableStates = stateStack.push( pointCloud.states );

        StateScope instance = stateStack.newScope();
        instance->bindConstantBuffer( pointCloud.constantBuffer, RenderState::InstanceConstants );

        if( pointCloud.material.lighting == LightingModel::Unlit ) {
            instance->disableFeatures( ShaderAmbientColor );
        }

        commands.drawPrimitives( 0, Renderer::PrimPoints, stateStack.states(), 0, pointCloud.count );
    }
}

// ** RenderPassBase::diffuseMaterial
RenderScene::CBuffer::Material RenderPassBase::diffuseMaterial( const Rgba& color )
{
    RenderScene::CBuffer::Material material;
    memset( &material, 0, sizeof material );
    material.diffuse = color;
    return material;
}

// ** RenderPassBase::orthoView
RenderScene::CBuffer::View RenderPassBase::orthoView( const Viewport& viewport )
{
    RenderScene::CBuffer::View view;
    view.near      = -9999;
    view.far       =  9999;
    view.transform = Matrix4::ortho( 0, viewport.width(), 0, viewport.height(), -9999, 9999 );
    return view;
}

} // namespace Scene

DC_END_DREEMCHEST
