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

#include "Renderers.h"

#include "Particles.h"
#include "ParticleSystem.h"
#include "Emitter.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

// -------------------------------------- ParticleRenderer -------------------------------------- //

// ** ParticleRenderer::ParticleRenderer
ParticleRenderer::ParticleRenderer( const IRenderingInterfacePtr& renderingInterface ) : m_renderingInterface( renderingInterface )
{

}

// ** ParticleRenderer::create
ParticleRendererPtr ParticleRenderer::create( RenderingMode type, const IRenderingInterfacePtr& renderingInterface )
{
    switch( type ) {
    case RenderPoints:      return ParticleRendererPtr( DC_NEW PointRenderer( renderingInterface ) );
    case RenderQuads:       return ParticleRendererPtr( DC_NEW QuadRenderer( renderingInterface ) );
    case RenderLines:       return ParticleRendererPtr( DC_NEW LineRenderer( renderingInterface ) );
    case RenderThickLines:  return ParticleRendererPtr( DC_NEW ThickLineRenderer( renderingInterface ) );
    case RenderPaths:       return ParticleRendererPtr( DC_NEW PathRenderer( renderingInterface ) );
    case RenderThickPaths:  return ParticleRendererPtr( DC_NEW ThickPathRenderer( renderingInterface ) );
    default:				break;
    }

	DC_BREAK_IF( true, "unhandled rendering mode" );
    return ParticleRendererPtr();
}

// ------------------------------------ PointRenderer ------------------------------------ //

// ** PointRenderer::PointRenderer
PointRenderer::PointRenderer( const IRenderingInterfacePtr& renderingInterface ) : ParticleRenderer( renderingInterface )
{

}

// ** PointRenderer::render
void PointRenderer::render( const IMaterialWPtr& material, const Particle *particles, s32 count )
{
	DC_NOT_IMPLEMENTED
//	m_renderingInterface->renderPoints( &particles->m_position, &particles->m_color.current, &particles->m_size.current, count, sizeof( Particle ) );
//	m_renderingInterface->flush();
}

// ------------------------------------ QuadRenderer ------------------------------------ //

// ** QuadRenderer::QuadRenderer
QuadRenderer::QuadRenderer( const IRenderingInterfacePtr& renderingInterface ) : ParticleRenderer( renderingInterface )
{

}

// ** QuadRenderer::render
void QuadRenderer::render( const IMaterialWPtr& material, const Particle *particles, s32 count )
{
    Vec2 up, side;
	const Vec3*				position	 = particles->position;
	const Particle::Scalar* transparency = particles->transparency;
	const f32*				rotation	 = particles->rotation;
	const Particle::Scalar* size		 = particles->size;
	const Particle::Color*	color		 = particles->color;

    for( int i = 0; i < count; i++ ) {
        Vec2		    pos = Vec2( position[i].x, position[i].y );
        Rgba			clr = Rgba( color[i].current.r, color[i].current.g, color[i].current.b, transparency[i].current );

        f32 c = sinf( radians( rotation[i] ) );
        f32 s = cosf( radians( rotation[i] ) );

        up.x   = c; up.y   = -s;
        side.x = s; side.y =  c;

        m_renderingInterface->renderOrientedQuadUV( material, pos.x, pos.y, size[i].current * 0.5f, size[i].current * 0.5f, up, side, clr );
    }

    m_renderingInterface->flush();

#if 0
    Vec2 up, side;

    for( int i = 0; i < count; i++ ) {
        const Particle& p        = particles[i];
        Vec2		    position = Vec2( p.m_position.x, p.m_position.y );
        Rgba			color    = Rgba( p.m_color.current.rgb.r, p.m_color.current.rgb.g, p.m_color.current.rgb.b, p.m_color.current.alpha );

        f32 c = sinf( radians( p.m_rotation ) );
        f32 s = cosf( radians( p.m_rotation ) );

        up.x   = c; up.y   = -s;
        side.x = s; side.y =  c;

        m_renderingInterface->renderOrientedQuadUV( material, position.x, position.y, p.m_size.current * 0.5f, p.m_size.current * 0.5f, up, side, color );
    }

    m_renderingInterface->flush();
#endif
}

// ------------------------------------ LineRenderer ------------------------------------ //

// ** LineRenderer::LineRenderer
LineRenderer::LineRenderer( const IRenderingInterfacePtr& renderingInterface ) : ParticleRenderer( renderingInterface )
{

}

// ** LineRenderer::render
void LineRenderer::render( const IMaterialWPtr& material, const Particle *particles, s32 count )
{
#if 0
    for( s32 i = 0; i < count; i++ ) {
        const Particle& p = particles[i];

        Rgba color = Rgba( p.m_color.current.rgb.r, p.m_color.current.rgb.g, p.m_color.current.rgb.b, p.m_color.current.alpha );
        Vec2 end   = Vec2( p.m_position.x, p.m_position.y );
        Vec2 start = end + Vec2::fromAngle( p.m_direction ) /** Vec2( p.m_velocity.x, p.m_velocity.y ) + Vec2( p.m_force.velocity.x, p.m_force.velocity.y )*/;

        m_renderingInterface->renderLine( start.x, start.y, end.x, end.y, color, Rgba( color.r, color.g, color.b, 0 ) );
    }

    m_renderingInterface->flush();
#else
	DC_NOT_IMPLEMENTED
#endif
}

// ---------------------------------- ThickLineRenderer ------------------------------------ //

// ** ThickLineRenderer::ThickLineRenderer
ThickLineRenderer::ThickLineRenderer( const IRenderingInterfacePtr& renderingInterface ) : ParticleRenderer( renderingInterface )
{

}

