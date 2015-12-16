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

#include "../Composer.h"

DC_BEGIN_COMPOSER

	//! Assets model interface.
	class AssetsModel : public IInterface {
	public:

		//! Base class for all asset events.
		struct AssetEvent {
									AssetEvent( const FileInfoPtr& file )
										: file( file ) {}

			FileInfoPtr				file;	//!< Asset file info.
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

			s32						leftToScan;	//!< The total number of items left to scan.
		};

		//! Sets root model path.
		virtual void				setRootPath( const String& value ) = 0;

		//! Sets the read-only flag.
		virtual void				setReadOnly( bool value ) = 0;

		//! Returns assets UUID for a specified asset file.
		virtual String				uuid( const FileInfoWPtr& assetFile ) const = 0;

		//! Updates meta data for specified asset file.
		virtual void				setMetaData( const FileInfoWPtr& assetFile, const Io::KeyValue& data ) = 0;

		//! Returns meta data for a specified asset file.
		virtual Io::KeyValue		metaData( const FileInfoWPtr& assetFile ) const = 0;

		//! Returns meta data for a specified asset file name.
		virtual Io::KeyValue		metaData( const String& assetFileName ) const = 0;
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_AssetsModel_H__	*/