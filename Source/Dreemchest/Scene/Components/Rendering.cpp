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

// ------------------------------------------------ Light ------------------------------------------------- //

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

// ** Light::serialize
void Light::serialize( Ecs::SerializationContext& ctx, Archive& ar ) const
{
    DC_NOT_IMPLEMENTED;
}

// ** Light::deserialize
void Light::deserialize( Ecs::SerializationContext& ctx, const Archive& ar )
{
    DC_NOT_IMPLEMENTED;
}

// ---------------------------------------------- StaticMesh ---------------------------------------------- //

// ** StaticMesh::mesh
MeshHandle StaticMesh::mesh( void ) const
{
	return m_mesh;
}

// ** StaticMesh::setMesh
void StaticMesh::setMesh( MeshHandle value )
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

// ** StaticMesh::const 
bool StaticMesh::isVisible( u8 camera ) const
{
	DC_ABORT_IF( camera >= 16, "index is out of range" );
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
MaterialHandle StaticMesh::material( u32 index ) const
{
	return index < materialCount() ? m_materials[index] : MaterialHandle();
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

// ** StaticMesh::serialize
void StaticMesh::serialize( Ecs::SerializationContext& ctx, Archive& ar ) const
{
    VariantArray materials;

    for( u32 i = 0, n = materialCount(); i < n; i++ ) {
        MaterialHandle m = material( i );
        materials << (m.isValid() ? m.uniqueId() : "");
    }

    ar = KvBuilder() << "asset" << (m_mesh.isValid() ? m_mesh.uniqueId() : "") << "materials" << materials;
}

// ** StaticMesh::deserialize
void StaticMesh::deserialize( Ecs::SerializationContext& ctx, const Archive& ar )
{
    Assets::Assets* assets = ctx.get<Assets::Assets>();
	
    if( !assets ) {
        LogError( "staticMesh", "no Assets attached to serialization context.\n" );
        return;
    }

    KeyValue     kv        = ar.as<KeyValue>();
    VariantArray materials = kv.get<VariantArray>( "materials" );
    const VariantArray::Container& items = materials;

    for( s32 i = 0, n = items.size(); i < n; i++ ) {
        setMaterial( i, assets->find<Material>( items[i].as<String>() ) );
    }

    m_mesh = assets->find<Mesh>( kv.get<String>( "asset" ) );
    if( !m_mesh.isValid() ) {
        LogWarning( "staticMesh", "unresolved asset '%s'\n", kv.get<String>( "asset" ).c_str() );
    }
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

// ** Sprite::image
ImageHandle Sprite::image( void ) const
{
	return m_image;
}

// ** Sprite::color
const Rgba& Sprite::color( void ) const
{
	return m_color;
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
	DC_ABORT_IF( !m_target.valid(), "invalid render target" )

	u32 w = m_target->width();
	u32 h = m_target->height();

	return Rect( w * m_ndc.min().x, h * m_ndc.min().y, w * m_ndc.max().x, h * m_ndc.max().y );
}

// ** Camera::setView
void Camera::setTarget( const RenderTargetPtr& value )
{
	m_target = value;
}

// ** Camera::view
const RenderTargetPtr& Camera::target( void ) const
{
	return m_target;
}

// ** Camera::calculateProjectionMatrix
Matrix4 Camera::calculateProjectionMatrix( void ) const
{
	DC_ABORT_IF( !m_target.valid(), "invalid render target" )

	Rect rect   = viewport();
	f32  width  = rect.width();
	f32  height = rect.height();

	switch( m_projection ) {
	case Perspective:	return Matrix4::perspective( m_fov, width / height, m_near, m_far );
	case Ortho:			return Matrix4::ortho( 0, width, 0, height, -10000, 10000 );
	case OrthoCenter:	return Matrix4::ortho( -width * 0.5f, width * 0.5f, height * 0.5f, -height * 0.5f, -10000, 10000 );
	default:			DC_BREAK;
	}

	return Matrix4();
}

// ** Camera::calculateViewProjection
Matrix4 Camera::calculateViewProjection( const Matrix4& transform ) const
{
	return calculateProjectionMatrix() * transform.inversed() /*Matrix4::lookAt( Vec3( 5, 5, 5 ), Vec3( 0, 0, 0 ), Vec3( 0, 1, 0 ) )*/;
}

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

// ** Camera::serialize
void Camera::serialize( Ecs::SerializationContext& ctx, Archive& ar ) const
{
    ar = KvBuilder()
            << "clearMask" << m_clearMask
            << "projection" << m_projection
            << "clearColor" << m_clearColor
            << "fov" << m_fov
            << "near" << m_near
            << "far" << m_far
            ;
}

// ** Camera::deserialize
void Camera::deserialize( Ecs::SerializationContext& ctx, const Archive& ar )
{
    KeyValue kv     = ar.as<KeyValue>();
    m_clearMask     = kv.get<u8>( "clearMask" );
    m_projection    = static_cast<Projection>( kv.get<s32>( "projection", Perspective ) );
    m_clearColor    = kv.get( "clearColor", Rgba() );
    m_fov           = kv.get( "fov", 60.0f );
    m_near          = kv.get( "near", 0.01f );
    m_far           = kv.get( "far", 1000.0f );
}

} // namespace Scene

DC_END_DREEMCHEST