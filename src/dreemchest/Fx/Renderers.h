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

#ifndef __DC_Fx_Renderers_H__
#define __DC_Fx_Renderers_H__

#include "Emitter.h"

#include "../Renderer/Hal.h"
#include "../Renderer/Renderer2D.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

    //! Particle vertex struct used for rendering.
    struct ParticleVertex {
        Vec3        pos;		//!< Vertex position.
        Rgba        color;		//!< Vertex color.
    };

	//! Rendering material interface.
	class IMaterial : public RefCounted {
	public:

									//! Constructs IMaterial instance.
									IMaterial( RefCountedPtr data = RefCountedPtr() )
										: m_data( data ) {}
		virtual						~IMaterial( void ) {}

		//! Returns the raw material pointer.
		virtual RefCountedPtr		get( void ) const { return m_data; }

	private:

		RefCountedPtr				m_data;	//!< Implementation reference.
	};

	//! Material factory interface.
	class IMaterialFactory : public RefCounted {
	public:

		virtual						~IMaterialFactory( void ) {}

		//! Creates a new material instance.
		virtual IMaterialPtr		createMaterial( const String& identifier ) = 0;
	};

	//! Hardware rendering interface.
	class IRenderingInterface : public RefCounted {
	public:

		virtual						~IRenderingInterface( void ) {}

		//! Renders an array of 3d points.
		virtual void				renderPoints( const Vec3* position, const Rgba* color, const f32* size, s32 count, s32 stride ) = 0;

		//! Renders an array of 2d points.
		virtual void				renderPoints( const Vec2* position, const Rgba* color, const f32* size, s32 count, s32 stride ) = 0;

		//! Renders the oriented 2d quad.
		virtual void				renderOrientedQuadUV( const IMaterialWPtr& material, f32 x, f32 y, f32 width, f32 height, const Vec2& up, const Vec2& side, const Rgba& color ) = 0;

		//! Renders the 2d line segment.
		virtual void				renderLine( f32 x1, f32 y1, f32 x2, f32 y2, const Rgba& color1, const Rgba& color2 ) = 0;

		//! Renders the 2d thick line.
		virtual void				renderThickLine( const IMaterialWPtr& material, f32 x1, f32 y1, f32 x2, f32 y2, f32 size1, f32 size2, const Rgba& color1, const Rgba& color2 ) = 0;

		//! Renders the 2d line strip.
		virtual void				renderLineStrip( const Vec2* position, const Rgb* color, s32 count, s32 stride, f32 alpha = 1.0f ) = 0;

		//! Renders the 2d thick line strip.
		virtual void				renderThickLineStrip( const IMaterialWPtr& material, const Vec2* positions, const Rgb* colors, const f32* sizes, s32 count, s32 stride, f32 alpha = 1.0f ) = 0;

		//! Flushes the accumulated particles.
		virtual void				flush( void ) = 0;
	};

	//! The built-in rendering interface.
	class BuiltInRenderingInterface : public IRenderingInterface {
	public:

									//! Constructs the BuiltInRenderingInterface instance.
									BuiltInRenderingInterface( Renderer::Renderer2DPtr renderer );

		//! Renders an array of 3d points.
		virtual void				renderPoints( const Vec3* position, const Rgba* color, const f32* size, s32 count, s32 stride );

		//! Renders an array of 2d points.
		virtual void				renderPoints( const Vec2* position, const Rgba* color, const f32* size, s32 count, s32 stride );

		//! Renders the oriented 2d quad.
		virtual void				renderOrientedQuadUV( const IMaterialWPtr& material, f32 x, f32 y, f32 width, f32 height, const Vec2& up, const Vec2& side, const Rgba& color );

		//! Renders the 2d line segment.
		virtual void				renderLine( f32 x1, f32 y1, f32 x2, f32 y2, const Rgba& color1, const Rgba& color2 );

		//! Renders the 2d thick line.
		virtual void				renderThickLine( const IMaterialWPtr& material, f32 x1, f32 y1, f32 x2, f32 y2, f32 size1, f32 size2, const Rgba& color1, const Rgba& color2 );

		//! Renders the 2d line strip.
		virtual void				renderLineStrip( const Vec2* position, const Rgb* color, s32 count, s32 stride, f32 alpha = 1.0f );

		//! Renders the 2d thick line strip.
		virtual void				renderThickLineStrip( const IMaterialWPtr& material, const Vec2* positions, const Rgb* colors, const f32* sizes, s32 count, s32 stride, f32 alpha = 1.0f );

		//! Flushes the accumulated particles.
		virtual void				flush( void );

	private:

		Renderer::Renderer2DPtr		m_renderer;	//!< 2d rendering interface.
	};

    //! Base class for all particle renderers.
    class ParticleRenderer : public RefCounted {
    public:

		virtual                     ~ParticleRenderer( void ) {}

		//! Returns the particle renderer type.
        virtual RenderingMode		type( void ) const = 0;

		//! Renders the particles with a specified blending mode & texture.
        virtual void                render( const IMaterialWPtr& material, BlendingMode blendMode, const Particle *particles, s32 count ) = 0;

		//! Creates the particle renderer instance by type.
        static ParticleRendererPtr	create( RenderingMode type, const IRenderingInterfacePtr& renderingInterface );

	protected:

									//! Constructs ParticleRenderer instance.
									ParticleRenderer( const IRenderingInterfacePtr& renderingInterface );

	protected:

		IRenderingInterfacePtr		m_renderingInterface;
    };

    // ** class PointRenderer
    class PointRenderer : public ParticleRenderer {
    public:
        
                                    PointRenderer( const IRenderingInterfacePtr& renderingInterface );

        // ** Renderer
        virtual RenderingMode		type( void ) const { return RenderPoints; }
        virtual void                render( const IMaterialWPtr& material, BlendingMode blendMode, const Particle *particles, int count );
    };

    // ** class QuadRenderer
    class QuadRenderer : public ParticleRenderer {
    public:

                                    QuadRenderer( const IRenderingInterfacePtr& renderingInterface );

        // ** Renderer
        virtual RenderingMode		type( void ) const { return RenderQuads; }
        virtual void                render( const IMaterialWPtr& material, BlendingMode blendMode, const Particle *particles, int count );
    };

    // ** class LineRenderer
    class LineRenderer : public ParticleRenderer {
    public:

                                    LineRenderer( const IRenderingInterfacePtr& renderingInterface );

        // ** Renderer
        virtual RenderingMode		type( void ) const { return RenderLines; }
        virtual void                render( const IMaterialWPtr& material, BlendingMode blendMode, const Particle *particles, int count );
    };

    // ** class ThickLineRenderer
    class ThickLineRenderer : public ParticleRenderer {
    public:

                                    ThickLineRenderer( const IRenderingInterfacePtr& renderingInterface );

        // ** Renderer
        virtual RenderingMode		type( void ) const { return RenderThickLines; }
        virtual void                render( const IMaterialWPtr& material, BlendingMode blendMode, const Particle *particles, int count );
    };

    // ** class PathRenderer
    class PathRenderer : public ParticleRenderer {
    public:

                                    PathRenderer( const IRenderingInterfacePtr& renderingInterface );

        // ** Renderer
        virtual RenderingMode	    type( void ) const { return RenderPaths; }
        virtual void                render( const IMaterialWPtr& material, BlendingMode blendMode, const Particle *particles, int count );
    };

    // ** class ThickPathRenderer
    class ThickPathRenderer : public ParticleRenderer {
    public:

                                    ThickPathRenderer( const IRenderingInterfacePtr& renderingInterface );

        // ** Renderer
        virtual RenderingMode		type( void ) const { return RenderThickPaths; }
        virtual void                render( const IMaterialWPtr& material, BlendingMode blendMode, const Particle *particles, int count );
    };

} // namespace Fx

DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_Renderers_H__	*/