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

#ifndef __DC_Renderer_RenderFrame_H__
#define __DC_Renderer_RenderFrame_H__

#include "RenderState.h"
#include "Ubershader.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! Render frame contains all required frame data captured by a render scene.
    class RenderFrame
    {
    public:
                        
                                                //! Constructs a RenderFrame instance.
                                                RenderFrame( void );

        //! Returns a total number of captured command buffers.
        s32                                     commandBufferCount( void ) const;

        //! Returns a command buffer at specified index.
        const CommandBuffer&                    commandBufferAt( s32 index ) const;

        //! Creates a new command buffer.
        CommandBuffer&                          createCommandBuffer( void );

        //! Returns a state stack.
        StateStack&                             stateStack( void );

        //! Returns a root command buffer.
        const CommandBuffer&                    entryPoint( void ) const;
        CommandBuffer&                          entryPoint( void );

        //! Interns a memory buffer that will by used during a frame rendering.
        const void*                             internBuffer( const void* data, s32 size );

        //! Allocates a block of memory that is used during a frame rendering.
        void*                                   allocate( s32 size );
        
        //! Clears all data recorded by this frame.
        void                                    clear( void );

    private:

        //! Container type to store recorded command buffers.
        typedef Array<CommandBufferUPtr>        Commands;

        CommandBuffer*                          m_entryPoint;           //!< A root command buffer.
        Commands                                m_commandBuffers;       //!< An array of recorded commands buffers.
        StateStack                              m_stateStack;           //!< Current state stack.
        LinearAllocator                         m_allocator;            //!< A linear allocator used by a frame renderers.
    };

    //! Returns a total number of captured command buffers.
    NIMBLE_INLINE s32 RenderFrame::commandBufferCount( void ) const
    {
        return static_cast<s32>( m_commandBuffers.size() );
    }

    //! Returns a command buffer at specified index.
    NIMBLE_INLINE const CommandBuffer& RenderFrame::commandBufferAt( s32 index ) const
    {
        NIMBLE_ABORT_IF( index < 0 || index >= commandBufferCount(), "index is out of range" );
        return *m_commandBuffers[index].get();
    }

} // namespace Renderer

DC_END_DREEMCHEST

#endif    /*    !__DC_Renderer_RenderFrame_H__    */
