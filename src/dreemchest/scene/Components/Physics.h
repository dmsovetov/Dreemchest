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

#include "Component.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! Shape of an object used for hit tests and/or 2D physics.
	class Shape2D : public SceneComponent {
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
						Material( f32 density = 0.0f, f32 friction = 0.2f, f32 restitution = 0.0f )
							: density( density ), friction( friction ), restitution( restitution ) {}
		};

		//! The shape is composed from parts.
		struct Part {
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
					f32		vertices[8];	//!< Polygon vertices.
					u32		count;			//!< Polygon vertex count.
				} polygon;	//! Polygon shape data.
			};
		};

							OverrideComponent( Shape2D, SceneComponent )

							//! Constructs Shape2D instance.
							Shape2D( void ) {}

		//! Removes all nested shape parts.
		void				clear( void );

		//! Returns the total number of nested shapes.
		u32					size( void ) const;

		//! Returns the nested shape by index.
		const Part&			part( u32 index ) const;

		//! Adds a new circle shape part.
		void				addCircle( f32 radius, f32 x = 0, f32 y = 0, const Material& material = Material() );

		//! Adds a new rectangle shape part.
		void				addRect( f32 width, f32 height, f32 x = 0, f32 y = 0, const Material& material = Material() );

		//! Adds a new polygon shape part.
		void				addPolygon( const Vec2* vertices, u32 count, const Material& material = Material() );

	private:

		Array<Part>			m_parts;	//!< Shape parts.
	};

	//! 2D rigid body.
	class RigidBody2D : public SceneComponent {
	public:

		//! Supported rigid body types.
		enum Type {
			  Static = 0	//!< Static rigid body.
			, Dynamic		//!< Dynamic rigid body.
			, Kinematic		//!< Kinematic rigid body.
			, TotalTypes	//!< The total number of rigid body types.
		};

							OverrideComponent( RigidBody2D, SceneComponent )

							//! Constructs the RigidBody2D instance.
							RigidBody2D( f32 mass = 0.0f, Type type = Static )
								: m_mass( mass ), m_type( type ), m_internal( NULL ) {}

		//! Returns the rigid body mass.
		f32					mass( void ) const;

		//! Returns the rigid body type.
		Type				type( void ) const;

		//! Returns an internal engine-specific rigid body implementation.
		void*				internal( void ) const;

		//! Sets an internal engine-specific rigid body implementation.
		void				setInternal( void* value );

	private:

		f32					m_mass;			//!< The rigid body mass.
		Type				m_type;			//!< The rigid body type.
		void*				m_internal;		//!< Internal rigid body implementation.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Physics_H__    */