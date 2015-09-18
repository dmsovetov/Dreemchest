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
	Rvm::RasterizationOptions additive = Rvm::OpaqueRasterization;
	additive.m_blend[0] = additive.m_blend[1] = Renderer::BlendOne;

	rvm.setRasterization( RenderOpaque, additive );
	rvm.setRasterization( RenderCutout, additive );

	// Additive render mode can't be lit
	rvm.setRasterization( RenderAdditive, Rvm::SkipRasterization );

	// Override blend mode for lit translucent objects
	Rvm::RasterizationOptions options = Rvm::TranslucentRasterization;
	options.m_blend[0] = Renderer::BlendSrcAlpha;
	options.m_blend[1] = Renderer::BlendOne;
	rvm.setRasterization( RenderTranslucent, options );
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
	rvm.setRasterization( RenderOpaque, Rvm::SkipRasterization );
	rvm.setRasterization( RenderCutout, Rvm::SkipRasterization );
	rvm.setRasterization( RenderTranslucent, Rvm::SkipRasterization );
}

} // namespace Scene

DC_END_DREEMCHEST