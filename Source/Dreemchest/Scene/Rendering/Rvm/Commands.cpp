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
#if DEV_OLD_RENDER_COMMANDS
    , m_userData( 16 )
#else
    , m_commands( 131072 * 10 )
#endif  /*  DEV_OLD_RENDER_COMMANDS */
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
#if DEV_OLD_RENDER_COMMANDS
    m_userData.reset();
#else
    m_commands.reset();
#endif  /*  DEV_OLD_RENDER_COMMANDS */

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
#if DEV_OLD_RENDER_COMMANDS
    for( s32 i = 0; i < size(); i++ ) {
        const Rop& rop = m_operations[i];
        const UserData& ud = m_userData[rop.bits.userData];

        if( rop.bits.command ) {
            LogVerbose( "rvm", "%d: %s %s\n", rop.bits.sequence, commands[rop.bits.mode], rop.bits.mode == Commands::Rop::ProgramInput ? inputs[ud.input.location] : "" );
        }
        else {
            LogVerbose( "rvm", "%d: drawCall : %s technique=%d, renderable=%d\n", rop.bits.sequence, modes[rop.bits.mode], rop.bits.technique, rop.bits.renderable );
        }
    }
#else
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
        //case OpSetRasterOptions:    {
        //                                const SetRasterOptions& cmd = commandAt<SetRasterOptions>( i );
        //                                LogVerbose( "rvm", "%s blend=(%s %s)\n", commands[opCode], cmd.viewport[0], cmd.viewport[1], cmd.viewport[2], cmd.viewport[3] );
        //                            }
        //                            break;
        default:                    {
                                        LogVerbose( "rvm", "%s\n", commands[opCode] );
                                    }
        }

   /*     if( rop.bits.command ) {
            LogVerbose( "rvm", "%d: %s %s\n", rop.bits.sequence, commands[rop.bits.mode], rop.bits.mode == Commands::Rop::ProgramInput ? inputs[ud.input.location] : "" );
        }
        else {
            LogVerbose( "rvm", "%d: drawCall : %s technique=%d, renderable=%d\n", rop.bits.sequence, modes[rop.bits.mode], rop.bits.technique, rop.bits.renderable );
        }*/
    }
#endif
}

// ** Commands::size
s32 Commands::size( void ) const
{
    return m_operations.allocatedCount();
}

#if DEV_OLD_RENDER_COMMANDS
// ** Commands::ropAt
const Commands::Rop& Commands::ropAt( s32 index ) const
{
    NIMBLE_ABORT_IF( index < 0 || index >= size(), "index is out of range" );
    return m_operations[index];
}

// ** Commands::ropUserData
const Commands::UserData& Commands::ropUserData( const Rop& rop ) const
{
    return m_userData[rop.bits.userData];
}

// ** Commands::allocateUserData
Commands::UserData* Commands::allocateUserData( Rop* rop )
{
    s32 idx = m_userData.allocate();
    rop->bits.userData = idx;
    return &m_userData[idx];
}

// ** Commands::allocateRop
Commands::Rop* Commands::allocateRop( void )
{
    s32  idx = m_operations.allocate();
    Rop& rop = m_operations[idx];
    rop.bits.sequence = m_sequence;
    return &rop;
}
#endif

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
#if DEV_OLD_RENDER_COMMANDS
    Rop* rop = allocateRop();
    rop->setCommand( Rop::PushRenderTarget );
    rop->bits.sequence = beginSequence();

    UserData* userData = allocateUserData( rop );
    userData->rt.instance = renderTarget.get();
    userData->rt.viewport[0] = static_cast<u32>( viewport.min().x );
    userData->rt.viewport[1] = static_cast<u32>( viewport.min().y );
    userData->rt.viewport[2] = static_cast<u32>( viewport.width() );
    userData->rt.viewport[3] = static_cast<u32>( viewport.height() );

    for( s32 i = 0; i < 16; i++ ) {
        userData->rt.vp[i] = viewProjection[i];
    }
