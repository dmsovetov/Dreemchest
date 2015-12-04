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

#ifndef __DC_Composer_Qt_AssetFilesModel_H__
#define __DC_Composer_Qt_AssetFilesModel_H__

#include "../IUserInterface.h"

namespace Ui {

	//! Asset files model.
	class AssetFilesModel : public QFileSystemModel {

		Q_OBJECT

	public:

							//! Constructs AssetFilesModel
							AssetFilesModel( QObject* parent, const String& path );

	protected:

		//! Returns the model data by index.
		virtual QVariant	data( const QModelIndex& index, int role ) const Q_DECL_OVERRIDE;

		//! Sets the model data by index.
		virtual bool		setData( const QModelIndex& index, const QVariant& value, int role ) Q_DECL_OVERRIDE;

		virtual bool		beginMoveRows(const QModelIndex & sourceParent, int sourceFirst, int sourceLast, const QModelIndex & destinationParent, int destinationChild);

	private slots:

		void				layoutChanged( void );

		//! Handles added assets signal.
		void				assetsAdded( const QModelIndex& parent, int start, int end );

		//! Handles removed assets signal.
		void				assetsAboutToBeRemoved( const QModelIndex& parent, int start, int end );

		//! Handles renamed asset signal.
		void				assetRenamed( const QString& path, const QString& oldName, const QString& newName );

	private:

		bool				m_isEditingModel;	//!< The model editing is in progress.
		QSet<QString>		m_skipAddRemove;	//!< This set contains paths to all assets that were renamed or moved.
	};

} // namespace Ui

#endif	/*	!__DC_Composer_Qt_AssetFilesModel_H__	*/