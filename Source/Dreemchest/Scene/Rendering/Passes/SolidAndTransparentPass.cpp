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

#include "SolidAndTransparentPass.h"

#include "../Emitters/StaticMeshEmitter.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** SolidAndTransparentPass::SolidAndTransparentPass
SolidAndTransparentPass::SolidAndTransparentPass( RenderingContext& context )
    : RenderPassBase( context )
{
    m_opaque        = DC_NEW StaticMeshEmitter( context, RenderOpaqueBit | RenderCutoutBit );
    m_translucent   = DC_NEW StaticMeshEmitter( context, RenderTranslucentBit, true );

    static_cast<StaticMeshEmitter*>( m_opaque.get() )->construct();
    static_cast<StaticMeshEmitter*>( m_translucent.get() )->construct();
}

// ** SolidAndTransparentPass::render
void SolidAndTransparentPass::render( const Vec3& camera, ShaderSourceHandle shader )
{
    // Get the commands from rendering context
    Commands& commands = m_context.commands();

    // Set the default shader
    commands.emitLightingShader( AllLightingModelsBit, m_context.requestShaderSource( shader ) );

    // Emit operations for opaque objects
    commands.emitRasterOptions( RenderOpaqueBit, RasterizationOptions::opaque() );
    commands.emitRasterOptions( RenderCutoutBit, RasterizationOptions::cutout() );
    m_opaque->emit( camera );

    // Emit operations for translucent objects
    commands.emitRasterOptions( RenderTranslucentBit, RasterizationOptions::translucent() );
    m_translucent->emit( camera );
}

} // namespace Scene

DC_END_DREEMCHEST