#else
    PushRenderTarget* cmd = allocateCommand<PushRenderTarget>( RVM_SORT( (beginSequence() << 24) ) );

    cmd->instance = renderTarget.get();
    cmd->viewport[0] = static_cast<u32>( viewport.min().x );
    cmd->viewport[1] = static_cast<u32>( viewport.min().y );
    cmd->viewport[2] = static_cast<u32>( viewport.width() );
    cmd->viewport[3] = static_cast<u32>( viewport.height() );

    for( s32 i = 0; i < 16; i++ ) {
        cmd->vp[i] = viewProjection[i];
    }
#endif
}

// ** Commands::emitPopRenderTarget
void Commands::emitPopRenderTarget( void )
{
#if DEV_OLD_RENDER_COMMANDS
    Rop* rop = allocateRop();
    rop->setCommand( Rop::PopRenderTarget );
    rop->bits.sequence = beginSequence();
#else
    allocateCommand<PopRenderTarget>( beginSequence() << 24 );
#endif
}

// ** Commands::emitRasterOptions
void Commands::emitRasterOptions( u8 renderingModes, const RasterizationOptions& options )
{
#if DEV_OLD_RENDER_COMMANDS
    Rop* rop = allocateRop();
    rop->setCommand( Rop::RasterOptions );
    rop->bits.sequence = beginSequence();

    UserData* userData = allocateUserData( rop );
    userData->rasterization = options;
    userData->rasterization.modes = renderingModes;
#else
    SetRasterOptions* cmd = allocateCommand<SetRasterOptions>( beginSequence() << 24 );
    cmd->options = options;
    cmd->modes = renderingModes;
#endif
}

// ** Commands::emitLightingShader
void Commands::emitLightingShader( u8 models, s32 shader )
{
#if DEV_OLD_RENDER_COMMANDS
    Rop* rop = allocateRop();
    rop->setCommand( Rop::Shader );
    rop->bits.sequence = beginSequence();

    UserData* userData = allocateUserData( rop );
    userData->shader.shader = shader;
    userData->shader.models = models;
#else
    Shader* cmd = allocateCommand<Shader>( beginSequence() << 24 );
    cmd->shader = shader;
    cmd->models = models;
#endif
}

// ** Commands::emitConstantColor
void Commands::emitConstantColor( const Rgba& value )
{
#if DEV_OLD_RENDER_COMMANDS
    Rop* rop = allocateRop();
    rop->setCommand( Rop::ConstantColor );
    rop->bits.sequence = beginSequence();

    UserData* userData = allocateUserData( rop );
    userData->color[0] = value.r;
    userData->color[1] = value.g;
    userData->color[2] = value.b;
    userData->color[3] = value.a;
#else
    ConstantColor* cmd = allocateCommand<ConstantColor>( beginSequence() << 24 );
    cmd->color[0] = value.r;
    cmd->color[1] = value.g;
    cmd->color[2] = value.b;
    cmd->color[3] = value.a;
#endif
}

// ** Commands::emitProgramInput
void Commands::emitProgramInput( Program::Input location, const Vec4& value )
{
#if DEV_OLD_RENDER_COMMANDS
    Rop* rop = allocateRop();
    rop->setCommand( Rop::ProgramInput );
    rop->bits.sequence = beginSequence();

    UserData* userData = allocateUserData( rop );
    userData->input.location = location;
    memcpy( userData->input.value, &value.x, sizeof( userData->input.value ) );
#else
    ProgramInput* cmd = allocateCommand<ProgramInput>( beginSequence() << 24 );
    cmd->location = location;
    memcpy( cmd->value, &value.x, sizeof( cmd->value ) );
#endif
}

#if DEV_OLD_RENDER_COMMANDS
// ** Commands::Rop::Rop
Commands::Rop::Rop( void )
    : key( 0 )
{
}

// ** Commands::Rop::operator <
bool Commands::Rop::operator < ( const Rop& other ) const
{
    return key < other.key;
}
#endif

} // namespace Scene

DC_END_DREEMCHEST