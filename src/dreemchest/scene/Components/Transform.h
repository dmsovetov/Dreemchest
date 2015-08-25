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

	//! Basic scene transformation
	class BasicTransform : public SceneComponent {
	public:

									OverrideComponent( BasicTransform, SceneComponent )

									//! Constructs BasicTransform instance
									BasicTransform( BasicTransform* parent = NULL );

		//! Calculates the affine transform matrix.
		Matrix4						affine( void ) const;

		//! Returns parent transform.
		const BasicTransformWPtr&	parent( void ) const;

		//! Sets parent transform.
		void						setParent( const BasicTransformWPtr& value );

		//! Calculates the transformation matrix.
		virtual Matrix4				matrix( void ) const;

	private:

		BasicTransformWPtr			m_parent;	//!< Parent transform
	};

	//! Scene object 2D transformation component.
	class Transform2D : public BasicTransform {
	public:

								OverrideComponent( Transform2D, BasicTransform )
									
								//! Constructs Transform2D instance.
								Transform2D( f32 x = 0.0f, f32 y = 0.0f, f32 rotation = 0.0f, f32 sx = 1.0f, f32 sy = 1.0f, BasicTransform* parent = NULL )
									: BasicTransform( parent ), m_x( x ), m_y( y ), m_rotation( rotation ), m_scaleX( sx ), m_scaleY( sy ) {}

		//! Returns an affine transformation matrix.
		virtual Matrix4			matrix( void ) const;

		//! Returns transformation position as a 2D vector.
		Vec2					position( void ) const;

		//! Sets position from a 2D vector.
		void					setPosition( const Vec2& value );

		//! Returns transformation X coordinate
		f32						x( void ) const;

		//! Sets transform X coordinate.
		void					setX( f32 value );

		//! Returns transformation Y coordinate
		f32						y( void ) const;

		//! Sets transform Y coordinate.
		void					setY( f32 value );

		//! Returns transform rotation.
		f32						rotation( void ) const;

		//! Sets transform rotation.
		void					setRotation( f32 value );

	private:

		f32						m_x;		//!< Object X coordinate.
		f32						m_y;		//!< Object Y coordinate.
		f32						m_rotation;	//!< Object rotation.
		f32						m_scaleX;	//!< Object X scale.
		f32						m_scaleY;	//!< Object Y scale.
	};

	//! Scene object 3D transformation component.
	class Transform : public BasicTransform {
	public:

								OverrideComponent( Transform, BasicTransform )

								//! Constructs Transform instance.
								Transform( const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3( 1.0f, 1.0f, 1.0f ), BasicTransform* parent = NULL )
									: BasicTransform( parent ), m_position( position ), m_rotation( rotation ), m_scale( scale ) {}

		//! Returns an affine transformation matrix.
		virtual Matrix4			matrix( void ) const;

		//! Returns transform position.
		const Vec3&				position( void ) const;

		//! Sets transform position.
		void					setPosition( const Vec3& value );

		//! Returns transform rotation.
		const Quat&				rotation( void ) const;

		//! Sets transform rotation.
		void					setRotation( const Quat& value );

	private:

		//! Object position.
		Vec3					m_position;

		//! Object rotation.
		Quat					m_rotation;

		//! Object scale.
		Vec3					m_scale;
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

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Transform_H__    */