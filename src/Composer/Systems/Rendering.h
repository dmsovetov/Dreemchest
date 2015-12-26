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

#ifndef __DC_Composer_Rendering_H__
#define __DC_Composer_Rendering_H__

#include "RotationTool.h"
#include "TranslationTool.h"
#include "ArcballRotationTool.h"
#include "Terrain.h"

DC_BEGIN_COMPOSER

	//! Scene helpers rendering component.
	class RenderSceneHelpers : public Ecs::Component<RenderSceneHelpers> {
	public:
	};

	//! Renders highlight & selection indicators for scene objects.
	class SceneObjectIndicatorsPass : public Scene::RenderPass<Editors::SceneEditorInternal> {
	public:

						//! Constructs SceneObjectIndicatorsPass instance.
						SceneObjectIndicatorsPass( Ecs::EcsWPtr ecs )
							: RenderPass( ecs, "SceneObjectIndicatorsPass" ) {}

	protected:

		//! Renders scene object indicators.
		virtual void	render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const Editors::SceneEditorInternal& internal, const Scene::Transform& transform ) DC_DECL_OVERRIDE;	
	};

	//! Scene editor helpers renderer type.
	class SceneHelpersRenderer : public Scene::RenderingSystem<RenderSceneHelpers> {
	public:

								SceneHelpersRenderer( Ecs::EcsWPtr ecs )
									: RenderingSystem( ecs, "SceneHelpersRenderer" )
								{
									addPass<TranslationToolPass>();
									addPass<RotationToolPass>();
									addPass<SceneObjectIndicatorsPass>();
									addPass<TerrainToolPass>();
									addPass<ArcballRotationToolPass>();
									addPass<Scene::GridPass>();
								}
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Rendering_H__	*/