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

#include "ForwardLighting.h"

#include "../Passes/SolidAndTransparentPass.h"
#include "../Passes/AdditivePass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** ForwardLighting::ForwardLighting
ForwardLighting::ForwardLighting( RenderingContext& context )
    : RenderSystem( context )
{
    // Create shaders
    m_ambientShader     = context.createShaderSource( "Ambient", "../Source/Dreemchest/Scene/Rendering/Shaders/Ambient.shader" );
    m_additiveShader    = context.createShaderSource( "Unlit", "../Source/Dreemchest/Scene/Rendering/Shaders/Unlit.shader" );
    m_phongShader       = context.createShaderSource( "Phong", "../Source/Dreemchest/Scene/Rendering/Shaders/Phong.shader" );

    // Create render passes
    m_solidTransparent  = DC_NEW SolidAndTransparentPass( context );
    m_additive          = DC_NEW AdditivePass( context );

    // Create light index
    m_lights = context.scene()->ecs()->requestIndex( "Lights", Ecs::Aspect::all<Light, Transform>() );

    // Create render operation emitters
    m_opaque        = DC_NEW StaticMeshEmitter( context, RenderOpaqueBit | RenderCutoutBit );
    m_translucent   = DC_NEW StaticMeshEmitter( context, RenderTranslucentBit );
}

// ** ForwardLighting::emitRenderOperations
void ForwardLighting::emitRenderOperations( const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const RenderForwardLit& forwardLit )
{
    // Get the camera position.
    const Vec3& position = transform.worldSpacePosition();

    // Get active command buffer
    Commands& commands = m_context.commands();

    // Ambient pass
    m_solidTransparent->render( position, m_ambientShader );

    // Opaque objects lighting
    {
        // Set the default shader
        commands.emitLightingShader( AllLightingModelsBit, m_context.requestShaderSource( m_phongShader ) );

        // Set additive blending for opaque & cutout objects
        commands.emitRasterOptions( RenderOpaqueBit | RenderCutoutBit, RasterizationOptions::additive() );

        const Ecs::EntitySet& lights = m_lights->entities();
        for( Ecs::EntitySet::const_iterator i = lights.begin(), end = lights.end(); i != end; ++i ) {
            // Get the light components
            const Transform& transform = *(*i)->get<Transform>();
            const Light&     light     = *(*i)->get<Light>();

            // Get the light color
            const Rgb& color = light.color();

            // Set program inputs for lighting
            commands.emitProgramInput( Program::LightColor, Vec4( color.r, color.g, color.b, light.intensity() ) );
            commands.emitProgramInput( Program::LightPosition, Vec4( transform.x(), transform.y(), transform.z(), light.range() ) );
            m_opaque->emit( position );
        }
    }

    // Translucent objects lighting
    {
        // Set additive blending for opaque & cutout objects
        commands.emitRasterOptions( RenderTranslucentBit, RasterizationOptions::translucent().overrideBlending( Renderer::BlendSrcAlpha, Renderer::BlendOne ) );

        const Ecs::EntitySet& lights = m_lights->entities();
        for( Ecs::EntitySet::const_iterator i = lights.begin(), end = lights.end(); i != end; ++i ) {
            // Get the light components
            const Transform& transform = *(*i)->get<Transform>();
            const Light&     light     = *(*i)->get<Light>();

            // Get the light color
            const Rgb& color = light.color();

            // Set program inputs for lighting
            commands.emitProgramInput( Program::LightColor, Vec4( color.r, color.g, color.b, light.intensity() ) );
            commands.emitProgramInput( Program::LightPosition, Vec4( transform.x(), transform.y(), transform.z(), light.range() ) );
            m_translucent->emit( position );
        }
    }

    // Additive pass
    m_additive->render( position, m_additiveShader );
}

} // namespace Scene

DC_END_DREEMCHEST