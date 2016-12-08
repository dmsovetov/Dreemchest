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

#ifndef __DC_Renderer_Renderer2D_H__
#define __DC_Renderer_Renderer2D_H__

#include "Renderer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer {

#if 0
    //! Renderer2D is used for displaying 2D vector graphics.
    class Renderer2D : public RefCounted {
    public:

        //! Available blending modes.
        enum BlendingMode {
              BlendingDisabled        //!< The blending is disabled.
            , BlendingAlpha            //!< Use the alpha blending.
            , BlendingAdditive        //!< Use the additive blending.
        };

        //! Render vertex.
        struct Vertex {
            f32                    x, y, z;        //!< Vertex coordinate.
            u8                    r, g, b, a;        //!< Vertex color.
            f32                    u, v;            //!< Vertex texture coordinates.

                                //! Construct Vertex instance.
                                Vertex( void )
                                    : x( 0.0f ), y( 0.0f ), z( 0.0f ) {}
        };

        //! Begins the 2D rendering.
        void                    begin( const Matrix4& value, Compare depthTest = Always, BlendingMode blending = BlendingAlpha );

        void                    end( void );

        //! Renders a fullscreen quad.
        void                    fullscreenQuad( const Texture2DPtr& texture, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders an oriented quad.
        void                    orientedQuad( const Texture2DPtr& texture, f32 x, f32 y, f32 w, f32 h, const Vec2& up, const Vec2& side, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders a filled quad with four vertices.
        void                    quad( const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders a wireframe quad with four vertices.
        void                    wireQuad( const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders a single point.
        void                    point( f32 x, f32 y, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders an array of 2d points.
        void                    points( const Vec2* points, const Rgba* colors, const f32* sizes, s32 count, s32 stride );

        //! Renders the line segment.
        void                    line( f32 x1, f32 y1, f32 x2, f32 y2, const Rgba& color1 = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ), const Rgba& color2 = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders the line segment in 3D space.
        void                    line( const Vec3& start, const Vec3& end, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders bounding box in 3D space.
        void                    wireBox( const Bounds& bounds, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders filled bounding bix in 3D space.
        void                    box( const Bounds& bounds, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders the wireframe cone.
        void                    wireCone( const Vec3& bottom, const Vec3& top, const Vec3& u, const Vec3& v, f32 radius, s32 sides, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders a wire sphere in 3D space.
        void                    wireSphere( const Vec3& center, f32 radius, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders a wire circle.
        void                    wireCircle( const Vec3 position, const Vec3& u, const Vec3& v, f32 radius, s32 sides, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Renders a wire half circle.
        void                    wireHalfCircle( const Vec3 position, const Vec3& u, const Vec3& v, const Vec3& w, f32 radius, s32 sides, const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) );

        //! Sets the blend mode.
        void                    setBlendMode( BlendingMode value );

        //! Renders the accumulated mesh vertices and resets the vertex buffer.
        void                    flush( void );

        //! Pushes the model transform matrix.
        void                    pushTransform( const Matrix4& matrix );

        //! Pops the model transform matrix.
        void                    popTransform( void );

        //! Creates a new instance of Renderer2D.
        static Renderer2DPtr    create( const HalPtr& hal, u32 maxVertexBufferSize = 1024 );

    private:

                                //! Constructs the Renderer2D instance.
                                Renderer2D( const HalPtr& hal, u32 maxVertexBufferSize );

        //! Pushes a new set of vertices to a buffer.
        void                    emitVertices( PrimitiveType primitiveType, const Texture2DPtr& texture, const Vertex* vertices, u32 count );

        //! Returns the total number of indices used for rendering a primitive set.
        static u32                indexCountForPrimitives( PrimitiveType primitiveType, u32 count );

    private:

        //! Available shader types.
        enum ShaderType {
            ShaderDefault,        //!< Renders primitives with just a color.
            ShaderTextured,        //!< Renders primitives with color & texture.
            TotalShaderTypes,    //!< Total shader types.
        };

        //! Current render state.
        struct State {
            PrimitiveType        primitiveType;        //!< Active primitive type.
            Texture2DPtr        texture;            //!< Active texture.
            BlendingMode        blending;            //!< Active blending mode.
            u32                    nVertices;            //!< Total number of emitted vertices.
            u32                    nIndices;            //!< Total number of emitted indices.

                                //! Constructs the State instance.
                                State( void )
                                    : primitiveType( TotalPrimitiveTypes ), nVertices( 0 ), nIndices( 0 ), blending( BlendingDisabled ) {}

                                //! Constructs the State instance.
                                State( PrimitiveType primitiveType, const Texture2DPtr& texture, BlendingMode blending )
                                    : primitiveType( primitiveType ), texture( texture ), nVertices( 0 ), nIndices( 0 ), blending( blending ) {}

            //! Compares two states.
            bool                operator != ( const State& other ) const { return primitiveType != other.primitiveType || texture != other.texture || blending != other.blending; }
        };

        HalPtr                    m_hal;                                    //!< Rendering HAL.
        Matrix4                    m_viewProjection;                        //!< The view-projection matrix.
        Stack<Matrix4>            m_transformStack;                        //!< Model transform stack.
        VertexBufferPtr            m_vertexBuffer;                            //!< This vertex buffer stores a generated 2D mesh.
        Vertex*                    m_vertices;                                //!< Locked vertex buffer.
        IndexBufferPtr            m_indexBuffers[TotalPrimitiveTypes];    //!< Index buffers for each primitive type.
        VertexDeclarationPtr    m_vertexFormat[TotalPrimitiveTypes];    //!< Available vertex formats.
        ShaderPtr                m_shaders[TotalShaderTypes];            //!< Loaded shaders.
        State                    m_state;                                //!< Current renderer state.
        Stack<Matrix4>            m_transform;                            //!< Transform matrix stack.
        u32                        m_maxVertexBufferSize;                    //!< The maximum size of a vertex buffer.
        u32                        m_maxIndexBufferSize;                    //!< The maximum size of an index buffer.
    };
#endif

} // namespace Renderer

DC_END_DREEMCHEST

#endif /*    !__DC_Renderer_Renderer2D_H__    */
