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

#ifndef __DC_Scene_Image_H__
#define __DC_Scene_Image_H__

#include "Assets.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! 2D image data
	class Image {
	public:

								//! Constructs an empty Image instance.
								Image( void );

		//! Returns image width.
		s32						width( void ) const;

		//! Sets image width.
		void					setWidth( s32 value );

		//! Returns image height.
		s32						height( void ) const;

		//! Sets image height.
		void					setHeight( s32 value );

		//! Returns the bytes per pixel value.
		s32						bytesPerPixel( void ) const;

		//! Sets the bytes per pixel value.
		void					setBytesPerPixel( s32 value );

		//! Returns the total number of mip levels.
		s32						mipLevelCount( void ) const;

		//! Sets the total number of mip levels.
		void					setMipLevelCount( s32 value );

		//! Returns image pixels at specified mip level.
		const ByteArray&		mipLevel( s32 index ) const;

		//! Set image pixels at specified mip level.
		void					setMipLevel( s32 index, const ByteArray& value );

		//! Returns the mip level width.
		s32						mipLevelWidth( s32 index ) const;

		//! Returns the mip level height.
		s32						mipLevelHeight( s32 index ) const;

	private:

		s32						m_width;			//!< Image base mip level width.
		s32						m_height;			//!< Image base mip level height.
		s32						m_bytesPerPixel;	//!< Number of bytes used to encode a single image pixel.
		u8						m_channels;			//!< Image channels.
		Array<ByteArray>		m_mips;				//!< Actual image pixels.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Image_H__    */