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

#ifndef __DC_Composer_ImageImporter_H__
#define __DC_Composer_ImageImporter_H__

#include "AssetImporter.h"

DC_BEGIN_COMPOSER

namespace Importers {

	//! Base class for all image importers.
	class ImageImporter : public AssetImporter {
	public:

		//! Writes the imported image to file.
		virtual bool		import( IFileSystemWPtr fs, const Asset& asset, const io::Path& path ) DC_DECL_OVERRIDE;

	private:

		//! Imports an image from file.
		virtual bool		importImage( IFileSystemWPtr fs, const Asset& asset, const io::Path& path ) = 0;

	protected:

		//! Imported image.
		struct Image {
			u16				width;		//!< Image width.
			u16				height;		//!< Image height.
			u8				channels;	//!< Image channels.
			Array<u8>		pixels;		//!< Image pixel buffer.
		};

		Image				m_image;	//!< The imported image.
	};

	//! Imports the TGA image.
	class ImageImporterTGA : public ImageImporter {
	public:

		//! Performs importing of a TGA image.
		virtual bool		importImage( IFileSystemWPtr fs, const Asset& asset, const io::Path& path ) DC_DECL_OVERRIDE;

	private:

		//! Reads 16-bit image.
		void				readPixels16( io::StreamPtr stream, Image& image, u16 width, u16 height ) const;

		//! Reads 24/32-bit image.
		void				readPixels( io::StreamPtr stream, Image& image, u16 width, u16 height, u8 bitsPerPixel ) const;

		//! Swaps red & blue channels.
		void				swapChannels( Image& image ) const;
	};

#ifdef HAVE_TIFF

	//! Imports the TIF image.
	class ImageImporterTIF : public ImageImporter {
	public:

		//! Performs importing of a TIF image.
		virtual bool		importImage( IFileSystemWPtr fs, const Asset& asset, const io::Path& path ) DC_DECL_OVERRIDE;
	};

#endif	/*	HAVE_TIFF	*/

} // namespace Importers

DC_END_COMPOSER

#endif	/*	!__DC_Composer_ImageImporter_H__	*/