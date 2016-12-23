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


#ifndef __Examples_MeshLoader_H__
#define __Examples_MeshLoader_H__

#include <fstream>

namespace Examples
{
    DC_USE_DREEMCHEST
    using namespace Renderer;
    
    //! A loaded mesh descriptor.
    struct Mesh
    {
        Array<u16>      indices;        //!< An index buffer.
        Array<u8>       vertices;       //!< A vertex buffer.
        u8              vertexFormat;   //!< Mesh vertex format.
        PrimitiveType   primitives;     //!< A primitive type used by a mesh.
        
                        Mesh()
                            : primitives(TotalPrimitiveTypes) {}
        
        operator bool() const { return vertices.size() > 0; }
    };
    
    //! Loads an OBJ mesh from a file.
    Mesh objFromFile(const String& fileName)
    {
        std::ifstream in(Platform::Application::sharedInstance()->pathForResource(fileName));
        
        if (!in.is_open())
        {
            return Mesh();
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
        Mesh mesh;
        
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
                        return Mesh();
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

#endif  //  __Examples_MeshLoader_H__
