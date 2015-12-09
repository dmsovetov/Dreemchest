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

#ifndef __DC_Composer_Qt_AssetTree_H__
#define __DC_Composer_Qt_AssetTree_H__

#include "UserInterface.h"

namespace Ui {

	//! Subclass of a QTreeView to extend the context menu & key press behaviour.
	class QAssetTree : public QTreeView {

		Q_OBJECT

	public:

								//! Constructs asset tree bound to a specified path.
								QAssetTree( Project::ProjectWPtr project, QAssetsModel* model );

		//! Returns the selected items.
		StringArray				selection( void ) const;

		//! Expands the selected items.
		virtual void			expandSelectedItems( void );

		//! Sets the parent asset tree.
		void					setParent( IAssetTreeWPtr value );

	protected:

		//! Handles the deletion and renaming items.
		virtual void			keyPressEvent( QKeyEvent* e ) Q_DECL_OVERRIDE;

		//! Handles the context menu requests.
		virtual void			contextMenuEvent( QContextMenuEvent* e ) Q_DECL_OVERRIDE;

	private slots:

		//! Handles the doubleClicked signal.
		void					itemDoubleClicked( const QModelIndex& index );

	private:

		IAssetTreeWPtr			m_parent;	//!< Parent AssetTree instance.
		Project::ProjectWPtr	m_project;	//!< Parent project instance.
		QAssetsModel*			m_model;	//!< File system model used.
	};

	//! Asset tree widget.
	class AssetTree : public UserInterface<IAssetTree, QAssetTree> {
	public:

								//! Constructs asset tree bound to a specified path.
								AssetTree( Project::ProjectWPtr project, QAssetsModel* model );

		//! Returns the selected items.
		virtual StringArray		selection( void ) const;

		//! Expands the selected items.
		virtual void			expandSelectedItems( void );
	};

} // namespace Ui

#endif	/*	!__DC_Composer_Qt_AssetTree_H__	*/