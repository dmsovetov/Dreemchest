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

namespace scene {

	//! 2D image asset
	class Image : public Asset {
	friend class AssetBundle;
	public:

								ClassEnableTypeInfoSuper( Image, Asset )

		//! Returns image width.
		u16						width( void ) const;

		//! Returns image height.
		u16						height( void ) const;

		//! Loads image from file.
		virtual bool			loadFromStream( const io::StreamPtr& stream );

		//! Unloads image from memory.
		virtual void			unload( void );

		//! Constructs HAL texture from loaded data.
		renderer::TexturePtr	requestTexture( renderer::Hal* hal );

	private:

								//! Constructs Image instance.
								Image( AssetBundle* bundle, const String& name, ImageFormat format, u16 width, u16 height );

	private:

		u16						m_width;	//!< Image width.
		u16						m_height;	//!< Image height.
		u8						m_channels;	//!< Image channels.
		ImageFormat				m_format;	//!< Image format.

		renderer::TexturePtr	m_texture;	//!< Loaded texture.
		AutoPtr<u8>				m_pixels;	//!< Pixel buffer.
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Image_H__    */