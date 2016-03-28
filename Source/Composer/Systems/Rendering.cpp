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

#include "Rendering.h"

DC_BEGIN_COMPOSER

// -------------------------------------------------------- SceneObjectIndicatorsPass -------------------------------------------------------- //

// ** SceneObjectIndicatorsPass::render
void SceneObjectIndicatorsPass::render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const Editors::SceneEditorInternal& internal, const Scene::Transform& transform )
{
	if( internal.isPrivate() ) {
		return;
	}

	if( !internal.parent().valid() ) {
        LogError( "scene", "SceneEditorInternal has no valid scene object\n" );
		return;
	}

	Renderer::Renderer2DPtr renderer = context->renderer();

	if( internal.isSelected() ) {
		if( Scene::StaticMesh* mesh = internal.parent()->has<Scene::StaticMesh>() ) {
			renderer->wireBox( mesh->worldSpaceBounds(), Rgba( 1.0f, 1.0f, 0.0f ) );
		}	
	}
	else if( internal.isHighlighted() ) {
		if( Scene::StaticMesh* mesh = internal.parent()->has<Scene::StaticMesh>() ) {
			renderer->wireBox( mesh->worldSpaceBounds(), Rgba( 0.0f, 1.0f, 0.0f ) );
		}
	}
}

DC_END_COMPOSER