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

		//! Transformation types.
		enum Transform {
			  X					//!< Transform X axis.
			, Y					//!< Transform Y axis.
			, Z					//!< Transform Z axis.
			, XY				//!< Transform X & Y axes.
			, YZ				//!< Transform Y & Z axes.
			, XZ				//!< Transfrom X & Z axes.
			, Nothing			//!< Transform nothing by this gizmo.
			
			, TotalTransforms	//!< The total number of gizmo transforms.
		};

		//! Available gizmo states.
		enum State {
			  Idle		//!< Gizmo does not have any highlighted parts & not locked.
			, Active	//!< Gizmo has a highlighted part.
			, Locked	//!< Gizmo is now being transformed.
		};

							//! Constructs Gizmo instance.
							Gizmo( void )
								: m_state( Idle ), m_transform( Nothing ) {}

		//! Returns gizmo state.
		Gizmo::State		state( void ) const;

		//! Returns the active transform.
		Gizmo::Transform	activeTransform( void ) const;

		//! Sets active transform.
		void				setActiveTransform( Gizmo::Transform value );

		//! Returns grabbed offset from gizmo position to mouse point.
		const Vec3&			offset( void ) const;

		//! Returns active transformation plane.
		const Plane&		plane( void ) const;

		//! Locks this gizmo with specified origin point.
		void				lock( const Plane& plane, const Vec3& origin );

		//! Unlocks this gizmo.
		void				unlock( void );

	private:

		Vec3				m_offset;		//!< Gizmo offset.
		State				m_state;		//!< Current gizmo state.
		Transform			m_transform;	//!< Transformation applied by gizmo.
		Plane				m_plane;		//!< Plane to use for transformations.
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Systems_Gizmo_H__	*/