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

#ifndef __DC_Composer_AssetTree_H__
#define __DC_Composer_AssetTree_H__

#include "../Composer.h"

DC_BEGIN_COMPOSER

namespace Ui {

	//! Asset selector widget.
	class AssetSelector : public QWidget {

		Q_OBJECT
		Q_PROPERTY( Scene::AssetWPtr value READ value WRITE setValue NOTIFY valueChanged USER true )

	Q_SIGNALS:

		//! Emitted when the selected asset was changed.
		void				valueChanged( void );

	public:

							//! Constructs AssetSelector widget.
							AssetSelector( u32 mask = ~0, QWidget* parent = NULL );

		//! Returns the selected asset.
		Scene::AssetWPtr	value( void ) const;

		//! Sets the selected asset.
		void				setValue( const Scene::AssetWPtr& value );

	private:

		//! Event filter to handle drop events.
		virtual bool		eventFilter( QObject* target, QEvent* e ) Q_DECL_OVERRIDE;

	private:

		QLineEdit*			m_line;		//!< Asset selector line edit.
		QToolButton*		m_button;	//!< Asset selector button.
		u32					m_mask;		//!< Accepted asset types.
		Scene::AssetWPtr	m_asset;	//!< The selected asset.
	};

	//! Subclass of a QTreeView to extend the context menu & key press behaviour.
	class AssetTree : public QTreeView {

		Q_OBJECT

	public:

									//! Constructs asset tree bound to a specified path.
									AssetTree( Project::ProjectWPtr project );

		//! Returns the selected items.
		FileInfoArray				selection( void ) const;

		//! Expands the selected items.
		void						expandSelectedItems( void );

		//! Sets asset tree model.
		void						setModel( AssetsModelWPtr value );

        //! Returns asset model.
        AssetsModelWPtr             model( void ) const;

	protected:

		//! Handles the deletion and renaming items.
		virtual void				keyPressEvent( QKeyEvent* e ) Q_DECL_OVERRIDE;

		//! Handles the context menu requests.
		virtual void				contextMenuEvent( QContextMenuEvent* e ) Q_DECL_OVERRIDE;

		//! Resets the selection changed flag.
		virtual void				mousePressEvent( QMouseEvent* e ) Q_DECL_OVERRIDE;

		//! Handles the asset selection.
		virtual void				mouseReleaseEvent( QMouseEvent* e ) Q_DECL_OVERRIDE;

		//! Binds the selected asset to an object inspector.
		void						bindToInspector( const QModelIndexList& selection );

	private slots:

		//! Handles the doubleClicked signal.
		void						itemDoubleClicked( const QModelIndex& index );

		//! Handles the selectionChanged signal of QItemSelectionModel.
		void						selectionChanged( const QItemSelection& selected, const QItemSelection& deselected );

	private:

		Project::ProjectWPtr		m_project;			//!< Parent project instance.
		FilteredAssetsModelPtr	    m_proxy;			//!< Filtered assets model to be used.
		bool						m_selectionChanged;	//!< This flag indicates that selection was changed.
	};

} // namespace Ui

DC_END_COMPOSER

#endif	/*	!__DC_Composer_AssetTree_H__	*/