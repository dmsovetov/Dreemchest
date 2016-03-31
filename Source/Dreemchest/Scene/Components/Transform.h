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
#include "../Bindings.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Scene object transformation component.
	class Transform : public Ecs::Component<Transform> {

        INTROSPECTION( Transform
            , PROPERTY( position, position, setPosition, "The local position of this Transform relative to parent." )
            , PROPERTY( rotation, rotation, setRotation, "The local rotation of this Transform relative to parent." )
            , PROPERTY( scale,    scale,    setScale,    "The local scaling of this Transform relative to parent."  )
            )

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

		//! Calculates the local X axis.
		Vec3					axisX( void ) const;

        //! Calculates the world space X axis.
        Vec3                    worldSpaceAxisX( void ) const;

		//! Calculates the local Y axis.
		Vec3					axisY( void ) const;

        //! Calculates the world space Y axis.
        Vec3                    worldSpaceAxisY( void ) const;

		//! Calculates the local Z axis.
		Vec3					axisZ( void ) const;

        //! Calculates the world space Z axis.
        Vec3                    worldSpaceAxisZ( void ) const;

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

    protected:

    #if DEV_DEPRECATED_SERIALIZATION
        //! Writes the transform data to a key-value archive.
		virtual void            serialize( Ecs::SerializationContext& ctx, Archive& ar ) const NIMBLE_OVERRIDE;

		//! Reads the transform data from a key-value archive.
		virtual void		    deserialize( Ecs::SerializationContext& ctx, const Archive& ar ) NIMBLE_OVERRIDE;
    #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

	private:

		TransformWPtr			m_parent;		//!< Parent transform.
		Vec3					m_position;		//!< Object position.
		Quat					m_rotation;		//!< Object rotation.
		Vec3					m_scale;		//!< Object scale.
		Matrix4					m_transform;	//!< Affine transform matrix.
	};

	//! The coordinate system axes.
	enum CoordinateSystemAxis {
		  AxisX						//!< The X axis.
		, AxisY						//!< The Y axis.
		, AxisZ						//!< The Z axis.
		
		, TotalCoordinateSystemAxes	//!< The total number of coordinate system axes.
	};

	//! Bitset flags used in transformations.
	enum CoordinateSystemFlags {
		  CSLocalX	= BIT( 0 )							//!< Use local X axis for transformations.
		, CSLocalY	= BIT( 1 )							//!< Use local Y axis for transformations.
		, CSLocalZ	= BIT( 2 )							//!< Use local Z axis for transformations.
		, CSLocal	= CSLocalX | CSLocalY | CSLocalZ	//!< Use the local coordinate system for transformations.
		, CSWorld	= 0									//!< Use the world coordinate system for transformations.
	};

	//! Identifier component.
	class Identifier : public Ecs::Component<Identifier> {

        INTROSPECTION( Identifier
            , PROPERTY( name, name, setName, "The entity identifier value." )
            )

	public:

								//! Constructs the Identifier instance.
								Identifier( const String& name = "" )
									: m_name( name ) {}

		//! Returns the identifier.
		const String&			name( void ) const;

		//! Sets the identifier.
		void					setName( const String& value );

    protected:

    #if DEV_DEPRECATED_SERIALIZATION
        //! Writes the identifier to a key-value archive.
		virtual void            serialize( Ecs::SerializationContext& ctx, Archive& ar ) const NIMBLE_OVERRIDE;

		//! Reads the identifier from a key-value archive.
		virtual void		    deserialize( Ecs::SerializationContext& ctx, const Archive& ar ) NIMBLE_OVERRIDE;
    #endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */

	private:

		String					m_name;	//!< Scene object name.
	};

	//! Moves the scene object transform along the coordinate axes.
	class MoveAlongAxes : public Ecs::Component<MoveAlongAxes> {
	public:

								//! Constructs MoveAlongAxes instance.
								MoveAlongAxes( f32 speed = 1.0f, u8 coordinateSystem = CSWorld, const Vec3BindingPtr& delta = Vec3BindingPtr() )
									: m_coordinateSystem( coordinateSystem ), m_speed( speed ), m_delta( delta ) {}

		//! Returns the movement speed.
		f32						speed( void ) const;

		//! Returns the coordinate system used for transformations.
		u8						coordinateSystem( void ) const;

		//! Returns the movement delta values.
		Vec3					delta( void ) const;

		//! Sets the range for each of axes.
		void					setRangeForAxis( CoordinateSystemAxis axis, const Range& value );

		//! Returns the range for a specified axis.
		const Range&			rangeForAxis( CoordinateSystemAxis axis ) const;

	private:

		u8						m_coordinateSystem;					//!< Coordinate system flags.
		f32						m_speed;							//!< Movement speed.
		Vec3BindingPtr			m_delta;							//!< Movement deltas.
		Range					m_range[TotalCoordinateSystemAxes];	//!< Range for each of CS axes.
	};

	//! Rotates the scene object transform around axes.
	class RotateAroundAxes : public Ecs::Component<RotateAroundAxes> {
	public:

								//! Constructs RotateAroundAxes instance.
								RotateAroundAxes( f32 speed = 1.0f, u8 coordinateSystem = CSWorld, const Vec3BindingPtr& delta = Vec3BindingPtr() )
									: m_coordinateSystem( coordinateSystem ), m_speed( speed ), m_delta( delta ) { memset( m_rotation, 0, sizeof( m_rotation ) ); }

		//! Returns the rotation speed.
		f32						speed( void ) const;

		//! Sets the rotation speed.
		void					setSpeed( f32 value );

		//! Returns the coordinate system used for transformations.
		u8						coordinateSystem( void ) const;

		//! Returns rotation delta values.
		Vec3					delta( void ) const;

		//! Returns the rotation binding.
		Vec3BindingWPtr			binding( void ) const;

		//! Sets the rotation binding.
		void					setBinding( const Vec3BindingPtr& value );

		//! Returns the range for a specified axis.
		const Range&			rangeForAxis( CoordinateSystemAxis axis ) const;

		//! Sets the range for each of axes.
		void					setRangeForAxis( CoordinateSystemAxis axis, const Range& value );

		//! Returns the accumulated rotation value for a specified axis.
		f32						rotationForAxis( CoordinateSystemAxis axis ) const;

		//! Sets the rotation value for a specified axis.
		void					setRotationForAxis( CoordinateSystemAxis axis, f32 value );

	private:

		u8						m_coordinateSystem;						//!< Coordinate system flags.
		f32						m_speed;								//!< Rotation speed.
		Vec3BindingPtr			m_delta;								//!< Rotation delta values.
		Range					m_range[TotalCoordinateSystemAxes];		//!< Range for each of CS axes.
		f32						m_rotation[TotalCoordinateSystemAxes];	//!< Contains the accumulated rotation values.
	};

	//! Moves the transform to a target position.
	class MoveTo : public Ecs::Component<MoveTo> {
	public:

		//! Available follow types.
		enum Type {
			Immediate,	//!< Sets the transform to a target transform.
			Smooth,		//!< Smoothly changes the transform to a target one.
			Elastic,	//!< Calculates the spring force.
		};

								//! Constructs the MoveTo instance.
								MoveTo( const Vec3BindingPtr& target = Vec3BindingPtr(), bool isContinuous = true, Type type = Immediate, f32 damping = 0.25f, f32 springForce = 0.1f )
									: m_target( target ), m_isContinuous( isContinuous ), m_type( type ), m_damping( damping ), m_springForce( springForce ) {}

		//! Returns the target position.
		Vec3					target( void ) const;

		//! Returns the follow type
		Type					type( void ) const;

		//! Returns true if the move to is continuous.
		bool					isContinuous( void ) const;

		//! Returns the damping factor.
		f32						damping( void ) const;

		//! Returns the spring force.
		f32						springForce( void ) const;

	private:

		Vec3BindingPtr			m_target;		//!< The transform to follow.
		bool					m_isContinuous;	//!< If the move to is no continuous it will be detached once the target is reached.
		Type					m_type;			//!< The following type.
		f32						m_damping;		//!< Damping factor.
		f32						m_springForce;	//!< The spring force.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Transform_H__    */