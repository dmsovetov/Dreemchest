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

#ifndef __DC_Scene_Renderable_H__
#define __DC_Scene_Renderable_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Renderable asset is used by a renderer as a source of vertex & index buffers.
    class Renderable {
    public:

                                    //! Constructs Renderable instance.
                                    Renderable( void );

        //! Returns the primitive type.
        Renderer::PrimitiveType     primitiveType( void ) const;

        //! Returns the total number of chunks.
        s32                         chunkCount( void ) const;

        //! Sets the primitive type.
        void                        setPrimitiveType( Renderer::PrimitiveType value );

        //! Returns the vertex buffer instance.
        Renderer::VertexBufferWPtr  vertexBuffer( s32 chunk ) const;

        //! Sets the vertex buffer instance.
        void                        setVertexBuffer( s32 chunk, Renderer::VertexBufferPtr value );

        //! Returns the index buffer instance.
        Renderer::IndexBufferWPtr   indexBuffer( s32 chunk ) const;

        //! Sets the index buffer instance.
        void                        setIndexBuffer( s32 chunk, Renderer::IndexBufferPtr value );

    private:

        //! Helper type to store vertex & index buffer for a single mesh chunk
        struct Chunk {
            Renderer::VertexBufferPtr   vertexBuffer;   //!< Vertex buffer instance.
            Renderer::IndexBufferPtr    indexBuffer;    //!< Index buffer instance.       
        };

        Renderer::PrimitiveType     m_primitiveType;    //!< Rendered primitive type.
        Array<Chunk>                m_chunks;           //!< Renderable chunks.
    };

    //! Texture asset is used by renderer and is constructed from image asset.
    class Texture {
    public:

        //! Returns the texture instance.
        Renderer::TextureWPtr       texture( void ) const;

        //! Sets the texture instance.
        void                        setTexture( Renderer::TexturePtr value );

    private:

        Renderer::TexturePtr        m_texture;  //!< Texture 2D instance.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Renderable_H__    */