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


#ifndef __Examples_TextureLoader_H__
#define __Examples_TextureLoader_H__

namespace Examples
{
    DC_USE_DREEMCHEST
    using namespace Renderer;

    typedef Array<u8> Surface;
    
    //! A loaded image descriptor.
    struct Image
    {
        u16         width;      //!< An image width.
        u16         height;     //!< An image height.
        PixelFormat format;     //!< An image pixel format.
        Surface     pixels;     //!< Loaded image pixels.
        
        Image( void )
        : width(0), height(0) {}
        operator bool() const { return pixels.size() > 0; }
    };
    
    struct CubeMap
    {
        u16         size;       //!< A cube map size.
        s32         mipLevels;  //!< A total number of mip levels.
        PixelFormat format;     //!< A stored image pixel format
        Surface     pixels;     //!< Six sides of a cube map.
    };
        
    // ** convertRgbToBgr
    void convertBgrToRgb(Array<u8>& image, s32 channels)
    {
        for (size_t i = 0, n = image.size(); i < n; i += channels)
        {
            u8 temp      = image[i];
            image[i]     = image[i + 2];
            image[i + 2] = temp;
        }
    }
    
    // ** calculateImageSize
    u32 calculateImageSize(u16 width, u16 height, s32 totalMipLevels, PixelFormat format)
    {
        u32 size = 0;
        
        switch (format)
        {
            case PixelDxtc1:
            case PixelDxtc3:
            case PixelDxtc5:
                size = ((width + 3) >> 2) * ((height + 3) >> 2) * bytesPerBlock(format);
                break;
                
            case PixelPvrtc2:
                size = (width / 8) * (height / 4) * ((bytesPerBlock(format) * 4) / 8);
                break;
                
            case PixelPvrtc4:
                size = (width / 4) * (height / 4) * ((bytesPerBlock(format) * 4) / 8);
                break;
                
            default:
                size = width * height * bytesPerPixel(format);
        }
        
        return size;
    }
    
    // ** tgaFromFile
    Image tgaFromFile(const String& fileName)
    {
        // Open the file
        FILE* file = fopen(Platform::Application::sharedInstance()->pathForResource(fileName).c_str(), "rb");
        NIMBLE_BREAK_IF(!file, "failed to open file");
        
        if(!file)
        {
            return Image();
        }
        
        Image image;
        
        // Read the TGA header
        u8 len, imageType, bitsPerPixel;
        fread(&len, 1, 1, file);
        fseek(file, 1, SEEK_CUR);
        
        fread(&imageType, 1, 1, file);
        fseek(file, 9, SEEK_CUR);
        
        fread(&image.width, 1, 2, file);
        fread(&image.height, 1, 2, file);
        fread(&bitsPerPixel, 1, 1, file);
        fseek(file, len + 1, SEEK_CUR);
        
        // Read an image
        NIMBLE_BREAK_IF(imageType == 10, "RLE-encoded images are not supported");
        
        if(bitsPerPixel == 16)
        {
            image.format = PixelRgb8;
            image.pixels.resize(3 * image.width * image.height);
            
            for (s32 i = 0; i < image.width * image.height; i++)
            {
                u16 pixel;
                fread(&pixel, 1, 2, file);
                
                image.pixels[i * 3 + 0] = ( pixel        & 0x1F) << 3;
                image.pixels[i * 3 + 1] = ((pixel >> 5)  & 0x1F) << 3;
                image.pixels[i * 3 + 2] = ((pixel >> 10) & 0x1F) << 3;
            }
        }
        else if(imageType == 3)
        {
            image.format = PixelLuminance8;
            image.pixels.resize(image.width * image.height);
            fread(&image.pixels[0], 1, image.width * image.height, file);
        }
        else
        {
            s32 bytesPerPixel = bitsPerPixel / 8;
            image.format = bytesPerPixel == 3 ? PixelRgb8 : PixelRgba8;
            image.pixels.resize(image.width * image.height * bytesPerPixel);
            fread(&image.pixels[0], 1, image.pixels.size(), file);
            convertBgrToRgb(image.pixels, bytesPerPixel);
        }
        
        s32      pitch = image.width * bitsPerPixel / 8;
        UPtr<u8> temp  = new u8[pitch];
        
        for (s32 i = 0; i < image.height / 2; i++)
        {
            memcpy(temp.get(), &image.pixels[i * pitch], pitch);
            memcpy(&image.pixels[i * pitch], &image.pixels[(image.height - i - 1) * pitch], pitch);
            memcpy(&image.pixels[(image.height - i - 1) * pitch], temp.get(), pitch);
        }
        
        fclose(file);
        
        return image;
    }
    
} // namespace Examples

#endif  //  __Examples_TextureLoader_H__
