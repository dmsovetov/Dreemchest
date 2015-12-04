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

#ifdef DC_QT4_ENABLED
	#include <QtGui>
	#include <QtCore>
	#include <QtOpenGL>

	#define Q_DECL_OVERRIDE override
#elif DC_QT5_ENABLED
	#error Qt5 headers here!
#endif

namespace Ui {

	dcDeclarePtrs( IMainWindow )
	dcDeclarePtrs( IAction )
	dcDeclarePtrs( IMenu )
	dcDeclarePtrs( IToolBar )
	dcDeclarePtrs( IRenderingFrame )
	dcDeclarePtrs( IDocument )
	dcDeclarePtrs( IFileSystem )
	dcDeclarePtrs( IAssetTree )

	//! File info struct.
	struct FileInfo {
		String		path;			//!< The file absolute path.
		String		directory;		//!< Parent directory absolute path.
		String		baseName;		//!< The file base name.
		String		fileName;		//!< The base name with extension.
		String		ext;			//!< The file extension.
		u32			timestamp;		//!< The file timestamp.
	};

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

	//! Auto ptr type for signal delegate instances.
	typedef AutoPtr<class SignalDelegate> SignalDelegatePtr;

	//! Container type to store array of documents.
	typedef Array<IDocumentWPtr> DocumentsWeak;

	//! Menu action callback type.
	typedef std::function<void(IActionWPtr)> ActionCallback;

} // namespace Ui

namespace Editors {

	dcDeclarePtrs( AssetEditor )

} // namespace Editors

namespace Project {

	dcDeclarePtrs( Project )

/*	#define dcDeclareEventID( ... )
	#define dcBeginClass( ... )
	#define dcEndClass

	struct sForEachFileCallback {};

	typedef void* dcDiskFileSystem;
	typedef void* dcTask;

	struct _Context {
	};
	typedef _Context* dcContext;

	namespace reflection {
		struct cObject { cObject( dcContext ctx ) {} void SetClass( CString value ) {} };
	}

	namespace asset {
		struct Asset {};
		struct cAssetManager {};
	}

	typedef asset::Asset* dcAsset;

	namespace event {
		struct EventDispatcher { EventDispatcher( dcContext ctx ) {} String m_type; };
		struct Event { Event( dcContext ctx ) {} String m_type; };
	}

	typedef event::Event* dcEvent;

    class cProject;
    class ProjectModel;
    class Configuration;
    class Files;
    class Item;
    class Meta;
		class ComponentMeta;
		class ImageMeta;
    class Cache;
        class ClassCache;
        class BuildCache;

    typedef cClosure<void(Item*, void*)>    ProjectItemIterator;

    typedef std::vector<Item*>      ProjectItemArray;
    typedef std::stack<Item*>       ProjectItemStack;
    typedef std::list<Item*>        ProjectItemList;
    typedef std::set<Item*>         ProjectItemSet;
    typedef std::map<const Item*, const Item*>  MapItemToItem;*/

} // namespace Project

dcDeclarePtrs( Composer )

//! Root composer class.
class Composer : public RefCounted {
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

	//! Generic method to subscribe for events.
	template<typename TEvent>
    void subscribe( const typename event::EventEmitter::Callback<TEvent>::Type& callback )
	{
		m_event.subscribe<TEvent>( callback );
	}

	//! Generic method to unsubscribe from events.
	template<typename TEvent>
	void unsubscribe( const typename event::EventEmitter::Callback<TEvent>::Type& callback )
	{
		m_event.unsubscribe<TEvent>( callback );
	}

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

	//! Opens the existing project at path.
	void				openExistingProject( const String& path );

	//! Creates new project at path.
	void				createNewProject( const String& path );

	//! Performs the composer initialization.
	bool				initialize( void );

private:

	static ComposerWPtr	s_instance;				//!< Shared composer instance.
	Ui::IMainWindowPtr	m_mainWindow;			//!< Main composer window.
	Ui::IMenuWPtr		m_menues[TotalMenues];	//!< Default menues.
	Project::ProjectPtr	m_project;				//!< Active project.
	event::EventEmitter	m_event;				//!< Event emitter.
};

#endif	/*	!__DC_Composer_H__	*/