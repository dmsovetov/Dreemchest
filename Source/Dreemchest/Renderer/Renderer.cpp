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

#include "Renderer.h"
#include "RenderingContext.h"
#include "VertexFormat.h"
#include "../Platform/Application.h"

#include <fstream>

#define MAKEFOURCC(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))

DC_BEGIN_DREEMCHEST

namespace Renderer {
    
// ---------------------------------------------------------------- RenderingApplicationDelegate ---------------------------------------------------------------- //
    
// ** RenderingApplicationDelegate::initialize
bool RenderingApplicationDelegate::initialize(s32 width, s32 height)
{
    // First create a window
    if (!WindowedApplicationDelegate::initialize(width, height))
    {
        return false;
    }
    
    // Create a rendering view.
    RenderViewPtr view = createOpenGLView(m_window->handle(), Renderer::PixelD24S8);
    
    // Now create a rendering context that is attached to a created window instance
#if 0
    // Now create the main renderer interface called HAL (hardware abstraction layer).
    m_renderingContext = createDeprecatedRenderingContext(view, Hal::create(OpenGL, view));
#else
    m_renderingContext = createOpenGL2RenderingContext(view);
#endif
    
    return m_renderingContext.valid();
}

// ** RenderingApplicationDelegate::handleWindowUpdate
void RenderingApplicationDelegate::handleWindowUpdate(const Platform::Window::Update& e)
{
    handleRenderFrame(e);
}
    
// ------------------------------------------------------------------------- ImageLoader ------------------------------------------------------------------------ //

// ** bytesPerBlock
u32 bytesPerBlock(PixelFormat format)
{
    switch (format)
    {
        case PixelDxtc1:    return 8;
        case PixelDxtc3:    return 16;
        case PixelDxtc5:    return 16;
        case PixelPvrtc2:   return 32;
        case PixelPvrtc4:   return 16;
        default:            NIMBLE_NOT_IMPLEMENTED
    }
    
    return 0;
}

// ** bytesPerPixel
u32 bytesPerPixel(PixelFormat format)
{
    switch (format)
    {
        case PixelLuminance8:   return 1;
        case PixelRgb8:         return 3;
        case PixelRgba8:        return 4;
        case PixelR16F:         return 2;
        case PixelRg16F:        return 4;
        case PixelRgba16F:      return 8;
        case PixelR32F:         return 4;
        case PixelRg32F:        return 8;
        case PixelRgba32F:      return 16;
        default:                NIMBLE_NOT_IMPLEMENTED
    }
    
    return 0;
}

// ** bytesPerMip
u32 bytesPerMip(PixelFormat format, u16 width, u16 height)
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

// ** bytesPerMipChain
u32 bytesPerMipChain(PixelFormat format, u16 width, u16 height, u16 mipLevels)
{
    u32 size = 0;
    
    for (s32 i = 0; i < mipLevels; i++)
    {
        size += bytesPerMip(format, width, height);
        width = width >> 1;
        height = height >> 1;
    }
    
    return size;
}
    
namespace ImageLoader
{
    
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
Descriptor tgaFromFile(const String& fileName)
{
    // Open the file
    FILE* file = fopen(Platform::Application::sharedInstance()->pathForResource(fileName).c_str(), "rb");
    NIMBLE_BREAK_IF(!file, "failed to open file");
    
    if(!file)
    {
        return Descriptor();
    }

    Descriptor image;
    
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
    
    fclose(file);
    
    return image;
}
    
// ** cubeFromDds
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
    
