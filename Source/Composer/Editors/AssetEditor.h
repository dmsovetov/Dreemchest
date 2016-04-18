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

#ifndef __DC_Composer_AssetEditor_H__
#define __DC_Composer_AssetEditor_H__

#include "../Composer.h"
#include "../FileSystem.h"

#include "../Widgets/Document.h"
#include "../Widgets/MainWindow.h"
#include "../Widgets/SceneTree.h"
#include "../Widgets/RenderingFrame.h"
#include "../Widgets/ComboBox.h"

#include "../Models/SceneModel.h"

DC_BEGIN_COMPOSER

namespace Editors {

	//! Base class for all asset editors.
	class AssetEditor : public QObject {
	public:

		//! Asset editor has entered the foreground.
		virtual void			notifyEnterForeground( Ui::MainWindowQPtr window ) {}

		//! Asset editor has entered the background.
		virtual void			notifyEnterBackground( Ui::MainWindowQPtr window ) {}

		//! Performs asset editor initialization.
		virtual bool			initialize( ProjectQPtr project, const FileInfo& asset, Ui::DocumentQPtr document );

		//! Saves the asset to disk.
		virtual void			save( void ) {}

		//! Returns true if the document has changes.
		bool					hasChanges( void ) const;

		//! Returns the edited asset.
		const FileInfo&		    asset( void ) const;

		//! Returns the parent document.
		Ui::DocumentQPtr		document( void ) const;

	protected:

		ProjectQPtr	            m_project;	//!< Parent project instance.
		Ui::DocumentQPtr		m_document;	//!< Parent document.
		FileInfo			    m_asset;	//!< An asset being edited.
	};

} // namespace Editors

DC_END_COMPOSER

#endif	/*	!__DC_Composer_AssetEditor_H__	*/