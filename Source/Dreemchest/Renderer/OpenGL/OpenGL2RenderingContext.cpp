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
#include "../VertexFormat.h"
#include "../CommandBuffer.h"

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
        definition += "\t" + s_types[element->type] + " " + element->name.value() + ";\n";
    }
    definition += "}; uniform " + type + " cb_" + toString(slot) + ";\n#define " + name + " cb_" + toString(slot) + "\n";

    return definition;
}

// ----------------------------------------------------------------- OpenGL2RenderingContext ----------------------------------------------------------- //

// ** OpenGL2RenderingContext::OpenGL2RenderingContext
OpenGL2RenderingContext::OpenGL2RenderingContext(RenderViewPtr view)
    : OpenGLRenderingContext(view)
{
    if (m_view.valid())
    {
        m_view->makeCurrent();
    }

    m_shaderLibrary.addPreprocessor(DC_NEW ShaderPreprocessor);
}

// ** OpenGL2RenderingContext::applyStateBlock
PipelineFeatures OpenGL2RenderingContext::applyStateBlock(const RenderFrame& frame, const StateBlock& stateBlock)
{
    const StateBlock* blocks[] = { &stateBlock };
    return applyStates(frame, blocks, 1).features;
}

// ** OpenGL2RenderingContext::executeCommandBuffer
void OpenGL2RenderingContext::executeCommandBuffer(const RenderFrame& frame, const CommandBuffer& commands)
{
    RequestedState              requestedState;
    TransientRenderTarget       transientRenderTarget;
    GLenum                      id;
    
    for (s32 i = 0, n = commands.size(); i < n; i++)
    {
        // Get a render operation at specified index
        const CommandBuffer::OpCode& opCode = commands.opCodeAt( i );
        
        // Perform a draw call
        switch(opCode.type)
        {
            case CommandBuffer::OpCode::Clear:
                OpenGL2::clear(opCode.clear.color, opCode.clear.mask, opCode.clear.depth, opCode.clear.stencil);
                break;
                
            case CommandBuffer::OpCode::Execute:
                execute(frame, *opCode.execute.commands);
                break;
                
            case CommandBuffer::OpCode::UploadConstantBuffer:
            {
                ConstantBuffer& constantBuffer = m_constantBuffers[opCode.upload.id];
                NIMBLE_ABORT_IF(static_cast<s32>(constantBuffer.data.size()) < opCode.upload.size, "buffer is too small");
                memcpy(&constantBuffer.data[0], opCode.upload.data, opCode.upload.size);
            }
                break;
                
            case CommandBuffer::OpCode::UploadVertexBuffer:
                OpenGL2::Buffer::subData(GL_ARRAY_BUFFER, m_vertexBuffers[opCode.upload.id], 0, opCode.upload.size, opCode.upload.data);
                break;
                
            case CommandBuffer::OpCode::CreateInputLayout:
                m_inputLayouts.emplace(opCode.createInputLayout.id, createVertexBufferLayout(opCode.createInputLayout.format));
                break;
                
            case CommandBuffer::OpCode::CreateTexture:
                id = OpenGL2::Texture::create(GL_TEXTURE_2D, opCode.createTexture.data, opCode.createTexture.width, opCode.createTexture.height, opCode.createTexture.format);
                m_textures.emplace(opCode.createTexture.id, id);
                break;
                
            case CommandBuffer::OpCode::CreateIndexBuffer:
                id = OpenGL2::Buffer::create(GL_ARRAY_BUFFER, opCode.createBuffer.data, opCode.createBuffer.size, GL_DYNAMIC_DRAW);
                m_indexBuffers.emplace(opCode.createBuffer.id, id);
                break;
                
            case CommandBuffer::OpCode::CreateVertexBuffer:
                id = OpenGL2::Buffer::create(GL_ELEMENT_ARRAY_BUFFER, opCode.createBuffer.data, opCode.createBuffer.size, GL_DYNAMIC_DRAW);
                m_vertexBuffers.emplace(opCode.createBuffer.id, id);
                break;
                
            case CommandBuffer::OpCode::CreateConstantBuffer:
            {
                ConstantBuffer constantBuffer;
                constantBuffer.layout = &m_uniformLayouts[opCode.createBuffer.layout][0];
                constantBuffer.data.resize(opCode.createBuffer.size);
                if (opCode.createBuffer.data)
                {
                    memcpy(&constantBuffer.data[0], opCode.createBuffer.data, opCode.createBuffer.size);
                }
                m_constantBuffers.emplace(opCode.createBuffer.id, constantBuffer);
            }
                break;
                
            case CommandBuffer::OpCode::RenderTarget:
                NIMBLE_NOT_IMPLEMENTED
                break;
                
            case CommandBuffer::OpCode::AcquireRenderTarget:
                NIMBLE_NOT_IMPLEMENTED
                //TransientRenderTarget id = acquireRenderTarget(opCode.intermediateRenderTarget.width, opCode.intermediateRenderTarget.height, opCode.intermediateRenderTarget.format);
                //loadTransientTarget(opCode.intermediateRenderTarget.id, id);
                break;
                
            case CommandBuffer::OpCode::ReleaseRenderTarget:
                NIMBLE_NOT_IMPLEMENTED
                //TransientRenderTarget id = intermediateTarget(opCode.intermediateRenderTarget.id);
                //releaseRenderTarget(id);
                //unloadTransientTarget(opCode.intermediateRenderTarget.id);
                break;
                
            case CommandBuffer::OpCode::DrawIndexed:
                // Apply rendering states from a stack
                requestedState = applyStates(frame, opCode.drawCall.states, MaxStateStackDepth);
                
                // Now activate a matching shader permutation
                compilePipelineState(requestedState);
                
                // Perform an actual draw call
                OpenGL2::drawElements(opCode.drawCall.primitives, GL_UNSIGNED_SHORT, opCode.drawCall.first, opCode.drawCall.count);
                break;
                
            case CommandBuffer::OpCode::DrawPrimitives:
                // Apply rendering states from a stack
                requestedState = applyStates(frame, opCode.drawCall.states, MaxStateStackDepth);
                
                // Now activate a matching shader permutation
                compilePipelineState(requestedState);
                
                // Perform an actual draw call
                OpenGL2::drawArrays(opCode.drawCall.primitives, opCode.drawCall.first, opCode.drawCall.count);
                break;
                
            default:
                NIMBLE_NOT_IMPLEMENTED;
        }
    }
}
    
