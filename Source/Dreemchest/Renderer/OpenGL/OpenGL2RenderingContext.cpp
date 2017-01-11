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

#include "OpenGL2RenderingContext.h"
#include "../VertexBufferLayout.h"
#include "../VertexFormat.h"
#include "../Commands/CommandBuffer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ** createOpenGL2RenderingContext
RenderingContextPtr createOpenGL2RenderingContext(RenderViewPtr view)
{
    if (!OpenGL2::initialize())
    {
        return RenderingContextPtr();
    }

    return RenderingContextPtr(DC_NEW OpenGL2RenderingContext(view));
}
    
// ------------------------------------------------------ OpenGL2RenderingContext::ShaderPreprocessor -------------------------------------------------- //

// ** OpenGL2RenderingContext::ShaderPreprocessor::generateBufferDefinition
String OpenGL2RenderingContext::ShaderPreprocessor::generateBufferDefinition(const RenderingContext& renderingContext, const String& type, const String& name, s32 slot) const
{
    // A static array to map from an element type to GLSL data type.
    static const String s_types[] = 
    {
          "int"
        , "float"
        , "vec2"
        , "vec3"
        , "vec4"
        , "mat4"
    };

    // First find a uniform layout by name
    const UniformElement* elements = renderingContext.findUniformLayout(type);
    
    if (elements == NULL)
    {
        return "";
    }

    String definition = "struct " + type + " {\n";

    for (const UniformElement* element = elements; element->name; element++)
    {
        String def;
        
        if (element->size > 0)
        {
            def = String(element->name.value()) + "[" + toString(element->size) + "]";
        }
        else
        {
            def = element->name.value();
        }
        
        definition += "\t" + s_types[element->type] + " " + def + ";\n";
    }
    definition += "}; uniform " + type + " cb_" + toString(slot) + ";\n#define " + name + " cb_" + toString(slot) + "\n";

    return definition;
}

// ----------------------------------------------------------------- OpenGL2RenderingContext ----------------------------------------------------------- //

// ** OpenGL2RenderingContext::OpenGL2RenderingContext
OpenGL2RenderingContext::OpenGL2RenderingContext(RenderViewPtr view)
    : OpenGLRenderingContext(view)
    , m_activeInputLayout(NULL)
{
    if (m_view.valid())
    {
        m_view->makeCurrent();
    }

    m_shaderLibrary.addPreprocessor(DC_NEW ShaderPreprocessor);
    m_shaderLibrary.addPreprocessor(DC_NEW ShaderVersionPreprocessor(110));
    
    m_textures.emplace(0, Texture());
    m_constantBuffers.emplace(0, ConstantBuffer());
    m_vertexBuffers.emplace(0, 0);
    m_indexBuffers.emplace(0, 0);
}
    
// ** OpenGL2RenderingContext::acquireTexture
ResourceId OpenGL2RenderingContext::acquireTexture(u8 type, u16 width, u16 height, PixelFormat format)
{
    // First search for a free render target
    for (List<Texture_>::const_iterator i = m_transientTextures.begin(), end = m_transientTextures.end(); i != end; ++i)
    {
        // Get a texture info by id
        const TextureInfo& info = m_textureInfo[*i];
        
        // Does the render target format match the requested one?
        if (type == info.type && info.width == width && info.height == height && info.pixelFormat == format)
        {
            return *i;
        }
    }
    
    static CString s_textureType[] =
    {
        "1D", "2D", "3D", "cube"
    };
    
    LogVerbose("renderingContext", "allocating a transient %s texture of size %dx%d\n", s_textureType[type], width, height);
    return allocateTexture(type, NULL, width, height, 1, format, FilterLinear);
}

// ** OpenGL2RenderingContext::releaseTexture
void OpenGL2RenderingContext::releaseTexture(ResourceId id)
{
    m_transientTextures.push_back(Texture_::create(id));
}
    
