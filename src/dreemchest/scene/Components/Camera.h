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

	//! Scene rendering view.
	class View : public RefCounted {
	public:

		virtual						~View( void ) {}

		//! Returns the view width.
		virtual u32					width( void ) const { return 0; }

		//! Returns the view height.
		virtual u32					height( void ) const { return 0; }

		//! Begins rendering to this view.
		virtual void				begin( void ) const {}

		//! Ends rendering to this view.
		virtual void				end( void ) const {}
	};

	//! WindowView is used for rendering the scene to window.
	class WindowView : public View {
	public:

		//! Returns the window width.
		virtual u32					width( void ) const;

		//! Returns the window height.
		virtual u32					height( void ) const;

		//! Creates the WindowView instance.
		static ViewPtr				create( const WindowWPtr& window );

	private:

									//! Constructs the WindowView instance.
									WindowView( const WindowWPtr& window );

	private:

		WindowWPtr					m_window;	//!< The output window.
	};

	//! Camera component.
	class Camera : public SceneComponent {
	public:

		//! Available camera projections.
		enum Projection {
			Perspective = 0,
			Ortho,
			OrthoCenter
		};

									OverrideComponent( Camera, SceneComponent )

									//! Constructs Camera instance.
									Camera( Projection projection = Perspective, const ViewPtr& view = ViewPtr(), const Rgba& clearColor = Rgba(), const Rect& ndc = Rect( 0.0f, 0.0f, 1.0f, 1.0f ) )
										: m_projection( projection ), m_ndc( ndc ), m_view( view ), m_clearColor( clearColor ), m_fov( 60.0f ), m_near( 0.01f ), m_far( 1000.0f ) {}


		//! Sets the clear color.
		void						setClearColor( const Rgba& value );

		//! Returns the clear color.
		const Rgba&					clearColor( void ) const;

		//! Returns camera field of view.
		f32							fov( void ) const;

		//! Sets camera field of view.
		void						setFov( f32 value );

		//! Returns the Z-near value.
		f32							near( void ) const;

		//! Sets the Z-near value.
		void						setNear( f32 value ) const;

		//! Returns the Z-far value.
		f32							far( void ) const;

		//! Sets the Z-far value.
		void						setFar( f32 value ) const;

		//! Sets the normalized device coordinates to render frame to.
		void						setNdc( const Rect& value );

		//! Returns the normalized device coordinates to render frame to.
		const Rect&					ndc( void ) const;

		//! Calculates the output viewport coordinates.
		Rect						viewport( void ) const;

		//! Sets the camera render view.
		void						setView( const ViewPtr& value );

		//! Returns the camera render view.
		const ViewPtr&				view( void ) const;

		//! Calculates the projection matrix.
		Matrix4						calculateProjectionMatrix( void ) const;

	private:

		Projection					m_projection;	//!< Camera projection.
		Rect						m_ndc;			//!< Output normalized device coordinates.
		ViewPtr						m_view;			//!< Render view.
		Rgba						m_clearColor;	//!< The clear color.
		f32							m_fov;			//!< Camera field of view.
		f32							m_near;			//!< Z-near value.
		f32							m_far;			//!< Z-far value.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Camera_H__    */