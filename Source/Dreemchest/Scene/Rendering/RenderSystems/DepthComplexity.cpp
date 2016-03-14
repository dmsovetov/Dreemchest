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

#include "DepthComplexity.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** DepthComplexity::DepthComplexity
DepthComplexity::DepthComplexity( RenderingContext& context )
    : RenderSystem( context )
{
    // Create shader
    m_technique = context.createTechnique( "constantColor", "../Source/Dreemchest/Scene/Rendering/Shaders/ConstantColor.shader" );

    // Create render operation emitters
    for( s32 i = 0; i < TotalRenderModes; i++ ) {
        m_emitters[i] = DC_NEW StaticMeshEmitter( context, BIT( i ), 0 );
    }
}

// ** DepthComplexity::emitRenderOperations
void DepthComplexity::emitRenderOperations( const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const RenderDepthComplexity& depthComplexity )
{
    // Get active command buffer
    Commands& commands = m_context.commands();

    // Set additive blend rasterization for all rendering modes
    commands.emitRasterOptions( AllRenderModesBit, RasterizationOptions::additive().overrideZWrite( true ) );

    // Set the default technique
    const Technique& tech = m_technique.readLock();
    commands.emitPushTechnique( m_technique );

    // Emit operations for all rendering mode
    for( s32 i = 0; i < TotalRenderModes; i++ ) {
        // Change the constant color for this mode
        commands.emitConstantColor( depthComplexity.colorForMode( static_cast<RenderingMode>( i ) ) * depthComplexity.intensity() );

        // Emit draw calls
        m_emitters[i]->emit( transform.position() );
    }

    // Pop the default technique
    commands.emitPopTechnique();
}

} // namespace Scene

DC_END_DREEMCHEST