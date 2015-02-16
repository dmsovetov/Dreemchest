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

#ifndef		__DC_BatchRenderer_H__
#define		__DC_BatchRenderer_H__

#include	"Renderer.h"

DC_BEGIN_DREEMCHEST

namespace renderer {

#if 0

    // ** class BatchRenderer
    class BatchRenderer {
    public:

                                BatchRenderer( dcEngine engine, dcHal hal, int maxRenderBufferSize );
        virtual                 ~BatchRenderer( void );

		void					beginMask( void );
		void					endMask( void );
		void					disableMask( int width, int height );
        void                    pushTransform( Transform transform );
        void                    popTransform( Transform transform );
		void					setBlendMode( BlendFactor src, BlendFactor dst );
        void                    setTransform( Transform transform, const cMatrix4& T, bool multiply = false );
        void                    setColorModulation( float r, float g, float b, float a );
        void                    begin2D( int width = 0, int height = 0 );
        void                    end2D( void );
        void                    flush( void );

        void                    renderVertices( PrimitiveType primitiveType, dcTexture2D texture, const sVertex *vertices, int count );

        void					renderShape( const cMatrix3& T, const stage::cShape *shape, const Rgba& fill = White, const Rgba& stroke = White );
		void					renderShapePart( const cMatrix3& T, const stage::sShapePart *shape, bool renderVertices, const Rgba& fill = White, const Rgba& stroke = White );
        void                    renderLine( float x1, float y1, float x2, float y2, const Rgba& colorA = White, const Rgba& colorB = White );
        void                    renderWireRect( float x, float y, float w, float h, const Rgba& color = White );
        void					renderCenteredRect( float x, float y, float w, float h, const Rgba& fill = White, const Rgba& stroke = White );
        void                    renderBounds( const bounds& bounds, const Rgba& color = White );
        void					renderCircle( float x, float y, float r, const Rgba& fill = White, const Rgba& stroke = White );
		void					renderRect( float x, float y, float w, float h, const Rgba& fill = White, const Rgba& stroke = White );
		void					renderPolygon( float x, float y, const Vec2 *vertices, int vertexCount, const Rgba& fill = White, const Rgba& stroke = White );
		void					renderCapsule( float x, float y, const Vec2& c1, const Vec2& c2, float r1, float r2, const rgba& fill = White, const Rgba& stroke = White );

        void                    renderOrientedQuad( dcTexture2D texture, float x, float y, float w, float h, const Vec2& up, const Vec2& side, const rgba& color = White );
		void                    renderOrientedQuadUV( dcTexture2D texture, float x, float y, float w, float h, const Vec2& up, const Vec2& side, const vec4& uv, const Rgba& color = White );
        void                    renderPoint( dcTexture2D texture, float x, float y, float size, const rgba& color = White );
        void                    renderPointArray( dcTexture2D texture, const void *positions, const void *colors, const void *sizes, int stride, int count );
        void                    renderLineStrip( const void *positions, const void *colors, int stride, int count, float alpha = 1.0f );
        void                    renderThickLine( dcTexture2D texture, float x1, float y1, float x2, float y2, float sizeA, float sizeB, const rgba& colorA = White, const Rgba& colorB = White );
        void                    renderThickLineStrip( dcTexture2D texture, const void *positions, const void *colors, const void *sizes, int stride, int count, float alpha = 1.0f );

    private:

        void                    initialize( void );
        int                     calculateIndexCount( ePrimitiveType primType, int vertexCount ) const;
        void                    setRenderState( ePrimitiveType primType, dcTexture2D texture );

    private:

        enum { TempBufferSize = 4096 };

        dcEngine                m_engine;
        dcHal                   m_hal;
        
        dcVertexDeclaration     m_vertexFormat, m_pointFormat;
		dcVertexBuffer          m_vertexBuffer;
		dcIndexBuffer           m_indexBuffer, m_fanIndexBuffer;
        int						m_totalVertices, m_totalIndices;
        int						m_maxVertices, m_maxIndices;

        sVertex                 m_temporary[TempBufferSize];
        sVertex*                m_vertices;
		u16*                    m_indices;
        u16*                    m_fanIndices;

        dcRasterizerState       m_rasterState2D;
        dcBlendState            m_blendState2D;
		dcRasterizerState		m_enableColorWrite, m_disableColorWrite;
		dcDepthStencilState     m_beginMask, m_endMask, m_disableMask;

        dcTexture2D             m_immediateTexture;
        ePrimitiveType          m_immediatePrimitive;
        std::stack<cMatrix4>    m_transformStack[TransformTotal];
        cMatrix4                m_transform[TransformTotal];
        vec4                    m_color;
		int						m_mask;
    };

#endif

} // namespace renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_BatchRenderer_H__ */