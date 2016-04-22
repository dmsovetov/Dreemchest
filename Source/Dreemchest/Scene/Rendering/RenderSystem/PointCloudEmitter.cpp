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

#include "PointCloudEmitter.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** PointCloudEmitter::PointCloudEmitter
PointCloudEmitter::PointCloudEmitter( RenderScene& renderScene )
    : RopEmitter( renderScene )
{
}

// ** PointCloudEmitter::emit
void PointCloudEmitter::emit( RenderingContext& context, RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Filter& filter )
{
    // Get all point clouds that reside in scene
    const RenderScene::PointClouds& pointClouds = m_renderScene.pointClouds();

    // Process each active point cloud
    for( s32 i = 0, n = pointClouds.count(); i < n; i++ ) {
        // Get a point cloud by index
        const RenderScene::PointCloudNode& pointCloud = pointClouds[i];

        // Read-lock a point cloud material
        const Material& material = pointCloud.material.readLock();

        // Does a material pass a filter?
        if( (filter.lightingModels & BIT( material.lightingModel() )) == 0 ) continue;
        if( (filter.renderModes    & BIT( material.renderingMode() )) == 0 ) continue;

        RenderStateBlock& instance = stateStack.push();
        instance.bindVertexBuffer( pointCloud.vertexBuffer );
        instance.bindConstantBuffer( pointCloud.instanceConstants, RenderState::InstanceConstants );
        instance.bindConstantBuffer( pointCloud.materialConstants, RenderState::MaterialConstants );
        instance.bindInputLayout( pointCloud.inputLayout );

        if( material.lightingModel() == LightingModel::Unlit ) {
            instance.disableFeatures( BIT( ShaderAmbientColor ) );
        }

        commands.drawPrimitives( 0, Renderer::PrimPoints, stateStack.states(), 0, pointCloud.vertexCount );
        stateStack.pop();
    }
}

} // namespace Scene

DC_END_DREEMCHEST