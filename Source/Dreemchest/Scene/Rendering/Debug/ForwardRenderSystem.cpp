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

#include "ForwardRenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** ForwardRenderSystem::ForwardRenderSystem
ForwardRenderSystem::ForwardRenderSystem( RenderingContext& context, RenderScene& renderScene )
    : RenderSystem( context, renderScene )
{
    m_phongShader       = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Phong.shader" );
    m_ambientShader     = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Ambient.shader" );
    m_shadowShader      = m_context.createShader( "../Source/Dreemchest/Scene/Rendering/Shaders/Shadow.shader" );
    m_shadowCBuffer     = m_context.requestConstantBuffer( NULL, sizeof RenderScene::CBuffer::Shadow, RenderScene::CBuffer::Shadow::Layout );
}

// ** ForwardRenderSystem::emitRenderOperations
void ForwardRenderSystem::emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const ForwardRenderer& forwardRenderer )
{
    // Ambient pass
    {
        StateScope pass = stateStack.newScope();
        pass->bindProgram( m_context.internShader( m_ambientShader ) );
        pass->enableFeatures( ShaderEmissionColor | ShaderAmbientColor );

        emitStaticMeshes( frame, commands, stateStack );
        emitPointClouds( frame, commands, stateStack );
    }

    // Get all light sources
    const RenderScene::Lights& lights = m_renderScene.lights();

    // Render a scene for each light in scene
    for( s32 i = 0, n = lights.count(); i < n; i++ ) {
        // Get a light by index
        const RenderScene::LightNode& light = lights[i];

        if( light.light->type() == LightType::Directional ) {
            for( s32 j = 0; j < 1; j++ ) {
                ShadowParameters parameters = directionalLightShadows( camera, transform.matrix(), transform.worldSpacePosition(), light, 1024, j, 1 );
                u8 shadows = renderShadows( frame, commands, stateStack, light, 1024, parameters );
                renderLight( frame, commands, stateStack, light, shadows );
                commands.releaseRenderTarget( shadows );
            }
            continue;
        }

        // A shadow map render target handle
        u8 shadows = 0;

        if( light.light->castsShadows() ) {
            ShadowParameters parameters = spotLightShadows( light, 1024 );
            shadows = renderShadows( frame, commands, stateStack, light, 1024, parameters );
        }

        // Render a light pass
        renderLight( frame, commands, stateStack, light, shadows );

        // Release an intermediate shadow render target
        if( shadows ) {
            commands.releaseRenderTarget( shadows );
        }
    }
}

// ** ForwardRenderSystem::renderShadows
u8 ForwardRenderSystem::renderShadows( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::LightNode& light, s32 dimensions, const ShadowParameters& parameters )
{
    DC_BREAK_IF( !light.light->castsShadows(), "a light instance does not cast shadows" );

    // Acquire a shadow render target
    u8 renderTarget = commands.acquireRenderTarget( dimensions, dimensions, Renderer::PixelD24X8 );

    // Render scene from a light's point of view
    RenderCommandBuffer& cmd = commands.renderToTarget( renderTarget );
    cmd.clear( Rgba( 1.0f, 1.0f, 1.0f, 1.0f ), ~0 );

    // Update a shadow constant buffer
    cmd.uploadConstantBuffer( m_shadowCBuffer, frame.internBuffer( &parameters, sizeof parameters ), sizeof parameters );

    // Push a shadow pass scope
    StateScope state = stateStack.newScope();
    state->bindConstantBuffer( m_shadowCBuffer, RenderState::ShadowConstants );
    state->bindProgram( m_context.internShader( m_shadowShader ) );
    state->setCullFace( Renderer::TriangleFaceFront );

    // Render all static meshes to a target
    emitStaticMeshes( frame, cmd, stateStack );

    return renderTarget;
}

// ** ForwardRenderSystem::renderLight
void ForwardRenderSystem::renderLight( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::LightNode& light, u8 shadows )
{
    // A light type feature bits
    Ubershader::Bitmask lightType[] = { ShaderPointLight, ShaderSpotLight, ShaderDirectionalLight };

    // Light state block
    StateScope state = stateStack.newScope();
    state->bindConstantBuffer( light.constantBuffer, RenderState::LightConstants );
    state->enableFeatures( lightType[light.light->type()] | ShaderShadowFiltering3 );
    state->bindProgram( m_context.internShader( m_phongShader ) );
    state->setBlend( Renderer::BlendOne, Renderer::BlendOne );

    // Bind a rendered shadowmap
    if( shadows ) {
        state->bindRenderedTexture( shadows, RenderState::Texture1, Renderer::RenderTarget::Depth );
        state->bindConstantBuffer( m_shadowCBuffer, RenderState::ShadowConstants );
    }

    // Emit render operations
    emitStaticMeshes( frame, commands, stateStack, RenderMaskPhong );
    emitPointClouds( frame, commands, stateStack, RenderMaskPhong );
}

