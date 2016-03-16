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
        void                            setProgram( const ShaderSource::Permutation& value );

        //! Sets the instance data.
        void                            setInstance( const Commands::InstanceData& instance );

        //! Sets the constant color.
        void                            setConstantColor( const Rgba& value );

        //! Sets the rendering mode.
        void                            setRenderingMode( u8 value );

        //! Executes the command.
        void                            executeCommand( const Commands::Rop& rop, const Commands::UserData& userData );

    private:

        //! Active rendering state is stored by this helper struct.
        struct ActiveState {
                                        //! Constructs ActiveState instance.
                                        ActiveState( void );

            s32                         technique;          //!< Active technique.
            s32                         renderable;         //!< Active renderable.
            u8                          renderingMode;      //!< Active rendering mode.
            Renderer::TriangleFace      culling;            //!< Active culling mode.
            ShaderSource::Permutation*  program;            //!< Active shader program permutation.
            Renderer::VertexBufferWPtr  vertexBuffer;       //!< Active vertex buffer.
        };

        RenderingContext&               m_context;          //!< Parent rendering context.
        Renderer::HalWPtr               m_hal;              //!< Parent rendering HAL instance.
        Stack<Commands::RenderTargetState>        m_renderTarget;     //!< All render targets are pushed and popped off from here.
        const ShaderSource*             m_shaders[TotalLightingModels];     //!< Shader to used for each lighting model.
        RasterizationOptions            m_rasterization[TotalRenderModes];  //!< Rasterization options for each rendering mode.
        ActiveState                     m_activeState;      //!< Active rendering state.
        Vec4                            m_constantColor;    //!< Constant color.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rvm_H__    */