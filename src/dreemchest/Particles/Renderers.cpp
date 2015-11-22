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

#include    "Renderers.h"

#include	"Particles.h"

#include    "../renderer/BatchRenderer.h"
#include    "../renderer/Hal.h"

#include	"../asset/TextureAsset.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

// -------------------------------------- Renderer -------------------------------------- //

// ** Renderer::Renderer
Renderer::Renderer( void )
{
}

// ** Renderer::~Renderer
Renderer::~Renderer( void )
{
}

// ** Renderer::createRenderer
Renderer* Renderer::createRenderer( RendererType type )
{
    switch( type ) {
    case RendererType::Points:      return DC_NEW PointRenderer;
    case RendererType::Quads:       return DC_NEW QuadRenderer;
    case RendererType::Lines:       return DC_NEW LineRenderer;
    case RendererType::ThickLines:  return DC_NEW ThickLineRenderer;
    case RendererType::Paths:       return DC_NEW PathRenderer;
    case RendererType::ThickPaths:  return DC_NEW ThickPathRenderer;
    default:						return NULL;
    }

    return NULL;
}

// ** Renderer::render
void Renderer::render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count )
{
	switch( blendMode ) {
	case BlendMode::Normal:			renderer->setBlendMode( renderer::BlendSrcAlpha, renderer::BlendInvSrcAlpha );
									break;
	case BlendMode::Additive:		renderer->setBlendMode( renderer::BlendOne, renderer::BlendOne );
									break;
	case BlendMode::Premultiplied:	renderer->setBlendMode( renderer::BlendOne, renderer::BlendInvSrcAlpha );
									break;
	}
}

// ** PointRenderer::PointRenderer
PointRenderer::PointRenderer( void )
{
    
}

// ** PointRenderer::render
void PointRenderer::render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count )
{
	Renderer::render( renderer, texture, blendMode, particles, count );

    renderer->renderPointArray( NULL, &particles->m_position, &particles->m_color, &particles->m_size, sizeof( sParticle ), count );
    renderer->flush();
}

// ------------------------------------ QuadRenderer ------------------------------------ //

// ** QuadRenderer::QuadRenderer
QuadRenderer::QuadRenderer( void )
{

}

// ** QuadRenderer::render
void QuadRenderer::render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count )
{
	Renderer::render( renderer, texture, blendMode, particles, count );

    vec2 up, side;

	dcTexture   tex   = texture ? texture->GetTexturePointer() : NULL;
	dcTexture2D tex2d = tex ? tex->isTexture2D() : NULL;
	vec4		uv	  = texture ? texture->GetUv() : vec4( 0, 0, 0, 0 );

    // ** Triangulate particles
    for( int i = 0; i < count; i++ ) {
        const sParticle& p        = particles[i];
        const vec2&		 position = p.m_position;
        rgba			 color    = rgba( p.m_color.current.rgb.r, p.m_color.current.rgb.g, p.m_color.current.rgb.b, p.m_color.current.alpha );

        float c = cosf( Radians( p.m_rotation ) );
        float s = sinf( Radians( p.m_rotation ) );

        up.x    = c; up.y   = -s;
        side.x  = s; side.y =  c;

        renderer->renderOrientedQuadUV( tex2d, position.x, position.y, p.m_size.current * 0.5f, p.m_size.current * 0.5f, up, side, uv, color );
    }

    renderer->flush();
}

// ------------------------------------ LineRenderer ------------------------------------ //

// ** LineRenderer::LineRenderer
LineRenderer::LineRenderer( void )
{

}

// ** LineRenderer::render
void LineRenderer::render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count )
{
	Renderer::render( renderer, texture, blendMode, particles, count );

    for( int i = 0; i < count; i++ ) {
        const sParticle& p = particles[i];

        rgba        color = rgba( p.m_color.current.rgb.r, p.m_color.current.rgb.g, p.m_color.current.rgb.b, p.m_color.current.alpha );
        const vec2& end   = p.m_position;
        const vec2& start = end + vec2::FromAngle( p.m_direction ) * p.m_linear.velocity + p.m_force.velocity;

        renderer->renderLine( start.x, start.y, end.x, end.y, color, rgba( ( unsigned int )color.r, color.g, color.b, 0 ) );
    }

    renderer->flush();
}

// ---------------------------------- ThickLineRenderer ------------------------------------ //

// ** ThickLineRenderer::ThickLineRenderer
ThickLineRenderer::ThickLineRenderer( void )
{

}

// ** ThickLineRenderer::render
void ThickLineRenderer::render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count )
{
	Renderer::render( renderer, texture, blendMode, particles, count );

	dcTexture   tex   = texture ? texture->GetTexturePointer() : NULL;
	dcTexture2D tex2d = tex ? tex->isTexture2D() : NULL;

    // ** Triangulate particles
    for( int i = 0; i < count; i++ ) {
        const sParticle& p = particles[i];

        rgba        color = rgba( p.m_color.current.rgb.r, p.m_color.current.rgb.g, p.m_color.current.rgb.b, p.m_color.current.alpha );
        const vec2& end   = p.m_position;
        const vec2& start = end + vec2::FromAngle( p.m_direction ) * p.m_linear.velocity + p.m_force.velocity;

        renderer->renderThickLine( tex2d, start.x, start.y, end.x, end.y, p.m_size.current, p.m_size.current, color, rgba( ( unsigned int )color.r, color.g, color.b, 0 ) );
    }
    
    renderer->flush();
}

// ------------------------------------ PathRenderer ------------------------------------ //

// ** PathRenderer::PathRenderer
PathRenderer::PathRenderer( void )
{

}

// ** PathRenderer::render
void PathRenderer::render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count )
{
	Renderer::render( renderer, texture, blendMode, particles, count );

    for( int i = 0; i < count; i++ ) {
        renderer->renderLineStrip( &particles[i].m_snapshots->pos, &particles[i].m_snapshots->color, sizeof( particles[i].m_snapshots[0] ), sParticle::MaxSnapshots, particles[i].m_color.current.alpha );
    }
    renderer->flush();
}

// ---------------------------------- ThickPathRenderer ------------------------------------ //

// ** ThickPathRenderer::ThickPathRenderer
ThickPathRenderer::ThickPathRenderer( void )
{

}

// ** ThickPathRenderer::render
void ThickPathRenderer::render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count )
{
	Renderer::render( renderer, texture, blendMode, particles, count );

	dcTexture   tex   = texture ? texture->GetTexturePointer() : NULL;
	dcTexture2D tex2d = tex ? tex->isTexture2D() : NULL;

    for( int i = 0; i < count; i++ ) {
        renderer->renderThickLineStrip( tex2d, &particles[i].m_snapshots->pos, &particles[i].m_snapshots->color, &particles[i].m_snapshots->size, sizeof( particles[i].m_snapshots[0] ), sParticle::MaxSnapshots, particles[i].m_color.current.alpha );
    }
    renderer->flush();
}


} // namespace particles

} // namespace dreemchest