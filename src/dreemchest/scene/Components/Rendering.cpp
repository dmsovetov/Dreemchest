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

#include "Rendering.h"

#include "../Assets/Mesh.h"
#include "../Assets/Material.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// ------------------------------------------------ Light ------------------------------------------------- //

// ** Light::type
Light::Type Light::type( void ) const
{
	return m_type;
}

// ** Light::setType
void Light::setType( Type value )
{
	m_type = value;
}

// ** Light::color
const Rgb& Light::color( void ) const
{
	return m_color;
}

// ** Light::setColor
void Light::setColor( const Rgb& value )
{
	m_color = value;
}

// ** Light::intensity
f32 Light::intensity( void ) const
{
	return m_intensity;
}

// ** Light::setIntensity
void Light::setIntensity( f32 value )
{
	m_intensity = value;
}

// ** Light::range
f32 Light::range( void ) const
{
	return m_range;
}

// ** Light::setRange
void Light::setRange( f32 value )
{
	m_range = value;
}

// ---------------------------------------------- StaticMesh ---------------------------------------------- //

// ** StaticMesh::mesh
const MeshPtr& StaticMesh::mesh( void ) const
{
	return m_mesh;
}

// ** StaticMesh::setMesh
void StaticMesh::setMesh( const MeshPtr& value )
{
	DC_BREAK_IF( !value.valid() );
	m_mesh = value;
}

// ** StaticMesh::bounds
const Bounds& StaticMesh::bounds( void ) const
{
	static Bounds empty;
	return m_mesh.valid() ? m_mesh->bounds() : empty;
}

// ** StaticMesh::const 
bool StaticMesh::isVisible( u8 camera ) const
{
	DC_BREAK_IF( camera >= 16 );
	return m_visibility.is( BIT( camera ) );
}

// ** StaticMesh::setVisibilityMask
void StaticMesh::setVisibilityMask( u16 mask, bool value )
{
	m_visibility.set( mask, value );
}

// ** StaticMesh::materialCount
u32 StaticMesh::materialCount( void ) const
{
	return ( u32 )m_materials.size();
}

// ** StaticMesh::material
MaterialPtr StaticMesh::material( u32 index ) const
{
	return index < materialCount() ? m_materials[index] : MaterialPtr();
}

// ** StaticMesh::setMaterial
void StaticMesh::setMaterial( u32 index, const MaterialPtr& value )
{
	DC_BREAK_IF( index > 8 );

	if( index >= materialCount() ) {
		m_materials.resize( index + 1 );
	}

	m_materials[index] = value;
}

// ** StaticMesh::lightmap
const Renderer::TexturePtr& StaticMesh::lightmap( void ) const
{
	return m_lightmap;
}

// ** StaticMesh::setLightmap
void StaticMesh::setLightmap( const Renderer::TexturePtr& value )
{
	m_lightmap = value;
}

// ---------------------------------------------- Sprite ---------------------------------------------- //

// ** Sprite::image
const ImagePtr& Sprite::image( void ) const
{
	return m_image;
}

// ** Sprite::color
const Rgba& Sprite::color( void ) const
{
	return m_color;
}

// --------------------------------------------- View --------------------------------------------- //

// ** View::calculateSplitRect
Rect View::calculateSplitRect( u32 x, u32 y, u32 nx, u32 ny )
{
	// Calculate the viewport dimensions in NDC
	f32 width  = 1.0f / nx;
	f32 height = 1.0f / ny;

	// Calculate the NDC of a viewport
	Rect ndc = Rect( x * width, y * height, (x + 1) * width, (y + 1) * height );

	return ndc;
}

// ------------------------------------------ WindowView ------------------------------------------ //

// ** WindowView::WindowView
WindowView::WindowView( const Platform::WindowWPtr& window ) : m_window( window )
{
}

// ** WindowView::width
u32 WindowView::width( void ) const
{
	return m_window->width();
}

// ** WindowView::height
u32 WindowView::height( void ) const
{
	return m_window->height();
}

// ** WindowView::create
ViewPtr WindowView::create( const Platform::WindowWPtr& window )
{
	return ViewPtr( DC_NEW WindowView( window ) );
}

// -------------------------------------------- Camera -------------------------------------------- //

// ** Camera::clearMask
u8 Camera::clearMask( void ) const
{
	return m_clearMask;
}

// ** Camera::setClearMask
void Camera::setClearMask( u8 value )
{
	m_clearMask = value;
}

// ** Camera::id
u8 Camera::id( void ) const
{
	return m_id;
}

// ** Camera::setId
void Camera::setId( u8 value )
{
	m_id = value;
}

// ** Camera:setFar
void Camera::setFar( f32 value )
{
	m_far = value;
}

// ** Camera:setNear
void Camera::setNear( f32 value )
{
	m_near = value;
}

// ** Camera:setFov
void Camera::setFov( f32 value )
{
	m_fov = value;
}

// ** Camera::clearColor
const Rgba& Camera::clearColor( void ) const
{
	return m_clearColor;
}

// ** Camera::setClearColor
void Camera::setClearColor( const Rgba& value )
{
	m_clearColor = value;
}

// ** Camera::ndc
const Rect& Camera::ndc( void ) const
{
	return m_ndc;
}

// ** Camera::setNdc
void Camera::setNdc( const Rect& value )
{
	m_ndc = value;
}

// ** Camera::viewport
Rect Camera::viewport( void ) const
{
	DC_BREAK_IF( m_view == NULL )

	u32 w = m_view->width();
	u32 h = m_view->height();

	return Rect( w * m_ndc.min().x, h * m_ndc.min().y, w * m_ndc.max().x, h * m_ndc.max().y );
}

// ** Camera::setView
void Camera::setView( const ViewPtr& value )
{
	m_view = value;
}

// ** Camera::view
const ViewPtr& Camera::view( void ) const
{
	return m_view;
}

// ** Camera::calculateProjectionMatrix
Matrix4 Camera::calculateProjectionMatrix( void ) const
{
	DC_BREAK_IF( m_view == NULL )

	Rect rect   = viewport();
	f32  width  = rect.width();
	f32  height = rect.height();

	switch( m_projection ) {
	case Perspective:	return Matrix4::perspective( m_fov, width / height, m_near, m_far );
	case Ortho:			return Matrix4::ortho( 0, width, 0, height, -10000, 10000 );
	case OrthoCenter:	return Matrix4::ortho( -width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, -10000, 10000 );
	default:			DC_BREAK;
	}

	return Matrix4();
}

// ** Camera::calculateClipSpace
Matrix4 Camera::calculateClipSpace( const Matrix4& transform ) const
{
	return calculateProjectionMatrix() * transform.inversed() /*Matrix4::lookAt( Vec3( 5, 5, 5 ), Vec3( 0, 0, 0 ), Vec3( 0, 1, 0 ) )*/;
}

// ** Camera::calculateFrustum
void Camera::calculateFrustum( const Matrix4& clip, Plane* planes )
{
	const f32 *m = clip.m;

	planes[0] = Plane( m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12] );
	planes[1] = Plane( m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12] );

	planes[2] = Plane( m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13] );
	planes[3] = Plane( m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13] );

	planes[4] = Plane( m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14] );
	planes[5] = Plane( m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14] );

	for( s32 i = 0; i < 6; i++ ) {
		planes[i].normalize();
	}
}

} // namespace Scene

DC_END_DREEMCHEST