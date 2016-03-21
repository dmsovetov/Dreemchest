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

#include "Commands.h"
#include "../RenderTarget.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** Commands::Commands
Commands::Commands( void )
    : m_operations( 16 )
    , m_commands( 131072 * 10 )
    , m_sequence( 0 )
{
}

// ** Commands::sort
void Commands::sort( void )
{
    std::sort( m_operations.items().begin(), m_operations.items().begin() + m_operations.allocatedCount() );
}

// ** Commands::clear
void Commands::clear( void )
{
    // Clear commands & user data
    m_operations.reset();
    m_commands.reset();

    // Reset the sequence number
    m_sequence = 0;
}

// ** Commands::dump
void Commands::dump( void ) const
{
    CString commands[] = {
          "pushRenderTarget"
        , "popRenderTarget"
        , "rasterOptions" 
        , "constantColor"
        , "shader"
        , "programInput"
        , "drawIndexed"
    };
    CString modes[] = {
		  "opaque"
		, "cutout"
		, "translucent"
		, "additive"
    };
    CString inputs[] = {
          "ViewProjection"            //!< The view-projection matrix input.
        , "Transform"                 //!< The model matrix input.
        , "InverseTransform"          //!< The inverse model matrix input.
        , "Color"                     //!< The constant color input.
        , "DiffuseTexture"            //!< Diffuse texture sampler input.
        , "SpecularTexture"           //!< Specular texture sampler input.
        , "NormalTexture"             //!< Normal texture sampler input.
        , "HeightmapTexture"          //!< Heightmap texture sampler input.
        , "AmbientOcclusionTexture"   //!< AO texture sampler input.
        , "EmissionTexture"           //!< Emission texture sampler input.
        , "DiffuseDetailTexture"      //!< Diffuse detail texture sampler input.
        , "NormalDetailTexture"       //!< Normal detail texture sampler input.
        , "TintTexture"               //!< Tint texture sampler input.
        , "DiffuseColor"              //!< Diffuse color input.
        , "SpecularColor"             //!< Specular color input.
        , "AmbientColor"              //!< Ambient color input.
        , "EmissionColor"             //!< Emission color input.
        , "TintColor"                 //!< Tint color input.
        , "LightPosition"             //!< Light position input.
        , "LightColor"                //!< Light color input.
    };

    for( s32 i = 0; i < size(); i++ ) {
        s32 opCode = opCodeAt( i );

        switch( opCode ) {
        case OpDrawIndexed:         {  
                                        const DrawIndexed& draw = commandAt<DrawIndexed>( i );
                                        LogVerbose( "rvm", "%s %s technique=%d, renderable=%d\n", commands[opCode], modes[draw.mode], draw.technique, draw.renderable );
                                    }
                                    break;
        case OpPushRenderTarget:    {
                                        const PushRenderTarget& cmd = commandAt<PushRenderTarget>( i );
                                        LogVerbose( "rvm", "%s viewport=(%d %d %d %d)\n", commands[opCode], cmd.viewport[0], cmd.viewport[1], cmd.viewport[2], cmd.viewport[3] );
                                    }
                                    break;
        default:                    {
                                        LogVerbose( "rvm", "%s\n", commands[opCode] );
                                    }
        }
    }
}

// ** Commands::size
s32 Commands::size( void ) const
{
    return m_operations.allocatedCount();
}

// ** Commands::opCodeAt
s32 Commands::opCodeAt( s32 index ) const
{
    return *reinterpret_cast<const s32*>( m_commands.data() + static_cast<u32>( m_operations[index] & 0xFFFFFFFF ) );
}

// ** Commands::beginSequence
u8 Commands::beginSequence( void )
{
    DC_BREAK_IF( static_cast<s32>( m_sequence ) + 1 >= 255, "sequence number overflow" );
    return m_sequence++;
}

// ** Commands::endSequence
u8 Commands::endSequence( void )
{
    DC_BREAK_IF( static_cast<s32>( m_sequence ) + 1 >= 255, "sequence number overflow" );
    return ++m_sequence;
}

// ** Commands::emitPushRenderTarget
void Commands::emitPushRenderTarget( RenderTargetWPtr renderTarget, const Matrix4& viewProjection, const Rect& viewport )
{
    PushRenderTarget* cmd = allocateCommand<PushRenderTarget>( RVM_SORT( (beginSequence() << 24) ) );

    cmd->instance = renderTarget.get();
    cmd->viewport[0] = static_cast<u32>( viewport.min().x );
    cmd->viewport[1] = static_cast<u32>( viewport.min().y );
    cmd->viewport[2] = static_cast<u32>( viewport.width() );
    cmd->viewport[3] = static_cast<u32>( viewport.height() );

    for( s32 i = 0; i < 16; i++ ) {
        cmd->vp[i] = viewProjection[i];
    }
}

// ** Commands::emitPopRenderTarget
void Commands::emitPopRenderTarget( void )
{
    allocateCommand<PopRenderTarget>( beginSequence() << 24 );
}

// ** Commands::emitRasterOptions
void Commands::emitRasterOptions( u8 renderingModes, const RasterizationOptions& options )
{
    SetRasterOptions* cmd = allocateCommand<SetRasterOptions>( beginSequence() << 24 );
    cmd->options = options;
    cmd->modes = renderingModes;
}

// ** Commands::emitLightingShader
void Commands::emitLightingShader( u8 models, s32 shader )
{
    Shader* cmd = allocateCommand<Shader>( beginSequence() << 24 );
    cmd->shader = shader;
    cmd->models = models;
}

// ** Commands::emitConstantColor
void Commands::emitConstantColor( const Rgba& value )
{
    ConstantColor* cmd = allocateCommand<ConstantColor>( beginSequence() << 24 );
    cmd->color[0] = value.r;
    cmd->color[1] = value.g;
    cmd->color[2] = value.b;
    cmd->color[3] = value.a;
}

// ** Commands::emitProgramInput
void Commands::emitProgramInput( Program::Input location, const Vec4& value )
{
    ProgramInput* cmd = allocateCommand<ProgramInput>( beginSequence() << 24 );
    cmd->location = location;
    memcpy( cmd->value, &value.x, sizeof( cmd->value ) );
}

} // namespace Scene

DC_END_DREEMCHEST