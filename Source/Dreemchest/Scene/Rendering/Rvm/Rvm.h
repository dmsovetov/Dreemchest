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

#ifndef __DC_Scene_Rvm_H__
#define __DC_Scene_Rvm_H__

#include "../RenderingContext.h"
#include "Commands.h"
#include "RasterizationOptions.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Renderer virtual machine.
    class Rvm {
    public:

                                        //! Constructs Rvm instance.
                                        Rvm( RenderingContext& context, Renderer::HalWPtr hal );

        //! Flushes all accumulated commands.
        void                            execute( const Commands& commands );

        //! Resets the rendering states.
        void                            reset( void );

    private:

        //! Setups rendering states for a technique.
        void                            setTechnique( s32 value );

        //! Setups rendering states needed by a renderable.
        void                            setRenderable( s32 value );

        //! Sets the active program.
        void                            setProgram( const Program& value );

#if DEV_OLD_RENDER_COMMANDS
        //! Sets the instance data.
        void                            setInstance( const Commands::InstanceData& instance );
#else
        //! Sets an instance data.
        void                            setInstance( const Commands::DrawIndexed& instance );
#endif  /*  DEV_OLD_RENDER_COMMANDS */

        //! Sets the constant color.
        void                            setConstantColor( const Rgba& value );

        //! Sets the rendering mode.
        void                            setRenderingMode( u8 value );

#if DEV_OLD_RENDER_COMMANDS
        //! Executes the command.
        void                            executeCommand( const Commands::Rop& rop, const Commands::UserData& userData );
#endif  /*  DEV_OLD_RENDER_COMMANDS */

    private:

        //! Active rendering state is stored by this helper struct.
        struct ActiveState {
                                        //! Constructs ActiveState instance.
                                        ActiveState( void );

            s32                         technique;          //!< Active technique.
            s32                         renderable;         //!< Active renderable.
            u8                          renderingMode;      //!< Active rendering mode.
            Renderer::TriangleFace      culling;            //!< Active culling mode.
            const Program*              program;            //!< Active shader program permutation.
            Renderer::VertexBufferWPtr  vertexBuffer;       //!< Active vertex buffer.
        };

        RenderingContext&               m_context;          //!< Parent rendering context.
        Renderer::HalWPtr               m_hal;              //!< Parent rendering HAL instance.
    #if DEV_OLD_RENDER_COMMANDS
        Stack<Commands::RenderTargetState>        m_renderTarget;     //!< All render targets are pushed and popped off from here.
    #else
        Stack<Commands::PushRenderTarget>   m_renderTarget; //!< All render targets are pushed and popped off from here.
    #endif  /*  DEV_OLD_RENDER_COMMANDS */
        s32                             m_shaders[TotalLightingModels];     //!< Shader to used for each lighting model.
        RasterizationOptions            m_rasterization[TotalRenderModes];  //!< Rasterization options for each rendering mode.
        ActiveState                     m_activeState;      //!< Active rendering state.
        Vec4                            m_inputs[Program::TotalInputs];     //!< Program input registers.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rvm_H__    */