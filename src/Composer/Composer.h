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

#ifndef __DC_Composer_H__
#define __DC_Composer_H__

#include <Dreemchest.h>
#include <functional>

#define DEV_CUSTOM_ASSET_MODEL			(0)
#define DEV_BACKGROUND_ASSET_LOADING	(0)
#define DEV_USE_DOCK_INDICATOR			(1)

#ifdef DC_QT4_ENABLED
	#include <QtGui>
	#include <QtCore>
	#include <QtOpenGL>

	#define Q_DECL_OVERRIDE override
#elif DC_QT5_ENABLED
	#include <QtWidgets>
	#include <QtOpenGL>
#endif

#define DC_BEGIN_COMPOSER	/*namespace Composer {*/
#define DC_END_COMPOSER		/*}*/

DC_BEGIN_COMPOSER

	namespace Ui {

		dcDeclarePtrs( IMainWindow )
		dcDeclarePtrs( IAction )
		dcDeclarePtrs( IMenu )
		dcDeclarePtrs( IToolBar )
		dcDeclarePtrs( IRenderingFrame )
		dcDeclarePtrs( IRenderingFrameDelegate )
		dcDeclarePtrs( IDocument )
		dcDeclarePtrs( IAssetTree )
		dcDeclarePtrs( ISceneTree )

		//! Message status.
		enum MessageStatus {
			  MessageInfo		//!< The information message.
			, MessageWarning	//!< The warning message.
			, MessageError		//!< The error message.
		};

		//! Message box result.
		enum MessageBoxResult {
			  MessageBoxYes		//!< Yes button was clicked.
			, MessageBoxNo		//!< No button was clicked.
			, MessageBoxCancel	//!< Cancel button was clicked.
		};

		//! Mouse buttons bitset flags.
		enum MouseButtonFlags {
			  LeftMouseButton	= BIT( 0 )	//!< Left mouse button.
			, RightMouseButton	= BIT( 1 )	//!< Right mouse button.
			, MiddleMouseButton	= BIT( 2 )	//!< Middle mouse button.
		};

		//! Auto ptr type for signal delegate instances.
		typedef AutoPtr<class QSignalDelegate> QSignalDelegatePtr;

		//! Container type to store array of documents.
		typedef Array<IDocumentWPtr> DocumentsWeak;

		//! Menu action callback type.
		typedef std::function<void(IActionWPtr)> ActionCallback;

		//! Factory method used for main window creation.
		extern IMainWindowPtr createMainWindow( const String& title );

	} // namespace Ui

	namespace Editors {

		//! This component marks scene objects as internally used by scene editor.
		class SceneEditorInternal : public Ecs::Component<SceneEditorInternal> {
		public:
		};

		dcDeclarePtrs( AssetEditor )
		dcDeclarePtrs( VisualEditor )
		dcDeclarePtrs( SceneEditor )

	} // namespace Editors

	namespace Importers {

		dcDeclarePtrs( AssetImporter )

	} // namespace Importers

	namespace Project {

		dcDeclarePtrs( Project )
		dcDeclarePtrs( Assets )

	} // namespace Project

	dcDeclarePtrs( Composer )
	dcDeclarePtrs( FileSystem )
	dcDeclarePtrs( FileInfo )
	dcDeclarePtrs( IMimeData )
	dcDeclarePtrs( AssetsModel )
	dcDeclarePtrs( SceneModel )

	//! Factory method used for assets model creation.
	extern AssetsModelPtr createAssetsModel( void );

	//! Factory method used for scene model creation.
	extern SceneModelPtr createSceneModel( Scene::SceneWPtr scene );

	//! Factory method used for material inspector creation.
	extern MaterialInspectorPtr createMaterialInspector( Scene::MaterialWPtr material );

	//! Container type to store file info.
	typedef Array<FileInfoPtr> FileInfoArray;

	//! Base interface class.
	class IInterface : public event::RefCountedEventEmitter {
	public:

		virtual						~IInterface( void ) {}

		//! Returns the raw private implementation pointer.
		virtual void*				ptr( void ) const = 0;

		//! Returns the private interface.
		template<typename T>
		T*							privateInterface( void ) const { return reinterpret_cast<T*>( ptr() ); }
	};

	//! Generic class to declare interface implementations.
	template<typename TBase, typename TPrivate>
	class PrivateInterface : public TBase {
	public:

									//! Constructs the PrivateInterface instance.
									PrivateInterface( TPrivate* instance )
										: m_private( instance ) {}

		//! Returns the private interface.
		virtual void*				ptr( void ) const { return m_private.get(); }

	protected:

		AutoPtr<TPrivate>			m_private;	//!< Actual implementation instance.
	};

	//! Root composer class.
	class Composer : public event::RefCountedEventEmitter {
	public:

		//! Asset MIME type string.
		static const String kAssetMime;

		//! Available menues
		enum Menu {
			  FileMenu
			, EditMenu
			, ViewMenu
			, AssetsMenu
		
			, TotalMenues
		};

		//! Event is emitted when the project is created.
		struct ProjectCreated {
									//! Constructs ProjectCreated event.
									ProjectCreated( Project::ProjectWPtr project );

			Project::ProjectWPtr	project;	//!< Created project.
		};

		//! Event is emitted when the project was opened.
		struct ProjectOpened {
									//! Constructs ProjectOpened event.
									ProjectOpened( Project::ProjectWPtr project );

			Project::ProjectWPtr	project;	//!< Opened project.
		};

		//! Event is emitted when the project was closed.
		struct ProjectClosed {
									//! Constructs ProjectClosed event.
									ProjectClosed( Project::ProjectWPtr project );

			Project::ProjectPtr		project;	//!< Closed project.
		};

		//! Opens the existing project at path.
		void				openProject( const String& path );

		//! Creates new project at path.
		void				createProject( const String& path );

		//! Extracts an asset set from MIME data.
		Scene::AssetSet		assetsFromMime( IMimeDataWPtr mime ) const;

		//! Extracts a single asset from MIME data.
		Scene::AssetPtr		assetFromMime( IMimeDataWPtr mime ) const;

		//! Creates the Composer instance.
		static ComposerPtr	create( void );

		//! Returns the Composer instance.
		static ComposerWPtr	instance( void );

	private:

							//! Constructs Composer instance.
							Composer( Ui::IMainWindowPtr mainWindow );

		//! Creates a new project.
		void				menuCreateProject( Ui::IActionWPtr action );

		//! Opens an existing project.
		void				menuOpenProject( Ui::IActionWPtr action );

		//! Performs the composer initialization.
		bool				initialize( void );

	private:

		static ComposerWPtr	s_instance;				//!< Shared composer instance.
		Ui::IMainWindowPtr	m_mainWindow;			//!< Main composer window.
		Ui::IMenuWPtr		m_menues[TotalMenues];	//!< Default menues.
		Project::ProjectPtr	m_project;				//!< Active project.
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_H__	*/