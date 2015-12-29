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

#ifndef __DC_Composer_Systems_Gizmo_H__
#define __DC_Composer_Systems_Gizmo_H__

#include "../Composer.h"

DC_BEGIN_COMPOSER

	//! Shared transformation gizmo data.
	class Gizmo {
	public:

		//! Gizmo color presets.
		static Rgba		kRed;
		static Rgba		kGreen;
		static Rgba		kBlue;
		static Rgba		kActive;
		static Rgba		kScreen;
		static Rgba		kArcball;

		//! Available gizmo states.
		enum State {
			  Idle		//!< Gizmo does not have any highlighted parts & not locked.
			, Active	//!< Gizmo has a highlighted part.
			, Locked	//!< Gizmo is now being transformed.
		};

								//! Constructs Gizmo instance.
								Gizmo( void )
									: m_state( Idle ), m_type( 0 ) {}

		//! Returns gizmo state.
		State					state( void ) const;

		//! Returns the selected type.
		u8						type( void ) const;

		//! Returns initial cursor position.
		const Vec2&				cursor( void ) const;

		//! Returns initial view ray.
		const Ray&				ray( void ) const;

		//! Returns screen space tangent axis.
		const Vec2&				tangent( void ) const;

		//! Returns initial transform.
		const Scene::Transform&	transform( void ) const;

		//! Activates the specified transform type.
		void					activate( u8 value );

		//! Deactivates the gizmo.
		void					deactivate( void );

		//! Locks this gizmo with specified origin point.
		void					lock( const Scene::Transform& transform, const Vec2& cursor, const Ray& ray = Ray(), const Vec2& tangent = Vec2() );

		//! Unlocks this gizmo.
		void					unlock( void );

	private:

		State					m_state;		//!< Current gizmo state.
		u8						m_type;			//!< Selected transform type.
		Scene::Transform		m_transform;	//!< Initial transform.
		Vec2					m_cursor;		//!< Initial cursor position.
		Ray						m_ray;			//!< Initial view ray.
		Vec2					m_tangent;		//!< Tangent axis to a grab point.
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Systems_Gizmo_H__	*/