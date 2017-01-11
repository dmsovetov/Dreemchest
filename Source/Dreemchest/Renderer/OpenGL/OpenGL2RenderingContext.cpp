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
    , m_requestedFeatureLayout(NULL)
    , m_activeInputLayout(NULL)
#if DEV_RENDERER_PROGRAM_CACHING
    , m_activePermutation(NULL)
    , m_activeProgram(0)
    , m_activeFeatures(0)
#endif  //  #if DEV_RENDERER_PROGRAM_CACHING
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
    GLuint             id;
    const Permutation* permutation = NULL;
    
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
            //  resetConstantBuffer(opCode.id);
                break;
                
            case OpCode::DeleteProgram:
            //  resetProgram(opCode.id);
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
                // Now update the pipeline state
                compilePipelineState(opCode.drawCall.stateBlock->states, opCode.drawCall.stateBlock->size);
                
                // Finally select a matching shader permutation
                permutation = applyProgramPermutation(m_requestedProgram, m_requestedFeatureLayout, opCode.drawCall.stateBlock->features | m_activeInputLayout->features());
                NIMBLE_ABORT_IF(permutation == NULL, "no valid permutation found");
                
                // And update all uniforms
                updateUniforms(permutation);
                
                // Perform an actual draw call
                OpenGL2::drawElements(opCode.drawCall.primitives, GL_UNSIGNED_SHORT, opCode.drawCall.first, opCode.drawCall.count);
                break;
                
            case OpCode::DrawPrimitives:
                // Now update the pipeline state
                compilePipelineState(opCode.drawCall.stateBlock->states, opCode.drawCall.stateBlock->size);

                // Finally select a matching shader permutation
                permutation = applyProgramPermutation(m_requestedProgram, m_requestedFeatureLayout, opCode.drawCall.stateBlock->features | m_activeInputLayout->features());
                NIMBLE_ABORT_IF(permutation == NULL, "no valid permutation found");
                
                // And update all uniforms
                updateUniforms(permutation);
                
                // Perform an actual draw call
                OpenGL2::drawArrays(opCode.drawCall.primitives, opCode.drawCall.first, opCode.drawCall.count);
                break;
                
            default:
                NIMBLE_NOT_IMPLEMENTED;
        }
    }
}

// ** OpenGL2RenderingContext::compilePipelineState
void OpenGL2RenderingContext::compilePipelineState(const State* states, s32 count)
{
    for (s32 i = 0; i < count; i++)
    {
        const State& state = states[i];
        
        switch (state.type)
        {
            case State::BindVertexBuffer:
                OpenGL2::Buffer::bind(GL_ARRAY_BUFFER, m_vertexBuffers[state.resourceId]);
                break;
                
            case State::BindIndexBuffer:
                OpenGL2::Buffer::bind(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffers[state.resourceId]);
                break;
                
            case State::SetInputLayout:
                m_requestedInputLayout = m_inputLayouts[state.resourceId].get();
                break;
                
            case State::BindProgram:
                m_requestedProgram = state.resourceId;
                break;
                
            case State::SetFeatureLayout:
                m_requestedFeatureLayout = m_pipelineFeatureLayouts[state.resourceId].get();
                break;
                
            case State::BindConstantBuffer:
                m_requestedCBuffer[state.data.index] = state.resourceId;
                break;
                
            case State::BindTexture:
                OpenGL2::Texture::bind(m_textures[state.resourceId].target, m_textures[state.resourceId].id, state.samplerIndex());
                break;
                
            case State::BindTransientTexture:
            {
                ResourceId id = transientResource(state.resourceId);
                OpenGL2::Texture::bind(m_textures[id].target, m_textures[id].id, state.samplerIndex());
            }
                break;
                
            case State::DepthState:
                OpenGL2::setDepthState(state.function(), state.data.depthWrite);
                break;
                
            case State::Blending:
                OpenGL2::setBlending(state.sourceBlendFactor(), state.destBlendFactor());
                break;
                
            case State::ColorMask:
                OpenGL2::setColorMask(state.mask);
                break;
                
            case State::Rasterization:
                OpenGL2::setRasterization(static_cast<PolygonMode>(state.rasterization));
                break;
                
            case State::AlphaTest:
                OpenGL2::setAlphaTest(state.function(), state.alphaReference());
                break;
                
            case State::PolygonOffset:
                OpenGL2::setPolygonOffset(state.polygonOffsetFactor(), state.polygonOffsetUnits());
                break;
                
            case State::StencilFunc:
                OpenGL2::Stencil::setFunction(static_cast<Compare>(state.stencilFunction.op), state.data.ref, state.stencilFunction.mask);
                break;
                
            case State::StencilOp:
                OpenGL2::Stencil::setOperations(state.stencilFail(), state.depthFail(), state.depthStencilPass());
                break;
                
            case State::CullFace:
                OpenGL2::setCullFace(static_cast<TriangleFace>(state.cullFace));
                break;
                
            default:
                LogFatal("renderingContext", "state type '%s' is not implemented\n", State::nameFromType(static_cast<State::Type>(state.type)).c_str());
                NIMBLE_NOT_IMPLEMENTED
        }
    }
    
#if DEV_RENDERER_INPUT_LAYOUT_CACHING
    if (m_activeInputLayout != m_requestedInputLayout)
#endif  //  #if DEV_RENDERER_INPUT_LAYOUT_CACHING
    {
        // Disable the previous input layout
        if (m_activeInputLayout)
        {
            OpenGL2::disableInputLayout(*m_activeInputLayout);
        }
     
        // Now enable a new one
        OpenGL2::enableInputLayout(NULL, *m_requestedInputLayout);
     
        // Track this switch
        m_counters.inputLayoutSwitches++;
    #if DEV_RENDERER_INPUT_LAYOUT_CACHING
        m_activeInputLayout = m_requestedInputLayout;
    #endif  //  #if DEV_RENDERER_INPUT_LAYOUT_CACHING
    }
}
    
