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

#define qDeclarePtrs( T )							\
		    typedef QSharedPointer<class T>	T##Ptr;	\
	        typedef QWeakPointer<class T>	T##WPtr;

#define DC_BEGIN_COMPOSER	/*namespace Composer {*/
#define DC_END_COMPOSER		/*}*/

DC_BEGIN_COMPOSER

	// Declare Qt metatypes
	Q_DECLARE_METATYPE( Scene::AssetWPtr )
	Q_DECLARE_METATYPE( Scene::ImageWPtr )
	Q_DECLARE_METATYPE( Scene::RenderingMode )
	Q_DECLARE_METATYPE( Scene::Material::Model )

	namespace Ui {

        qDeclarePtrs( AssetTree )

		dcDeclarePtrs( IMainWindow )
		dcDeclarePtrs( IAction )
		dcDeclarePtrs( IMenu )
		dcDeclarePtrs( IToolBar )
		dcDeclarePtrs( IRenderingFrame )
		dcDeclarePtrs( IRenderingFrameDelegate )
		dcDeclarePtrs( IDocument )
		dcDeclarePtrs( ISceneTree )
		dcDeclarePtrs( ObjectInspector )

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

		//! This marks terrain chunks.
		class TerrainChunk : public Ecs::Component<TerrainChunk> {
		public:

									//! Constructs TerrainChunk instance.
									TerrainChunk( Scene::TerrainPtr chunk = Scene::TerrainPtr(), u32 x = 0, u32 z = 0 )
										: m_terrain( chunk ), m_x( x ), m_z( z ) {}

			//! Returns terrain instance.
			Scene::TerrainWPtr		terrain( void ) const { return m_terrain; }

			//! Returns chunk X.
			u32						x( void ) const { return m_x; } // column

			//! Returns chunk Y.
			u32						z( void ) const { return m_z; } // row

		private:

			Scene::TerrainPtr		m_terrain;	//!< Terrain asset.
			u32						m_x;		//!< Chunk X coordinate.
			u32						m_z;		//!< Chunk Z coordinate.
		};

		//! This component marks scene objects as internally used by scene editor.
		class SceneEditorInternal : public Ecs::Component<SceneEditorInternal> {
		public:

			//! Scene object editor flags.
			enum Flags {
				  Selected		= BIT( 0 )	//!< The scene object is now selected.
				, Highlighted	= BIT( 1 )	//!< The scene object is highlighted.
				, Private		= BIT( 2 )	//!< Private scene objects should not be displayed in scene hierarchy window.
				, Freezed		= BIT( 3 )	//!< Freezed scene objects can't be selected by mouse.
			};

									//! Constructs SceneEditorInternal instance.
									SceneEditorInternal( Scene::SceneObjectWPtr parent = Scene::SceneObjectWPtr(), u8 flags = 0 );

			//! Returns parent scene object.
			Scene::SceneObjectWPtr	parent( void ) const;

			//! Returns true if this scene object is private.
			bool					isPrivate( void ) const;

			//! Returns true if this scene object is highlighted.
			bool					isHighlighted( void ) const;

			//! Marks this scene object as highlighted.
			void					setHighlighted( bool value );

			//! Returns true if this scene object is selected.
			bool					isSelected( void ) const;

			//! Marks this scene object as selected.
			void					setSelected( bool value );

		private:

			FlagSet8				m_flags;	//!< Bitset mask.
			Scene::SceneObjectWPtr	m_parent;	//!< Parent scene object.
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
	qDeclarePtrs( AssetsModel )
    qDeclarePtrs( FilteredAssetsModel )
	qDeclarePtrs( PropertyModel )
    qDeclarePtrs( SceneModel )

	//! Factory method used for assets model creation.
	extern AssetsModelPtr createAssetsModel( void );

	//! Factory method used for scene model creation.
	extern SceneModelPtr createSceneModel( Scene::AssetBundleWPtr assets, Scene::SceneWPtr scene );

	//! Factory method used for material model creation.
	extern PropertyModelPtr createMaterialModel( Scene::MaterialWPtr material );

	//! Container type to store file info.
	typedef Array<FileInfoPtr> FileInfoArray;

	//! Base interface class.
	class IInterface : public RefCountedEventEmitter {
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
	class Composer : public RefCountedEventEmitter {
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
		void					openProject( const String& path );

		//! Creates new project at path.
		void					createProject( const String& path );

		//! Returns opened project pointer.
		Project::ProjectWPtr	project( void ) const;

		//! Returns main window pointer.
		Ui::IMainWindowWPtr		window( void ) const;

		//! Extracts an asset set from MIME data.
		Scene::AssetSet			assetsFromMime( IMimeDataWPtr mime ) const;

		//! Extracts a single asset from MIME data.
		Scene::AssetPtr			assetFromMime( IMimeDataWPtr mime ) const;

		//! Creates the Composer instance.
		static ComposerPtr		create( void );

		//! Returns the Composer instance.
		static ComposerWPtr		instance( void );

	private:

								//! Constructs Composer instance.
								Composer( Ui::IMainWindowPtr mainWindow );

		//! Creates a new project.
		void					menuCreateProject( Ui::IActionWPtr action );

		//! Opens an existing project.
		void					menuOpenProject( Ui::IActionWPtr action );

		//! Saves current project.
		void					menuSaveProject( Ui::IActionWPtr action );

		//! Undo the last action.
		void					menuUndo( Ui::IActionWPtr action );

		//! Redo the last action.
		void					menuRedo( Ui::IActionWPtr action );

		//! Performs the composer initialization.
		bool					initialize( void );

	private:

		static ComposerWPtr		s_instance;				//!< Shared composer instance.
		Ui::IMainWindowPtr		m_mainWindow;			//!< Main composer window.
		Ui::IMenuWPtr			m_menues[TotalMenues];	//!< Default menues.
		Project::ProjectPtr		m_project;				//!< Active project.
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_H__	*/