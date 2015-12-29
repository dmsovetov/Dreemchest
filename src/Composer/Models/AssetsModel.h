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

#ifndef __DC_Composer_Qt_AssetsModelPrivate_H__
#define __DC_Composer_Qt_AssetsModelPrivate_H__

#include "AbstractTreeModel.h"

DC_BEGIN_COMPOSER

	class AssetsModelPrivate;

#if DEV_CUSTOM_ASSET_MODEL

	//! Asset files model.
	class QAssetsModel : public QLocalFileSystemModel {

		Q_OBJECT

	public:

								//! Constructs the QAssetsModel instance.
		explicit				QAssetsModel( AssetsModel* parentAssetsModel, QObject* parent = NULL );

		//! Returns assets UUID for a specified asset file.
		String					uuid( const FileInfoWPtr& assetFile ) const;

		//! Updates meta data for specified asset file.
		void					setMetaData( const FileInfoWPtr& assetFile, const Io::KeyValue& data );

		//! Returns meta data for a specified asset file.
		Io::KeyValue			metaData( const FileInfoWPtr& assetFile ) const;

		//! Returns asset file info by model index.
		FileInfoPtr				assetFile( const QModelIndex& index ) const;

	private:

		//! Returns true if the file should not be put to model.
		virtual bool			shouldSkipFile( const QFileInfo& file ) const Q_DECL_OVERRIDE;

		//! Returns list of MIME types.
		virtual QStringList		mimeTypes( void ) const Q_DECL_OVERRIDE;

		//! Returns the meta file name from file info.
		QString					metaFileName( const FileInfoWPtr& assetFile ) const;

		//! Returns the meta file name from absolute file path.
		QString					metaFileNameFromPath( const QString& fileName ) const;

		//! Renames meta file.
		bool					renameMetaFile( const QString& oldFileName, const QString& newFileName );

	private slots:

		//! Processes an added file.
		void					fileAdded( const QFileInfo& file );

		//! Processes removed file.
		void					fileRemoved( const QFileInfo& file );

		//! Processes the file change.
		void					fileChanged( const QFileInfo& file );

		//! Processes the file rename.
		void					fileRenamed( const QFileInfo& file, const QString& oldFileName, const QString& newFileName );

		//! Processes the file movement.
		void					fileMoved( const QFileInfo& file, const QString& oldFileName, const QString& newFileName );

	private:

		AssetsModel*			m_parent;				//!< Parent assets model.
		QString					m_metaFileExtension;	//!< Meta file extension.
	};

#else

	//! Asset file system model.
	class AssetsModel : public InjectedEventEmitter<QFileSystemModel> {

		Q_OBJECT

	public:

								//! Constructs AssetsModel instance
		explicit				AssetsModel( QObject* parent = NULL );

		//! Returns assets UUID for a specified asset file.
		String					uuid( const FileInfoWPtr& assetFile ) const;

		//! Returns true if the meta data exists for an asset file.
		bool					hasMetaData( const FileInfoWPtr& assetFile ) const;

		//! Updates meta data for specified asset file.
		void					setMetaData( const FileInfoWPtr& assetFile, const Io::KeyValue& data );

		//! Returns meta data for a specified asset file.
		Io::KeyValue			metaData( const FileInfoWPtr& assetFile ) const;

		//! Returns asset file info by model index.
		FileInfoPtr				assetFile( const QModelIndex& index ) const;

		//! Returns root model path.
		String					rootPath( void ) const;

		//! Sets root model path.
		void					setRootPath( const String& value );

		//! Returns meta data for a specified asset file name.
		Io::KeyValue			metaData( const String& assetFileName ) const;

		//! Base class for all asset events.
		struct AssetEvent {
								AssetEvent( const FileInfoPtr& file )
									: file( file ) {}

			FileInfoPtr			file;	//!< Asset file info.
		};

		//! This event is emitted when a new asset was added to a project.
		struct Added : public AssetEvent {
								Added( const FileInfoPtr& file )
									: AssetEvent( file ) {}
		};

		//! This event is emitted when a new asset was removed from a project.
		struct Removed : public AssetEvent {
								Removed( const FileInfoPtr& file )
									: AssetEvent( file ) {}
		};

		//! This event is emitted when a new asset was changed.
		struct Changed : public AssetEvent {
								Changed( const FileInfoPtr& file )
									: AssetEvent( file ) {}
		};

		//! This event is emitted when scanning progress was started.
		struct ScanningStarted {
		};

		//! This event is emitted when scanning progress was completed.
		struct ScanningCompleted {
		};

		//! This event is emitted when directory was scanned.
		struct FolderScanned {
								FolderScanned( s32 leftToScan )
									: leftToScan( leftToScan ) {}

			s32					leftToScan;	//!< The total number of items left to scan.
		};

	protected:

		//! Returns the model data by index.
		virtual QVariant		data( const QModelIndex& index, int role ) const Q_DECL_OVERRIDE;

		//! Sets the model data by index.
		virtual bool			setData( const QModelIndex& index, const QVariant& value, int role ) Q_DECL_OVERRIDE;

		//! Handles the drop operation.
		virtual bool			dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) Q_DECL_OVERRIDE;

		//! Returns the meta file name from file info.
		QString					metaFileName( const FileInfoWPtr& assetFile ) const;

		//! Returns the meta file name from absolute file path.
		QString					metaFileNameFromPath( const QString& fileName ) const;

		//! Renames meta file.
		bool					renameMetaFile( const QString& oldFileName, const QString& newFileName );

		//! Returns true if this file was just moved.
		bool					wasMoved( const FileInfoWPtr& assetFile ) const;

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
										FilteredAssetsModel( AssetsModelWPtr assetsModel, QObject* parent = NULL );

		//! Returns root model index.
		QModelIndex						root( void ) const;

		//! Returns internal model pointer.
		AssetsModelWPtr			        model( void ) const;

		//! Returns asset file info by model index.
		FileInfoPtr						assetFile( const QModelIndex& index ) const;

		//! Removes asset file by index.
		void							remove( const QModelIndex& index );

	protected:

		//! Returns true if the item in the row indicated by the given row and parent should be included in the model.
		virtual bool					filterAcceptsRow( int row, const QModelIndex& parent ) const Q_DECL_OVERRIDE;

	private:

		AssetsModelWPtr	                m_model;	//!< Actual assets model.
	};

#endif	/*	DEV_CUSTOM_ASSET_MODEL	*/

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Qt_AssetsModelPrivate_H__	*/