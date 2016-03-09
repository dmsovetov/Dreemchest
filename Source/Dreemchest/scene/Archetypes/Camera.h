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

#ifndef __DC_Scene_Archetypes_Camera_H__
#define __DC_Scene_Archetypes_Camera_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! The spectator camera archetype.
	class SpectatorCamera : public Ecs::Archetype<SpectatorCamera> {
	public:

		//! Enables or disables camera rotation.
		void						setRotationEnabled( bool value );

		//! Returns true if camera rotation is enabled.
		bool						isRotationEnabled( void ) const;

		//! Enables or disables camera movement.
		void						setMovementEnabled( bool value );

		//! Returns true if camera movement is enabled.
		bool						isMovementEnabled( void ) const;

		//! Returns the camera view vector.
		Vec3						view( void ) const;

		//! Returns the camera side vector.
		Vec3						side( void ) const;

		//! Returns the camera up vector.
		Vec3						up( void ) const;

		//! Returns the camera position.
		const Vec3&					position( void ) const;

		//! Sets the camera position.
		void						setPosition( const Vec3& value );

		//! Returns the clear color.
		const Rgba&					clearColor( void ) const;

		//! Sets the clear color.
		void						setClearColor( const Rgba& value );

		//! Construct the view ray from screen space coordinates.
		Ray							toWorldSpace( u32 x, u32 y ) const;

		//! Projects the point in world space to a screen space.
		bool						toScreenSpace( const Vec3& world, Vec3& screen ) const;

		//! Creates the spectator camera instance.
		static SpectatorCameraPtr	create( const RenderTargetPtr& renderTarget, const Vec3BindingPtr& cursor = Vec3BindingPtr() );

	protected:

		//! Constructs the SpectatorCamera instance.
		virtual void				construct( void ) DC_DECL_OVERRIDE;
	};
	
	//! Orthogonal 2D camera.
	class Camera2D : public Ecs::Archetype<Camera2D> {
	public:

		//! Returns current zoom value.
		f32							zoom( void ) const;

		//! Sets the zoom value.
		void						setZoom( f32 value );

		//! Returns panning.
		Vec2						pan( void ) const;

		//! Sets panning.
		void						setPan( const Vec2& value );

		//! Creates the 2D camera instance.
		static Camera2DPtr			create( const RenderTargetPtr& renderTarget );

	protected:

		//! Constructs the Camera2D instance.
		virtual void				construct( void ) DC_DECL_OVERRIDE;
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Archetypes_Camera_H__    */