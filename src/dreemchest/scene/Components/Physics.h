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

#ifndef __DC_Scene_Component_Physics_H__
#define __DC_Scene_Component_Physics_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Shape of an object used for hit tests and/or 2D physics.
	class Shape2D : public Ecs::Component<Shape2D> {
	public:

		//! The shape part types.
		enum Type {
			  Circle = 0		//!< The circle shape.
			, Rect				//!< The rectangular shape.
			, Polygon			//!< The polygonal shape.
			, TotalShapeTypes	//!< The total number of shape types.
		};

		//! Shape part material.
		struct Material {
			f32			density;
			f32			friction;
			f32			restitution;

						//! Constructs the material instance.
						Material( f32 density = 1.0f, f32 friction = 0.2f, f32 restitution = 0.0f )
							: density( density ), friction( friction ), restitution( restitution ) {}
		};

		//! The shape is composed from parts.
		struct Part {
			enum { MaxVertices = 8 };		//!< Maximum number of polygon vertices.

			Type		type;				//!< Shape type.
			Material	material;			//! Shape material.

			union {
				struct {
					f32		radius;			//!< Circle radius.
					f32		x;				//!< Circle centroid X.
					f32		y;				//!< Circle centroid Y.
				} circle;	//!< Circle shape data.

				struct {
					f32		width;			//!< Rectangle width.
					f32		height;			//!< Rectangle height.
					f32		x;				//!< Rectangle centroid X.
					f32		y;				//!< Rectangle centroid Y.
				} rect;		//!< Rectangle shape data.

				struct {
					f32		vertices[(MaxVertices + 1) * 2];	//!< Polygon vertices.
					u32		count;								//!< Polygon vertex count.
				} polygon;	//! Polygon shape data.
			};
		};

							//! Constructs Shape2D instance.
							Shape2D( void ) {}

		//! Removes all nested shape parts.
		void				clear( void );

		//! Returns the total number of nested shapes.
		u32					partCount( void ) const;

		//! Returns the nested shape by index.
		const Part&			part( u32 index ) const;

        //! Adds a new shape part.
        void                addPart( const Part& part );

		//! Adds a new circle shape part.
		void				addCircle( f32 radius, f32 x = 0, f32 y = 0, const Material& material = Material() );

		//! Adds a new rectangle shape part.
		void				addRect( f32 width, f32 height, f32 x = 0, f32 y = 0, const Material& material = Material() );

		//! Adds a new polygon shape part.
		void				addPolygon( const Vec2* vertices, u32 count, const Material& material = Material() );

		//! Writes 2D shape to a key-value archive.
		virtual void        serialize( Ecs::SerializationContext& ctx, Io::KeyValue& ar ) const DC_DECL_OVERRIDE;

		//! Reads 2D shape from a key-value archive.
		virtual void		deserialize( Ecs::SerializationContext& ctx, const Io::KeyValue& ar ) DC_DECL_OVERRIDE;

	private:

		Array<Part>			m_parts;	//!< Shape parts.
	};

	//! 2D rigid body.
	class RigidBody2D : public Ecs::Component<RigidBody2D> {
	public:

		//! Supported rigid body types.
		enum Type {
			  Static = 0	//!< Static rigid body.
			, Dynamic		//!< Dynamic rigid body.
			, Kinematic		//!< Kinematic rigid body.
			, TotalTypes	//!< The total number of rigid body types.
		};

		//! A single force or impulse applied is stored in this structure.
		struct AppliedForce {
							    //! Constructs the AppliedForce instance.
							    AppliedForce( const Vec2& value = Vec2(), const Vec2& point = Vec2() )
								    : m_value( value ), m_point( point ) {}

			Vec2			    m_value;    //!< The force applied.
			Vec2			    m_point;    //!< The application point.
		};

        //! Collision event data.
        struct CollisionEvent {
            //! Collision event type.
            enum Type {
                  Begin     //!< Contact begin event.
                , End       //!< Contact end event.
            };

                                //! Constructs empty CollisionEvent instance.
                                CollisionEvent( void )
                                    {}
                                //! Constructs Event instance.
                                CollisionEvent( Type type, SceneObjectWPtr other )
                                    : type( type ), other( other ) {}

            SceneObjectWPtr     other;  //!< First contact body.
            Type                type;   //!< Collision type.
        };

							    //! Constructs the RigidBody2D instance.
							    RigidBody2D( f32 mass = 0.0f, Type type = Static )
								    : m_mass( mass ), m_type( type ), m_linearDamping( 0.0f ), m_angularDamping( 0.0f ), m_torque( 0.0f ) {}

		//! Returns the rigid body mass.
		f32					    mass( void ) const;

		//! Returns the rigid body type.
		Type				    type( void ) const;

		//! Returns the linear damping applied to a body.
		f32					    linearDamping( void ) const;

		//! Sets the linear damping applied to a body.
		void				    setLinearDamping( f32 value );

		//! Returns the angular damping applied to a body.
		f32					    angularDamping( void ) const;

		//! Sets the angular damping applied to a body.
		void				    setAngularDamping( f32 value );

		//! Returns the torque applied to this rigid body.
		f32					    torque( void ) const;

		//! Applies the torque to the rigid body.
		void				    applyTorque( f32 value );

		//! Returns the force applied to this rigid body.
		const Vec2&			    force( void ) const;

		//! Applies force to the center of mass.
		void				    applyForce( const Vec2& value );

		//! Applies force to a specified point.
		void				    applyForceToPoint( const Vec2& value, const Vec2& point );

        //! Applies linear impulse to the center of mass.
        void                    applyImpulse( const Vec2& value );

        //! Applies linear impulse to a specified point.
        void                    applyImpulseToPoint( const Vec2& value, const Vec2& point );

		//! Returns the total number of applied forces.
		u32					    appliedForceCount( void ) const;

		//! Returns the applied force by index.
		const AppliedForce&	    appliedForce( u32 index ) const;

        //! Returns the total number of applied impulses.
        u32                     appliedImpulseCount( void ) const;

        //! Returns the applied impulse by index.
        const AppliedForce&     appliedImpulse( u32 index ) const;

        //! Returns the total number of queued events.
        u32                     collisionEventCount( void ) const;

        //! Returns the collision event by index.
        const CollisionEvent&   collisionEvent( u32 index ) const;

        //! Queues new collision event.
        void                    queueCollisionEvent( const CollisionEvent& e );

        //! Clears all queued events.
        void                    clearEvents( void );

		//! Clears all applied forces and impulses.
		void				    clear( void );

	private:

		f32					    m_mass;				//!< The rigid body mass.
		Type				    m_type;				//!< The rigid body type.
		f32					    m_linearDamping;	//!< The linear damping applied to a body.
		f32					    m_angularDamping;	//!< The angular damping applied to a body.
		f32					    m_torque;			//!< The total torque applied.
		Vec2				    m_force;			//!< The total force applied.
		Array<AppliedForce>	    m_forces;			//!< All applied forces.
        Array<AppliedForce>     m_impulses;         //!< All applied impulses.
        Array<CollisionEvent>   m_collisionEvents;  //!< All collision events recorded since on simulation step.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Physics_H__    */