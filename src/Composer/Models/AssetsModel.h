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

#ifndef __DC_Composer_AssetsModel_H__
#define __DC_Composer_AssetsModel_H__

#include "AbstractTreeModel.h"

DC_BEGIN_COMPOSER

	//! Asset file system model.
	class AssetsModel : public QFileSystemModel {

		Q_OBJECT

    Q_SIGNALS:

		//! This event is emitted when a new asset file was added to a project.
        void                    fileAdded( const FileInfo& file );

		//! This event is emitted when a new asset file was removed from a project.
        void                    fileRemoved( const FileInfo& file );

		//! This event is emitted when a new asset file was changed.
        void                    fileChanged( const FileInfo& file );

		//! This event is emitted when scanning progress was started.
        void                    scanningStarted( void );

		//! This event is emitted when scanning progress was completed.
        void                    scanningCompleted( void );

		//! This event is emitted when directory was scanned.
        void                    folderScanned( s32 leftToScan );

	public:

								//! Constructs AssetsModel instance
		explicit				AssetsModel( QObject* parent );

		//! Returns assets UUID for a specified asset file.
		String					uuid( const FileInfo& assetFile ) const;

		//! Returns true if the meta data exists for an asset file.
		bool					hasMetaData( const FileInfo& assetFile ) const;

		//! Updates meta data for specified asset file.
		void					setMetaData( const FileInfo& assetFile, const Io::KeyValue& data );

		//! Returns meta data for a specified asset file.
		Io::KeyValue			metaData( const FileInfo& assetFile ) const;

		//! Returns asset file info by model index.
		FileInfo				assetFile( const QModelIndex& index ) const;

		//! Returns root model path.
		String					rootPath( void ) const;

		//! Sets root model path.
		void					setRootPath( const String& value );

		//! Returns meta data for a specified asset file name.
		Io::KeyValue			metaData( const String& assetFileName ) const;

	protected:

		//! Returns the model data by index.
		virtual QVariant		data( const QModelIndex& index, int role ) const Q_DECL_OVERRIDE;

		//! Sets the model data by index.
		virtual bool			setData( const QModelIndex& index, const QVariant& value, int role ) Q_DECL_OVERRIDE;

		//! Handles the drop operation.
		virtual bool			dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) Q_DECL_OVERRIDE;

		//! Returns the meta file name from file info.
		QString					metaFileName( const FileInfo& assetFile ) const;

		//! Returns the meta file name from absolute file path.
		QString					metaFileNameFromPath( const QString& fileName ) const;

		//! Renames meta file.
		bool					renameMetaFile( const QString& oldFileName, const QString& newFileName );

		//! Returns true if this file was just moved.
		bool					wasMoved( const FileInfo& assetFile ) const;

	private slots:

		//! Handles added assets signal.
		void					assetsAdded( const QModelIndex& parent, int start, int end );

		//! Handles removed assets signal.
		void					assetsRemoved( const QModelIndex& parent, int start, int end );

		//! Handles renamed asset signal.
		void					assetRenamed( const QString& path, const QString& oldName, const QString& newName );

		//! Scans the directory when it's loaded.
		void					scanLoadedDirectory( const QString& path );

	private:

		QString					m_metaFileExtension;	//!< Meta file extension.
		bool					m_isEditingModel;		//!< The model editing is in progress.
		mutable QSet<QString>	m_wasMoved;				//!< Set of file names that were moved.
		mutable QSet<QString>	m_foldersToScan;		//!< Set of paths waiting for scan.
	};

	//! Filters asset model before displaying.
	class FilteredAssetsModel : public QSortFilterProxyModel {
	public:

										//! Constructs FilteredAssetsModel instance.
										FilteredAssetsModel( AssetsModelQPtr assetsModel, QObject* parent );

		//! Returns root model index.
		QModelIndex						root( void ) const;

		//! Returns internal model pointer.
		AssetsModelQPtr			        model( void ) const;

		//! Returns asset file info by model index.
		FileInfo						assetFile( const QModelIndex& index ) const;

		//! Removes asset file by index.
		void							remove( const QModelIndex& index );

	protected:

		//! Returns true if the item in the row indicated by the given row and parent should be included in the model.
		virtual bool					filterAcceptsRow( int row, const QModelIndex& parent ) const Q_DECL_OVERRIDE;

	private:

		AssetsModelQPtr	                m_model;	//!< Actual assets model.
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Qt_AssetsModel_H__	*/