// ** OpenGL2RenderingContext::allocateTexture
ResourceId OpenGL2RenderingContext::allocateTexture(u8 type, const void* data, u16 width, u16 height, u16 mipLevels, u16 pixelFormat, u8 filter, ResourceId id)
{
    // Allocate a resource identifier if it was not passed
    if (!id)
    {
        id = allocateIdentifier<Texture_>();
    }
    
    Texture texture;
    PixelFormat   format        = static_cast<PixelFormat>(pixelFormat);
    TextureFilter textureFilter = static_cast<TextureFilter>(filter);
    
    // Create a texture instance according to a type.
    switch (type)
    {
        case TextureType2D:
            texture.id     = OpenGL2::Texture::create2D(data, width, width, mipLevels, format, textureFilter);
            texture.target = GL_TEXTURE_2D;
            break;
            
        case TextureTypeCube:
            texture.id     = OpenGL2::Texture::createCube(data, width, mipLevels, format, textureFilter);
            texture.target = GL_TEXTURE_CUBE_MAP;
            break;
            
        default:
            NIMBLE_NOT_IMPLEMENTED
    }
    
    // Construct a texture info
    TextureInfo textureInfo;
    textureInfo.width       = width;
    textureInfo.height      = height;
    textureInfo.pixelFormat = format;
    textureInfo.type        = static_cast<TextureType>(type);
    
    // Save a created texture identifier and a texture info
    m_textures.emplace(id, texture);
    m_textureInfo.emplace(id, textureInfo);
    
    return id;
}

