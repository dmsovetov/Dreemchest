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

#ifndef __DC_Composer_RotationTool_H__
#define __DC_Composer_RotationTool_H__

#include "Gizmo.h"

DC_BEGIN_COMPOSER

	//! Rotation tool component.
	class RotationTool: public Ecs::Component<RotationTool> {
	public:

		//! Transformation axes.
		enum Axis {
			  X					//!< Rotate around the X axis.
			, Y					//!< Rotate around the Y axis.
			, Z					//!< Rotate around the Z axis.
			, Arcball			//!< Use arcball for a free rotation.
			, Screen			//!< Rotate in screen soace.
			, Null				//!< Transform nothing by this gizmo.
		};


								//! Construct RotationTool instance.
								RotationTool( f32 radius = 0.175f, f32 screenSpaceRadiusScale = 1.2f, f32 width = 5.0f );

		//! Returns the radius of a gizmo.
		f32						radius( void ) const;

		//! Returns the scale factor of a screen space rotation indicator.
		f32						screenSpaceRadius( void ) const;

		//! Returns the selector width.
		f32						width( void ) const;

		//! Returns true if this gizmo is locked.
		bool					isLocked( void ) const;

		//! Returns gizmo data.
		const Gizmo&			gizmo( void ) const;
		Gizmo&					gizmo( void );

	private:

		f32						m_radius;					//!< The gizmo radius.
		f32						m_width;					//!< The axis selector width.
		f32						m_screenSpaceRadiusScale;	//!< The screen space rotation indicator scale factor.
		Gizmo					m_gizmo;					//!< Actual gizmo state.
	};

	//! Rotates transform by a rotation tool.
	class RotationToolSystem : public Scene::GenericTouchSystem<RotationToolSystem, RotationTool, Scene::Transform> {
	public:

									//! Constructs RotationToolSystem instance
									RotationToolSystem( Scene::ViewportWPtr viewport );

	protected:

		//! Handles mouse moved event and transforms object by active tool.
		virtual void				touchMovedEvent( Scene::Viewport::TouchMoved& e, Ecs::Entity& entity, RotationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Handles mouse pressed event and locks the selected tool.
		virtual void				touchBeganEvent( Scene::Viewport::TouchBegan& e, Ecs::Entity& entity, RotationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Handles mouse pressed event and unlocks all locked tools.
		virtual void				touchEndedEvent( Scene::Viewport::TouchEnded& e, Ecs::Entity& entity, RotationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Maps the view ray to a transformation axis.
		u8							mapRayToAxis( RotationTool& tool, Scene::Transform& transform, f32 scale, const Ray& ray, Vec2& axis ) const;
	};

#if DEV_DEPRECATED_SCENE_RENDERER
	//! Renders active rotation tools.
	class RotationToolPass : public Scene::RenderPass<RotationTool> {
	public:

						//! Constructs RotationToolPass instance.
						RotationToolPass( Ecs::EcsWPtr ecs )
							: RenderPass( ecs, "RotationToolPass" ) {}

	protected:

		//! Renders the mesh of a single translation tool.
		virtual void	render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const RotationTool& tool, const Scene::Transform& transform ) DC_DECL_OVERRIDE;
	};
#else
#endif  /*  DEV_DEPRECATED_SCENE_RENDERER   */

DC_END_COMPOSER

#endif	/*	!__DC_Composer_RotationTool_H__	*/