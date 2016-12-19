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

#include <fstream>

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

// ** tgaFromFile
Descriptor tgaFromFile(const String& fileName)
{
    // Open the file
    FILE* file = fopen(fileName.c_str(), "rb");
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
    
} // namespace ImageLoader
    
// ------------------------------------------------------------------------- MeshLoader ------------------------------------------------------------------------ //

namespace MeshLoader
{
    
// ** objFromFile
Descriptor objFromFile(const String& fileName)
{
    std::ifstream in(fileName);
    
    if (!in.is_open())
    {
        return Descriptor();
    }
    
    String line;
    Descriptor mesh;
    
    Array<Vec3> vertices, normals;
    Array<Vec2> uvs;

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
                s32 vi[3], ti[3], ni[3];
                
                if (sscanf(line.c_str(), "f %i %i %i", &vi[0], &vi[1], &vi[2]) == 3)
                {
                    mesh.format = VertexFormat::Position;
                    
                    for (s32 i = 0; i < 3; i++)
                    {
                        Vertex v;
                        v.position = vertices[vi[i] - 1];
                        mesh.vertices.push_back(v);
                    }
                }
                else if(sscanf(line.c_str(), "f %i/%i %i/%i %i/%i", &vi[0], &ti[0], &vi[1], &ti[1], &vi[2], &ti[2]) == 6)
                {
                    mesh.format = VertexFormat::Position | VertexFormat::TexCoord0;
                    
                    for (s32 i = 0; i < 3; i++)
                    {
                        Vertex v;
                        v.position = vertices[vi[i] - 1];
                        v.uv = uvs[ti[i] - 1];
                        mesh.vertices.push_back(v);
                    }
                }
                else if(sscanf(line.c_str(), "f %i//%i %i//%i %i//%i", &vi[0], &ni[0], &vi[1], &ni[1], &vi[2], &ni[2]) == 6)
                {
                    mesh.format = VertexFormat::Position | VertexFormat::Normal;
                    
                    for (s32 i = 0; i < 3; i++)
                    {
                        Vertex v;
                        v.position = vertices[vi[i] - 1];
                        v.normal = normals[ni[i] - 1];
                        mesh.vertices.push_back(v);
                    }
                }
                else if(sscanf(line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i", &vi[0], &ti[0], &ni[0], &vi[1], &ti[1], &ni[1], &vi[2], &ti[2], &ni[2]) == 9)
                {
                    mesh.format = VertexFormat::Position | VertexFormat::Normal | VertexFormat::TexCoord0;
                    
                    for (s32 i = 0; i < 3; i++)
                    {
                        Vertex v;
                        v.position = vertices[vi[i] - 1];
                        v.uv = uvs[ti[i] - 1];
                        v.normal = normals[ni[i] - 1];
                        mesh.vertices.push_back(v);
                    }
                }
                else
                {
                    LogWarning("obj", "%s", "unhandled vertex format\n");
                    return Descriptor();
                }
            }
                break;
        }
    }
    in.close();
    
    mesh.format = VertexFormat::Position | VertexFormat::Normal | VertexFormat::TexCoord0;

    return mesh;
}
    
} // namespace MeshLoader
    
} // namespace Renderer

DC_END_DREEMCHEST
