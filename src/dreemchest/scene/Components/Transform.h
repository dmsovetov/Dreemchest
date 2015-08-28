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

#ifndef __DC_Scene_Component_Transform_H__
#define __DC_Scene_Component_Transform_H__

#include "Component.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! Scene object transformation component.
	class Transform : public SceneComponent {
	public:

								OverrideComponent( Transform, SceneComponent )
									
								//! Constructs Transform2D instance.
								Transform( f32 x = 0.0f, f32 y = 0.0f, f32 rotation = 0.0f, f32 sx = 1.0f, f32 sy = 1.0f, const TransformWPtr& parent = TransformWPtr() )
									: m_position( x, y, 0.0f ), m_rotation( Quat::rotateAroundAxis( rotation, Vec3( 0.0f, 0.0f, -1.0f ) ) ), m_scale( sx, sy, 1.0f ) {}

		//! Returns an affine transformation matrix.
		virtual Matrix4			matrix( void ) const;

		//! Returns parent transform.
		const TransformWPtr&	parent( void ) const;

		//! Sets parent transform.
		void					setParent( const TransformWPtr& value );

		//! Returns transformation position.
		const Vec3&				position( void ) const;

		//! Sets the position.
		void					setPosition( const Vec3& value );

		//! Returns transformation X coordinate
		f32						x( void ) const;

		//! Sets transform X coordinate.
		void					setX( f32 value );

		//! Returns transformation Y coordinate
		f32						y( void ) const;

		//! Sets transform Y coordinate.
		void					setY( f32 value );

		//! Returns transformation Z coordinate
		f32						z( void ) const;

		//! Sets transform Z coordinate.
		void					setZ( f32 value );

		//! Returns transform rotation.
		f32						rotation( void ) const;

		//! Sets transform rotation.
		void					setRotation( f32 value );

	private:

		TransformWPtr			m_parent;	//!< Parent transform.
		Vec3					m_position;	//!< Object position.
		Quat					m_rotation;	//!< Object rotation.
		Vec3					m_scale;	//!< Object scale.
	};

	//! Rotates the 2D transform
	class Rotator2D : public SceneComponent {
	public:

								OverrideComponent( Rotator2D, SceneComponent )

								//! Constructs the Rotator2D instance.
								Rotator2D( f32 speed = 1.0f )
									: m_speed( speed ) {}

		//! Returns the rotation speed.
		f32						speed( void ) const;

	private:

		f32						m_speed;	//!< The rotation speed.
	};

	//! Follows the transform
	class Follow : public SceneComponent {
	public:

		//! Available follow types.
		enum Type {
			Immediate,	//!< Sets the transform to a target transform.
			Smooth,		//!< Smoothly changes the transform to a target one.
			Elastic,	//!< Calculates the spring force.
		};

								OverrideComponent( Follow, SceneComponent )

								//! Constructs the Follow instance.
								Follow( const TransformWPtr& target = TransformWPtr(), Type type = Immediate, f32 damping = 0.25f, f32 springForce = 0.1f )
									: m_target( target ), m_type( type ), m_damping( damping ), m_springForce( springForce ) {}

		//! Returns the target transform.
		const TransformWPtr&	target( void ) const;

		//! Returns the follow type
		Type					type( void ) const;

		//! Returns the damping factor.
		f32						damping( void ) const;

		//! Returns the spring force.
		f32						springForce( void ) const;

	private:

		TransformWPtr			m_target;		//!< The transform to follow.
		Type					m_type;			//!< The following type.
		f32						m_damping;		//!< Damping factor.
		f32						m_springForce;	//!< The spring force.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Transform_H__    */