// ** OpenGL2RenderingContext::applyStates
OpenGL2RenderingContext::RequestedState OpenGL2RenderingContext::applyStates(const RenderFrame& frame, const StateBlock* const * stateBlocks, s32 count)
{
    State states[MaxStateChanges];
    PipelineFeatures userDefined;
    
    // This will be modified by a render state changes below
    RequestedState requestedState = m_activeState;
    
    // This will notify a pipeline that we started the stage change process
    s32 stateCount = startPipelineConfiguration(stateBlocks, count, states, MaxStateChanges, userDefined);
    
    // Apply all states
    for (s32 i = 0; i < stateCount; i++)
    {
        // Get a state at specified index
        const State& state = states[i];
        
        // And now apply it
        switch (state.type)
        {
            case State::VertexBuffer:
                requestedState.vertexBuffer.set(state.resourceId);
                break;
                
            case State::IndexBuffer:
                requestedState.indexBuffer.set(state.resourceId);
                break;
                
            case State::InputLayout:
                requestedState.inputLayout.set(state.resourceId);
                break;
                
            case State::FeatureLayout:
                requestedState.featureLayout.set(state.resourceId);
                m_pipeline.setFeatureLayout(m_pipelineFeatureLayouts[state.resourceId].get());
                break;
                
            case State::ConstantBuffer:
                requestedState.constantBuffer[state.data.index].set(state.resourceId);
                m_pipeline.activateConstantBuffer(state.data.index);
                break;
                
            case State::Shader:
                requestedState.program.set(state.resourceId);
                m_pipeline.setProgram(requestedState.program);
                break;
                
            case State::Blending:
            {
                // Decode blend factors from a command
                BlendFactor source      = state.sourceBlendFactor();
                BlendFactor destination = state.destBlendFactor();
                
                // Apply the blend state
                if(source == BlendDisabled || destination == BlendDisabled)
                {
                    glDisable(GL_BLEND);
                }
                else
                {
                    glEnable(GL_BLEND);
                    glBlendFunc(OpenGL2::convertBlendFactor(source), OpenGL2::convertBlendFactor(destination));
                }
            }
                break;
                
            case State::PolygonOffset:
            {
                f32 factor = state.polygonOffsetFactor();
                f32 units  = state.polygonOffsetUnits();
                
                if (equal3(factor, units, 0.0f))
                {
                    glDisable(GL_POLYGON_OFFSET_FILL);
                } else
                {
                    glEnable(GL_POLYGON_OFFSET_FILL);
                    glPolygonOffset(factor, units);
                }
            }
                break;
                
            case State::DepthState:
                glDepthMask(state.data.depthWrite ? GL_TRUE : GL_FALSE);
                glDepthFunc(OpenGL2::convertCompareFunction(static_cast<Compare>(state.compareFunction)));
                break;
                
            case State::AlphaTest:
                if (state.compareFunction == CompareDisabled)
                {
                    glDisable(GL_ALPHA_TEST);
                }
                else
                {
                    glEnable(GL_ALPHA_TEST);
                    glAlphaFunc(OpenGL2::convertCompareFunction(static_cast<Compare>(state.compareFunction)), state.alphaReference());
                }
                break;
                
            case State::CullFace:
                if (state.cullFace == TriangleFaceNone)
                {
                    glDisable(GL_CULL_FACE);
                }
                else
                {
                    glEnable(GL_CULL_FACE);
                    glFrontFace(GL_CCW);
                    glCullFace(OpenGL2::convertTriangleFace(static_cast<TriangleFace>(state.cullFace)));
                }
                break;
                
            case State::Texture:
                requestedState.texture[state.data.index].set(state.resourceId);
                m_pipeline.activateSampler(state.data.index);
                break;
                
            case State::Rasterization:
                switch (state.rasterization)
                {
                    case PolygonFill:
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                        break;
                    case PolygonWire:
                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                        break;
                    default:
                        NIMBLE_NOT_IMPLEMENTED
                }
                break;
                
            default:
                NIMBLE_NOT_IMPLEMENTED
        }
    }
    
    // Finish pipeline configuration and store the final features bitmask
    requestedState.features = finishPipelineConfiguration(userDefined);
    
    return requestedState;
}

