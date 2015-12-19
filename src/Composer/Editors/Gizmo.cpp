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

#include "Gizmo.h"

DC_BEGIN_COMPOSER

// ---------------------------------------------------------- TranslationGizmo ---------------------------------------------------------- //

// ** TranslationGizmo::TranslationGizmo
TranslationGizmo::TranslationGizmo( f32 orthRadius, f32 scalingFactor )
	: m_scalingFactor( scalingFactor ), m_orthRadius( orthRadius )
{
}

// ** TranslationGizmo::scalingFactor
f32 TranslationGizmo::scalingFactor( void ) const
{
	return m_scalingFactor;
}

// ** TranslationGizmo::orthRadius
f32 TranslationGizmo::orthRadius( void ) const
{
	return m_orthRadius;
}

// -------------------------------------------------------- TranslationGizmoPass -------------------------------------------------------- //

// ** TranslationGizmoPass::render
void TranslationGizmoPass::render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const TranslationGizmo& gizmo, const Scene::Transform& transform )
{
	Renderer::Renderer2DPtr renderer = context->renderer();

	// Get the center point of a gizmo.
	Vec3 pos = transform.worldSpacePosition();

	// Calculate the scaling based on distance to camera.
	f32 scale = (m_transform->position() - pos).length() * gizmo.scalingFactor();

	// Plane selectors scale factor.
	f32 selectorScale = 0.35f;

	// Get the transform axes.
	Vec3 ax = transform.axisX() * scale;
	Vec3 ay = transform.axisY() * scale;
	Vec3 az = transform.axisZ() * scale;

	// Construct colors.
	Rgba red = Rgba::fromBytes( 240, 68, 31 );
	Rgba green = Rgba::fromBytes( 187, 237, 82 );
	Rgba blue = Rgba::fromBytes( 61, 133, 236 );

	// Get the selector planes vertices
	Vec3 xz[4] = { pos, pos + ax * selectorScale, pos + ax * selectorScale + az * selectorScale, pos + az * selectorScale };
	Vec3 yz[4] = { pos, pos + ay * selectorScale, pos + ay * selectorScale + az * selectorScale, pos + az * selectorScale };
	Vec3 xy[4] = { pos, pos + ax * selectorScale, pos + ax * selectorScale + ay * selectorScale, pos + ay * selectorScale };

	// Render XZ selector.
	renderer->quad( xz[0], xz[1], xz[2], xz[3], green.transparent( 0.1f ) );
	renderer->line( xz[1], xz[2], green );
	renderer->line( xz[3], xz[2], green );

	// Render YZ selector.
	renderer->quad( yz[0], yz[1], yz[2], yz[3], red.transparent( 0.1f ) );
	renderer->line( yz[1], yz[2], red );
	renderer->line( yz[3], yz[2], red );

	// Render XY selector.
	renderer->quad( xy[0], xy[1], xy[2], xy[3], blue.transparent( 0.1f ) );
	renderer->line( xy[1], xy[2], blue );
	renderer->line( xy[3], xy[2], blue );

	// Render gizmo orths
	renderer->line( pos, pos + ax, red );
	renderer->line( pos, pos + ay, green );
	renderer->line( pos, pos + az, blue );

	// Render cones
	renderer->wireCone( pos + ax - ax * 0.3f, pos + ax, ay, az, gizmo.orthRadius(), 6, red );
	renderer->wireCone( pos + ay - ay * 0.3f, pos + ay, ax, az, gizmo.orthRadius(), 6, green );
	renderer->wireCone( pos + az - az * 0.3f, pos + az, ax, ay, gizmo.orthRadius(), 6, blue );
}

DC_END_COMPOSER