    DdsHeader		ddsd;
    char				header[4];
    uint				mipFactor;
    //uint				imageSize, bufferSize;
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
    
} // namespace ImageLoader
    
// ------------------------------------------------------------------------- MeshLoader ------------------------------------------------------------------------ //

namespace MeshLoader
{
    
// ** objFromFile
Descriptor objFromFile(const String& fileName)
{
    std::ifstream in(Platform::Application::sharedInstance()->pathForResource(fileName));
    
    if (!in.is_open())
    {
        return Descriptor();
    }
    
    struct Face
    {
        s32      v[4];
        s32      vn[4];
        s32      vt[4];
        
        Face()
        {
            memset(v, 0, sizeof(v));
            memset(vn, 0, sizeof(vn));
            memset(vt, 0, sizeof(vt));
        }
    };
    
    String line;
    Descriptor mesh;
    
    Array<Vec3>  vertices, normals;
    Array<Vec2>  uvs;
    Array<Face>  faces;
    VertexFormat vertexFormat(0);
    s32          verticesPerFace = 0;

    while (!in.eof())
    {
        getline(in, line);
        
        switch (line[0])
        {
            case 'v':
                switch (line[1])
                {
                    case 't':
                        uvs.push_back(Vec2());
                        sscanf(line.c_str(), "vt %f %f", &uvs.back().x, &uvs.back().y);
                        break;
                    case 'n':
                        normals.push_back(Vec3());
                        sscanf(line.c_str(), "vn %f %f %f", &normals.back().x, &normals.back().y, &normals.back().z);
                        break;
                    default:
                        vertices.push_back(Vec3());
                        sscanf(line.c_str(), "v %f %f %f", &vertices.back().x, &vertices.back().y, &vertices.back().z);
                        break;
                }
                break;
            case 'f':
            {
                // Detect a primitive type
                if (verticesPerFace == 0)
                {
                    for (size_t i = 0, length = line.length(); i < length; i++)
                    {
                        if (line[i] == ' ')
                        {
                            verticesPerFace++;
                            
                            while (line[i] == ' ')
                            {
                                i++;
                            }
                        }
                    }
                }
                
                Face f;

                if (verticesPerFace == 3)
                {
                    if (sscanf(line.c_str(), "f %i %i %i", &f.v[0], &f.v[1], &f.v[2]) == 3)
                    {
                        vertexFormat = VertexFormat::Position;
                    }
                    else if(sscanf(line.c_str(), "f %i/%i %i/%i %i/%i", &f.v[0], &f.vt[0], &f.v[1], &f.vt[1], &f.v[2], &f.vt[2]) == 6)
                    {
                        vertexFormat = VertexFormat::Position | VertexFormat::TexCoord0;
                    }
                    else if(sscanf(line.c_str(), "f %i//%i %i//%i %i//%i", &f.v[0], &f.vn[0], &f.v[1], &f.vn[1], &f.v[2], &f.vn[2]) == 6)
                    {
                        vertexFormat = VertexFormat::Position | VertexFormat::Normal;
                    }
                    else if(sscanf(line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i", &f.v[0], &f.vt[0], &f.vn[0], &f.v[1], &f.vt[1], &f.vn[1], &f.v[2], &f.vt[2], &f.vn[2]) == 9)
                    {
                        vertexFormat = VertexFormat::Position | VertexFormat::Normal | VertexFormat::TexCoord0;
                    }
                    else
                    {
                        LogWarning("obj", "unhandled vertex format in line '%s'\n", line.c_str());
                        continue;
                    }
                }
                else if(verticesPerFace == 4)
                {
                    if (sscanf(line.c_str(), "f %i %i %i %i", &f.v[0], &f.v[1], &f.v[2], &f.v[3]) == 4)
                    {
                        vertexFormat = VertexFormat::Position;
                    }
                    else if(sscanf(line.c_str(), "f %i/%i %i/%i %i/%i %i/%i", &f.v[0], &f.vt[0], &f.v[1], &f.vt[1], &f.v[2], &f.vt[2], &f.v[3], &f.vt[3]) == 8)
                    {
                        vertexFormat = VertexFormat::Position | VertexFormat::TexCoord0;
                    }
                    else if(sscanf(line.c_str(), "f %i//%i %i//%i %i//%i %i//%i", &f.v[0], &f.vn[0], &f.v[1], &f.vn[1], &f.v[2], &f.vn[2], &f.v[3], &f.vn[3]) == 8)
                    {
                        vertexFormat = VertexFormat::Position | VertexFormat::Normal;
                    }
                    else if(sscanf(line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i", &f.v[0], &f.vt[0], &f.vn[0], &f.v[1], &f.vt[1], &f.vn[1], &f.v[2], &f.vt[2], &f.vn[2], &f.v[3], &f.vt[3], &f.vn[3]) == 12)
                    {
                        vertexFormat = VertexFormat::Position | VertexFormat::Normal | VertexFormat::TexCoord0;
                    }
                    else
                    {
                        LogWarning("obj", "unhandled vertex format in line '%s'\n", line.c_str());
                        continue;
                    }
                }
                else
                {
                    LogWarning("obj", "unsupported face size '%d'\n", verticesPerFace);
                    return Descriptor();
                }
                faces.push_back(f);
            }
                break;
        }
    }
    in.close();

    // Build final mesh
    mesh.vertexFormat = vertexFormat;
    mesh.primitives   = verticesPerFace == 3 ? PrimTriangles : PrimQuads;
    mesh.vertices.resize(faces.size() * verticesPerFace * vertexFormat.vertexSize());
    
    for (size_t f = 0, n = faces.size(); f < n; f++)
    {
        const Face& face = faces[f];
        
        for (s32 v = 0; v < verticesPerFace; v++)
        {
            vertexFormat.setVertexAttribute(VertexFormat::Position, vertices[face.v[v] - 1], &mesh.vertices[0], f * verticesPerFace + v);
            
            if (vertexFormat & VertexFormat::Normal)
            {
                vertexFormat.setVertexAttribute(VertexFormat::Normal, normals[face.vn[v] - 1], &mesh.vertices[0], f * verticesPerFace + v);
            }
            if (vertexFormat & VertexFormat::TexCoord0)
            {
                vertexFormat.setVertexAttribute(VertexFormat::TexCoord0, uvs[face.vt[v] - 1], &mesh.vertices[0], f * verticesPerFace + v);
            }
        }
    }

    return mesh;
}
    
} // namespace MeshLoader
    
namespace Presets
{

namespace CBuffer {
    
// ** Projection::Layout
const UniformElement Projection::Layout[] =
{
      { "transform", UniformElement::Matrix4, offsetof(Projection, transform) }
    , { "viewport",  UniformElement::Vec4,    offsetof(Projection, viewport)  }
    , { NULL }
};
    
// ** Projection::ortho
Projection Projection::ortho(s32 left, s32 right, s32 bottom, s32 top, f32 zNear, f32 zFar)
{
    Projection projection;
    projection.transform = Matrix4::ortho(left, right, bottom, top, zNear, zFar);
    projection.viewport  = Vec4(left, bottom, right - left, top - bottom);
    return projection;
}

// ** Projection::perspective
Projection Projection::perspective(f32 fov, s32 width, s32 height, f32 zNear, f32 zFar)
{
    Projection projection;
    projection.transform = Matrix4::perspective(fov, static_cast<f32>(width) / height, zNear, zFar);
    projection.viewport  = Vec4(0, 0, width, height);
    return projection;
}
    
// ** Camera::Layout
const UniformElement Camera::Layout[] =
{
      { "transform", UniformElement::Matrix4, offsetof(Camera, transform) }
    , { "position",  UniformElement::Vec3,    offsetof(Camera, position)  }
    , { NULL }
};
    
// ** Camera::lookAt
Camera Camera::lookAt(const Vec3& position, const Vec3& target)
{
    Camera camera;
    camera.position = position;
    camera.transform = Matrix4::lookAt(position, target, Vec3::axisY());
    return camera;
}

// ** Instance::Layout
const UniformElement Instance::Layout[] =
{
      { "transform",        UniformElement::Matrix4, offsetof(Instance, transform)        }
    , { "inverseTranspose", UniformElement::Matrix4, offsetof(Instance, inverseTranspose) }
    , { NULL }
};

// ** Instance::fromTransform
Instance Instance::fromTransform(const Matrix4& transform)
{
    Instance instance;
    instance.transform = transform;
    instance.inverseTranspose = transform.inversed().transposed();
    return instance;
}
    
} // namespace CBuffer
    
// ** Shaders::VertexIdentity
const String Shaders::VertexIdentity =
    "void main()                    \n"
    "{                              \n"
    "   gl_Position = gl_Vertex;    \n"
    "}                              \n"
    ;

// ** VertexBuffers::Triangle
const f32 VertexBuffers::Triangle[9] =
{
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     0.0f,  1.0f, 0.0f,
};

// ** VertexBuffers::FullscreenQuad
const f32 VertexBuffers::FullscreenQuad[12] =
{
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
};

// ** VertexBuffers::QuatterScreenQuad
const f32 VertexBuffers::QuatterScreenQuad[12] =
{
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
};
    
// ** VertexBuffers::TexturedQuad
const f32 VertexBuffers::TexturedQuad[20] =
{
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
};

// ** IndexBuffers::TriangulatedQuad
const u16 IndexBuffers::TriangulatedQuad[6] =
{
    0, 1, 2,
    0, 2, 3,
};
    
} // namespace Presets
    
} // namespace Renderer

DC_END_DREEMCHEST