// ** OpenGL2RenderingContext::compilePipelineState
void OpenGL2RenderingContext::compilePipelineState(RequestedState requested)
{
    NIMBLE_ABORT_IF(!requested.inputLayout, "no valid input layout set");
    NIMBLE_ABORT_IF(!requested.vertexBuffer, "no valid vertex buffer set");
    
    // Bind an indexed buffer
    if (requested.indexBuffer != m_activeState.indexBuffer)
    {
        OpenGL2::Buffer::bind(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffers[requested.indexBuffer]);
    }
    
    // Bind a vertex buffer
    if (requested.vertexBuffer != m_activeState.vertexBuffer)
    {
        OpenGL2::Buffer::bind(GL_ARRAY_BUFFER, m_vertexBuffers[requested.vertexBuffer]);
    }
    
    // Bind texture samplers
    for (s32 i = 0; i < State::MaxTextureSamplers; i++)
    {
        if (requested.texture[i] == m_activeState.texture[i])
        {
            continue;
        }
        
        OpenGL2::Texture::bind(GL_TEXTURE_2D, m_textures[requested.texture[i]], i);
    }
    
    // Switch the input layout
    if (requested.inputLayout != m_activeState.inputLayout)
    {
        // Disable the previous input layout
        if (m_activeState.inputLayout)
        {
            OpenGL2::disableInputLayout(*m_inputLayouts[m_activeState.inputLayout]);
        }
        
        // Now enable a new one
        OpenGL2::enableInputLayout(NULL, *m_inputLayouts[requested.inputLayout]);
    }
    
    Program          program  = m_pipeline.program();
    PipelineFeatures features = m_pipeline.features();
    
    // Get an active program
    NIMBLE_ABORT_IF(!program && !m_defaultProgram, "no valid program set and no default one specified");
    
    // Use a default program if nothing was set by a user
    if (!program)
    {
        program = m_defaultProgram;
    }
    
    // Switch the program one the pipeline state was changed
    if (m_pipeline.changes())
    {
        // Now compile a program permutation
        GLuint activeProgram = compileShaderPermutation(program, features, m_pipeline.featureLayout());
        OpenGL2::Program::use(activeProgram);
        
        // Accept these changes
        m_pipeline.acceptChanges();
    }
    
    // Update all uniforms
    updateUniforms(requested, features, program);
    
    // Update an active rendering state
    m_activeState = requested;
}
    
