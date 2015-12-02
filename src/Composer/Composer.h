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

	#define Q_DECL_OVERRIDE override
#elif DC_QT5_ENABLED
	#error Qt5 headers here!
#endif

namespace Ui {

	dcDeclarePtrs( IMainWindow )
	dcDeclarePtrs( IAction )
	dcDeclarePtrs( IMenu )
	dcDeclarePtrs( IToolBar )

	//! Menu action callback type.
	typedef std::function<void(IActionWPtr)> ActionCallback;

} // namespace Ui

//! Root composer class.
class Composer {
public:

	//! Creates the Composer instance.
	static Composer*	create( Ui::IMainWindowPtr mainWindow );

private:

						//! Constructs Composer instance.
						Composer( Ui::IMainWindowPtr mainWindow );

private:

	Ui::IMainWindowPtr	m_mainWindow;	//!< Main composer window.
};

#endif	/*	!__DC_Composer_H__	*/