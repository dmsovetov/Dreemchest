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

		//! Locks the arcball tool.
		void						lock( const Quat& rotation, const Vec3& direction );

		//! Returns the initial direction.
		const Vec3&					initialDirection( void ) const;

		//! Returns the initial rotation.
		const Quat&					initialRotation( void ) const;

	private:

		f32							m_radius;			//!< Tool radius.
		Quat						m_initialRotation;	//!< Initial transform rotation.
		Vec3						m_initialDirection;	//!< Initial vector.
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

		//! Maps the screen space coordinates to a unit vector on arcball.
		bool						mapToVector( const ArcballRotationTool& arcball, const Scene::Transform& transform, const Vec2& cursor, Vec3& direction ) const;

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