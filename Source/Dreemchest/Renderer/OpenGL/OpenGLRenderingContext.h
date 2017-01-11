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

#ifndef __DC_Renderer_OpenGLRenderingContext_H__
#define __DC_Renderer_OpenGLRenderingContext_H__

#include "../RenderingContext.h"
#include "OpenGLHal.h"
#include "OpenGL2.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! A base class for all OpenGL-based rendering contexts.
    class OpenGLRenderingContext : public RenderingContext
    {
    protected:
        
        //! A container type to store cached uniform locations.
        typedef HashMap<String32, GLint> UniformLocations;
        
    #if DEV_RENDERER_UNIFORM_CACHING
        //! A constant buffer revision to skip redundant uniform uploads.
        typedef union
        {
            struct
            {
                ResourceId  id;                 //!< A last known constant buffer id.
                u16         value;              //!< A last known revision of that buffer.
            };
            u32             hash;               //!< A composed hash value.
        } Revision;
    #endif  //  #if DEV_RENDERER_UNIFORM_CACHING
        
        //! A shader program permutation entry.
        struct Permutation
        {
            struct Uniform
            {
                String32        hash;               //!< A hash value of a uniform name.
                GLenum          type;               //!< A uniform data type.
                GLint           size;               //!< A uniform size.
                u8              index;              //!< A uniform sampler of constant buffer index.
                GLint           location;           //!< A uniform location.
            };
            
        #if DEV_RENDERER_UNIFORM_CACHING
            mutable Revision    buffers[State::MaxConstantBuffers];
        #endif  //  #if DEV_RENDERER_UNIFORM_CACHING
            
            GLuint              program;            //!< An OpenGL program identifier.
            Array<Uniform>      uniforms;           //!< Active uniforms extracted from a program by means of an introspection.
        };
        
                                                //! Constructs an OpenGLRenderingContext instance.
                                                OpenGLRenderingContext(RenderViewPtr view);
        
        //! Searches for a suitable shader program permutation.
        bool                                    lookupPermutation(ResourceId program, PipelineFeatures features, const Permutation** permutation) const;
        
        //! Puts a new permutation to a cache.
        const Permutation*                      savePermutation(ResourceId program, PipelineFeatures features, GLuint id);
        
        //! Searches for a framebuffer and marks it a used.
        s32                                     acquireFramebuffer(u16 width, u16 height);
        
        //! Releases a framebuffer.
        void                                    releaseFramebuffer(s32 index);
        
        //! Adds a new framebuffer.
        s32                                     allocateFramebuffer(GLuint id, GLuint depth, u16 width, u16 height);
        
        //! Deletes a program and all it's permutations.
        void                                    deleteProgram(ResourceId id);
        
    protected:
        
        //! A texture instance descriptor.
        struct Texture
        {
            GLuint              id;             //!< A texture OpenGL id.
            GLenum              target;         //!< A texture target (1D, 2D, 3D, Cube).
            
                                //! Constructs an instance of texture
                                Texture()
                                    : id(0), target(0) {}
            
                                //! Constructs an instance of texture
                                Texture(GLuint id, GLenum target)
                                    : id(id), target(target) {}
        };
        
        //! A texture instance info.
        struct TextureInfo
        {
            u16                 width;          //!< A texture width.
            u16                 height;         //!< A texture height.
            PixelFormat         pixelFormat;    //!< A pixel format.
            TextureType         type;           //!< A texture type.
        };
        
        //! A framebuffer descriptor.
        struct Framebuffer
        {
            GLuint              id;             //!< A framebuffer id.
            GLuint              depth;          //!< An attached depth renderbuffer.
            u16                 width;          //!< A framebuffer width.
            u16                 height;         //!< A framebuffer height.
            bool                acquired;       //!< Indicates that a framebuffer is in use now.
        };

        //! Adds a version directive to a shader source code
        class ShaderVersionPreprocessor : public ShaderPreprocessor
        {
        public:

                                            ShaderVersionPreprocessor(s32 version);

            //! Adds a #version directive to a shader source code
            virtual bool                    preprocess(const RenderingContext& renderingContext, String& shader) const;

        private:

            s32                             m_version;  //!< A shader version to be inserted.
        };
    
        //! A container type to store program permutations.
        typedef HashMap<PipelineFeatures, Permutation> ProgramPermutations;
        
        FixedArray<GLuint>                      m_vertexBuffers;        //!< Allocated vertex buffers.
        FixedArray<GLuint>                      m_indexBuffers;         //!< Allocated index buffers.
        FixedArray<Texture>                     m_textures;             //!< Allocated textures.
        FixedArray<TextureInfo>                 m_textureInfo;          //!< A corresponding texture info array.
        List<Texture_>                          m_transientTextures;    //!< A list of free textures.
        FixedArray<Framebuffer>                 m_framebuffers;         //!< An array of allocated framebuffers.
        mutable FixedArray<ProgramPermutations> m_permutations;         //!< Available program permutations.
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_OpenGLRenderingContext_H__    */
