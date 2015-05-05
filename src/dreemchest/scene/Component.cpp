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

#include "Component.h"
#include "Material.h"

DC_BEGIN_DREEMCHEST

namespace scene {

// ----------------------------------------- Transform ----------------------------------------- //

// ** Transform::parent
const TransformWPtr& Transform::parent( void ) const
{
	return m_parent;
}

// ** Transform::setParent
void Transform::setParent( const TransformWPtr& value )
{
	m_parent = value;
}

// ** Transform::affine
Matrix4 Transform::affine( void ) const
{
	if( const Transform* parent = m_parent.get() ) {
		return parent->affine() * Matrix4::translation( m_position ) * m_rotation * Matrix4::scale( m_scale );
	}

	return Matrix4::translation( m_position ) * m_rotation * Matrix4::scale( m_scale );
}

// ---------------------------------------- MeshRenderer ---------------------------------------- //

// ** MeshRenderer::mesh
const MeshPtr& MeshRenderer::mesh( void ) const
{
	return m_mesh;
}

// ** MeshRenderer::setMesh
void MeshRenderer::setMesh( const MeshPtr& value )
{
	m_mesh = value;
}

// ** MeshRenderer::materialCount
u32 MeshRenderer::materialCount( void ) const
{
	return ( u32 )m_materials.size();
}

// ** MeshRenderer::material
MaterialPtr MeshRenderer::material( u32 index ) const
{
	return index < materialCount() ? m_materials[index] : MaterialPtr();
}

// ** MeshRenderer::setMaterial
void MeshRenderer::setMaterial( u32 index, const MaterialPtr& value )
{
	DC_BREAK_IF( index > 8 );

	if( index >= materialCount() ) {
		m_materials.resize( index + 1 );
	}

	m_materials[index] = value;
}

// ** MeshRenderer::lightmap
const renderer::TexturePtr& MeshRenderer::lightmap( void ) const
{
	return m_lightmap;
}

// ** MeshRenderer::setLightmap
void MeshRenderer::setLightmap( const renderer::TexturePtr& value )
{
	m_lightmap = value;
}

// ---------------------------------------- Camera ---------------------------------------- //

// ** Camera::move
void Camera::move( f32 amount )
{
	m_position = m_position + m_direction * amount;
}

// ** Camera::strafe
void Camera::strafe( f32 amount )
{
	m_position = m_position + m_right * amount;
}

// ** Camera::pitch
void Camera::pitch( f32 amount )
{
	m_direction = Quat::rotateAroundAxis( amount, m_right ).rotate( m_direction );
}

// ** Camera::yaw
void Camera::yaw( f32 amount )
{
	// ** Rotate the view direction
	m_direction = Quat::rotateAroundAxis( amount, m_up ).rotate( m_direction );

	// ** Calculate a new side vector.
	m_right = m_direction % m_up;
	m_right.normalize();
}

// ** Camera::viewProj
Matrix4 Camera::viewProj( f32 aspect ) const
{
	return view() * proj( aspect );
}

// ** Camera::view
Matrix4 Camera::view( void ) const
{
	return Matrix4::lookAt( m_position, m_position + m_direction, m_up );
}

// ** Camera::proj
Matrix4 Camera::proj( f32 aspect ) const
{
	return Matrix4::perspective( m_fov, aspect, m_near, m_far );
}

} // namespace scene

DC_END_DREEMCHEST