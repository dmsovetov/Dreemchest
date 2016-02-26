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

#ifndef __DC_Scene_Viewport_H__
#define __DC_Scene_Viewport_H__

#include "Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Scene viewport class is used by entity systems to handle user's input.
	class Viewport : public InjectEventEmitter<RefCounted> {
	public:

		//! This event is emitted when touch began.
		struct TouchBegan {
							TouchBegan( ViewportWPtr viewport, u8 buttons )
								: viewport( viewport ), buttons( buttons ) {}
			ViewportWPtr	viewport;
			FlagSet8		buttons;
		};

		//! This event is emitted when touch ended.
		struct TouchEnded {
							TouchEnded( ViewportWPtr viewport, u8 buttons )
								: viewport( viewport ), buttons( buttons ) {}
			ViewportWPtr	viewport;
			FlagSet8		buttons;
		};

		//! This event is emitted when touch was moved.
		struct TouchMoved {
							TouchMoved( ViewportWPtr viewport, s32 dx, s32 dy )
								: viewport( viewport ), dx( dx ), dy( dy ) {}
			ViewportWPtr	viewport;
			s32				dx;
			s32				dy;
		};

							//! Constructs Viewport instance.
							Viewport( SceneObjectWPtr camera );

		//! Returns cursor X position.
		s32					x( void ) const;

		//! Returns cursor Y position.
		s32					y( void ) const;

		//! Returns cursor position.
		Vec2				pos( void ) const;

		//! Returns camera position.
		const Vec3&			eye( void ) const;

		//! Returns cursor view ray.
		const Ray&			ray( void ) const;

		//! Returns viewport flags.
		const FlagSet8&		flags( void ) const;

		//! Begins the touch at specified coordinates.
		void				touchBegan( s32 x, s32 y );

		//! Begins the touch at specified coordinates and flags.
		void				touchBegan( s32 x, s32 y, const FlagSet8& flags );

		//! Ends the touch at specified coordinates.
		void				touchEnded( s32 x, s32 y );

		//! Ends the touch at specified coordinates and flags.
		void				touchEnded( s32 x, s32 y, const FlagSet8& flags );

		//! Moves touch to a specified coordinates and flags.
		void				touchMoved( s32 x, s32 y, const FlagSet8& flags );

		//! Sets the camera that owns this cursor.
		void				setCamera( SceneObjectWPtr value );

		//! Returns the camera that owns this cursor.
		SceneObjectWPtr		camera( void ) const;

		//! Constructs the world space ray at specified viewport coordinates.
		Ray					constructViewRay( s32 x, s32 y ) const;

	private:

		//! Updates viewport cursor position.
		void				setCursor( s32 x, s32 y );

	private:

		SceneObjectWPtr		m_camera;	//!< Parent camera instance.
		s32					m_x;		//!< Cursor X position.
		s32					m_y;		//!< Cursor Y position.
		Ray					m_ray;		//!< Cursor view ray.
		FlagSet8			m_flags;	//!< Viewport flags.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Viewport_H__    */