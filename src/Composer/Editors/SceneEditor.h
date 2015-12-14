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

#ifndef __DC_Composer_SceneEditor_H__
#define __DC_Composer_SceneEditor_H__

#include "VisualEditor.h"

DC_BEGIN_COMPOSER

namespace Editors {

	//! Scene editor.
	class SceneEditor : public VisualEditor {
	public:

	protected:

		//! Performs the scene editor initialization.
		virtual bool					initialize( const Asset& asset, Ui::IDocumentWPtr document ) DC_DECL_OVERRIDE;

		//! Renders the scene.
		virtual void					render( f32 dt ) DC_DECL_OVERRIDE;

		//! Handles the mouse press event.
		virtual void					handleMousePress( s32 x, s32 y, u8 button ) DC_DECL_OVERRIDE;

		//! Handles the mouse release event.
		virtual void					handleMouseRelease( s32 x, s32 y, u8 button ) DC_DECL_OVERRIDE;

		//! Handles the mouse move event.
		virtual void					handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, u8 buttons ) DC_DECL_OVERRIDE;

		//! Adds new camera instance to the scene.
		Scene::SceneObjectPtr			addCamera( Scene::RenderTargetPtr renderTarget );

	private:

		Scene::ScenePtr					m_scene;			//!< Actual scene.
		Scene::RenderingContextPtr		m_renderingContext;	//!< Scene rendering context.
		Scene::SceneObjectPtr			m_camera;			//!< Main editor camera.
		Scene::Vec3BindingPtr			m_cursorMovement;	//!< Cursor to Vec3 binding.

		Scene::AssetBundlePtr			m_assets;			//!< Assets bundle.
		Scene::SceneObjectPtr			m_chunk;			//!< Loaded terrain.
	};

	//! FrameTarget is used for rendering the scene to rendering frame.
	class FrameTarget : public Scene::RenderTarget {
	public:

		//! Returns the frame width.
		virtual u32						width( void ) const { return m_frame->width(); }

		//! Returns the frame height.
		virtual u32						height( void ) const { return m_frame->height(); }

		//! Creates the WindowView instance.
		static Scene::RenderTargetPtr	create( const Ui::IRenderingFrameWPtr& frame ) { return new FrameTarget( frame ); }

	private:

										//! Constructs the WindowTarget instance.
										FrameTarget( const Ui::IRenderingFrameWPtr& frame )
											: m_frame( frame ) {}

	private:

		Ui::IRenderingFrameWPtr			m_frame;	//!< The output frame.
	};

} // namespace Editors

DC_END_COMPOSER

#endif	/*	!__DC_Composer_SceneEditor_H__	*/