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

#include "ParticleRendering.h"

#include "../Assets/Material.h"
#include "../Assets/Image.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ** ParticleRenderingInterface::ParticleRenderingInterface
ParticleRenderingInterface::ParticleRenderingInterface( Renderer::Renderer2DPtr renderer ) : m_renderer( renderer )
{

}

// ** ParticleRenderingInterface::renderPoints
void ParticleRenderingInterface::renderPoints( const Vec3* position, const Rgba* color, const f32* size, s32 count, s32 stride )
{
	DC_NOT_IMPLEMENTED;
}

// ** ParticleRenderingInterface::renderPoints
void ParticleRenderingInterface::renderPoints( const Vec2* position, const Rgba* color, const f32* size, s32 count, s32 stride )
{
	m_renderer->points( position, color, size, count, stride );
}

// ** ParticleRenderingInterface::renderPoints
void ParticleRenderingInterface::renderOrientedQuadUV( const Fx::IMaterialWPtr& material, f32 x, f32 y, f32 width, f32 height, const Vec2& up, const Vec2& side, const Rgba& color )
{
	MaterialPtr			 mat = static_cast<Material*>( material->get().get() );
	ImagePtr			 img = mat->texture( Material::Diffuse );

	DC_NOT_IMPLEMENTED;
	Renderer::TexturePtr tex;
//	Renderer::TexturePtr tex = img.valid() ? (img->data().valid() ? img->data()->texture : Renderer::TexturePtr()) : Renderer::TexturePtr();

	// Material blending mapping
	Renderer::Renderer2D::BlendingMode blending[TotalRenderModes] = {
		  Renderer::Renderer2D::BlendingDisabled	// RenderOpaque
		, Renderer::Renderer2D::BlendingDisabled	// RenderCutout
		, Renderer::Renderer2D::BlendingAlpha		// RenderTranslucent
		, Renderer::Renderer2D::BlendingAdditive	// RenderAdditive
	};

	m_renderer->setBlendMode( blending[mat->renderingMode()] );
	m_renderer->orientedQuad( tex, x, y, width, height, up, side, color );
}

// ** ParticleRenderingInterface::renderPoints
void ParticleRenderingInterface::renderLine( f32 x1, f32 y1, f32 x2, f32 y2, const Rgba& color1, const Rgba& color2 )
{
	m_renderer->line( x1, y1, x2, y2, color1, color2 );
}

// ** ParticleRenderingInterface::renderPoints
void ParticleRenderingInterface::renderThickLine( const Fx::IMaterialWPtr& material, f32 x1, f32 y1, f32 x2, f32 y2, f32 size1, f32 size2, const Rgba& color1, const Rgba& color2 )
{
	DC_NOT_IMPLEMENTED;
}

// ** ParticleRenderingInterface::renderPoints
void ParticleRenderingInterface::renderLineStrip( const Vec2* position, const Rgb* color, s32 count, s32 stride, f32 alpha )
{
	DC_NOT_IMPLEMENTED;
}

// ** ParticleRenderingInterface::renderPoints
void ParticleRenderingInterface::renderThickLineStrip( const Fx::IMaterialWPtr& material, const Vec2* positions, const Rgb* colors, const f32* sizes, s32 count, s32 stride, f32 alpha )
{
	DC_NOT_IMPLEMENTED;
}

// ** ParticleRenderingInterface::renderPoints
void ParticleRenderingInterface::flush( void )
{
	m_renderer->flush();
}

} // namespace Scene

DC_END_DREEMCHEST