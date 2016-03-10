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

#include "LightPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

#if DEV_DEPRECATED_SCENE_RENDERER

// ** ForwardLightPass::ForwardLightPass
ForwardLightPass::ForwardLightPass( Ecs::EcsWPtr ecs ) : RenderPass( ecs, "ForwardLightPass" )
{
	addEmitter( DC_NEW StaticMeshRopEmitter( ecs, StaticMeshRopEmitter::All ) );
}

// ** ForwardLightPass::setup
void ForwardLightPass::setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection )
{
	// Override blend mode for opaque & cutout rendering modes
	rvm.setRasterization( RenderOpaqueBit | RenderCutoutBit, Rvm::AdditiveRasterization );

	// Skip additive & translucent objects in this pass
	rvm.setRasterization( RenderAdditiveBit | RenderTranslucentBit, Rvm::SkipRasterization );
}

// ** ForwardLightPass::render
void ForwardLightPass::render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const Light& light, const Transform& transform )
{
	const Rgb& color	= light.color();
	Vec3	   position = transform.worldSpacePosition();

	// Check if light is in camera frustum.
	Vec3		 range   = Vec3( light.range(), light.range(), light.range() );
	PlaneClipper frustum = PlaneClipper::createFromFrustum( m_camera->calculateViewProjection( m_transform->matrix() ) );
	Bounds		 bounds  = Bounds( position - range, position + range );

	if( !frustum.inside( bounds ) ) {
		return;
	}

	// Set light registers
	rvm.setRegister( Rvm::LightColor, Vec4( color.r, color.g, color.b, light.intensity() ) );
	rvm.setRegister( Rvm::LightPosition, Vec4( position.x, position.y, position.z, light.range() ) );

	// Set the light clipper
	setClipper( PlaneClipper::createFromBox( position, light.range() ) );

	// Emit render operations
	emit( *context.get(), rvm, shaders );

	rvm.flush();
}

// ** AdditiveLightPass::AdditiveLightPass
AdditiveLightPass::AdditiveLightPass( Ecs::EcsWPtr ecs )
{
	addEmitter( DC_NEW StaticMeshRopEmitter( ecs, StaticMeshRopEmitter::Unlit ) );
}

// ** AdditiveLightPass::setup
void AdditiveLightPass::setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection )
{
	// Only additive render mode will pass through
	rvm.setRasterization( RenderOpaqueBit | RenderCutoutBit | RenderTranslucentBit, Rvm::SkipRasterization );
}

// ** TranslucentLightPass::TranslucentLightPass
TranslucentLightPass::TranslucentLightPass( Ecs::EcsWPtr ecs ) : ForwardLightPass( ecs )
{
	addEmitter( DC_NEW StaticMeshRopEmitter( ecs, StaticMeshRopEmitter::Unlit ) );
}

// ** TranslucentLightPass::setup
void TranslucentLightPass::setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection )
{
	// Disable all render modes except the translucent one
	rvm.setRasterization( RenderOpaqueBit | RenderCutoutBit | RenderAdditiveBit, Rvm::SkipRasterization );

	// Override blend mode for lit translucent objects
	Rvm::RasterizationOptions litTranslucent = Rvm::TranslucentRasterization.overrideBlending( Renderer::BlendSrcAlpha, Renderer::BlendOne );
	rvm.setRasterization( RenderTranslucent, litTranslucent );
}

#endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */

} // namespace Scene

DC_END_DREEMCHEST