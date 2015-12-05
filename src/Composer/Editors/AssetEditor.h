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

#include "../UI/IDocument.h"
#include "../UI/IRenderingFrame.h"

namespace Editors {

	//! Base class for all asset editors.
	class AssetEditor : public RefCounted {
	public:

		//! Asset editor has entered the foreground.
		virtual void		notifyEnterForeground( void ) {}

		//! Asset editor has entered the background.
		virtual void		notifyEnterBackground( void ) {}

		//! Performs asset editor initialization.
		virtual bool		initialize( const Ui::FileInfo& asset, Ui::IDocumentWPtr document );

		//! Saves the asset to disk.
		virtual void		save( void ) {}

		//! Returns true if the document has changes.
		bool				hasChanges( void ) const;

		//! Returns the edited asset.
		const Ui::FileInfo&	asset( void ) const;

		//! Returns the parent document.
		Ui::IDocumentWPtr	document( void ) const;

	protected:

		Ui::IDocumentWPtr	m_document;	//!< Parent document.
		Ui::FileInfo		m_asset;	//!< An edited asset.
	};

} // namespace Editors

#endif	/*	!__DC_Composer_AssetEditor_H__	*/