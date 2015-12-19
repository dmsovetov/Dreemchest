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

#ifndef __DC_Composer_Gizmo_H__
#define __DC_Composer_Gizmo_H__

#include "../Composer.h"

DC_BEGIN_COMPOSER

	//! Translation gizmo component.
	class TranslationGizmo : public Ecs::Component<TranslationGizmo> {
	public:

						//! Construct TranslationGizmo instance.
						TranslationGizmo( f32 orthRadius = 0.05f, f32 scalingFactor = 0.2f );

		//! Returns the scaling factor of a gizmo.
		f32				scalingFactor( void ) const;

		//! Returns the orth radius.
		f32				orthRadius( void ) const;

	private:

		f32				m_scalingFactor;	//!< The gizmo scaling factor.
		f32				m_orthRadius;		//!< Orth radius.
	};

	//! Gizmo rendering component.
	class RenderGizmo : public Ecs::Component<RenderGizmo> {
	public:
	};

	//! Renders active translation gizmos.
	class TranslationGizmoPass : public Scene::RenderPass<TranslationGizmo> {
	public:

						//! Constructs TranslationGizmoPass instance.
						TranslationGizmoPass( Ecs::EcsWPtr ecs )
							: RenderPass( ecs, "TranslationGizmoPass" ) {}

	protected:

		//! Renders the mesh of a single translation gizmo.
		virtual void	render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const TranslationGizmo& gizmo, const Scene::Transform& transform ) DC_DECL_OVERRIDE;
	};

	//! Scene editor gizmo renderer type.
	typedef Scene::SinglePassRenderingSystem<RenderGizmo, TranslationGizmoPass> GizmoRenderingSystem;

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Gizmo_H__	*/