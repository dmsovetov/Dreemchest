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
StaticMeshEmitter::StaticMeshEmitter( RenderScene& renderScene, u32 renderModes, bool depthSort )
    : AbstractRopEmitter( renderScene )
    , m_renderModes( renderModes )
    , m_depthSort( depthSort )
{
}

// ** StaticMeshEmitter::emit
void StaticMeshEmitter::emit( Commands& commands, const Vec3& camera )
{
    // Get all renderable meshes
    const RenderScene::Meshes& meshes = m_renderScene.meshes();

    // Process each mesh entity
    for( s32 i = 0, n = meshes.count(); i < n; i++ ) {
        // Get mesh entity by index
        const RenderScene::RenderableMesh& mesh = meshes[i];

        // Get the material
        const Material& material = mesh.material.readLock();

        // Get the material rendering mode
        RenderingMode mode = material.renderingMode();

        // Does this material passes the filter?
        if( (BIT( mode ) & m_renderModes) == 0 ) {
            continue;
        }

        mesh.mesh.readLock();

        // Compose the sorting key
        u32 sortKey = 0;

        if( m_depthSort ) {
            // Calculate the distance to this renderable
            f32 distance = m_depthSort ? (camera - mesh.transform->worldSpacePosition()).length() : 0.0f;

            // Convert depth to an integer value
            sortKey = static_cast<u32>( 4194303 * (1.0f - (distance / 1000.0f)) );
        } else {
            // No depth sorting, so just compose the key from renderable and technique
            sortKey = (mesh.technique << 11) | mesh.renderable;
        }

        // Emit additional draw call for additive & translucent two-sided materials.
        if( material.isTwoSided() ) {
            Commands::DrawIndexed* instance = commands.emitDrawCall( sortKey, mesh.matrix, mesh.renderable, mesh.technique, mode );
            instance->culling = Renderer::TriangleFaceFront;
        }

        // Emit the rendering command
        commands.emitDrawCall( sortKey, mesh.matrix, mesh.renderable, mesh.technique, mode );
    }
}

} // namespace Scene

DC_END_DREEMCHEST