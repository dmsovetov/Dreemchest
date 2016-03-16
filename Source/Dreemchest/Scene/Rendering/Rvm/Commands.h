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

#ifndef __DC_Scene_Rvm_Commands_H__
#define __DC_Scene_Rvm_Commands_H__

#include "../RenderingContext.h"
#include "RasterizationOptions.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Rvm command buffer that is generated by render passes and executed by rvm.
    class Commands {
    public:

        //! Rendering operation emitted by scene object processing.
        struct Rop {
            //! Available command types
            enum {
                  PushRenderTarget  //!< Begins rendering to target by pushing it to a stack and setting the viewport.
                , PopRenderTarget   //!< Ends rendering to target by popping it from a stack.
                , RasterOptions     //!< Setups the rasterization options.
                , ConstantColor     //!< Sets the constant color value.
                , Shader            //!< Sets the shader to be used for a lighting model.
            };

                        //! Constructs Rop instance.
                        Rop( void );

            //! Compares two commands by their key.
            bool        operator < ( const Rop& other ) const;

            struct Bits {
                u16     userData    : 15;   //!< Command user data index.
                u16     technique   : 15;   //!< Rendering technique index.
                u16     renderable  : 15;   //!< Mesh index.
                u8      depth       : 8;    //!< Instance depth.
                u8      mode        : 3;    //!< Rendering mode.
                u8      command     : 1;    //!< Indicates that that this is not a draw call.
                u8      sequence    : 4;    //!< Command sequence number.
            };

            union {
                Bits    bits;   //!< Command feature bits.
                u64     key;    //!< The composed command key.
            };

            //! Setups this render operation as a command.
            NIMBLE_INLINE void setCommand( u8 value )
            {
                bits.command = 1;
                bits.mode    = value;
            }

            //! Returns the command type stored in this render operation.
            NIMBLE_INLINE u8 command( void ) const
            {
                DC_BREAK_IF( !bits.command, "render operation is not a command" );
                return bits.mode;
            }
        };

        NIMBLE_STATIC_ASSERT( sizeof( Rop ) == 8, "Rop size is expected to be 8 bytes" );

        //! Render target state data.
        struct RenderTargetState {
            const RenderTarget*         instance;       //!< Render target instance to be pushed.
            f32                         vp[16];         //!< The view-projection matrix to be set for a render target.
            u32                         viewport[4];    //!< The viewport inside this render target.
        };

        //! Instance data associated with a draw call.
        struct InstanceData {
            const Matrix4*              transform;      //!< Instance transform.
            Renderer::TriangleFace      culling;        //!< Triangle face culling.
        };

        //! Lighting model shader to be used.
        struct LightingModelShader {
            u8                          models;         //!< The bitmask of models that are eligible to use this shader.
            const Shader*               shader;         //!< Shader instance.
        };

        //! User data used by rendering commands.
        struct UserData {
            union {
                InstanceData            instance;       //!< Instance user data.
                RenderTargetState       rt;             //!< Render target info.
                RasterizationOptions    rasterization;  //!< Rasterization options.
                LightingModelShader     shader;         //!< Lighting shader.
                f32                     color[4];       //!< The constant color value.
            };
        };

                                        //! Constructs the Commands instance.
                                        Commands( void );

        //! Sorts the command buffer.
        void                            sort( void );

        //! Clears the command buffer.
        void                            clear( void );

        //! Dumps the generated command buffer to a log.
        void                            dump( void ) const;

        //! Returns the total number of render operations.
        s32                             size( void ) const;

        //! Returns the render operation by index.
        const Rop&                      ropAt( s32 index ) const;

        //! Returns the user data for a specified rendering operation.
        const UserData&                 ropUserData( const Rop& rop ) const;

        //! Pushes a single draw call instruction.
        NIMBLE_INLINE InstanceData*     emitDrawCall( const Matrix4* transform, s32 renderable, s32 technique, u8 mode, f32 depth );

        //! Emits the command to push a render target.
        void                            emitPushRenderTarget( RenderTargetWPtr renderTarget, const Matrix4& viewProjection, const Rect& viewport );

        //! Emits the command to pop a render target.
        void                            emitPopRenderTarget( void );

        //! Emits the rasterization options command.
        void                            emitRasterOptions( u8 renderingModes, const RasterizationOptions& options );

        //! Emits the lighting shader setup command.
        void                            emitLightingShader( u8 models, const Shader& shader );

        //! Emits the constant color operation.
        void                            emitConstantColor( const Rgba& value );

    private:

        //! Forward declaration of a command user data.
        struct UserData;

        //! Forward declaration of a render target state data.
        struct RenderTargetState;

        //! Allocates new user data for a specified rop instance instance.
        UserData*                       allocateUserData( Rop* rop );

        //! Allocates new Rop instance.
        Rop*                            allocateRop( void );

    private:

        s32                             m_sequence;     //!< Current sequence number of render operation.
        IndexAllocator<Rop>             m_operations;   //!< Allocated instructions.
        IndexAllocator<UserData>        m_userData;     //!< Allocated instruction user data.
    };

    // ** Commands::emitDrawCall
    NIMBLE_INLINE Commands::InstanceData* Commands::emitDrawCall( const Matrix4* transform, s32 renderable, s32 technique, u8 mode, f32 depth )
    {
        // Allocate new command instance
        Rop* rop = allocateRop();
        rop->bits.renderable = renderable;
        rop->bits.technique  = technique;
        rop->bits.mode       = mode;
        rop->bits.depth      = static_cast<u8>( 255 * (1.0f - (depth / 100.0f)) );

        // Allocate instance user data
        UserData* data = allocateUserData( rop );
        data->instance.transform = transform;
        data->instance.culling   = Renderer::TriangleFaceBack;

        return &data->instance;
    }

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rvm_Commands_H__    */