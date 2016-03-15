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

#include "Unlit.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Unlit::Unlit
Unlit::Unlit( RenderingContext& context )
    : RenderSystem( context )
{
    // Create the default technique
    m_technique = context.createTechnique( "unlit", "../Source/Dreemchest/Scene/Rendering/Shaders/Unlit.shader" );
    m_technique.writeLock()->setColor( 0, Rgba( 1.0f, 1.0f, 1.0f ) );

    // Create render operation emitters
    m_opaque        = DC_NEW StaticMeshEmitter( context, RenderOpaqueBit | RenderCutoutBit, 0 );
    m_translucent   = DC_NEW StaticMeshEmitter( context, RenderTranslucentBit, 0 );
    m_additive      = DC_NEW StaticMeshEmitter( context, RenderAdditiveBit, 0 );
}

// ** Unlit::emitRenderOperations
void Unlit::emitRenderOperations( const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const RenderUnlit& unlit )
{
    // Get active command buffer
    Commands& commands = m_context.commands();

    // Set the default technique
    const Technique& tech = m_technique.readLock();
    commands.emitPushTechnique( m_technique );

    // Emit operations for opaque objects
    commands.emitRasterOptions( RenderOpaqueBit, RasterizationOptions::opaque() );
    commands.emitRasterOptions( RenderCutoutBit, RasterizationOptions::cutout() );
    m_opaque->emit( transform.position() );

    // Emit operations for translucent objects
    commands.emitRasterOptions( RenderTranslucentBit, RasterizationOptions::translucent() );
    m_translucent->emit( transform.position() );

    // Emit operations for additive objects
    commands.emitRasterOptions( RenderAdditiveBit, RasterizationOptions::additive() );
    m_additive->emit( transform.position() );

    // Pop the default technique
    commands.emitPopTechnique();
}

} // namespace Scene

DC_END_DREEMCHEST