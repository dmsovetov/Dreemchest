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

#if DEV_VIRTUAL_EMITTERS
// ** StaticMeshEmitter::StaticMeshEmitter
StaticMeshEmitter::StaticMeshEmitter( RenderingContext& context, u32 renderModes )
    : RopEmitter( context )
    , m_renderModes( renderModes )
{
}

// ** StaticMeshEmitter::emit
void StaticMeshEmitter::emit( const Vec3& camera, const StaticMesh& staticMesh, const Transform& transform )
{
    // Get the material
    const MaterialHandle& material = staticMesh.material( 0 );
    RenderingMode         mode     = material->renderingMode();

    // Does this material passes the filter?
    if( (BIT( mode ) & m_renderModes) == 0 ) {
        return;
    }

    // Get the command buffer
    Commands& commands = m_context.commands();

    // Request the renderable asset for this mesh.
    s32 renderable = m_context.requestRenderable( staticMesh.mesh() );

    // Request the technique asset for a material.
    s32 technique = m_context.requestTechnique( material );

    // Calculate the distance to this renderable
    f32 distance = (camera - transform.position()).length();

    // Emit additional draw call for additive & translucent two-sided materials.
    if( material->isTwoSided() ) {
        Commands::InstanceData* instance = commands.emitDrawCall( &transform.matrix(), renderable, technique, mode, distance );
        instance->culling = Renderer::TriangleFaceFront;
    }

    // Emit the rendering command
    commands.emitDrawCall( &transform.matrix(), renderable, technique, mode, distance );
}
#else

// ** StaticMeshEmitter::StaticMeshEmitter
StaticMeshEmitter::StaticMeshEmitter( RenderingContext& context, u32 renderModes )
    : FixedRopEmitter( context )
    , m_renderModes( renderModes )
{
}

// ** StaticMeshEmitter::emit
void StaticMeshEmitter::emit( const Vec3& camera )
{
    // Get all renderable meshes
    const RenderEntities& meshes = renderEntities();

    // Get the command buffer
    Commands& commands = m_context.commands();

    // Process each mesh entity
    for( s32 i = 0, n = meshes.count(); i < n; i++ ) {
        // Get mesh entity by index
        const StaticMeshRenderable& mesh = meshes[i];

        // Get the material
        const Material& material = mesh.material.readLock();

        // Get the material rendering mode
        RenderingMode mode = material.renderingMode();

        // Does this material passes the filter?
        if( (BIT( mode ) & m_renderModes) == 0 ) {
            continue;
        }

        mesh.mesh.readLock();

        // Calculate the distance to this renderable
        f32 distance = (camera - mesh.transform->worldSpacePosition()).length();

        // Emit additional draw call for additive & translucent two-sided materials.
        if( material.isTwoSided() ) {
            Commands::InstanceData* instance = commands.emitDrawCall( mesh.matrix, mesh.renderable, mesh.technique, mode, distance );
            instance->culling = Renderer::TriangleFaceFront;
        }

        // Emit the rendering command
        commands.emitDrawCall( mesh.matrix, mesh.renderable, mesh.technique, mode, distance );
    }
}

// ** StaticMeshEmitter::createRenderEntity
StaticMeshRenderable StaticMeshEmitter::createRenderEntity( const Ecs::Entity& entity )
{
    const Transform*  transform  = entity.get<Transform>();
    const StaticMesh* staticMesh = entity.get<StaticMesh>();

    StaticMeshRenderable mesh;
    mesh.transform  = transform;
    mesh.matrix     = &transform->matrix();
    mesh.material   = staticMesh->material( 0 );
    mesh.mesh       = staticMesh->mesh();
    mesh.renderable = m_context.requestRenderable( staticMesh->mesh() );
    mesh.technique  = m_context.requestTechnique( mesh.material );
    return mesh;
}

#endif  /*  DEV_VIRTUAL_EMITTERS    */

} // namespace Scene

DC_END_DREEMCHEST