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

#include "StaticMeshEmitter.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** StaticMeshEmitter::StaticMeshEmitter
StaticMeshEmitter::StaticMeshEmitter( RenderScene& renderScene )
    : RopEmitter( renderScene )
{
}

// ** StaticMeshEmitter::emit
void StaticMeshEmitter::emit( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Filter& filter )
{
    // Get all static meshes meshes
    const RenderScene::StaticMeshes& meshes = m_renderScene.staticMeshes();

    // A default material instance
    Material defaultMaterial;

    // Process each mesh entity
    for( s32 i = 0, n = meshes.count(); i < n; i++ ) {
        // Get mesh entity by index
        const RenderScene::StaticMeshNode& mesh = meshes[i];

        // Get the material
        const Material& material = mesh.material.isValid() ? mesh.material.readLock() : defaultMaterial;

        // Does a material pass a filter?
        if( (filter.lightingModels & BIT( material.lightingModel() )) == 0 ) continue;
        if( (filter.renderModes    & BIT( material.renderingMode() )) == 0 ) continue;

        RenderStateBlock& instance = stateStack.push();
        instance.bindVertexBuffer( frame.internVertexBuffer( mesh.vertexBuffer ) );
        instance.bindIndexBuffer( frame.internIndexBuffer( mesh.indexBuffer ) );
        instance.bindConstantBuffer( frame.internConstantBuffer( mesh.instanceConstants ), RenderState::InstanceConstants );
        instance.bindConstantBuffer( frame.internConstantBuffer( mesh.materialConstants ), RenderState::MaterialConstants );
        instance.bindInputLayout( frame.internInputLayout( mesh.inputLayout ) );

        if( material.lightingModel() == LightingModel::Unlit ) {
            instance.disableFeatures( BIT( ShaderAmbientColor ) );
        }

        commands.drawIndexed( 0, Renderer::PrimTriangles, stateStack.states(), 0, mesh.indexCount );
        stateStack.pop();
    }
}

} // namespace Scene

DC_END_DREEMCHEST