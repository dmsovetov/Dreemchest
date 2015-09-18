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

// ** ForwardLightPass::ForwardLightPass
ForwardLightPass::ForwardLightPass( Ecs::Entities& entities ) : RenderPass( entities, "ForwardLightPass" )
{
	addEmitter( DC_NEW StaticMeshRopEmitter( entities, StaticMeshRopEmitter::All ) );
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
void ForwardLightPass::render( Rvm& rvm, ShaderCache& shaders, const Light& light, const Transform& transform )
{
	const Rgb&  color	 = light.color();
	const Vec3& position = transform.position();

	// Set light registers
	rvm.setRegister( Rvm::LightColor, Vec4( color.r, color.g, color.b, light.intensity() ) );
	rvm.setRegister( Rvm::LightPosition, Vec4( position.x, position.y, position.z, light.range() ) );

	emit( rvm, shaders );

	rvm.flush();
}

// ** AdditiveLightPass::AdditiveLightPass
AdditiveLightPass::AdditiveLightPass( Ecs::Entities& entities )
{
	addEmitter( DC_NEW StaticMeshRopEmitter( entities, StaticMeshRopEmitter::Unlit ) );
}

// ** AdditiveLightPass::setup
void AdditiveLightPass::setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection )
{
	// Only additive render mode will pass through
	rvm.setRasterization( RenderOpaqueBit | RenderCutoutBit | RenderTranslucentBit, Rvm::SkipRasterization );
}

// ** TranslucentLightPass::TranslucentLightPass
TranslucentLightPass::TranslucentLightPass( Ecs::Entities& entities ) : ForwardLightPass( entities )
{
	addEmitter( DC_NEW StaticMeshRopEmitter( entities, StaticMeshRopEmitter::Unlit ) );
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


} // namespace Scene

DC_END_DREEMCHEST