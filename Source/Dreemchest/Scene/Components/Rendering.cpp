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
#include "Transform.h"

#include "../Assets/Mesh.h"
#include "../Assets/Material.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

// -------------------------------------------------------------- SpriteRenderer -------------------------------------------------------------- //

// ** SpriteRenderer::SpriteRenderer
SpriteRenderer::SpriteRenderer( f32 scaleFactor )
	: m_scaleFactor( scaleFactor )
{
}

// ** SpriteRenderer::setScaleFactor
f32 SpriteRenderer::scaleFactor( void ) const
{
	return m_scaleFactor;
}

// ** SpriteRenderer::setScaleFactor
void SpriteRenderer::setScaleFactor( f32 value )
{
	m_scaleFactor = value;
}

// ------------------------------------------------ Light ------------------------------------------------- //

// ** Light::Light
Light::Light( LightType type, const Rgb& color, f32 intensity, f32 range )
	: m_type( type )
    , m_color( color )
    , m_intensity( intensity )
    , m_range( range )
    , m_cutoff( 45.0f )
    , m_castsShadow( false )
{
}

// ** Light::type
LightType Light::type( void ) const
{
	return m_type;
}

// ** Light::setType
void Light::setType( LightType value )
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

// ** Light::cutoff
f32 Light::cutoff( void ) const
{
	return m_cutoff;
}

// ** Light::setCutoff
void Light::setCutoff( f32 value )
{
	m_cutoff = value;
}

// ** Light::castsShadows
bool Light::castsShadows( void ) const
{
    return m_castsShadow;
}

// ** Light::setCastsShadows
void Light::setCastsShadows( bool value )
{
    m_castsShadow = value;
}

// ---------------------------------------------- StaticMesh ---------------------------------------------- //

// ** StaticMesh::mesh
const MeshHandle& StaticMesh::mesh( void ) const
{
	return m_mesh;
}

// ** StaticMesh::setMesh
void StaticMesh::setMesh( const MeshHandle& value )
{
	DC_ABORT_IF( !value.isValid(), "invalid mesh" );
	m_mesh = value;
}

// ** StaticMesh::worldSpaceBounds
const Bounds& StaticMesh::worldSpaceBounds( void ) const
{
	return m_worldSpaceBounds;
}

// ** StaticMesh::setWorldSpaceBounds
void StaticMesh::setWorldSpaceBounds( const Bounds& value )
{
	m_worldSpaceBounds = value;
}

