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

#ifndef __DC_Composer_Qt_AssetsModel_H__
#define __DC_Composer_Qt_AssetsModel_H__

#include "AbstractTreeModel.h"

namespace Ui {

#if DEV_CUSTOM_ASSET_MODEL

	//! Asset info.
	class QAsset {
	friend class QAssetsModel;
	public:

		//! Meta file extension to use.
		static QString		kExtension;

							//! Constructs QAsset instance.
							QAsset( const QFileInfo& fileInfo = QFileInfo() );

							//! Constructs QAsset instance from absolute path.
							QAsset( const QString& path );

		//! Returns meta-file name.
		const QString&		metaFileName( void ) const;

		//! Returns unique identifier.
		const QUuid&		uuid( void ) const;

		//! Returns asset name.
		QString				name( void ) const;

		//! Returns asset name with extension.
		QString				fileName( void ) const;

		//! Returns asset absolute file path.
		QString				absoluteFilePath( void ) const;

		//! Loads meta file to string.
		QString				readMetaFile( void ) const;

		//! Returns asset file info.
		const QFileInfo&	fileInfo( void ) const;

	private:

		//! Updates file info.
		void				updateFileInfo( const QString& path );

	private:

		QUuid				m_uuid;			//!< Unique identifier.
		QFileInfo			m_fileInfo;		//!< Asset file info.
		QString				m_metaFileName;	//!< Last known meta-file name.
	};

	//! Keeps track of meta files.
	class QMetaFiles : public QObject {
		
		Q_OBJECT

	public:

							//! Constructs QMetaFiles instance.
							explicit QMetaFiles( QObject* parent = NULL );

	private Q_SLOTS:


		//! Adds new meta file when asset was added.
		void				addMetaFile( const QAsset& asset ) const;

		//! Removes meta file when asset was removed.
		void				removeMetaFile( const QAsset& asset ) const;

		//! Renames meta file when an asset was renamed.
		void				renameMetaFile( const QAsset& asset, const QString& oldName, const QString& newName ) const;

		//! Moves meta file when an asset was moved.
		void				moveMetaFile( const QAsset& asset, const QString& oldPath, const QString& newPath ) const;
	};

	//! Asset files model.
	class QAssetsModel : public QGenericTreeModel<QAsset> {

		Q_OBJECT

	Q_SIGNALS:

	#if DEV_BACKGROUND_ASSET_LOADING
		//! Emitted when the model loading was started.
		void					loadingStarted( void );

		//! Emitted when the model loading was finished.
		void					loadingFinished( void );
	#endif	/*	DEV_BACKGROUND_ASSET_LOADING	*/

		//! Emitted when a new asset was added.
		void					assetAdded( const QAsset& asset ) const;

		//! Emitted when an asset was removed.
		void					assetRemoved( const QAsset& asset ) const;

		//! Emitted when an asset was changed.
		void					assetChanged( const QAsset& asset ) const;

		//! Emitted when an asset was renamed.
		void					assetRenamed( const QAsset& asset, const QString& oldName, const QString& newName ) const;

		//! Emitted when an asset was moved.
		void					assetMoved( const QAsset& asset, const QString& oldPath, const QString& newPath ) const;

		//! Emitted when the asset movement operation was failed.
		void					failedToMove( const QAsset& asset, const QString& newPath ) const;

		//! Emitted when the rename operation was failed.
		void					failedToRename( const QAsset& asset, const QString& newName ) const;

		//! Emitted when the remove operation was failed.
		void					failedToRemove( const QAsset& asset ) const;

	public:

								//! Constructs the QAssetsModel instance.
		explicit				QAssetsModel( QObject* parent = NULL );
		virtual					~QAssetsModel( void );

		//! Returns true if the model supports background loading.
		bool					supportsBackgroundLoading( void ) const;

		//! Returns the root assets path.
		QString					rootPath( void ) const;

		//! Sets the root assets path.
		void					setRootPath( const QString& value );

		//! Returns the read-only flag.
		bool					isReadOnly( void ) const;

		//! Sets the read-only flag.
		void					setReadOnly( bool value );

		//! Returns index for a specified asset path.
		QModelIndex				index( const QString& path ) const;

		//! Removes an asset at specified index.
		void					remove( const QModelIndex& index );

		//! Returns true if an asset at specified index is a directory.
		bool					isDir( const QModelIndex& index ) const;

		//! Returns an asset at specified index.
		const QAsset&			asset( const QModelIndex& index ) const;

	private:

		//! Scans the selected folder.
		void					scan( const QString& path, TreeItem* parent = NULL );

	#if DEV_BACKGROUND_ASSET_LOADING
		//! Scans the selected folder in background.
		void					scanInBackground( const QString& path, TreeItem* parent = NULL );

		//! Scans the selected folder.
		static void				workerScan( QAssetsModel* model, const QString& path );
	#endif	/*	DEV_BACKGROUND_ASSET_LOADING	*/

