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

#ifndef __DC_Scene_Rendering_StreamedRenderPass_H__
#define __DC_Scene_Rendering_StreamedRenderPass_H__

#include "RenderPass.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Streamed render pass generates a mesh vertex and index buffers on a CPU side and submits it to GPU upon a state change.
	class StreamedRenderPassBase : public RenderPassBase {
	public:

                                        //! Constructs StreamedRenderPassBase instance.
                                        StreamedRenderPassBase( RenderingContext& context, RenderScene& renderScene, s32 maxVerticesInBatch );

        //! Flushes a generated vertex stream.
        virtual void                    end( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack ) NIMBLE_OVERRIDE;

    protected:

        //! Writes transformed vertices of a single line instance.
        void                            emitLine( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Vec3& start, const Vec3& end, const Rgba& color );

        //! Writes a wire box to an output stream.
        void                            emitWireBounds( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Bounds& bounds, const Rgba& color );

        //! Writes a wire box base on it's vertices to an output stream.
        void                            emitWireBounds( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Vec3 vertices[8], const Rgba& color );

        //! Writes a frustum to an output stream.
        void                            emitFrustum( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, f32 fov, f32 aspect, f32 near, f32 far, const Matrix4& transform, const Rgba& color );

        //! Writes a basis to an output stream.
        void                            emitBasis( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Matrix4& transform );

        //! Writes a set of vertices to an output stream.
        void                            emitVertices( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, Renderer::PrimitiveType primitive, const Vec3* positions, const Rgba* colors, s32 count );

        //! Begins a new batch.
        void                            beginBatch( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, Renderer::PrimitiveType primitive, s32 capacity );

        //! Restarts a batch with a same parameters by flushing an active buffer.
        void                            restartBatch( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, Renderer::PrimitiveType primitive );

        //! Flushes an active batch.
        void                            flush( RenderCommandBuffer& commands, RenderStateStack& stateStack );

        //! Returns true if a batch can hold an additional amount of vertices.
        bool                            hasEnoughSpace( s32 additionalVertices ) const;
    
	private:

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

            Renderer::PrimitiveType     primitive;              //!< An active primitive type.
            s32                         size;                   //!< A total number of vertices written to a stream.
            s32                         capacity;               //!< A maximum number of vertices that can be written to a stream.
            void*                       stream;                 //!< A batch vertex stream.
        };

		VertexFormat					m_vertexFormat;		    //!< A debug vertex format.
		RenderResource					m_vertexBuffer;		    //!< An intermediate vertex buffer used for batching.
		RenderResource					m_inputLayout;		    //!< An input layout constructed from a vertex format.
        s32                             m_maxVerticesInBatch;   //! A maximum number of vertices that can be rendered in a single batch
        mutable ActiveBatch             m_activeBatch;          //!< An active batch state.
	};

	//! Generic render pass class.
	template<typename TRenderable>
	class StreamedRenderPass : public StreamedRenderPassBase {
	public:

							        //! Constructs StreamedRenderPass instance.
							        StreamedRenderPass( RenderingContext& context, RenderScene& renderScene, s32 maxVerticesInBatch );

		//! Processes an entity family and generates a vertex stream from it.
		virtual void		        emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack ) NIMBLE_OVERRIDE;

	protected:

		//! Emits render operation for a single renderable entity.
		virtual void		        emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack, const Ecs::Entity& entity, const TRenderable& renderable, const Transform& transform ) NIMBLE_ABSTRACT;

	protected:

		Ecs::IndexPtr		        m_index;        //!< Entity index to be processed.
	};

	// ** StreamedRenderPass::StreamedRenderPass
	template<typename TRenderable>
	StreamedRenderPass<TRenderable>::StreamedRenderPass( RenderingContext& context, RenderScene& renderScene, s32 maxVerticesInBatch )
        : StreamedRenderPassBase( context, renderScene, maxVerticesInBatch )
	{
		m_index = renderScene.scene()->ecs()->requestIndex( "StreamedRenderPass<>", Ecs::Aspect::all<TRenderable, Transform>() );
	}

	// ** StreamedRenderPass::emitRenderOperations
	template<typename TRenderable>
	void StreamedRenderPass<TRenderable>::emitRenderOperations( RenderFrame& frame, RenderCommandBuffer& commands, RenderStateStack& stateStack )
	{
        // Get the entity set from index
		const Ecs::EntitySet& entities = m_index->entities();

        // Process each entity
		for( Ecs::EntitySet::const_iterator i = entities.begin(), end = entities.end(); i != end; ++i ) {
			emitRenderOperations( frame, commands, stateStack, *i->get(), *(*i)->get<TRenderable>(), *(*i)->get<Transform>() );
		}
	}

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rendering_StreamedRenderPass_H__    */