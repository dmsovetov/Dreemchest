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

#ifndef __DC_Composer_Systems_ArcballRotationTool_H__
#define __DC_Composer_Systems_ArcballRotationTool_H__

#include "../Composer.h"
#include "../Editors/VisualEditor.h"

#define DEV_SCREENSPACE_ARCBALL	(1)

DC_BEGIN_COMPOSER

	//! Arcball rotation tool component.
	class ArcballRotationTool : public Ecs::Component<ArcballRotationTool> {
	public:

									//! Constructs ArcballRotationTool instance.
									ArcballRotationTool( f32 radius = 0.175f );

		//! Returns tool radius.
		f32							radius( void ) const;

		//! Returns true if this arcball is locked.
		bool						isLocked( void ) const;

		//! Unlocks the arcball tool.
		void						unlock( void );

	#if DEV_SCREENSPACE_ARCBALL
		//! Locks the arcball tool.
		void						lock( s32 x, s32 y );

		//! Returns last used X cursor coordinate.
		s32							lastX( void ) const;

		//! Returns last used Y cursor coordinate.
		s32							lastY( void ) const;

		//! Sets last used cursor coordinates.
		void						setLastCursor( s32 x, s32 y );
	#else
		//! Locks the arcball tool.
		void						lock( const Quat& rotation, const Vec3& direction );

		//! Returns the initial direction.
		const Vec3&					initialDirection( void ) const;

		//! Returns the initial rotation.
		const Quat&					initialRotation( void ) const;
	#endif	/*	DEV_SCREENSPACE_ARCBALL	*/

	private:

		f32							m_radius;			//!< Tool radius.
	#if DEV_SCREENSPACE_ARCBALL
		s32							m_lastX;			//!< Last used X cursor coordinate.
		s32							m_lastY;			//!< Last used Y cursor coordinate.
	#else
		Quat						m_initialRotation;	//!< Initial transform rotation.
		Vec3						m_initialDirection;	//!< Initial vector.
	#endif	/*	DEV_SCREENSPACE_ARCBALL	*/
		bool						m_isLocked;			//!< Marks this arcball as locked.
	};

	//! Rotates the transforms by an arcball rotation tool.
	class ArcballRotationToolSystem : public Ecs::GenericEntitySystem<ArcballRotationTool, Scene::Transform> {
	public:

									//! Constructs ArcballRotationToolSystem instance
									ArcballRotationToolSystem( Editors::CursorWPtr cursor, Scene::SpectatorCameraWPtr camera );

		//! Processes the single arcball rotation tool.
		virtual void				process( u32 currentTime, f32 dt, Ecs::Entity& entity, ArcballRotationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

	protected:

		//! Handles mouse pressed event.
		void						handleMousePressed( const Editors::Cursor::Pressed& e );

		//! Handles mouse released event.
		void						handleMouseReleased( const Editors::Cursor::Released& e );

		//! Finds the intersection point of a view ray & arcball.
		bool						findIntersectionPoint( ArcballRotationTool& tool, Scene::Transform& transform, Vec3& point ) const;

	private:

		Editors::CursorWPtr			m_cursor;	//!< Cursor binding to use.
		Scene::SpectatorCameraWPtr	m_camera;	//!< Camera instance used for ray casting.
	};

	//! Renders the arcball rotation tool indicator.
	class ArcballRotationToolPass : public Scene::RenderPass<ArcballRotationTool> {
	public:

						//! Constructs ArcballRotationToolPass instance.
						ArcballRotationToolPass( Ecs::EcsWPtr ecs )
							: RenderPass( ecs, "ArcballRotationToolPass" ) {}

	protected:

		//! Renders arcball tool indicator.
		virtual void	render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const ArcballRotationTool& tool, const Scene::Transform& transform ) DC_DECL_OVERRIDE;
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Systems_ArcballRotationTool_H__	*/