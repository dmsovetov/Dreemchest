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

        Q_OBJECT

	public:

                                        //! Constructs SceneEditor instance.
                                        SceneEditor( void );

	protected:

		//! Scene editing tool types.
		enum ActiveTool {
			  NoTool		        //!< All tools are disabled.
			, ToolTranslate	        //!< Translation tool is now active.
			, ToolRotate		    //!< Rotation tool is now active.
			, ToolScale		        //!< Scaling tool is now active.
            , ToolRaiseTerrain      //!< Terrain raising tool is now active.
            , ToolLowerTerrain      //!< Terrain lowering tool is now active.
            , ToolFlattenTerrain    //!< Terrain flattening tool is now active.
            , ToolLevelTerrain      //!< Terrain leveling tool is now active.
            , ToolSmoothTerrain     //!< Terrain smoothing tool is now active.
		};


		//! Performs the scene editor initialization.
		virtual bool					initialize( ProjectQPtr project, const Scene::AssetPtr& asset, Ui::DocumentQPtr document ) DC_DECL_OVERRIDE;

		//! Renders the scene.
		virtual void					render( f32 dt ) DC_DECL_OVERRIDE;

		//! Handles the mouse press event.
		virtual void					handleMousePress( s32 x, s32 y, const Ui::MouseButtons& button ) DC_DECL_OVERRIDE;

		//! Handles the mouse release event.
		virtual void					handleMouseRelease( s32 x, s32 y, const Ui::MouseButtons& button ) DC_DECL_OVERRIDE;

		//! Handles the mouse move event.
		virtual void					handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, const Ui::MouseButtons& buttons ) DC_DECL_OVERRIDE;

		//! Handles the mouse wheel event.
		virtual void					handleMouseWheel( s32 delta ) DC_DECL_OVERRIDE;

		//! Handles the drag enter event.
		virtual bool					handleDragEnter( MimeDataQPtr mime ) DC_DECL_OVERRIDE;

		//! Handles the drag move event.
		virtual void					handleDragMove( MimeDataQPtr mime, s32 x, s32 y ) DC_DECL_OVERRIDE;

		//! Handles the drop event.
		virtual void					handleDrop( MimeDataQPtr mime, s32 x, s32 y ) DC_DECL_OVERRIDE;

		//! Scene editor has entered the foreground.
		virtual void					notifyEnterForeground( Ui::MainWindowQPtr window ) DC_DECL_OVERRIDE;

		//! Scene editor has entered the background.
		virtual void					notifyEnterBackground( Ui::MainWindowQPtr window ) DC_DECL_OVERRIDE;

		//! Disables all transform tools.
		void							menuTransformSelect( Ui::ActionQPtr action );

		//! Activates translation transform tool.
		void							menuTransformTranslate( Ui::ActionQPtr action );

		//! Activates rotation transform tool.
		void							menuTransformRotate( Ui::ActionQPtr action );

		//! Activates scaling transform tool.
		void							menuTransformScale( Ui::ActionQPtr action );

		//! Activates terrain raising tool.
		void							menuTerrainRaise( Ui::ActionQPtr action );

		//! Activates terrain lowering tool.
		void							menuTerrainLower( Ui::ActionQPtr action );

		//! Activates terrain flattening tool.
		void							menuTerrainFlatten( Ui::ActionQPtr action );

		//! Activates terrain leveling tool.
		void							menuTerrainLevel( Ui::ActionQPtr action );

		//! Activates terrain smoothing tool.
		void							menuTerrainSmooth( Ui::ActionQPtr action );

		//! Returns the nearest scene object underneath the mouse cursor.
		Scene::SceneObjectWPtr			findSceneObjectAtPoint( s32 x, s32 y ) const;

		//! Highlights the scene object.
		void							highlightSceneObject( Scene::SceneObjectWPtr sceneObject );

		//! Selects the scene object.
		void							selectSceneObject( Scene::SceneObjectWPtr sceneObject );

		//! Sets active transform tool.
		void							setTool( ActiveTool tool );

		//! Binds transformation tool gizmo to a scene object.
		void							bindTransformGizmo( Scene::SceneObjectWPtr sceneObject, ActiveTool tool ) const;

    private Q_SLOTS:

        //! Handles the scene object double click in scene hierarchy widget.
		void							navigateToObject( Scene::SceneObjectWPtr sceneObject );


	private:

		Scene::ScenePtr					m_scene;					//!< Actual scene.
		AutoPtr<SceneModel>				m_sceneModel;				//!< Scene model used by user interface.
		Scene::RenderingContextPtr		m_renderingContext;			//!< Scene rendering context.
		Scene::SpectatorCameraPtr		m_camera;					//!< Main editor camera.
		Scene::Vec3BindingPtr			m_cursorMovement;			//!< Cursor to Vec3 binding.

		Ui::ToolBarQPtr					m_tools;					//!< Scene editor toolbar.
		ActiveTool				        m_activeTool;		        //!< Active editing tool.
        Scene::SceneObjectPtr           m_terrainTool;              //!< Terrain tool scene object.

		Scene::SceneObjectWPtr			m_activeSceneObject;		//!< The scene object underneath the mouse cursor is stored here.
		Scene::SceneObjectWPtr			m_selectedSceneObject;		//!< The selected scene object.
	};

	//! FrameTarget is used for rendering the scene to rendering frame.
	class FrameTarget : public Scene::RenderTarget {
	public:

		//! Returns the frame width.
		virtual u32						width( void ) const { return m_frame->width(); }

		//! Returns the frame height.
		virtual u32						height( void ) const { return m_frame->height(); }

		//! Creates the WindowView instance.
		static Scene::RenderTargetPtr	create( const Ui::RenderingFrameQPtr& frame ) { return new FrameTarget( frame ); }

	private:

										//! Constructs the WindowTarget instance.
										FrameTarget( const Ui::RenderingFrameQPtr& frame )
											: m_frame( frame ) {}

	private:

		Ui::RenderingFrameQPtr			m_frame;	//!< The output frame.
	};

} // namespace Editors

DC_END_COMPOSER

#endif	/*	!__DC_Composer_SceneEditor_H__	*/