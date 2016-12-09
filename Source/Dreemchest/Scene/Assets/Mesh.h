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

#ifndef __DC_Scene_Mesh_H__
#define __DC_Scene_Mesh_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene
{
    //! Mesh data container.
    class Mesh
    {
    public:

        //! Mesh vertex.
        struct Vertex
        {
            enum { MaxTexCoords = 2 };
            Vec3                position;            //!< Vertex position.
            Vec3                normal;                //!< Vertex normal.
            Vec2                uv[MaxTexCoords];    //!< Texture coordinates.
        };

        typedef Array<Vertex>           VertexBuffer;        //!< Mesh vertex buffer type.
        typedef Array<u16>              IndexBuffer;        //!< Mesh index buffer type.

                                        //! Constructs Mesh instance.
                                        Mesh( void );

        //! Returns mesh bounds.
        const Bounds&                   bounds( void ) const;

        //! Returns a vertex format.
        const Renderer::VertexFormat&   vertexFormat( void ) const;

        //! Returns the total number of mesh chunks.
        s32                             chunkCount( void ) const;

        //! Sets the total number of mesh chunks.
        void                            setChunkCount( s32 value );

        //! Sets chunk texture name.
        void                            setTexture( s32 chunk, const String& value );

        //! Returns chunk texture name.
        const String&                   texture( s32 chunk ) const;

        //! Returns vertex buffer for a specified mesh chunk.
        const VertexBuffer&             vertexBuffer( void ) const;

        //! Sets chunk vertex buffer.
        void                            setVertexBuffer( const VertexBuffer& value );

        //! Returns index buffer for a specified mesh chunk.
        const IndexBuffer&              indexBuffer( void ) const;

        //! Sets chunk index buffer.
        void                            setIndexBuffer( const IndexBuffer& value );

        //! Updates mesh bounds.
        void                            updateBounds( void );

    private:

        //! Internal mesh chunk.
        struct Chunk
        {
            String                      texture;    //!< Mesh node texture name.
            s32                         offset;     //!< The first index of a mesh chunk.
            s32                         count;      //!< A total number of indices in a chunk.
        };

        Renderer::VertexFormat          m_vertexFormat; //!< A mesh vertex format.
        VertexBuffer                    m_vertexBuffer; //!< Mesh vertex buffer.
        IndexBuffer                     m_indexBuffer;  //!< Mesh index buffer
        Bounds                          m_bounds;       //!< Bounding box of a mesh.
        Array<Chunk>                    m_chunks;       //!< Mesh chunks.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Mesh_H__    */
