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

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Renderer virtual machine.
    class Rvm {
    public:

        //! Rendering operation emitted by scene object processing.
        struct Command {
                        //! Constructs command instance.
                        Command( void );

            //! Compares two commands by their key.
            bool        operator < ( const Command& other ) const;

            struct Bits {
                u32     instance    : 14;   //!< Instance data index (instance transform, uniform, etc).
                u32     technique   : 18;   //!< Rendering technique index.
                u32     renderable  : 18;   //!< Mesh index.
                u32     depth       : 12;   //!< Instance depth.
                u32     mode        : 2;    //!< Rendering mode.
            };

            union {
                Bits    bits;   //!< Command feature bits.
                u64     key;    //!< The composed command key.
            };
        };

                                        //! Constructs Rvm instance.
                                        Rvm( const Array<RenderableHandle>& renderables, const Array<TechniqueHandle>& techniques, Renderer::HalWPtr hal );

        //! Pushes a single rendering command to a buffer.
        void                            push( const Matrix4* transform, s32 renderable, s32 technique );

        //! Flushes all accumulated commands.
        void                            flush( void );

        //! Sets the view projection matrix.
        void                            setViewProj( const Matrix4& value );

        //! Sets the constant color.
        void                            setColor( const Rgba& value );

    private:

        //! Setups rendering states for a technique.
        void                            setTechnique( s32 value );

        //! Setups rendering states needed by a renderable.
        void                            setRenderable( s32 value );

        //! Sets the active shader.
        void                            setShader( Renderer::ShaderWPtr value );

        //! Sets the instance data.
        void                            setInstance( s32 value );

    private:

        //! Instance data used by rendering.
        struct Instance {
            const Matrix4*              transform;      //!< Instance transform.
        };

        //! Active rendering state is stored by this helper struct.
        struct ActiveState {
                                        //! Constructs ActiveState instance.
                                        ActiveState( void );

            s32                         technique;          //!< Active technique.
            s32                         renderable;         //!< Active renderable.
            Renderer::ShaderWPtr        shader;             //!< Active shader.
        };

        Renderer::HalWPtr               m_hal;              //!< Parent rendering HAL instance.
        const Array<RenderableHandle>&  m_renderables;      //!< Renderable asset handles.
        const Array<TechniqueHandle>&   m_techniques;       //!< Technique asset handles.
        Array<Command>                  m_commands;         //!< Actual command buffer.
        Array<Instance>                 m_instances;        //!< Instance data.
        ActiveState                     m_activeState;      //!< Active rendering state.
        Matrix4                         m_viewProjection;   //!< Active view projection matrix.
        Vec4                            m_constantColor;    //!< Constant color.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rvm_H__    */