// ** OpenGL2RenderingContext::executeCommandBuffer
void OpenGL2RenderingContext::executeCommandBuffer(const RenderFrame& frame, const CommandBuffer& commands)
{
    PipelineState    pipelineState;
    GLuint           id;
    
    for (s32 i = 0, n = commands.size(); i < n; i++)
    {
        // Get a render operation at specified index
        const OpCode& opCode = commands.opCodeAt(i);
        
        // Perform a draw call
        switch(opCode.type)
        {
            case OpCode::Clear:
                OpenGL2::clear(opCode.clear.color, opCode.clear.mask, opCode.clear.depth, opCode.clear.stencil);
                break;
                
            case OpCode::Execute:
                execute(frame, *opCode.execute.commands);
                break;
                
            case OpCode::UploadConstantBuffer:
            {
                ConstantBuffer& constantBuffer = m_constantBuffers[opCode.upload.id];
                NIMBLE_ABORT_IF(static_cast<s32>(constantBuffer.data.size()) < opCode.upload.buffer.size, "buffer is too small");
                memcpy(&constantBuffer.data[0], opCode.upload.buffer.data, opCode.upload.buffer.size);
            #if DEV_RENDERER_UNIFORM_CACHING
                constantBuffer.revision.value++;
            #endif  //  #if DEV_RENDERER_UNIFORM_CACHING
            }
                break;
                
            case OpCode::UploadVertexBuffer:
                OpenGL2::Buffer::subData(GL_ARRAY_BUFFER, m_vertexBuffers[opCode.upload.id], 0, opCode.upload.buffer.size, opCode.upload.buffer.data);
                break;
                
            case OpCode::CreateInputLayout:
                m_inputLayouts.emplace(opCode.createInputLayout.id, createVertexBufferLayout(opCode.createInputLayout.format));
                break;
                
            case OpCode::CreateTexture:
                allocateTexture(  opCode.createTexture.type
                                , opCode.createTexture.buffer.data
                                , opCode.createTexture.width
                                , opCode.createTexture.height
                                , opCode.createTexture.mipLevels
                                , opCode.createTexture.format
                                , opCode.createTexture.filter
                                , opCode.createTexture.id
                                );
                break;
                
            case OpCode::CreateIndexBuffer:
                id = OpenGL2::Buffer::create(GL_ARRAY_BUFFER, opCode.createBuffer.buffer.data, opCode.createBuffer.buffer.size, GL_DYNAMIC_DRAW);
                m_indexBuffers.emplace(opCode.createBuffer.id, id);
                break;
                
            case OpCode::CreateVertexBuffer:
                id = OpenGL2::Buffer::create(GL_ELEMENT_ARRAY_BUFFER, opCode.createBuffer.buffer.data, opCode.createBuffer.buffer.size, GL_DYNAMIC_DRAW);
                m_vertexBuffers.emplace(opCode.createBuffer.id, id);
                break;
                
            case OpCode::CreateConstantBuffer:
            {
                ConstantBuffer constantBuffer;
                constantBuffer.layout = m_uniformLayouts[opCode.createBuffer.layout];
                constantBuffer.data.resize(opCode.createBuffer.buffer.size);
            #if DEV_RENDERER_UNIFORM_CACHING
                constantBuffer.revision.id    = opCode.createBuffer.id;
                constantBuffer.revision.value = 0;
            #endif  //  #if DEV_RENDERER_UNIFORM_CACHING
                
                // Cache uniform names
                if (opCode.createBuffer.buffer.data)
                {
                    memcpy(&constantBuffer.data[0], opCode.createBuffer.buffer.data, opCode.createBuffer.buffer.size);
                }
                m_constantBuffers.emplace(opCode.createBuffer.id, constantBuffer);
            }
                break;
                
            case OpCode::DeleteConstantBuffer:
                m_constantBuffers.emplace(opCode.id, ConstantBuffer());
                releaseIdentifier(RenderResourceType::ConstantBuffer, opCode.id);
                m_pipeline.resetConstantBuffer(opCode.id);
                break;
                
            case OpCode::DeleteProgram:
                m_pipeline.resetProgram(opCode.id);
                deleteProgram(opCode.id);
                releaseIdentifier(RenderResourceType::Program, opCode.id);
                break;
                
            case OpCode::AcquireTexture:
            {
                ResourceId id = acquireTexture(opCode.transientTexture.type, opCode.transientTexture.width, opCode.transientTexture.height, opCode.transientTexture.format);
                loadTransientResource(opCode.transientTexture.id, id);
            }
                break;
                
            case OpCode::ReleaseTexture:
            {
                ResourceId id = transientResource(opCode.transientTexture.id);
                releaseTexture(id);
                unloadTransientResource(opCode.transientTexture.id);
            }
                break;
                
            case OpCode::RenderToTexture:
            case OpCode::RenderToTransientTexture:
            {
                DC_CHECK_GL_CONTEXT;
                DC_CHECK_GL;
                
                // Get a transient resource id by a slot
                ResourceId id = opCode.type == OpCode::RenderToTexture ? opCode.renderToTextures.id : transientResource(opCode.renderToTextures.id);
                NIMBLE_ABORT_IF(!id, "invalid transient identifier");
                
                // Get a render target by an id.
                const Texture&     texture = m_textures[id];
                const TextureInfo& info    = m_textureInfo[id];
                
                // Save current viewport
                GLint prevViewport[4];
                glGetIntegerv(GL_VIEWPORT, prevViewport);
                
                // Save the bound framebuffer
                GLint prevFramebuffer;
                glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFramebuffer);
                
                // Acquire the framebuffer
                s32 framebufferIndex = acquireFramebuffer(info.width, info.height);
                
                if (!framebufferIndex)
                {
                    LogVerbose("opengl2", "allocating a framebuffer of size %dx%d\n", info.width, info.height);
                    GLuint id = OpenGL2::Framebuffer::create();
                    GLuint depth = OpenGL2::Framebuffer::renderbuffer(id, info.width, info.height, GL_DEPTH_ATTACHMENT, OpenGL2::textureInternalFormat(PixelD24X8));
                    framebufferIndex = allocateFramebuffer(id, depth, info.width, info.height);
                }

                OpenGL2::Framebuffer::bind(m_framebuffers[framebufferIndex].id);
                
                if (opCode.renderToTextures.side == 255)
                {
                    OpenGL2::Framebuffer::texture2D(texture.id, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0);
                }
                else
                {
                    OpenGL2::Framebuffer::texture2D(texture.id, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + opCode.renderToTextures.side, 0);
                }

                // Set a viewport before executing an attached command buffer
                const NormalizedViewport& viewport = opCode.renderToTextures.viewport;
                glViewport(viewport.x * info.width, viewport.y * info.height, viewport.width * info.width, viewport.height * info.height);
                
                // Execute an attached command buffer
                execute(frame, *opCode.renderToTextures.commands);
                
                // Release an acquired framebuffer
                releaseFramebuffer(framebufferIndex);
                
                // Disable the framebuffer
                OpenGL2::Framebuffer::bind(prevFramebuffer);
                
                // Restore the previous viewport
                glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
            }
                break;

            case OpCode::DrawIndexed:
                // Apply rendering states bundled with a draw call
                applyStates(pipelineState, opCode.drawCall.stateBlock->states, opCode.drawCall.stateBlock->size);
                pipelineState.activateUserFeatures(opCode.drawCall.stateBlock->features);

                // Now update the pipeline state
                compilePipelineState(pipelineState);
                
                // Finally select a matching shader permutation
                applyProgramPermutation(pipelineState);
                
                // Perform an actual draw call
                OpenGL2::drawElements(opCode.drawCall.primitives, GL_UNSIGNED_SHORT, opCode.drawCall.first, opCode.drawCall.count);
                break;
                
            case OpCode::DrawPrimitives:
                // Apply rendering states bundled with a draw call
                applyStates(pipelineState, opCode.drawCall.stateBlock->states, opCode.drawCall.stateBlock->size);
                pipelineState.activateUserFeatures(opCode.drawCall.stateBlock->features);

                // Now update the pipeline state
                compilePipelineState(pipelineState);

                // Finally select a matching shader permutation
                applyProgramPermutation(pipelineState);
                
                // Perform an actual draw call
                OpenGL2::drawArrays(opCode.drawCall.primitives, opCode.drawCall.first, opCode.drawCall.count);
                break;
                
            default:
                NIMBLE_NOT_IMPLEMENTED;
        }
    }
}

