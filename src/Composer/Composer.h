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

#define DEV_CUSTOM_ASSET_MODEL			(1)
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

		//! Mouse buttons.
		enum MouseButton {
			  LeftMouseButton	//!< Left mouse button.
			, RightMouseButton	//!< Right mouse button.
			, MiddleMouseButton	//!< Middle mouse button.
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

		dcDeclarePtrs( AssetEditor )
		dcDeclarePtrs( VisualEditor )

	} // namespace Editors

	namespace Importers {

		dcDeclarePtrs( AssetImporter )

	} // namespace Importers

	namespace Project {

		dcDeclarePtrs( Project )
		dcDeclarePtrs( Cache )

	} // namespace Project

	dcDeclarePtrs( Composer )
	dcDeclarePtrs( IFileSystem )
	dcDeclarePtrs( IMimeData )
	dcDeclarePtrs( AssetsModel )

	//! Factory method used for assets model creation.
	extern AssetsModelPtr createAssetsModel( void );

	//! File info struct.
	struct FileInfo {
		String		path;			//!< The file absolute path.
		String		directory;		//!< Parent directory absolute path.
		String		baseName;		//!< The file base name.
		String		fileName;		//!< The base name with extension.
		String		ext;			//!< The file extension.
		u32			timestamp;		//!< The file timestamp.
	};

	//! Container type to store file info.
	typedef Array<FileInfo> FileInfoArray;

	//! Asset info struct.
	struct Asset {
		String		ext;				//!< Asset's type.
		String		name;				//!< Asset name.
		String		absoluteFilePath;	//!< Absolute file path.
		io::Bson	metaInfo;			//!< Attached metainfo.

		//! Returns true if two assets are the same.
		bool			operator == ( const Asset& other ) const { return uuid() == other.uuid(); }

		//! Return asset UUID.
		const String&	uuid( void ) const { return metaInfo["uuid"].asString(); }
	};

	//! Base class for event emitter classes.
	class EventEmitter : public RefCounted {
	public:

		//! Subscribes for a project event.
		template<typename TEvent>
		void						subscribe( const typename event::EventEmitter::Callback<TEvent>::Type& callback ) { m_eventEmitter.subscribe<TEvent>( callback ); }

		//! Removes an event listener.
		template<typename TEvent>
		void						unsubscribe( const typename event::EventEmitter::Callback<TEvent>::Type& callback ) { m_eventEmitter.unsubscribe<TEvent>( callback ); }

		//! Constructs and emits a new event instance.
		template<typename TEvent, typename ... TArgs>
		void						notify( const TArgs& ... args ) { m_eventEmitter.notify<TEvent, TArgs...>( args... ); }

	protected:

		event::EventEmitter			m_eventEmitter;	//!< Event emitter instance.
	};

	//! Base interface class.
	class IInterface : public EventEmitter {
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
	class Composer : public EventEmitter {
	public:

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