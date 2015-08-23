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

#ifndef __DC_Scene_Component_Camera_H__
#define __DC_Scene_Component_Camera_H__

#include "Component.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! Basic camera
	class BasicCamera : public SceneComponent {
	public:

									OverrideComponent( BasicCamera, SceneComponent )

		//! Returns a composed view projection matrix.
		Matrix4						viewProj( void ) const;


		//! Returns a view matrix composed from a camera vectors.
		virtual Matrix4				view( void ) const;

		//! Returns a projection matrix.
		virtual Matrix4				proj( void ) const;
	};

	//! 2D space camera component.
	class Camera2D : public BasicCamera {
	public:

									OverrideComponent( Camera2D, BasicCamera )

									//! Constructs Camera instance.
									Camera2D( void )
										: m_width( 0 ), m_height( 0 ), m_zoom( 1.0f ), m_pan( 0.0f, 0.0f ) {}

		//! Returns the viewport width.
		u32							width( void ) const;

		//! Sets the viewport width.
		void						setWidth( u32 value );

		//! Returns the viewport height.
		u32							height( void ) const;

		//! Sets the viewport height.
		void						setHeight( u32 value );

		//! Returns a view matrix composed from a camera vectors.
		virtual Matrix4				view( void ) const;

		//! Returns a projection matrix.
		virtual Matrix4				proj( void ) const;

	private:

		u32							m_width;		//!< The viewport width.
		u32							m_height;		//!< The viewport height.
		f32							m_zoom;			//!< The zoom.
		Vec2						m_pan;			//!< Camera pan.
	};

	//! 3D space camera component.
	class Camera : public BasicCamera {
	public:

									OverrideComponent( Camera, BasicCamera )

									//! Constructs Camera instance.
									Camera( void )
										: m_direction( 0, 0, 1 ), m_up( 0, 1, 0 ), m_right( 1, 0, 0 ), m_fov( 60.0f ), m_near( 0.01f ), m_far( 100.0f ), m_aspect( 1.0f ) {}

		//! Moves camera in a direction of view by a specified distance.
		void						move( f32 amount );

		//! Moves camera to a side of a right vector.
		void						strafe( f32 amount );

		//! Rotates camera view around the side vector.
		void						pitch( f32 amount );

		//! Rotates camera view around the up vector.
		void						yaw( f32 amount );

		//! Sets camera aspect ratio.
		void						setAspect( f32 value );

		//! Returns a view matrix composed from a camera vectors.
		virtual Matrix4				view( void ) const;

		//! Returns a projection matrix with a specified aspect.
		virtual Matrix4				proj( void ) const;

	private:

		Vec3						m_position;		//!< Camera position.
		Vec3						m_direction;	//!< Camera view direction.
		Vec3						m_up;			//!< Camera up vector.
		Vec3						m_right;		//!< Camera right side vector.
		f32							m_fov;			//!< Camera field of view angle.
		f32							m_near;			//!< Camera Z near.
		f32							m_far;			//!< Camera Z far.
		f32							m_aspect;		//!< Camera aspect ratio.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Camera_H__    */