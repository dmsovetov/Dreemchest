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

#ifndef __DC_Composer_LocalFileSystemModel_H__
#define __DC_Composer_LocalFileSystemModel_H__

#include "AbstractTreeModel.h"

DC_BEGIN_COMPOSER

	//! File system model.
	class LocalFileSystemModel : public GenericTreeModel<QFileInfo> {

		Q_OBJECT

	Q_SIGNALS:

	#if DEV_BACKGROUND_ASSET_LOADING
		//! Emitted when the model loading was started.
		void					loadingStarted( void );

		//! Emitted when the model loading was finished.
		void					loadingFinished( void );
	#endif	/*	DEV_BACKGROUND_ASSET_LOADING	*/

		//! Emitted when a new file was added.
		void					fileAdded( const QFileInfo& file ) const;

		//! Emitted when a file was removed.
		void					fileRemoved( const QFileInfo& file ) const;

		//! Emitted when a file was changed.
		void					fileChanged( const QFileInfo& file ) const;

		//! Emitted when a file was renamed.
		void					fileRenamed( const QFileInfo& file, const QString& oldName, const QString& newName ) const;

		//! Emitted when a file was moved.
		void					fileMoved( const QFileInfo& file, const QString& oldPath, const QString& newPath ) const;

		//! Emitted when the file movement operation was failed.
		void					failedToMove( const QFileInfo& file, const QString& newPath ) const;

		//! Emitted when the rename operation was failed.
		void					failedToRename( const QFileInfo& file, const QString& newName ) const;

		//! Emitted when the remove operation was failed.
		void					failedToRemove( const QFileInfo& asset ) const;

	public:

								//! Constructs the LocalFileSystemModel instance.
		explicit				LocalFileSystemModel( QObject* parent = NULL );
		virtual					~LocalFileSystemModel( void );

		//! Returns true if the model supports background loading.
		bool					supportsBackgroundLoading( void ) const;

		//! Returns the root folder path.
		QString					rootPath( void ) const;

		//! Sets the root folder path.
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

		//! Returns an file info at specified index.
		const QFileInfo&		fileInfo( const QModelIndex& index ) const;

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

		//! Constructs an absolute file name from item.
		QString					constructAbsoluteFileName( const Item* item, const Item* parent ) const;

		//! Constructs the relative file name from item.
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
		virtual bool			shouldSkipFile( const QFileInfo& fileInfo ) const;

	protected:

		//! Returns the item flags.
		virtual Qt::ItemFlags	flags( const QModelIndex& index ) const Q_DECL_OVERRIDE;

		//! Performs the file movement
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

		QFileInfo				m_directory;	//!< Root directory.
		QFileIconProvider*		m_iconProvider;	//!< Icon provider to use.
		QFileSystemWatcher*		m_watcher;		//!< File system watcher.
		bool					m_isReadOnly;	//!< Is model a read-only.

	#if DEV_BACKGROUND_ASSET_LOADING
		QList<QFutureWatcher<void>*>	m_scanWatchers;	//!< Future watchers used for asset folder scanning.
	#endif	/*	DEV_BACKGROUND_ASSET_LOADING	*/
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_LocalFileSystemModel_H__	*/