// ** StaticMesh::setMaterial
void StaticMesh::setMaterial( u32 index, MaterialHandle value )
{
	DC_ABORT_IF( index > 8, "index is out of range" );

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

// ------------------------------------------- PointCloud ----------------------------------------- //

// ** PointCloud::PointCloud
PointCloud::PointCloud( s32 vertexCount, const VertexFormat& format )
    : m_format( format )
    , m_vertices( NULL )
    , m_vertexCount( vertexCount )
{
    resize( vertexCount );
}

// ** PointCloud::~PointCloud
PointCloud::~PointCloud( void )
{
    delete[]m_vertices;
}

// ** PointCloud::resize
void PointCloud::resize( s32 vertexCount )
{
    clear();
    m_vertexCount = vertexCount;
    m_vertices    = DC_NEW u8[vertexCount * m_format.vertexSize()];
}

// ** PointCloud::clear
void PointCloud::clear( void )
{
    delete[]m_vertices;
    m_vertices = NULL;
    m_vertexCount = 0;
}

// ** PointCloud::material
const MaterialHandle& PointCloud::material( void ) const
{
    return m_material;
}

// ** PointCloud::setMaterial
void PointCloud::setMaterial( const MaterialHandle& value )
{
    m_material = value;
}

// ** PointCloud::vertexFormat
const VertexFormat& PointCloud::vertexFormat( void ) const
{
    return m_format;
}

// ** PointCloud::setVertexFormat
void PointCloud::setVertexFormat( const VertexFormat& value )
{
    m_format = value;
}

// ** PointCloud::vertexCount
s32 PointCloud::vertexCount( void ) const
{
    return m_vertexCount;
}

// ** PointCloud::vertices
const void* PointCloud::vertices( void ) const
{
    return m_vertices;
}

// ** PointCloud::vertices
void* PointCloud::vertices( void )
{
    return m_vertices;
}

// ------------------------------------------- Particles ----------------------------------------- //

// ** Particles::Particles
Particles::Particles( const Fx::ParticleSystemPtr& particleSystem, const Fx::ParticleSystemInstancePtr& instance ) : m_particleSystem( particleSystem ), m_instance( instance )
{

}

// ** Particles::instance
Fx::ParticleSystemInstanceWPtr Particles::instance( void ) const
{
	return m_instance;
}

// ** Particles::particles
Fx::ParticleSystemWPtr Particles::particles( void ) const
{
	return m_particleSystem;
}

// ** Particles::material
MaterialHandle Particles::material( void ) const
{
	return m_material;
}

// ** Particles::setMaterial
void Particles::setMaterial( MaterialHandle value )
{
	m_material = value;
}

// ---------------------------------------------- Sprite ---------------------------------------------- //

// **  Sprite::width
s32 Sprite::width( void ) const
{
	return m_width;
}

// **  Sprite::height
s32 Sprite::height( void ) const
{
	return m_height;
}

// ** Sprite::material
MaterialHandle Sprite::material( void ) const
{
	return m_material;
}

// ** Sprite::color
const Rgba& Sprite::color( void ) const
{
	return m_color;
}

// -------------------------------------------- Camera -------------------------------------------- //

// ** Camera::Camera
Camera::Camera( Projection projection, const Rgba& clearColor, const Rect& ndc )
	: m_clearMask( ClearAll )
    , m_projection( projection )
    , m_ndc( ndc )
    , m_clearColor( clearColor )
    , m_fov( 60.0f )
    , m_near( 0.01f )
    , m_far( 1000.0f )
{
}

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

// ** Camera::near
f32 Camera::near( void ) const
{
    return m_near;
}

// ** Camera:setNear
void Camera::setNear( f32 value )
{
	m_near = value;
}

// ** Camera::far
f32 Camera::far( void ) const
{
    return m_far;
}

// ** Camera:setFar
void Camera::setFar( f32 value )
{
	m_far = value;
}

// ** Camera::fov
f32 Camera::fov( void ) const
{
    return m_fov;
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

// ** Camera::projection
Projection Camera::projection( void ) const
{
    return m_projection;
}

// ** Camera::setProjection
void Camera::setProjection( Projection value )
{
    m_projection = value;
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

// ** Camera::calculateProjectionMatrix
Matrix4 Camera::calculateProjectionMatrix( const Camera& camera, const Viewport& viewport )
{
	Rect rect   = viewport.denormalize( camera.ndc() );
	f32  width  = rect.width();
	f32  height = rect.height();

	switch( camera.projection() ) {
    case Projection::Perspective:	return Matrix4::perspective( camera.fov(), width / height, camera.near(), camera.far() );
	case Projection::Ortho:			return Matrix4::ortho( 0, width, 0, height, -10000, 10000 );
	case Projection::OrthoCenter:	return Matrix4::ortho( -width * 0.5f, width * 0.5f, height * 0.5f, -height * 0.5f, -10000, 10000 );
	default:			            DC_NOT_IMPLEMENTED;
	}

	return Matrix4();
}

// ** Camera::calculateViewProjection
Matrix4 Camera::calculateViewProjection( const Camera& camera, const Viewport& viewport, const Matrix4& transform )
{
	return calculateProjectionMatrix( camera, viewport ) * transform.inversed();
}

#if 0
// ** Camera::toWorldSpace
bool Camera::toWorldSpace( const Vec3& screen, Vec3& world, const Matrix4& transform ) const
{
	// Get the camera viewport rect.
	Rect viewport = this->viewport();

	// Convert to normalized device coordinates.
	f32 nx = (screen.x - viewport.left()  ) / viewport.width()  * 2.0f - 1.0f;
	f32 ny = (screen.y - viewport.bottom()) / viewport.height() * 2.0f - 1.0f;
	f32 nz =  screen.z * 2.0f - 1.0f;

	// Calculate the inversed view projection matrix.
	Matrix4 proj = calculateProjectionMatrix();
	Matrix4 vp   = transform * proj.inversed();

	// Transform the NDC point to a world space
	Vec4 worldSpace = vp * Vec4( nx, ny, nz );

	if( worldSpace.w == 0.0f ) {
		return false;
	}

	worldSpace.w = 1.0f / worldSpace.w;

	// Calculate resulting value
	world.x = worldSpace.x * worldSpace.w;
	world.y = worldSpace.y * worldSpace.w;
	world.z = worldSpace.z * worldSpace.w;

	return true;
}

// ** Camera::pointToScreenSpace
bool Camera::pointToScreenSpace( const Vec3& world, Vec3& screen, const Matrix4& transform ) const
{
	// Calculate the view projection matrix.
	Matrix4 vp = calculateViewProjection( transform );

	// Transform the input point.
	Vec4 projected = vp * Vec4( world.x, world.y, world.z );

	if( projected.w == 0.0f ) {
		return false;
	}

	// Perspective divide
	projected.x /= projected.w;
	projected.y /= projected.w;
	projected.z /= projected.w;

	// Get the camera viewport rect
	Rect viewport = this->viewport();

	// Map from [-1; 1] range to viewport
	screen.x = viewport.min().x + viewport.width()  * (projected.x * 0.5f + 0.5f);
	screen.y = viewport.min().y + viewport.height() * (projected.y * 0.5f + 0.5f);
	screen.z = projected.z * 0.5f + 0.5f;

	return true;
}

// ** Camera::sphereToScreenSpace
Circle Camera::sphereToScreenSpace( const Sphere& sphere, const TransformWPtr& transform ) const
{
	Vec3 center, tangent;

	// Extract the transformation matrix
	Matrix4 T = transform->matrix();

	// Project the center point of a sphere to a screen space.
	pointToScreenSpace( sphere.center(), center, T );

	// Project the tangent point on sphere to a screen space.
	pointToScreenSpace( sphere.center() + transform->axisX() * sphere.radius(), tangent, T );

	// Caclulate the screen space radius.
	f32 radius = (tangent - center).length();

	return Circle( center, radius );
}
#endif

// -------------------------------------------- Viewport -------------------------------------------- //

// ** Viewport::Viewport
Viewport::Viewport( RenderTargetWPtr renderTarget )
    : m_renderTarget( renderTarget )
{
}

// ** Viewport::width
s32 Viewport::width( void ) const
{
    return m_renderTarget->width();
}

// ** Viewport::height
s32 Viewport::height( void ) const
{
    return m_renderTarget->height();
}

// ** Viewport::denormalize
Rect Viewport::denormalize( const Rect& normalized ) const
{
    s32 w = width();
    s32 h = height();
    return Rect( w * normalized.min().x, h * normalized.min().y, w * normalized.max().x, h * normalized.max().y );
}

// ** Viewport::touchBegan
void Viewport::touchBegan( s32 id, s32 x, s32 y, u8 flags )
{
    TouchEvent touchEvent = { id, x, y, 0, 0 };
    m_events.push_back( InputEvent( InputEvent::TouchBeganEvent, touchEvent, flags ) );
}

// ** Viewport::touchEnded
void Viewport::touchEnded( s32 id, s32 x, s32 y, u8 flags )
{
    TouchEvent touchEvent = { id, x, y, 0, 0 };
    m_events.push_back( InputEvent( InputEvent::TouchEndedEvent, touchEvent, flags ) );
}

// ** Viewport::touchMoved
void Viewport::touchMoved( s32 id, s32 x, s32 y, s32 dx, s32 dy, u8 flags )
{
    TouchEvent touchEvent = { id, x, y, dx, dy };
    m_events.push_back( InputEvent( InputEvent::TouchMovedEvent, touchEvent, flags ) );
}

// ** Viewport::eventCount
s32 Viewport::eventCount( void ) const
{
    return static_cast<s32>( m_events.size() );
}

// ** Viewport::eventAt
const InputEvent& Viewport::eventAt( s32 index ) const
{
    DC_ABORT_IF( index < 0 || index >= eventCount(), "index is out of range" );
    return m_events[index];
}

// ** Viewport::clearEvents
void Viewport::clearEvents( void )
{
    m_events.clear();
}

} // namespace Scene

DC_END_DREEMCHEST