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

#ifndef __DC_Scene_Component_H__
#define __DC_Scene_Component_H__

#include "SceneObject.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! A base class for all components.
	class Component : public RefCounted {
	public:

		virtual					~Component( void ) {}

		//! Returns a parent scene object.
		const SceneObjectWPtr&	sceneObject( void ) const;

	protected:

								//! Constructs Component instance.
								Component( SceneObject* sceneObject )
									: m_sceneObject( sceneObject ) {}

	protected:

		//! Parent scene object.
		SceneObjectWPtr			m_sceneObject;
	};

	//! Scene object transformation component.
	class Transform : public Component {
	public:

								//! Constructs Transform instance.
								Transform( SceneObject* sceneObject, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3( 1.0f, 1.0f, 1.0f ), Transform* parent = NULL )
									: Component( sceneObject ), m_parent( parent ), m_position( position ), m_rotation( rotation ), m_scale( scale ) {}

		//! Returns parent transform.
		const TransformWPtr&	parent( void ) const;

		//! Sets parent transform.
		void					setParent( const TransformWPtr& value );

		//! Returns transform position.
		const Vec3&				position( void ) const;

		//! Sets transform position.
		void					setPosition( const Vec3& value );

		//! Returns transform rotation.
		const Quat&				rotation( void ) const;

		//! Sets transform rotation.
		void					setRotation( const Quat& value );

		//! Calculates the affine transform matrix.
		Matrix4					affine( void ) const;

	private:

		//! Parent transform.
		TransformWPtr			m_parent;

		//! Object position.
		Vec3					m_position;

		//! Object rotation.
		Quat					m_rotation;

		//! Object scale.
		Vec3					m_scale;
	};
	
	//! Mesh renderer component.
	class MeshRenderer : public Component {
	public:

									//! Constructs MeshRenderer instance.
									MeshRenderer( SceneObject* sceneObject, const MeshPtr& mesh = MeshPtr() )
										: Component( sceneObject ), m_mesh( mesh ) {}

		//! Returns mesh to be rendered.
		const MeshPtr&				mesh( void ) const;

		//! Sets a mesh to be rendered.
		void						setMesh( const MeshPtr& value );

		//! Returns the total number of materials.
		u32							materialCount( void ) const;

		//! Returns the material by index.
		MaterialPtr					material( u32 index ) const;

		//! Sets the material by index.
		void						setMaterial( u32 index, const MaterialPtr& value );

		//! Returns a lightmap texture.
		const renderer::TexturePtr&	lightmap( void ) const;

		//! Sets a lightmap texture.
		void						setLightmap( const renderer::TexturePtr& value );

	private:

		MeshPtr						m_mesh;			//!< Mesh to be rendered.
		Array<MaterialPtr>			m_materials;	//!< Mesh materials array.
		renderer::TexturePtr		m_lightmap;		//!< Lightmap texture that is rendered for this mesh.
	};

	//! Camera component.
	class Camera : public Component {
	public:

									//! Constructs Camera instance.
									Camera( SceneObject* sceneObject )
										: Component( sceneObject ), m_direction( 0, 0, 1 ), m_up( 0, 1, 0 ), m_right( 1, 0, 0 ), m_fov( 60.0f ), m_near( 0.01f ), m_far( 100.0f ) {}

		//! Moves camera in a direction of view by a specified distance.
		void						move( f32 amount );

		//! Moves camera to a side of a right vector.
		void						strafe( f32 amount );

		//! Rotates camera view around the side vector.
		void						pitch( f32 amount );

		//! Rotates camera view around the up vector.
		void						yaw( f32 amount );

		//! Returns a composed view projection matrix with a specified aspect.
		Matrix4						viewProj( f32 apsect ) const;

		//! Returns a view matrix composed from a camera vectors.
		Matrix4						view( void ) const;

		//! Returns a projection matrix with a specified aspect.
		Matrix4						proj( f32 apsect ) const;

	private:

		Vec3						m_position;		//!< Camera position.
		Vec3						m_direction;	//!< Camera view direction.
		Vec3						m_up;			//!< Camera up vector.
		Vec3						m_right;		//!< Camera right side vector.
		f32							m_fov;			//!< Camera field of view angle.
		f32							m_near;			//!< Camera Z near.
		f32							m_far;			//!< Camera Z far.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_H__    */