// ** OpenGL2RenderingContext::compilePipelineState
void OpenGL2RenderingContext::compilePipelineState(const PipelineState& state)
{
    // Bind an indexed buffer
    OpenGL2::Buffer::bind(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffers[state.indexBuffer()]);
    
    // Bind a vertex buffer
    OpenGL2::Buffer::bind(GL_ARRAY_BUFFER, m_vertexBuffers[state.vertexBuffer()]);
    
    // Bind texture samplers
    for (s32 i = 0; i < State::MaxTextureSamplers; i++)
    {
        const Texture& texture = m_textures[state.texture(i)];
        OpenGL2::Texture::bind(texture.target, texture.id, i);
    }
    
    // Switch the input layout
    {
        // Disable the previous input layout
        if (m_activeInputLayout)
        {
            OpenGL2::disableInputLayout(*m_activeInputLayout);
        }
        
        // Now enable a new one
        OpenGL2::enableInputLayout(NULL, *state.inputLayout());
        m_activeInputLayout = state.inputLayout();
    }
    
    // Alpha test
    OpenGL2::setAlphaTest(state.alphaTestFunction(), state.alphaTestRef());
    
    // Polygon offset
    OpenGL2::setPolygonOffset(state.polygonOffsetFactor(), state.polygonOffsetUnits());
    
    // Rasterization
    OpenGL2::setRasterization(state.rasterization());
    
    // Blending
    OpenGL2::setBlending(state.sourceBlendFactor(), state.destBlendFactor());
    
    // Color mask
    OpenGL2::setColorMask(state.colorMask());

    // Depth state
    OpenGL2::setDepthState(state.depthTestFunction(), state.depthWrite());

    // Cull face
    OpenGL2::setCullFace(state.cullFace());

    // Stencil op
    OpenGL2::Stencil::setOperations(state.stencilStencilFail(), state.stencilDepthFail(), state.stencilPass());

    // Stencil function
    OpenGL2::Stencil::setFunction(state.stencilFunction(), state.stencilRef(), state.stencilMask());
}
    
// ** OpenGL2RenderingContext::applyProgramPermutation
void OpenGL2RenderingContext::applyProgramPermutation(const PipelineState& state)
{
    ResourceId       program  = state.program();
    PipelineFeatures features = state.features();
    
    // Get an active program
    NIMBLE_ABORT_IF(!program && !m_defaultProgram, "no valid program set and no default one specified");
    
    // Use a default program if nothing was set by a user
    if (!program)
    {
        program = m_defaultProgram;
    }
    
    // Switch the program one the pipeline state was changed
    const Permutation* permutation = compileShaderPermutation(program, features, state.featureLayout());
    OpenGL2::Program::use(permutation->program);
    
    // Update all uniforms
    updateUniforms(state, permutation);
}
    
