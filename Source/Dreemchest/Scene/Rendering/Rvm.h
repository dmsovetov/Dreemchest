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

#include "RenderingContext.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Renderer virtual machine.
    class Rvm {
    public:

        //! Rendering operation emitted by scene object processing.
        struct Rop {
            //! Available command types
            enum {
                  PushRenderTarget  //!< Begins rendering to target by pushing it to a stack and setting the viewport.
                , PopRenderTarget   //!< Ends rendering to target by popping it from a stack.
            };

                        //! Constructs Rop instance.
                        Rop( void );

            //! Compares two commands by their key.
            bool        operator < ( const Rop& other ) const;

            struct Bits {
                u16     userData    : 14;   //!< Command user data index.
                u16     technique   : 16;   //!< Rendering technique index.
                u16     renderable  : 16;   //!< Mesh index.
                u8      depth       : 8;    //!< Instance depth.
                u8      mode        : 2;    //!< Rendering mode.
                u8      command     : 1;    //!< Indicates that that this is not a draw call.
                u8      sequence    : 4;    //!< Command sequence number.
            };

            union {
                Bits    bits;   //!< Command feature bits.
                u64     key;    //!< The composed command key.
            };
        };

        NIMBLE_STATIC_ASSERT( sizeof( Rop ) == 8, "Rop size is expected to be 8 bytes" );

                                        //! Constructs Rvm instance.
                                        Rvm( RenderingContextWPtr context, const Array<RenderableHandle>& renderables, const Array<TechniqueHandle>& techniques, Renderer::HalWPtr hal );

        //! Pushes a single rendering command to a buffer.
        NIMBLE_INLINE void              emitDrawCall( const Matrix4* transform, s32 renderable, s32 technique );

        //! Emits the command to push a render target.
        void                            emitPushRenderTarget( RenderTargetWPtr renderTarget, const Matrix4& viewProjection, const Rect& viewport );

        //! Emits the command to pop a render target.
        void                            emitPopRenderTarget( void );

        //! Flushes all accumulated commands.
        void                            flush( void );

        //! Sets the constant color.
        void                            setColor( const Rgba& value );

    private:

        //! Forward declaration of a command user data.
        struct UserData;

        //! Forward declaration of a render target state data.
        struct RenderTargetState;

        //! Setups rendering states for a technique.
        void                            setTechnique( s32 value );

        //! Setups rendering states needed by a renderable.
        void                            setRenderable( s32 value );

        //! Sets the active shader.
        void                            setShader( Renderer::ShaderWPtr value );

        //! Sets the instance data.
        void                            setInstance( s32 value );

        //! Allocates new user data for a specified rop instance instance.
        UserData*                       allocateUserData( Rop* rop );

        //! Allocates new Rop instance.
        Rop*                            allocateRop( void );

        //! Executes the command.
        void                            executeCommand( const Rop& rop, const UserData& userData );

    private:

        //! Render target state data.
        struct RenderTargetState {
            const RenderTarget*         instance;       //!< Render target instance to be pushed.
            f32                         vp[16];         //!< The view-projection matrix to be set for a render target.
            u32                         viewport[4];    //!< The viewport inside this render target.
        };

        //! User data used by rendering commands.
        struct UserData {
            union {
                // Instance user data
                struct {
                    const Matrix4*      transform;  //!< Instance transform.
                } instance;
                RenderTargetState       rt;         //!< Render target info.
            };
        };

        //! Active rendering state is stored by this helper struct.
        struct ActiveState {
                                        //! Constructs ActiveState instance.
                                        ActiveState( void );

            s32                         technique;          //!< Active technique.
            s32                         renderable;         //!< Active renderable.
            Renderer::ShaderWPtr        shader;             //!< Active shader.
            Renderer::VertexBufferWPtr  vertexBuffer;       //!< Active vertex buffer.
        };

        RenderingContextWPtr            m_context;          //!< Parent rendering context.
        Renderer::HalWPtr               m_hal;              //!< Parent rendering HAL instance.
        const Array<RenderableHandle>&  m_renderables;      //!< Renderable asset handles.
        const Array<TechniqueHandle>&   m_techniques;       //!< Technique asset handles.
        Stack<RenderTargetState>        m_renderTarget;     //!< All render targets are pushed and popped off from here.
        s32                             m_sequence;         //!< Current sequence number of render operation.
        Array<Rop>                      m_operations;       //!< Actual rop buffer.
        s32                             m_operationCount;   //!< The total number of allocated render operations.
        Array<UserData>                 m_userData;         //!< Instance data.
        s32                             m_instanceCount;    //!< The total number of allocated instances.
        ActiveState                     m_activeState;      //!< Active rendering state.
        Vec4                            m_constantColor;    //!< Constant color.
    };

    // ** Rvm::emitDrawCall
    NIMBLE_INLINE void Rvm::emitDrawCall( const Matrix4* transform, s32 renderable, s32 technique )
    {
        // Allocate new command instance
        Rop* rop = allocateRop();
        rop->bits.renderable = renderable;
        rop->bits.technique  = technique;
        rop->bits.sequence   = m_sequence;

        // Allocate instance user data
        UserData* data = allocateUserData( rop );
        data->instance.transform = transform;
    }

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rvm_H__    */