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

#ifndef __DC_Scene_Rendering_DebugRenderSystem_H__
#define __DC_Scene_Rendering_DebugRenderSystem_H__

#include "../RenderSystem/RenderSystem.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Renders a debug information for scene objects that reside in scene.
    class DebugRenderSystem : public RenderSystem<DebugRenderer> {
    public:

                                        DebugRenderSystem( RenderingContext& context, RenderScene& renderScene );

    protected:

		//! Emits render operations for all objects that reside in scene.
        virtual void			        emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const Camera& camera, const Transform& transform, const DebugRenderer& debugRenderer ) NIMBLE_OVERRIDE;

        //! Writes transformed vertices of a single line instance.
        void                            emitLine( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Vec3& start, const Vec3& end, const Rgba& color );

        //! Writes a wire box to an output stream.
        void                            emitWireBounds( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Bounds& bounds, const Rgba& color );

        //! Writes a light fixture to an output stream.
        void                            emitLight( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const RenderScene::LightNode& light, const Rgba& color );

        //! Writes a frustum to an output stream.
        void                            emitFrustum( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, f32 fov, f32 aspect, f32 near, f32 far, const Matrix4& transform, const Rgba& color );

        //! Writes a basis to an output stream.
        void                            emitBasis( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Matrix4& transform );

        //! Writes a single vertex to an output stream.
        void                            emitVertex( const Vec3& position, const Rgba& color );

        //! Begins a new batch.
        void                            beginBatch( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, Renderer::PrimitiveType primitive, s32 capacity );

        //! Restarts a batch with a same parameters by flushing an active buffer.
        void                            restartBatch( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack );

        //! Flushes an active batch.
        void                            flush( RenderCommandBuffer& commands, RenderStateStack& stateStack );

        //! Returns true if a batch can hold an additional amount of vertices.
        bool                            hasEnoughSpace( s32 additionalVertices ) const;
    
	private:

		//! A maximum number of vertices that can be rendered in a single batch
		enum { MaxVerticesInBatch = 1000 };

        //! A helper struct to hold an active batch state
        struct ActiveBatch {
                                        //! Constructs an ActiveBatch instance.
                                        ActiveBatch( void )
                                            : primitive( Renderer::TotalPrimitiveTypes )
                                            , size( 0 )
                                            , capacity( 0 )
                                            , stream( NULL )
                                            {
                                            }

            Renderer::PrimitiveType     primitive;          //!< An active primitive type.
            s32                         size;               //!< A total number of vertices written to a stream.
            s32                         capacity;           //!< A maximum number of vertices that can be written to a stream.
            void*                       stream;             //!< A batch vertex stream.
        };

		UbershaderPtr                   m_debugShader;		//!< A default shader that is used for debug rendering.
		VertexFormat					m_vertexFormat;		//!< A debug vertex format.
		RenderResource					m_vertexBuffer;		//!< An intermediate vertex buffer used for batching.
		RenderResource					m_inputLayout;		//!< An input layout constructed from a vertex format.
        mutable ActiveBatch             m_activeBatch;      //!< An active batch state.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_DebugRenderSystem_H__    */