// ** OpenGL2RenderingContext::applyProgramPermutation
const OpenGLRenderingContext::Permutation* OpenGL2RenderingContext::applyProgramPermutation(ResourceId program, const PipelineFeatureLayout* layout, PipelineFeatures features)
{
#if DEV_RENDERER_PROGRAM_CACHING
    if (program == m_activeProgram && features == m_activeFeatures)
    {
        return m_activePermutation;
    }
    m_activeProgram = program;
    m_activeFeatures = features;
#endif  //  #if DEV_RENDERER_PROGRAM_CACHING
    
    // Get an active program
    NIMBLE_ABORT_IF(!program && !m_defaultProgram, "no valid program set and no default one specified");
    
    // Use a default program if nothing was set by a user
    if (!program)
    {
        program = m_defaultProgram;
    }
    
    // Lookup a shader permutation in cache
    const Permutation* permutation = NULL;
    
    if (!lookupPermutation(program, features, &permutation))
    {
        permutation = compileShaderPermutation(program, features, layout);
    }
    
#if DEV_RENDERER_PROGRAM_CACHING
    if (m_activePermutation == permutation)
    {
        return permutation;
    }
    m_activePermutation = permutation;
#endif  //  #if DEV_RENDERER_PROGRAM_CACHING
    
    // Switch the program one the pipeline state was changed
    OpenGL2::Program::use(permutation->program);
    
    // Track this program switch
    m_counters.programSwitches++;
    
    return permutation;
}
    
// ** OpenGL2RenderingContext::updateUniforms
void OpenGL2RenderingContext::updateUniforms(const Permutation* permutation)
{
    struct UniformPointer
    {
        static const void* findByName(const Permutation::Uniform& uniform, const FixedArray<ConstantBuffer>& constantBuffers, const ResourceId* boundConstantBuffers)
        {
            ResourceId id = boundConstantBuffers[uniform.index];
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
        ResourceId id = m_requestedCBuffer[uniform.index];
        NIMBLE_ABORT_IF(!id, "no constant buffer bound");
        
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
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, m_requestedCBuffer);
                OpenGL2::Program::uniform1i(uniform.location, *reinterpret_cast<const s32*>(pointer));
                break;
                
            case GL_FLOAT:
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, m_requestedCBuffer);
                OpenGL2::Program::uniform1f(uniform.location, *reinterpret_cast<const f32*>(pointer));
                break;
                
            case GL_FLOAT_VEC2:
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, m_requestedCBuffer);
                OpenGL2::Program::uniform2f(uniform.location, reinterpret_cast<const f32*>(pointer), uniform.size);
                break;
                
            case GL_FLOAT_VEC3:
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, m_requestedCBuffer);
                OpenGL2::Program::uniform3f(uniform.location, reinterpret_cast<const f32*>(pointer), uniform.size);
                break;
                
            case GL_FLOAT_VEC4:
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, m_requestedCBuffer);
                OpenGL2::Program::uniform4f(uniform.location, reinterpret_cast<const f32*>(pointer), uniform.size);
                break;
                
            case GL_FLOAT_MAT4:
                pointer = UniformPointer::findByName(uniform, m_constantBuffers, m_requestedCBuffer);
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
        ResourceId                  id      = m_requestedCBuffer[uniform.index];
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
    
    // Track this compilation
    m_counters.permutationsCompiled++;

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