		//! Moves or renames item.
		bool					move( Item* item, const QString& source, const QString& destination ) const;

		//! Constructs an absolute asset file name from item.
		QString					constructAbsoluteFileName( const Item* item, const Item* parent ) const;

		//! Constructs the relative asset file name from item.
		QString					constructRelativeFileName( const Item* item, const Item* parent ) const;

		//! Returns item at specified path.
		Item*					itemAtPath( const QString& path, const TreeItem* root ) const;

		//! Returns the directory listing.
		QFileInfoList			listDirectory( const QString& path ) const;

		//! Adds asset to file system watcher.
		void					watch( const Item* item );

		//! Removes asset from file system watcher.
		void					unwatch( const Item* item );

		//! Returns true if the file should not be put to model.
		bool					shouldSkipFile( const QFileInfo& fileInfo ) const;

	protected:

		//! Returns the item flags.
		virtual Qt::ItemFlags	flags( const QModelIndex& index ) const Q_DECL_OVERRIDE;

		//! Performs the asset file movement
		virtual bool			moveItem( Item* sourceParent, Item* destinationParent, Item* item, int destinationRow ) const Q_DECL_OVERRIDE;

		//! Removes item from a model.
		virtual void			removeItem( TreeItem* item ) Q_DECL_OVERRIDE;

		//! Adds item to a model.
		virtual void			addItem( TreeItem* item, TreeItem* parent ) Q_DECL_OVERRIDE;

		//! Returns the data from the specified index.
		virtual QVariant		data( const QModelIndex& index, int role ) const Q_DECL_OVERRIDE;

		//! Writes new data to model at specified index.
		virtual bool			setData( const QModelIndex& index, const QVariant& value, int role ) Q_DECL_OVERRIDE;

	private slots:

		//! Handles the file changed signal.
		void					fileChanged( const QString& fileName );

		//! Handles the directory changed signal.
		void					directoryChanged( const QString& path );

	#if DEV_BACKGROUND_ASSET_LOADING
		void					scanFinished( void );
	#endif	/*	DEV_BACKGROUND_ASSET_LOADING	*/

	private:

		QFileInfo				m_directory;	//!< Root assets directory.
		QFileIconProvider*		m_iconProvider;	//!< Icon provider to use.
		QFileSystemWatcher*		m_watcher;		//!< File system watcher.
		bool					m_isReadOnly;	//!< Is model a read-only.
		QMetaFiles*				m_metaFiles;	//!< Tracks meta files associated with assets.

	#if DEV_BACKGROUND_ASSET_LOADING
		QList<QFutureWatcher<void>*>	m_scanWatchers;	//!< Future watchers used for asset folder scanning.
	#endif	/*	DEV_BACKGROUND_ASSET_LOADING	*/
	};

	//! Dispatches assets model signals to delegate.
	class QAssetsModelDispatcher : public QObject {

		Q_OBJECT

	public:

											//! Constructs QAssetsModelDispatcher instance.
											QAssetsModelDispatcher( QAssetsModel* model, IAssetsModelDelegateWPtr delegate );

		//! Returns the attached delegate.
		virtual IAssetsModelDelegateWPtr	delegate( void ) const;

	private:

		//! Extract asset info from QAsset.
		Asset								extractAsset( const QAsset& asset ) const;

	private Q_SLOTS:

		//! Dispatches asset added event to delegate.
		void								dispatchAssetAdded( const QAsset& asset );

		//! Dispatches asset removed event to delegate.
		void								dispatchAssetRemoved( const QAsset& asset );

		//! Dispatches asset changed event to delegate.
		void								dispatchAssetChanged( const QAsset& asset );

	private:

		IAssetsModelDelegateWPtr			m_delegate;	//!< Target delegate.
	};

	//! Assets model Qt implementation.
	class AssetsModel : public UserInterface<IAssetsModel, QAssetsModel> {
	public:

											//! Constructs AssetsModel instance.
											AssetsModel( QObject* parent );

		//! Sets the assets model delegate.
		virtual void						setDelegate( IAssetsModelDelegateWPtr value );

		//! Returns the assets model delegate.
		virtual IAssetsModelDelegateWPtr	delegate( void ) const;

	private:

		AutoPtr<QAssetsModelDispatcher>		m_dispatcher;		//!< Assets model signal dispatcher.
	};

#else
	//! Asset files model.
	class AssetsModel : public QFileSystemModel {

		Q_OBJECT

	public:

							//! Constructs AssetsModel
							AssetsModel( QObject* parent, const String& path );

	protected:

		//! Returns the MIME data for a specified index.
		virtual QMimeData*	mimeData( const QModelIndexList& indexes ) const;

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
#endif	/*	DEV_CUSTOM_ASSET_MODEL	*/

} // namespace Ui

#endif	/*	!__DC_Composer_Qt_AssetsModel_H__	*/