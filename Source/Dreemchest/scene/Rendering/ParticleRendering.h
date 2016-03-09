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

#ifndef __DC_Scene_ParticleRendering_H__
#define __DC_Scene_ParticleRendering_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Lowlevel interface for particle rendering.
	class ParticleRenderingInterface : public Fx::IRenderingInterface {
	public:

									//! Constructs the ParticleRenderingInterface instance.
									ParticleRenderingInterface( Renderer::Renderer2DPtr renderer );

		//! Renders an array of 3D points.
		virtual void				renderPoints( const Vec3* position, const Rgba* color, const f32* size, s32 count, s32 stride ) DC_DECL_OVERRIDE;

		//! Renders an array of 2D points.
		virtual void				renderPoints( const Vec2* position, const Rgba* color, const f32* size, s32 count, s32 stride ) DC_DECL_OVERRIDE;

		//! Renders the oriented 2D quad.
		virtual void				renderOrientedQuadUV( const Fx::IMaterialWPtr& material, f32 x, f32 y, f32 width, f32 height, const Vec2& up, const Vec2& side, const Rgba& color ) DC_DECL_OVERRIDE;

		//! Renders the 2D line segment.
		virtual void				renderLine( f32 x1, f32 y1, f32 x2, f32 y2, const Rgba& color1, const Rgba& color2 ) DC_DECL_OVERRIDE;

		//! Renders the 2D thick line.
		virtual void				renderThickLine( const Fx::IMaterialWPtr& material, f32 x1, f32 y1, f32 x2, f32 y2, f32 size1, f32 size2, const Rgba& color1, const Rgba& color2 ) DC_DECL_OVERRIDE;

		//! Renders the 2D line strip.
		virtual void				renderLineStrip( const Vec2* position, const Rgb* color, s32 count, s32 stride, f32 alpha = 1.0f ) DC_DECL_OVERRIDE;

		//! Renders the 2D thick line strip.
		virtual void				renderThickLineStrip( const Fx::IMaterialWPtr& material, const Vec2* positions, const Rgb* colors, const f32* sizes, s32 count, s32 stride, f32 alpha = 1.0f ) DC_DECL_OVERRIDE;

		//! Flushes the accumulated particles.
		virtual void				flush( void ) DC_DECL_OVERRIDE;

	private:

		Renderer::Renderer2DPtr		m_renderer;	//!< 2D rendering interface.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif	/*	!__DC_Scene_ParticleRendering_H__	*/