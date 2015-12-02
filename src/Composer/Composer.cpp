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

#include "Composer.h"

#include "UI/IMainWindow.h"
#include "UI/IMenu.h"

// ** Composer::Composer
Composer::Composer( Ui::IMainWindowPtr mainWindow ) : m_mainWindow( mainWindow )
{
	// Add default menues
	m_menues[FileMenu]	 = mainWindow->addMenu( "&File" );
	m_menues[EditMenu]	 = mainWindow->addMenu( "&Edit" );
	m_menues[ViewMenu]	 = mainWindow->addMenu( "&View" );
	m_menues[AssetsMenu] = mainWindow->addMenu( "&Assets" );

	// Add actions
	m_menues[FileMenu]->addAction( "Create Project", BindAction( Composer::onCreateProject ) );
	m_menues[FileMenu]->addAction( "Open Project", BindAction( Composer::onOpenProject ) );
	m_menues[AssetsMenu]->addAction( "Import Assets", BindAction( Composer::onImportAssets ) );
}

// ** Composer::create
Composer* Composer::create( Ui::IMainWindowPtr mainWindow )
{
	return new Composer( mainWindow );
}

// ** Composer::onCreateProject
void Composer::onCreateProject( Ui::IActionWPtr action )
{

}

// ** Composer::onOpenProject
void Composer::onOpenProject( Ui::IActionWPtr action )
{

}

// ** Composer::onImportAssets
void Composer::onImportAssets( Ui::IActionWPtr action )
{

}

// ** main
int main(int argc, char *argv[])
{
	QCoreApplication::setLibraryPaths( QCoreApplication::libraryPaths() << "." << "imageformats" << "platforms" );

    QApplication app( argc, argv );
    app.setApplicationName( "Dreemchest Composer" );

	Ui::IMainWindowPtr mainWindow = Ui::createMainWindow( "Dreemchest Composer" );
	Composer::create( mainWindow );

    return app.exec();
}