// ** OpenGL2RenderingContext::updateUniforms
void OpenGL2RenderingContext::updateUniforms(const RequestedState& state, PipelineFeatures features, Program program)
{
    // Bind texture samplers
    static FixedString s_samplers[] =
    {
          "Texture0"
        , "Texture1"
        , "Texture2"
        , "Texture3"
        , "Texture4"
        , "Texture5"
        , "Texture6"
        , "Texture7"
    };
    
    s32 nSamplers = sizeof(s_samplers) / sizeof(s_samplers[1]);
    NIMBLE_ABORT_IF(nSamplers != State::MaxTextureSamplers, "invalid sampler name initialization");
    
    for (s32 i = 0, n = nSamplers; i < n; i++ )
    {
        GLint location = findUniformLocation(program, features, s_samplers[i]);
        
        if (location)
        {
            OpenGL2::Program::uniform1i(location, i);
        }
    }
    
    // Process each bound constant buffer
    for (s32 i = 0; i < State::MaxConstantBuffers; i++)
    {
        // No constant buffer bound to this slot
        if (!state.constantBuffer[i])
        {
            continue;
        }
        
        // Get a constant buffer at index
        const ConstantBuffer& constantBuffer = m_constantBuffers[state.constantBuffer[i]];
    
        // Submit all constants to a shader
        for (const UniformElement* constant = constantBuffer.layout; constant->name; constant++)
        {
            // Create a uniform name here for now, but in future this sould be cached somewhere (probably in a ConstantBuffer instance).
            String uniform = "cb_" + toString(i) + "." + constant->name.value();

            // Lookup a uniform location by name
            GLint location = findUniformLocation(program, features, uniform.c_str());
            
            // Not found - skip
            if (location == 0)
            {
                LogWarning("opengl2", "a uniform location '%s' for constant buffer %d could not be found\n", constant->name.value(), i);
                continue;
            }
            
            // Submit constant to a shader
            switch (constant->type)
            {
                case UniformElement::Integer:
                    OpenGL2::Program::uniform1i(location, *reinterpret_cast<const s32*>(&constantBuffer.data[constant->offset]));
                    break;
                    
                case UniformElement::Float:
                    OpenGL2::Program::uniform1f(location, *reinterpret_cast<const f32*>(&constantBuffer.data[constant->offset]));
                    break;
                    
                case UniformElement::Vec2:
                    OpenGL2::Program::uniform2f(location, reinterpret_cast<const f32*>(&constantBuffer.data[constant->offset]));
                    break;
                    
                case UniformElement::Vec3:
                    OpenGL2::Program::uniform3f(location, reinterpret_cast<const f32*>(&constantBuffer.data[constant->offset]));
                    break;
                    
                case UniformElement::Vec4:
                    OpenGL2::Program::uniform4f(location, reinterpret_cast<const f32*>(&constantBuffer.data[constant->offset]));
                    break;
                    
                case UniformElement::Matrix4:
                    OpenGL2::Program::uniformMatrix4(location, reinterpret_cast<const f32*>(&constantBuffer.data[constant->offset]));
                    break;
            }
        }
    }
}
    
// ** OpenGL2RenderingContext::compileShaderPermutation
GLuint OpenGL2RenderingContext::compileShaderPermutation(Program program, PipelineFeatures features, const PipelineFeatureLayout* featureLayout)
{
    // Lookup a shader permutation in cache
    const Permutation* permutation = NULL;

    if (lookupPermutation(program, features, &permutation))
    {
        return permutation->program;
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
    savePermutation(program, features, id);
    
    return id;
}
    
} // namespace Renderer

DC_END_DREEMCHEST
