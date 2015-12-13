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

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Scene object transformation component.
	class Transform : public Ecs::Component<Transform> {
	public:

								//! Constructs Transform instance.
								Transform( f32 x = 0.0f, f32 y = 0.0f, f32 rotation = 0.0f, f32 sx = 1.0f, f32 sy = 1.0f, const TransformWPtr& parent = TransformWPtr() )
									: m_parent( parent ), m_position( x, y, 0.0f ), m_rotation( Quat::rotateAroundAxis( rotation, Vec3( 0, 0, 1 ) ) ), m_scale( sx, sy, 1.0f ) {}

								//! Constructs Transform instance.
								Transform( f32 x, f32 y, f32 z, const TransformWPtr& parent = TransformWPtr() )
									: m_parent( parent ), m_position( x, y, z ), m_scale( 1.0f, 1.0f, 1.0f ) {}

								//! Constructs Transform instance.
								Transform( s32 x, s32 y, s32 z, const TransformWPtr& parent = TransformWPtr() )
									: m_parent( parent ), m_position( x, y, z ), m_scale( 1.0f, 1.0f, 1.0f ) {}

		//! Returns an affine transformation matrix.
		virtual const Matrix4&	matrix( void ) const;

		//! Sets the affine transform.
		void					setMatrix( const Matrix4& value );

		//! Returns parent transform.
		const TransformWPtr&	parent( void ) const;

		//! Sets parent transform.
		void					setParent( const TransformWPtr& value );

		//! Calculates the world space position.
		Vec3					worldSpacePosition( void ) const;

		//! Returns the X axis.
		Vec3					axisX( void ) const;

		//! Returns the Y axis.
		Vec3					axisY( void ) const;

		//! Returns the Z axis.
		Vec3					axisZ( void ) const;

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

		//! Returns the transform rotation quaternion.
		const Quat&				rotation( void ) const;

		//! Rotates the transform around an axis.
		void					rotate( f32 angle, f32 x, f32 y, f32 z ); 

		//! Sets the transform rotation quaternion.
		void					setRotation( const Quat& value );

		//! Returns transform rotation around the X axis.
		f32						rotationX( void ) const;

		//! Sets transform rotation around the X axis.
		void					setRotationX( f32 value );

		//! Returns transform rotation around the Y axis.
		f32						rotationY( void ) const;

		//! Sets transform rotation around the Y axis.
		void					setRotationY( f32 value );

		//! Returns transform rotation around the Z axis.
		f32						rotationZ( void ) const;

		//! Sets transform rotation around the Z axis.
		void					setRotationZ( f32 value );

		//! Sets the scale.
		void					setScale( const Vec3& value );

		//! Returns the scale.
		const Vec3&				scale( void ) const;

		//! Returns the X scale.
		f32						scaleX( void ) const;

		//! Sets the X scale.
		void					setScaleX( f32 value );

		//! Returns the Y scale.
		f32						scaleY( void ) const;

		//! Sets the X scale.
		void					setScaleY( f32 value );

		//! Returns the Z scale.
		f32						scaleZ( void ) const;

		//! Sets the Z scale.
		void					setScaleZ( f32 value );

	private:

		TransformWPtr			m_parent;		//!< Parent transform.
		Vec3					m_position;		//!< Object position.
		Quat					m_rotation;		//!< Object rotation.
		Vec3					m_scale;		//!< Object scale.
		Matrix4					m_transform;	//!< Affine transform matrix.
	};

	//! Follows the transform
	class Follow : public Ecs::Component<Follow> {
	public:

		//! Available follow types.
		enum Type {
			Immediate,	//!< Sets the transform to a target transform.
			Smooth,		//!< Smoothly changes the transform to a target one.
			Elastic,	//!< Calculates the spring force.
		};

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

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Transform_H__    */