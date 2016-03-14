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

#include "RenderingContext.h"
#include "Rvm/Rvm.h"

#include "Emitters/StaticMeshEmitter.h"

#include "../Components/Rendering.h"
#include "../Components/Transform.h"
#include "../Assets/Renderable.h"
#include "../Assets/RenderAssetSources.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext( Assets::Assets& assets, Renderer::HalWPtr hal, SceneWPtr scene )
	: m_hal( hal )
    , m_scene( scene )
    , m_assets( assets )
{
    m_rvm           = DC_NEW Rvm( this, m_renderables.handles(), m_techniques.handles(), m_hal );
    m_opaque        = DC_NEW StaticMeshEmitter( scene, RenderOpaqueBit | RenderCutoutBit );
    m_translucent   = DC_NEW StaticMeshEmitter( scene, RenderTranslucentBit );
    m_additive      = DC_NEW StaticMeshEmitter( scene, RenderAdditiveBit );
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

// ** RenderingContext::begin
void RenderingContext::begin( void )
{
    // Get all active scene cameras
    const Ecs::EntitySet& cameras = m_scene->cameras()->entities();

    // Clear each render target
    for( Ecs::EntitySet::const_iterator i = cameras.begin(), end = cameras.end(); i != end; i++ ) {
		Camera*			 camera = (*i)->get<Camera>();
		RenderTargetWPtr target = camera->target();

		if( !target.valid() ) {
			continue;
		}

		target->begin( this );
		{
			m_hal->setViewport( camera->viewport() );
			u32 mask = ( camera->clearMask() & Camera::ClearColor ? Renderer::ClearColor : 0 ) | ( camera->clearMask() & Camera::ClearDepth ? Renderer::ClearDepth : 0 );
			m_hal->clear( camera->clearColor(), 1.0f, 0, mask );
			m_hal->setViewport( target->rect() );
		}
		target->end( this );
	}

    for( Ecs::EntitySet::const_iterator i = cameras.begin(), end = cameras.end(); i != end; i++ ) {
        renderFromCamera( *(*i).get(), *(*i)->get<Camera>(), *(*i)->get<Transform>() );
    }
    m_rvm->flush();
}

// ** RenderingContext::end
void RenderingContext::end( void )
{

}

// ** RenderingContext::renderFromCamera
void RenderingContext::renderFromCamera( Ecs::Entity& entity, Camera& camera, Transform& transform )
{
    // Begin rendering by pushing a render target
    m_rvm->emitPushRenderTarget( camera.target(), camera.calculateViewProjection( transform.matrix() ), camera.viewport() );

    // Now emit all render operations
    m_rvm->emitRasterOptions( RenderOpaqueBit, Rvm::RasterizationOptions::opaque() );
    m_rvm->emitRasterOptions( RenderCutoutBit, Rvm::RasterizationOptions::translucent() );
    m_opaque->emit( transform.position(), *this, *m_rvm );

    m_rvm->emitRasterOptions( RenderTranslucentBit, Rvm::RasterizationOptions::translucent() );
    m_translucent->emit( transform.position(), *this, *m_rvm );

    m_rvm->emitRasterOptions( RenderAdditiveBit, Rvm::RasterizationOptions::additive() );
    m_additive->emit( transform.position(), *this, *m_rvm );

    // End rendering by popping a render target
    m_rvm->emitPopRenderTarget();
}

} // namespace Scene

DC_END_DREEMCHEST