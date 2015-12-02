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

#ifndef __DC_Composer_MainWindow_H__
#define __DC_Composer_MainWindow_H__

#include "Composer.h"

//! Main application window, conatains the dock area.
class MainWindow : public QMainWindow {

    Q_OBJECT

public:

									//! Constructs MainWindow instance.
									MainWindow( void );

	//! Performs the basic initialization.
	bool							initialize( void );

private slots:

	//! Creates a new project.
	void							newProject( void );

	//! Opens an existing project.
	void							openProject( void );

	//! Starts asset importing.
	void							importAssets( void );

protected:

	//! Overrides the close application behaviour.
    void							closeEvent( QCloseEvent* event ) Q_DECL_OVERRIDE;

	//! Creates a new dock panel with specified widget.
	QDockWidget*					addDock( const QString& name, QWidget* widget, Qt::DockWidgetArea initialDockArea = Qt::LeftDockWidgetArea, Qt::DockWidgetArea allowedDockAreas = Qt::AllDockWidgetAreas );

	//! Creates a new session window.
	void							addSession( const QString& host, s32 port );

	//! Adds a new menu action with a target slot.
	QAction*						addAction( s32 menu, const QString& caption, CString slot );

private:

	enum {
		  MenuFile		//!< File menu index.
		, MenuView		//!< View menu index.
		, MenuAssets	//!< Assets menu index.
		, TotalMenues	//!< The total number of menues.
	};

	QMenu*							m_menues[TotalMenues];	//!< Menu bars.
};

/*//! Thread worker class.
class WorkerBase : public QThread {

	Q_OBJECT

signals:

	//! Emitted when the job is completed.
	void			completed();
};

template<typename TCallback>
class Worker : public WorkerBase {
public:

					//! Creates instance of a Worker.
					Worker( const TCallback& callback )
						: m_callback( callback ) {}

protected:

	//! Thread runner.
	virtual void run( void )
	{
		m_callback();
		emit completed();
	}

protected:

	TCallback		m_callback;	//!< The callback function to be run.
};

typedef Worker< cClosure<void()> > VoidWorker;*/

#endif	/*	!__DC_Composer_MainWindow_H__	*/