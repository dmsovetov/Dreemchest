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
#include "../Editors/VisualEditor.h"

DC_BEGIN_COMPOSER

	//! Translation tool component.
	class TranslationTool : public Ecs::Component<TranslationTool> {
	public:

							//! Construct TranslationTool instance.
							TranslationTool( f32 orthRadius = 0.05f, f32 scalingFactor = 0.2f, f32 selectorScale = 0.35f );

		//! Returns the scaling factor of a gizmo.
		f32					scalingFactor( void ) const;

		//! Returns the orth radius.
		f32					orthRadius( void ) const;

		//! Returns the selector bounding box.
		Bounds				selectorBoundingBox( Gizmo::Transform transform ) const;

		//! Returns true if this gizmo is active (has a highlighted transform indicator).
		bool				isActive( void ) const;

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
	class TranslationToolSystem : public Ecs::GenericEntitySystem<TranslationTool, Scene::Transform> {
	public:

									//! Constructs TranslationToolSystem instance
									TranslationToolSystem( Editors::CursorWPtr cursor, Scene::SpectatorCameraWPtr camera );

		//! Processes the single translation tool.
		virtual void				process( u32 currentTime, f32 dt, Ecs::Entity& entity, TranslationTool& tool, Scene::Transform& transform ) DC_DECL_OVERRIDE;

	protected:

		//! Handles mouse pressed event.
		void						handleMousePressed( const Editors::Cursor::Pressed& e );

		//! Handles mouse released event.
		void						handleMouseReleased( const Editors::Cursor::Released& e );

	private:

		//! Returns the selection from a mouse ray.
		Gizmo::Transform			findTransformByRay( TranslationTool& tool, f32 scale, const Ray& ray ) const;

		//! Selects the best matching plane from a ray.
		Plane						findBestPlane( Gizmo::Transform transform, const Vec3& position, const Ray& ray ) const;

	private:

		Editors::CursorWPtr			m_cursor;	//!< Cursor binding to use.
		Scene::SpectatorCameraWPtr	m_camera;	//!< Camera instance used for ray casting.
	};

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

DC_END_COMPOSER

#endif	/*	!__DC_Composer_System_TranslationTool_H__	*/