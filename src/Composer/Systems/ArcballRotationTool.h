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
	class ArcballRotationToolSystem : public Scene::GenericTouchSystem<ArcballRotationToolSystem, ArcballRotationTool, Scene::Transform> {
	public:

									//! Constructs ArcballRotationToolSystem instance
									ArcballRotationToolSystem( Scene::ViewportWPtr viewport );

	protected:

		//! Handles mouse moved event and rotates active arcball.
		virtual void				touchMovedEvent( Scene::Viewport::TouchMoved& e, Ecs::Entity& entity, ArcballRotationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Handles mouse pressed event and locks the selected arcball.
		virtual void				touchBeganEvent( Scene::Viewport::TouchBegan& e, Ecs::Entity& entity, ArcballRotationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Handles mouse pressed event and unlocks all locked arcballs.
		virtual void				touchEndedEvent( Scene::Viewport::TouchEnded& e, Ecs::Entity& entity, ArcballRotationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Maps the screen space coordinates to a unit vector on arcball.
		bool						mapToVector( const ArcballRotationTool& arcball, const Scene::Transform& transform, const Vec2& cursor, Vec3& direction ) const;
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