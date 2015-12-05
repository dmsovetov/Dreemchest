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

#ifndef __DC_Composer_IMainWindow_H__
#define __DC_Composer_IMainWindow_H__

#include "IUserInterface.h"

namespace Ui {

	//! Factory method used for main window creation.
	extern IMainWindowPtr createMainWindow( const String& title );

	//! Main application window interface.
	class IMainWindow : public IUserInterface {
	public:

		//! Adds a new toolbar to window.
		virtual IToolBarWPtr		addToolBar( void )						= 0;
	
		//! Removes the toolbar from a window.
		virtual void				removeToolBar( IToolBarWPtr toolBar )	= 0;
	
		//! Adds a new menu to window.
		virtual IMenuWPtr			addMenu( const String& text )			= 0;
	
		//! Removes the menu from a window.
		virtual void				removeMenu( IMenuWPtr menu )			= 0;

		//! Performs the main window initialization.
		virtual bool				initialize( ComposerWPtr composer )		= 0;

		//! Shows the message box.
		virtual void				message( const String& text, MessageStatus status = MessageInfo ) const = 0;

		//! Shows the message box with yes, no, cancel buttons.
		virtual MessageBoxResult	messageYesNoCancel( const String& text, const String& info, MessageStatus status = MessageInfo ) const = 0;

		//! Constructs a new asset editor dock window or brings to front the existing one.
		virtual IDocumentWPtr		editDocument( Editors::AssetEditorWPtr assetEditor, const Ui::FileInfo& fileInfo ) = 0;

		//! Closes the document.
		virtual bool				closeDocument( IDocumentWPtr document ) = 0;

		//! Returns the opened document editor by asset.
		virtual IDocumentWPtr		findDocument( const FileInfo& fileInfo ) const = 0;

		//! Returns an array of opened documents with a same type.
		virtual DocumentsWeak		findDocuments( const FileInfo& fileInfo ) const = 0;

		//! Returns the file system instance.
		virtual IFileSystemWPtr		fileSystem( void ) const				= 0;

		//! Returns the asset tree instance.
		virtual IAssetTreeWPtr		assetTree( void ) const					= 0;

		//! Sets an active document.
		virtual void				setActiveDocument( IDocumentWPtr dock ) = 0;

		//! Returns the shared rendering context.
		virtual IRenderingFrameWPtr	sharedRenderingContext( void ) const	= 0;
	};

} // namespace Ui

#endif	/*	!__DC_Composer_MainWindow_H__	*/