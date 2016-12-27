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

#ifndef MAKEFOURCC
    #define MAKEFOURCC(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))
#endif

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
    
    //! Loads a cube map from a DDS file.
    CubeMap cubeFromDds(const String& fileName)
    {
        enum
        {
            // Bit flags for header
            DDS_CAPS	    = 0x00000001,
            DDS_HEIGHT	    = 0x00000002,
            DDS_WIDTH	    = 0x00000004,
            DDS_PITCH	    = 0x00000008,
            DDS_PIXELFORMAT = 0x00001000,
            DDS_MIPMAPCOUNT = 0x00020000,
            DDS_LINEARSIZE  = 0x00080000,
            DDS_DEPTH	    = 0x00800000,
            
            // Flags for pixel formats
            DDS_ALPHA_PIXELS = 0x00000001,
            DDS_ALPHA        = 0x00000002,
            DDS_FOURCC	     = 0x00000004,
            DDS_RGB	         = 0x00000040,
            DDS_RGBA         = 0x00000041,
            
            // Flags for complex caps
            DDS_COMPLEX	   = 0x00000008,
            DDS_TEXTURE	   = 0x00001000,
            DDS_MIPMAP	   = 0x00400000,
            
            // Flags for cubemaps
            DDS_CUBEMAP	          = 0x00000200,
            DDS_CUBEMAP_POSITIVEX = 0x00000400,
            DDS_CUBEMAP_NEGATIVEX = 0x00000800,
            DDS_CUBEMAP_POSITIVEY = 0x00001000,
            DDS_CUBEMAP_NEGATIVEY = 0x00002000,
            DDS_CUBEMAP_POSITIVEZ = 0x00004000,
            DDS_CUBEMAP_NEGATIVEZ = 0x00008000,
            DDS_VOLUME		      = 0x00200000
        };
        
        struct DdsPixelFormat
        {
            s32 dwSize;
            s32 dwFlags;
            s32 dwFourCC;
            s32 dwRGBBitCount;
            s32 dwRBitMask;
            s32 dwGBitMask;
            s32 dwBBitMask;
            s32 dwABitMask;
        };
        
        struct DdsHeader
        {
            s32            dwSize;
            s32            dwFlags;
            s32            dwHeight;
            s32            dwWidth;
            s32            dwPitchOrLinearSize;
            s32            dwDepth;
            s32            dwMipMapCount;
            s32            dwReserved1[11];
            DdsPixelFormat ddspf;
            s32            dwCaps;
            s32            dwCaps2;
            s32            dwCaps3;
            s32            dwCaps4;
            s32            dwReserved2;
        };
        
        DdsHeader		    ddsd;
        char				header[4];
        u32				    mipFactor;
        int					sides[6] = {
            DDS_CUBEMAP_POSITIVEX, DDS_CUBEMAP_NEGATIVEX,
            DDS_CUBEMAP_POSITIVEY, DDS_CUBEMAP_NEGATIVEY,
            DDS_CUBEMAP_POSITIVEZ, DDS_CUBEMAP_NEGATIVEZ
        };
        
        FILE* file = fopen(Platform::Application::sharedInstance()->pathForResource(fileName).c_str(), "rb");
        if (!file)
        {
            return CubeMap();
        }
        
        fread(header, 1, 4, file);
        
        if (strncmp(header, "DDS ", 4))
        {
            return CubeMap();
        }
        
        fread(&ddsd, 1, sizeof(ddsd), file);
        
        //  CheckCaps( ddsd );
        
        //int numBlocks = ((ddsd.dwWidth + 3)/4) * ((ddsd.dwHeight + 3)/4);   // number of 4*4 texel blocks
        int blockSize = 0;
        
        CubeMap cubeMap;
        s32 channels = 0;
        
        if ((ddsd.dwFlags & DDS_FOURCC) && ddsd.ddspf.dwFourCC)
        {
            switch (ddsd.ddspf.dwFourCC)
            {
                case MAKEFOURCC('D', 'X', 'T', '1'):
                    cubeMap.format		= PixelDxtc1;
                    mipFactor	= 2;
                    blockSize	= 8;
                    break;
                    
                case MAKEFOURCC('D', 'X', 'T', '3'):
                    cubeMap.format		= PixelDxtc3;
                    mipFactor	= 4;
                    blockSize	= 16;
                    break;
                    
                case MAKEFOURCC('D', 'X', 'T', '5'):
                    cubeMap.format		= PixelDxtc5;
                    mipFactor	= 4;
                    blockSize	= 16;
                    break;
                    
                    //case MAKEFOURCC('A', 'T', 'I', '1'):
                    //    format = F_ATI1N;
                    //    break;
                    //case MAKEFOURCC('A', 'T', 'I', '2'):
                    //    format = F_ATI2N;
                    //    break;
                    
                    /*case 34:			format = PixelRG16F;
                     channels	= 2;
                     blockSize	= 4;
                     break;
                     case 36:			format = PixelRgba16F;
                     channels	= 4;
                     blockSize	= 8;
                     break;
                     case 111:			format = PixelR16F;
                     channels	= 1;
                     blockSize	= 8;
                     break;
                     case 112:			format = F_RG16F;
                     channels	= 2;
                     blockSize	= 2;
                     break;
                     case 113:			format = F_RGBA16F;
                     channels	= 4;
                     blockSize	= 8;
                     break;
                     case 114:			format = F_R32F;
                     channels	= 1;
                     blockSize	= 4;
                     break;
                     case 115:			format = F_RG32F;
                     channels	= 2;
                     blockSize	= 8;
                     break;
                     case 116:			format = F_RGBA32F;
                     channels	= 4;
                     blockSize	= 16;
                     break;
                     default:			cException::Error( "cDDSTextureCubeLoader::LoadTextureCube : unhandled FOURCC" );
                     break;*/
                default:
                    NIMBLE_NOT_IMPLEMENTED
            }
        }
        else
        {
            //	compression = TC_NONE;
            
            if (ddsd.dwFlags & DDS_RGBA && ddsd.ddspf.dwRGBBitCount == 32)
            {
                channels	= 4;
                blockSize	= 4;
                cubeMap.format		= PixelRgba8;
            }
            /*else if (ddsd.dwFlags & DDS_RGB && ddsd.ddspf.dwRGBBitCount == 32)
             {
             channels	= 4;
             blockSize	= 4;
             format		= F_X8R8G8B8;
             }*/
            else if(ddsd.dwFlags & DDS_RGB && ddsd.ddspf.dwRGBBitCount == 24)
            {
                channels	= 3;
                blockSize	= 3;
                cubeMap.format		= PixelRgb8;
            }
            else
            {
                NIMBLE_NOT_IMPLEMENTED;
            }
        }
        
        NIMBLE_ABORT_IF(ddsd.dwWidth != ddsd.dwHeight, "width and height should be equal for cubemaps");
        //	_ASSERTE( ddsd.dwLinearSize != 0 );
        //_ASSERTE( ddsd.dwWidth == ddsd.dwHeight );
        
        if (channels == 0)
        {
            channels = ( ddsd.ddspf.dwFlags & DDS_ALPHA_PIXELS ) ? 4 : 3;
        }
        
        cubeMap.size = ddsd.dwWidth;
        cubeMap.mipLevels = max2(1, ddsd.dwMipMapCount);
        
        u32 imageSize = bytesPerMipChain(cubeMap.format, cubeMap.size, cubeMap.size, cubeMap.mipLevels);
        cubeMap.pixels.resize(imageSize * 6);
        
        for (s32 i = 0; i < 6; i++ )
        {
            if ((ddsd.dwCaps2 & sides[i]) == 0)
            {
                continue;
            }
            
            fread(&cubeMap.pixels[i * imageSize], 1, imageSize, file);
        }
        
        return cubeMap;
    }
    
} // namespace Examples

#endif  //  __Examples_TextureLoader_H__
