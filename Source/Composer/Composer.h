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

#define DEV_BACKGROUND_ASSET_LOADING	    (0)
#define DEV_USE_DOCK_INDICATOR			    (1)
#define DEV_DISABLE_SYSTEM_ICON_PROVIDER    (1)

#ifdef DC_QT4_ENABLED
	#include <QtGui>
	#include <QtCore>
	#include <QtOpenGL>

	#define Q_DECL_OVERRIDE override
#elif DC_QT5_ENABLED
	#include <QtWidgets>
	#include <QtOpenGL>
#endif

#define qCheckParent( object )          \
            if( (object) == NULL )  {   \
                qWarning() << "Object has no parent" << __FUNCTION__ << __FILE__ << __LINE__;   \
            }

#define qDeclarePtrs( T )							\
		    typedef QSharedPointer<class T>	T##Ptr;	\
	        typedef QWeakPointer<class T>	T##WPtr;

#define qDeclarePtr( T )		        \
		    typedef class T* T##QPtr;

#define qComposer    qobject_cast<Composer*>( qApp )
#define qMainWindow  qComposer->window()

#define DC_BEGIN_COMPOSER	/*namespace Composer {*/
#define DC_END_COMPOSER		/*}*/

DC_BEGIN_COMPOSER

    DC_USE_DREEMCHEST

    // Declare the log tag in global namespace
    DREEMCHEST_LOGGER_TAG( Composer )

	// Declare Qt metatypes
    Q_DECLARE_METATYPE( Variant )
	Q_DECLARE_METATYPE( Assets::Handle )
	Q_DECLARE_METATYPE( Scene::ImageHandle )
	Q_DECLARE_METATYPE( Scene::RenderingMode )
	Q_DECLARE_METATYPE( Scene::LightingModel )

	namespace Ui {

        qDeclarePtr( AssetTree )
        qDeclarePtr( MainWindow )
        qDeclarePtr( Document )
        qDeclarePtr( SceneTree )
		qDeclarePtr( Action )
		qDeclarePtr( Menu )
		qDeclarePtr( ToolBar )
		qDeclarePtr( RenderingFrame )
        qDeclarePtr( PropertyInspector )
        qDeclarePtr( EntityInspector )

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

        //! A helper struct to store the mouse button.
        struct MouseButtons : public FlagSet8 {
            //! Mouse button bit mask.
            enum {
			      Left	    = Qt::LeftButton	//!< Left mouse button.
			    , Right	    = Qt::RightButton	//!< Right mouse button.
			    , Middle    = Qt::MiddleButton	//!< Middle mouse button.
            };

                        //! Constructs MouseButtons instance from Qt enum.
                        MouseButtons( Qt::MouseButton button = Qt::NoButton )
                        {
                            switch( button ) {
	                        case Qt::LeftButton:	on( Left );	    break;
	                        case Qt::RightButton:	on( Right );	break;
	                        case Qt::MidButton:		on( Middle );	break;	
                            }
                        }

                        //! Constructs
        };

		//! Menu action callback type.
		typedef std::function<void(ActionQPtr)> ActionCallback;

	    //! Converts the Qt key index to engine key.
	    extern Platform::Key convertKey( s32 key );

        //! Forward declaration of enumeration combo box.
        template<typename TModel> class EnumerationComboBox;

	} // namespace Ui

	namespace Editors {

		//! This marks terrain chunks.
		class TerrainChunk : public Ecs::Component<TerrainChunk> {
		public:

									//! Constructs TerrainChunk instance.
									TerrainChunk( Scene::TerrainHandle chunk = Scene::TerrainHandle(), u32 x = 0, u32 z = 0 )
										: m_terrain( chunk ), m_x( x ), m_z( z ) {}

			//! Returns terrain instance.
			Scene::TerrainHandle    terrain( void ) const { return m_terrain; }

			//! Returns chunk X.
			u32						x( void ) const { return m_x; } // column

			//! Returns chunk Y.
			u32						z( void ) const { return m_z; } // row

		private:

			Scene::TerrainHandle    m_terrain;	//!< Terrain asset.
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

		qDeclarePtr( AssetEditor )
		qDeclarePtr( VisualEditor )
		qDeclarePtr( SceneEditor )

	} // namespace Editors

	namespace Importers {

		dcDeclarePtrs( AssetImporter )

	} // namespace Importers

	qDeclarePtr( Project )
	qDeclarePtr( AssetManager )

	qDeclarePtr( Composer )
    qDeclarePtr( FileSystem )
    qDeclarePtr( SceneModel )
    qDeclarePtr( AssetFileSystemModel )
    qDeclarePtr( FilteredAssetsModel )
    qDeclarePtr( PropertyModel )

    typedef const QMimeData* MimeDataQPtr;

	//! Container type to store file info.
	typedef QVector<class FileInfo> FileInfoArray;

    //! Completely deletes layout and it's items
    inline void qDeleteLayout( QLayout* layout )
    {
	    if( !layout ) {
		    return;
	    }

	    QLayoutItem* child = NULL;

	    while( (child = layout->takeAt(0)) != 0 ) {
		    delete child->widget();
		    delete child;
	    }

	    delete layout;
    }

	//! Root composer class.
	class Composer : public QApplication {

        Q_OBJECT

	Q_SIGNALS:

		//! Event is emitted when the project is created.
		void				projectCreated( Project* project );

        //! Event is emitted when the project was opened.
        void				projectOpened( Project* project );

        //! Event is emitted when the project was closed.
        void				projectClosed( Project* project );

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

                                    //! Constructs Composer instance.
								    Composer( int argc, char ** argv );

		//! Opens the existing project at path.
		void					    openProject( const String& path );

		//! Creates new project at path.
		void					    createProject( const String& path );

        //! Closes an opened project.
        void                        closeProject( void );

		//! Returns opened project.
		ProjectQPtr	                project( void ) const;

        //! Returns global assembly instance.
        Reflection::AssemblyWPtr    assembly( void ) const;

		//! Returns main window.
		Ui::MainWindowQPtr		    window( void ) const;

		//! Returns the file system instance.
		FileSystemQPtr	            fileSystem( void ) const;

		//! Performs the composer initialization.
		bool					    initialize( void );

		//! Extracts an asset set from MIME data.
		Assets::AssetSet		    assetsFromMime( MimeDataQPtr mime ) const;

		//! Extracts a single asset from MIME data.
		Assets::Handle	            assetFromMime( MimeDataQPtr mime ) const;

	private:

		//! Creates a new project.
		void					    menuCreateProject( Ui::ActionQPtr action );

		//! Opens an existing project.
		void					    menuOpenProject( Ui::ActionQPtr action );

		//! Saves current project.
		void					    menuSaveProject( Ui::ActionQPtr action );

		//! Undo the last action.
		void					    menuUndo( Ui::ActionQPtr action );

		//! Redo the last action.
		void					    menuRedo( Ui::ActionQPtr action );

	private:

		Ui::MainWindowQPtr		    m_mainWindow;			//!< Main composer window.
		Ui::MenuQPtr			    m_menues[TotalMenues];	//!< Default menues.
		ProjectQPtr                 m_project;				//!< Active project.
        FileSystemQPtr		        m_fileSystem;		    //!< File system interface.
        Reflection::AssemblyPtr     m_assembly;             //!< Global assembly instance.
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_H__	*/