// ** OpenGL2RenderingContext::updateUniforms
void OpenGL2RenderingContext::updateUniforms(const PipelineState& state, const Permutation* permutation)
{
    struct UniformPointer
    {
        static const void* findByName(const Permutation::Uniform& uniform, const FixedArray<ConstantBuffer>& constantBuffers, const PipelineState& state)
        {
            ResourceId id = state.constantBuffer(uniform.index);
            NIMBLE_ABORT_IF(!id, "no constant buffer bound");
            const ConstantBuffer& cbuffer = constantBuffers[id];

            for (const UniformElement* constant = &cbuffer.layout[0]; constant->name; constant++)
            {
                if (constant->name.hash() == uniform.hash)
                {
                    return &cbuffer.data[constant->offset];
                }
            }
            
            NIMBLE_NOT_IMPLEMENTED
            return NULL;
        }
    };
    
    const void* pointer = NULL;
    
    for (size_t i = 0, n = permutation->uniforms.size(); i < n; i++)
    {
        const Permutation::Uniform& uniform = permutation->uniforms[i];
        
    #if DEV_RENDERER_UNIFORM_CACHING
        ResourceId            id      = state.constantBuffer(uniform.index);
        const ConstantBuffer& cbuffer = m_constantBuffers[id];
        
        // Nothing changed, so just skip this uniform
        if (permutation->buffers[uniform.index].hash == cbuffer.revision.hash)
        {
            continue;
        }
    #endif  //  #if DEV_RENDERER_UNIFORM_CACHING
        
        // Count this uniform update
        m_counters.uniformsUploaded++;
        
        switch (uniform.type)
        {
            case GL_INT:
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, state);
                OpenGL2::Program::uniform1i(uniform.location, *reinterpret_cast<const s32*>(pointer));
                break;
                
            case GL_FLOAT:
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, state);
                OpenGL2::Program::uniform1f(uniform.location, *reinterpret_cast<const f32*>(pointer));
                break;
                
            case GL_FLOAT_VEC2:
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, state);
                OpenGL2::Program::uniform2f(uniform.location, reinterpret_cast<const f32*>(pointer), uniform.size);
                break;
                
            case GL_FLOAT_VEC3:
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, state);
                OpenGL2::Program::uniform3f(uniform.location, reinterpret_cast<const f32*>(pointer), uniform.size);
                break;
                
            case GL_FLOAT_VEC4:
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, state);
                OpenGL2::Program::uniform4f(uniform.location, reinterpret_cast<const f32*>(pointer), uniform.size);
                break;
                
            case GL_FLOAT_MAT4:
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, state);
                OpenGL2::Program::uniformMatrix4(uniform.location, reinterpret_cast<const f32*>(pointer));
                break;
                
            default:
                NIMBLE_NOT_IMPLEMENTED
        }
    }
    
#if DEV_RENDERER_UNIFORM_CACHING
    for (size_t i = 0, n = permutation->uniforms.size(); i < n; i++)
    {
        const Permutation::Uniform& uniform = permutation->uniforms[i];
        ResourceId                  id      = state.constantBuffer(uniform.index);
        const ConstantBuffer&       cbuffer = m_constantBuffers[id];
        
        // Update uniform hash values.
        permutation->buffers[uniform.index] = cbuffer.revision;
    }
#endif  //  #if DEV_RENDERER_UNIFORM_CACHING
}
    
// ** OpenGL2RenderingContext::compileShaderPermutation
const OpenGL2RenderingContext::Permutation* OpenGL2RenderingContext::compileShaderPermutation(ResourceId program, PipelineFeatures features, const PipelineFeatureLayout* featureLayout)
{
    // Lookup a shader permutation in cache
    const Permutation* permutation = NULL;

    if (lookupPermutation(program, features, &permutation))
    {
        return permutation;
    }

    // Now create a shader source code from a descriptor
    String shaderSourceCode[TotalShaderTypes];
    m_shaderLibrary.generateShaderCode(m_programs[program], features, featureLayout, shaderSourceCode);

    // Nothing found so we have to compile a new one
    s8 error[2048];
    
    // Compile the vertex shader
    GLuint vertexShader = OpenGL2::Program::compileShader(GL_VERTEX_SHADER, shaderSourceCode[VertexShaderType].c_str(), error, sizeof(error));
    if (vertexShader == 0)
    {
        return 0;
    }
    
    // Compile the fragment shader
    GLuint fragmentShader = OpenGL2::Program::compileShader(GL_FRAGMENT_SHADER, shaderSourceCode[FragmentShaderType].c_str(), error, sizeof(error));
    if (fragmentShader == 0)
    {
        OpenGL2::Program::deleteShader(vertexShader);
        return 0;
    }
    
    // Now link a program
    GLuint shaders[] = {vertexShader, fragmentShader};
    GLuint id = OpenGL2::Program::createProgram(shaders, 2, error, sizeof(error));
    
    if (id == 0)
    {
        OpenGL2::Program::deleteShader(vertexShader);
        OpenGL2::Program::deleteShader(fragmentShader);
        return 0;
    }
    
    // Finally save a compiled permutation
    permutation = savePermutation(program, features, id);
    
    return permutation;
}
    
} // namespace Renderer

DC_END_DREEMCHEST
