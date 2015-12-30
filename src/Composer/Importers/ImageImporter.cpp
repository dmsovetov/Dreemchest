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

#include "ImageImporter.h"

#ifdef HAVE_TIFF
	#include <tiffio.h>
#endif	/*	HAVE_TIFF	*/

DC_BEGIN_COMPOSER

namespace Importers {

// -------------------------------------------------- ImageImporter ------------------------------------------------- //

// ** ImageImporter::import
bool ImageImporter::import( FileSystemQPtr fs, const Io::Path& sourceFileName, const Io::Path& destinationFileName )
{
	// Import image from file
	if( !importImage( fs, sourceFileName ) ) {
		return false;
	}

	// Write imported image to a binary stream
	Io::StreamPtr stream = Io::DiskFileSystem::open( destinationFileName, Io::BinaryWriteStream );
	DC_BREAK_IF( !stream.valid() );

	stream->write( &m_image.width, 2 );
	stream->write( &m_image.height, 2 );
	stream->write( &m_image.channels, 1 );
	stream->write( &m_image.pixels[0], m_image.pixels.size() );

	return true;
}

// ------------------------------------------------ ImageImporterTGA ------------------------------------------------ //

// ** ImageImporterTGA::importImage
bool ImageImporterTGA::importImage( FileSystemQPtr fs, const Io::Path& sourceFileName )
{
	// Open the file
	Io::StreamPtr stream = Io::DiskFileSystem::open( sourceFileName );
	DC_BREAK_IF( !stream.valid() );

	if( !stream.valid() ) {
		return false;
	}

	// Read the TGA header
	u8 len, imageType, bitsPerPixel;
	u16 width, height;
	stream->read( &len, 1 );
	stream->setPosition( 1, Io::SeekCur );

	stream->read( &imageType, 1 );
	stream->setPosition( 9, Io::SeekCur );

	stream->read( &width, 2 );
	stream->read( &height, 2 );
	stream->read( &bitsPerPixel, 1 );
	stream->setPosition( len + 1, Io::SeekCur );

	// Read an image
	DC_BREAK_IF( imageType == 10 );	// RLE-encoded images are not supported yet.

	if( bitsPerPixel == 16 ) {
		readPixels16( stream, m_image, width, height );
	} else {
		readPixels( stream, m_image, width, height, bitsPerPixel );
	}

	return true;
}

// ** ImageImporterTGA::readPixels16
void ImageImporterTGA::readPixels16( Io::StreamPtr stream, Image& image, u16 width, u16 height ) const
{
	image.channels	= 3;
	image.width		= width;
	image.height	= height;
	image.pixels.resize( image.channels * width * height );

	for( s32 i = 0; i < width * height; i++ ) {
		u16 pixel;
		stream->read( &pixel, 2 );

		image.pixels[i * 3 + 0] = ( pixel        & 0x1F) << 3;
		image.pixels[i * 3 + 1] = ((pixel >> 5)  & 0x1F) << 3;
		image.pixels[i * 3 + 2] = ((pixel >> 10) & 0x1F) << 3;
	}
}

// ** ImageImporterTGA::readPixels
void ImageImporterTGA::readPixels( Io::StreamPtr stream, Image& image, u16 width, u16 height, u8 bitsPerPixel ) const
{
	image.channels	= bitsPerPixel / 8;
	image.width		= width;
	image.height	= height;
	image.pixels.resize( width * height * image.channels );

	stream->read( &image.pixels[0], image.pixels.size() );

	swapChannels( image );
}

// ** ImageImporterTGA::swapChannels
void ImageImporterTGA::swapChannels( Image& image ) const
{
	for( s32 i = 0, n = ( s32 )image.pixels.size(); i < n; i += image.channels ) {
		u8 temp = image.pixels[i];
		image.pixels[i]		= image.pixels[i + 2];
		image.pixels[i + 2] = temp;
	}
}

#ifdef HAVE_TIFF

// ------------------------------------------------ ImageImporterTIF ------------------------------------------------ //

// ** ImageImporterTIF::import
bool ImageImporterTIF::importImage( FileSystemQPtr fs, const Io::Path& sourceFileName )
{
	// Open the TIFF file
	TIFF* tif = TIFFOpen( sourceFileName.c_str(), "r" );
	DC_BREAK_IF( !tif );

	// Extract image dimensions
	u32 width, height;
	TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &width );
	TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &height );

	// Allocate the pixelbuffer
	u32 npixels = width * height;
	u32* raster = ( u32* )_TIFFmalloc( npixels * sizeof( u32 ) );
	DC_BREAK_IF( !raster );

	// Read image
	s32 result = TIFFReadRGBAImage( tif, width, height, raster, 0 );
	DC_BREAK_IF( result != 1 );

	// Initialize the imported image
	m_image.width    = width;
	m_image.height   = height;
	m_image.channels = 4;
	m_image.pixels.resize( width * height * 4 );

	for( u32 i = 0; i < npixels; i++ ) {
		m_image.pixels[i * 4 + 0] = TIFFGetR( raster[i] );
		m_image.pixels[i * 4 + 1] = TIFFGetG( raster[i] );
		m_image.pixels[i * 4 + 2] = TIFFGetB( raster[i] );
		m_image.pixels[i * 4 + 3] = TIFFGetA( raster[i] );
	}

	// Free allocated buffer
	_TIFFfree( raster );

	// Close TIFF image
	TIFFClose( tif );

	return true;
}

#endif	/*	HAVE_TIFF	*/

} // namespace Importers

DC_END_COMPOSER