// ** ForwardRenderSystem::spotLightShadows
ForwardRenderSystem::ShadowParameters ForwardRenderSystem::spotLightShadows( const RenderScene::LightNode& light, s32 dimensions ) const
{
    ShadowParameters parameters;
    parameters.transform = Matrix4::perspective( light.light->cutoff() * 2.0f, 1.0f, 0.1f, light.light->range() * 2.0f ) * light.matrix->inversed();
    parameters.invSize   = 1.0f / dimensions;
    return parameters;
}

// ** ForwardRenderSystem::directionalLightShadows
ForwardRenderSystem::ShadowParameters ForwardRenderSystem::directionalLightShadows( const Camera& camera, const Matrix4& cameraInverseTransform, const Vec3& cameraPosition, const RenderScene::LightNode& light, s32 dimensions, s32 split, s32 maxSplits ) const
{
    f32 range     = camera.far() - camera.near();
    f32 splitSize = range / maxSplits;

    Bounds bounds = calculateSplitBounds( camera, cameraInverseTransform, *light.matrix, camera.near() + splitSize * split, camera.near() + splitSize * (split + 1) );

    ShadowParameters parameters;
    parameters.transform = Matrix4::ortho( bounds.min().x, bounds.max().x, bounds.min().y, bounds.max().y, bounds.min().z, bounds.max().z ) * light.matrix->inversed();
    parameters.invSize   = 1.0f / dimensions;
    return parameters;
}

// ** ForwardRenderSystem::calculateSplitBounds
Bounds ForwardRenderSystem::calculateSplitBounds( const Camera& camera, const Matrix4& cameraInverseTransform, const Matrix4& lightTransform, f32 near, f32 far ) const
{
    // Get the camera aspect ratio and field of view
    f32 ar  = camera.aspect();
    f32 fov = camera.fov();

    f32 tanHalfHFOV = tanf( radians( fov / 2.0f ) );
    f32 tanHalfVFOV = tanf( radians( (fov * ar) / 2.0f ) );

    // Calculate dimensions of a split far and near faces
    f32 xn = near * tanHalfHFOV;
    f32 xf = far  * tanHalfHFOV;
    f32 yn = near * tanHalfVFOV;
    f32 yf = far  * tanHalfVFOV;

    // Construct frustum vertices in a view space
    Vec4 frustumCorners[8] = {
          {  xn,  yn, near, 1.0 }   // Near split face
        , { -xn,  yn, near, 1.0 }
        , {  xn, -yn, near, 1.0 }
        , { -xn, -yn, near, 1.0 }
        , {  xf,  yf, far,  1.0 }   // Far split face
        , { -xf,  yf, far,  1.0 }
        , {  xf, -yf, far,  1.0 }
        , { -xf, -yf, far,  1.0 }
    };

    // Calculate a light space split center
    Bounds v;
    for( s32 i = 0; i < 8; i++ ) v << frustumCorners[i];
    Vec4 wsCenter = cameraInverseTransform.inversed() * Vec4( v.center().x, v.center().y, v.center().z, 1.0f );
    Vec4 lsCenter = lightTransform.inversed() * wsCenter;

    // Now transform vertices to a lightspace and calculate bounding box
    Bounds result;

    for( s32 i = 0; i < 8; i++ ) {
        // Transform the frustum coordinate from view to world space
        Vec4 wsVertex = cameraInverseTransform.inversed() * frustumCorners[i];

        // Transform the frustum coordinate from world to light space
        Vec4 lsVertex = lightTransform.inversed() * wsVertex;

        // Append a vertex in a light space to split bounding box
        result << lsVertex;
    }

    // Calculate a centered orhto projection matrix
    Vec3 offset = Vec3( result.width(), result.height(), result.depth() ) * 0.5f;

    return Bounds( Vec3( lsCenter.x, lsCenter.y, lsCenter.z ) - offset, Vec3( lsCenter.x, lsCenter.y, lsCenter.z ) + offset );
}

} // namespace Scene

DC_END_DREEMCHEST