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

#ifndef __DC_Composer_System_TranslationTool_H__
#define __DC_Composer_System_TranslationTool_H__

#include "Gizmo.h"

DC_BEGIN_COMPOSER

	//! Translation tool component.
	class TranslationTool : public Ecs::Component<TranslationTool> {
	public:

		//! Available transformation axes.
		enum Axis {
			  X				//!< Translate along the X axis.
			, Y				//!< Translate along th Y axis.
			, Z				//!< Translate along th Z axis.
			, XY			//!< Translate on XY plane.
			, YZ			//!< Translate on YZ plane.
			, XZ			//!< Translate on XZ plane.
			, Null			//!< No transformation axis selected.
		};

							//! Construct TranslationTool instance.
							TranslationTool( f32 orthRadius = 0.05f, f32 scalingFactor = 0.2f, f32 selectorScale = 0.35f );

		//! Returns the scaling factor of a gizmo.
		f32					scalingFactor( void ) const;

		//! Returns the orth radius.
		f32					orthRadius( void ) const;

		//! Returns the selector bounding box.
		Bounds				selectorBoundingBox( u8 idx ) const;

		//! Returns true if this gizmo is locked.
		bool				isLocked( void ) const;

		//! Returns gizmo data.
		const Gizmo&		gizmo( void ) const;
		Gizmo&				gizmo( void );

	private:

		f32					m_scalingFactor;	//!< The gizmo scaling factor.
		f32					m_selectorScale;	//!< The selector plane size.
		f32					m_orthRadius;		//!< Orth radius.
		Gizmo				m_gizmo;			//!< Actual gizmo data.
	};

	//! Translates the transforms by a translation tool.
	class TranslationToolSystem : public Scene::GenericTouchSystem<TranslationToolSystem, TranslationTool, Scene::Transform> {
	public:

									//! Constructs TranslationToolSystem instance
									TranslationToolSystem( Scene::ViewportWPtr viewport );

	private:

		//! Handles mouse moved event and transforms object by active tool.
		virtual void				touchMovedEvent( Scene::Viewport::TouchMoved& e, Ecs::Entity& entity, TranslationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Handles mouse pressed event and locks the selected tool.
		virtual void				touchBeganEvent( Scene::Viewport::TouchBegan& e, Ecs::Entity& entity, TranslationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Handles mouse pressed event and unlocks all locked tools.
		virtual void				touchEndedEvent( Scene::Viewport::TouchEnded& e, Ecs::Entity& entity, TranslationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Maps the view ray to a transformation axis.
		u8							mapRayToAxis( TranslationTool& tool, f32 scale, const Ray& ray ) const;

		//! Maps the view ray to an intersection point.
		Vec3						mapRayToPoint( TranslationTool& tool, const Scene::Transform& transform, f32 scale, const Ray& ray );
	};

#if DEV_DEPRECATED_SCENE_RENDERER
	//! Renders active translation tools.
	class TranslationToolPass : public Scene::RenderPass<TranslationTool> {
	public:

						//! Constructs TranslationToolPass instance.
						TranslationToolPass( Ecs::EcsWPtr ecs )
							: RenderPass( ecs, "TranslationToolPass" ) {}

	protected:

		//! Renders the mesh of a single translation tool.
		virtual void	render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const TranslationTool& tool, const Scene::Transform& transform ) DC_DECL_OVERRIDE;
	};
#else
#endif  /*  DEV_DEPRECATED_SCENE_RENDERER   */

DC_END_COMPOSER

#endif	/*	!__DC_Composer_System_TranslationTool_H__	*/