// ** ThickLineRenderer::render
void ThickLineRenderer::render( const IMaterialWPtr& material, const Particle *particles, s32 count )
{
#if 0
    for( s32 i = 0; i < count; i++ ) {
        const Particle& p = particles[i];

        Rgba color = Rgba( p.m_color.current.rgb.r, p.m_color.current.rgb.g, p.m_color.current.rgb.b, p.m_color.current.alpha );
        Vec2 end   = Vec2( p.m_position.x, p.m_position.y );
        Vec2 start = end + Vec2::fromAngle( p.m_direction ) /** Vec2( p.m_velocity.x, p.m_velocity.y ) + Vec2( p.m_force.velocity.x, p.m_force.velocity.y )*/;

        m_renderingInterface->renderThickLine( material, start.x, start.y, end.x, end.y, p.m_size.current, p.m_size.current, color, Rgba( color.r, color.g, color.b, 0 ) );
    }
    
    m_renderingInterface->flush();
#else
	DC_NOT_IMPLEMENTED
#endif
}

// ------------------------------------ PathRenderer ------------------------------------ //

// ** PathRenderer::PathRenderer
PathRenderer::PathRenderer( const IRenderingInterfacePtr& renderingInterface ) : ParticleRenderer( renderingInterface )
{

}

// ** PathRenderer::render
void PathRenderer::render( const IMaterialWPtr& material, const Particle *particles, s32 count )
{
	DC_NOT_IMPLEMENTED
//    for( s32 i = 0; i < count; i++ ) {
//        m_renderingInterface->renderLineStrip( &particles[i].m_snapshots->pos, &particles[i].m_snapshots->color, Particle::MaxSnapshots, sizeof( particles[i].m_snapshots[0] ), particles[i].m_color.current.alpha );
//    }
//    m_renderingInterface->flush();
}

// ---------------------------------- ThickPathRenderer ------------------------------------ //

ThickPathRenderer::ThickPathRenderer( const IRenderingInterfacePtr& renderingInterface ) : ParticleRenderer( renderingInterface )
{

}

// ** ThickPathRenderer::render
void ThickPathRenderer::render( const IMaterialWPtr& material, const Particle *particles, s32 count )
{
	DC_NOT_IMPLEMENTED
//    for( s32 i = 0; i < count; i++ ) {
//        m_renderingInterface->renderThickLineStrip( texture, &particles[i].m_snapshots->pos, &particles[i].m_snapshots->color, &particles[i].m_snapshots->size,  Particle::MaxSnapshots, sizeof( particles[i].m_snapshots[0] ),particles[i].m_color.current.alpha );
//    }
//    m_renderingInterface->flush();
}

// ---------------------------------------------------- BuiltInRenderingInterface ---------------------------------------------------- //

#if DEV_DEPRECATED_SCENE_RENDERER
// ** BuiltInRenderingInterface::BuiltInRenderingInterface
BuiltInRenderingInterface::BuiltInRenderingInterface( Renderer::Renderer2DPtr renderer ) : m_renderer( renderer )
{

}

// ** uiltInRenderingInterface::renderPoints
void BuiltInRenderingInterface::renderPoints( const Vec3* position, const Rgba* color, const f32* size, s32 count, s32 stride )
{
	DC_NOT_IMPLEMENTED;
}

// ** uiltInRenderingInterface::renderPoints
void BuiltInRenderingInterface::renderPoints( const Vec2* position, const Rgba* color, const f32* size, s32 count, s32 stride )
{
	DC_NOT_IMPLEMENTED;
//	m_renderer->points( position, color, size, count, stride );
}

// ** uiltInRenderingInterface::renderPoints
void BuiltInRenderingInterface::renderOrientedQuadUV( const IMaterialWPtr& material, f32 x, f32 y, f32 width, f32 height, const Vec2& up, const Vec2& side, const Rgba& color )
{
	DC_NOT_IMPLEMENTED;
//	m_renderer->orientedQuad( Renderer::Texture2DWPtr(), x, y, width, height, up, side, color );
}

// ** uiltInRenderingInterface::renderPoints
void BuiltInRenderingInterface::renderLine( f32 x1, f32 y1, f32 x2, f32 y2, const Rgba& color1, const Rgba& color2 )
{
	DC_NOT_IMPLEMENTED;
//	m_renderer->line( x1, y1, x2, y2, color1, color2 );
}

// ** uiltInRenderingInterface::renderPoints
void BuiltInRenderingInterface::renderThickLine( const IMaterialWPtr& material, f32 x1, f32 y1, f32 x2, f32 y2, f32 size1, f32 size2, const Rgba& color1, const Rgba& color2 )
{
	DC_NOT_IMPLEMENTED;
}

// ** uiltInRenderingInterface::renderPoints
void BuiltInRenderingInterface::renderLineStrip( const Vec2* position, const Rgb* color, s32 count, s32 stride, f32 alpha )
{
	DC_NOT_IMPLEMENTED;
}

// ** uiltInRenderingInterface::renderPoints
void BuiltInRenderingInterface::renderThickLineStrip( const IMaterialWPtr& material, const Vec2* positions, const Rgb* colors, const f32* sizes, s32 count, s32 stride, f32 alpha )
{
	DC_NOT_IMPLEMENTED;
}

// ** uiltInRenderingInterface::renderPoints
void BuiltInRenderingInterface::flush( void )
{
	DC_NOT_IMPLEMENTED;
//	m_renderer->flush();
}
#endif  /*  DEV_DEPRECATED_SCENE_RENDERER   */

} // namespace Fx

DC_END_DREEMCHEST
