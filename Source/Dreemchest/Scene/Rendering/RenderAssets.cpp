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

#include "RenderAssets.h"
#include "Rvm/Rvm.h"

#include "Emitters/StaticMeshEmitter.h"
#include "RenderSystems/RenderSystem.h"

#include "../Components/Rendering.h"
#include "../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext( Assets::Assets& assets, Renderer::HalWPtr hal, SceneWPtr scene )
	: m_hal( hal )
    , m_scene( scene )
    , m_assets( assets )
{
    m_rvm           = DC_NEW Rvm( *this, m_hal );

    // Create default assets
    {
        ShaderSourceHandle pink = createShaderSource( "Placeholder.shader", "../Source/Dreemchest/Scene/Rendering/Shaders/Null.shader" );
        pink.asset().setName( "Placeholder.shader" );
        pink.forceLoad();
        m_assets.setPlaceholder<ShaderSource>( pink );
    }

    {
        MaterialHandle pink = m_assets.add<Material>( "Placeholder.material", DC_NEW Assets::NullSource );
        pink.asset().setName( "Placeholder.material" );
        pink.forceLoad();
        pink.writeLock()->setColor( Material::Diffuse, Rgba( 1.0f, 0.0f, 1.0f ) );
        m_assets.setPlaceholder<Material>( pink );
    }
}

// ** RenderingContext::create
RenderingContextPtr RenderingContext::create( Assets::Assets& assets, Renderer::HalWPtr hal, SceneWPtr scene )
{
	return RenderingContextPtr( DC_NEW RenderingContext( assets, hal, scene ) );
}

// ** RenderingContext::hal
Renderer::HalWPtr RenderingContext::hal( void ) const
{
    return m_hal;
}

// ** RenderingContext::scene
SceneWPtr RenderingContext::scene( void ) const
{
    return m_scene;
}

Rvm* RenderingContext::rvm( void ) const
{
    return m_rvm.get();
}

// ** RenderingContext::createShaderSource
ShaderSourceHandle RenderingContext::createShaderSource( const String& identifier, const String& fileName )
{
    // First lookup a previously created shader by a file name
    ShaderSourceHandle shader = m_assets.find<ShaderSource>( identifier );

    if( shader.isValid() ) {
        return shader;
    }

    // No such shader - create a new one
    shader = m_assets.add<ShaderSource>( identifier, DC_NEW ShaderSourceFormatText( fileName ) );

    // Set the shader name
    shader.asset().setName( identifier + ".shader" );

    return shader;
}

} // namespace Scene

DC_END_DREEMCHEST