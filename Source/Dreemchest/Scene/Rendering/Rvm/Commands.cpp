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
    , m_userData( 16 )
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
    m_userData.reset();

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
        , "LightIntensity"            //!< Light intensity input.
    };

    for( s32 i = 0; i < size(); i++ ) {
        const Rop& rop = m_operations[i];
        const UserData& ud = m_userData[rop.bits.userData];

        if( rop.bits.command ) {
            LogVerbose( "rvm", "%d: %s %s\n", rop.bits.sequence, commands[rop.bits.mode], rop.bits.mode == Commands::Rop::ProgramInput ? inputs[ud.input.location] : "" );
        }
        else {
            LogVerbose( "rvm", "%d: drawCall : %s\n", rop.bits.sequence, modes[rop.bits.mode] );
        }
    }
}

// ** Commands::size
s32 Commands::size( void ) const
{
    return m_operations.allocatedCount();
}

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

// ** Commands::beginSequence
u8 Commands::beginSequence( void )
{
    DC_BREAK_IF( m_sequence + 1 >= 16, "sequence number overflow" );
    return m_sequence++;
}

// ** Commands::endSequence
u8 Commands::endSequence( void )
{
    DC_BREAK_IF( m_sequence + 1 >= 16, "sequence number overflow" );
    return ++m_sequence;
}

// ** Commands::emitPushRenderTarget
void Commands::emitPushRenderTarget( RenderTargetWPtr renderTarget, const Matrix4& viewProjection, const Rect& viewport )
{
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
}

// ** Commands::emitPopRenderTarget
void Commands::emitPopRenderTarget( void )
{
    Rop* rop = allocateRop();
    rop->setCommand( Rop::PopRenderTarget );
    rop->bits.sequence = endSequence();
}

// ** Commands::emitRasterOptions
void Commands::emitRasterOptions( u8 renderingModes, const RasterizationOptions& options )
{
    Rop* rop = allocateRop();
    rop->setCommand( Rop::RasterOptions );
    rop->bits.sequence = beginSequence();

    UserData* userData = allocateUserData( rop );
    userData->rasterization = options;
    userData->rasterization.modes = renderingModes;
}

// ** Commands::emitLightingShader
void Commands::emitLightingShader( u8 models, s32 shader )
{
    Rop* rop = allocateRop();
    rop->setCommand( Rop::Shader );
    rop->bits.sequence = beginSequence();

    UserData* userData = allocateUserData( rop );
    userData->shader.shader = shader;
    userData->shader.models = models;
}

// ** Commands::emitConstantColor
void Commands::emitConstantColor( const Rgba& value )
{
    Rop* rop = allocateRop();
    rop->setCommand( Rop::ConstantColor );
    rop->bits.sequence = beginSequence();

    UserData* userData = allocateUserData( rop );
    userData->color[0] = value.r;
    userData->color[1] = value.g;
    userData->color[2] = value.b;
    userData->color[3] = value.a;
}

// ** Commands::emitProgramInput
void Commands::emitProgramInput( Program::Input location, const Vec4& value )
{
    Rop* rop = allocateRop();
    rop->setCommand( Rop::ProgramInput );
    rop->bits.sequence = beginSequence();

    UserData* userData = allocateUserData( rop );
    userData->input.location = location;
    memcpy( userData->input.value, &value.x, sizeof( userData->input.value ) );
}

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

} // namespace Scene

DC_END_DREEMCHEST