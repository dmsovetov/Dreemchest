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

#include "RenderingContext.h"
#include "Commands/RenderCommandBuffer.h"
#include "Commands/ResourceCommandBuffer.h"
#include "VertexFormat.h"
#include "VertexBufferLayout.h"
#include "RenderFrame.h"

#include "../Io/DiskFileSystem.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
// ------------------------------------------------------------------ RenderingContext::TransientResourceStack ----------------------------------------------------------------- //

/*!
 Transient target stack is used to convert local indices that are
 stored in commands to a global index of an transient resources.
 */
class RenderingContext::TransientResourceStack
{
public:
    
    //! A maximum number of intermediate render targets that can be hold by a single stack frame.
    enum { StackFrameSize = 8 };
    
    //! A maximum number of stack frames that can be pushed during rendering.
    enum { MaxStackFrames = 8 };
    
    //! A total size of an intermediate stack size.
    enum { MaxStackSize = MaxStackFrames * StackFrameSize };
    
                                //! Constructs an TransientResourceStack instance.
                                TransientResourceStack();
    
    //! Pushes a new stack frame.
    void                        pushFrame();
    
    //! Pops an active stack frame.
    void                        popFrame();
    
    //! Returns a persistent resource by a transient one.
    ResourceId                  get(TransientResourceId index) const;
    
    //! Loads an acquired transient resource to a specified local slot.
    void                        load(TransientResourceId index, ResourceId id);
    
    //! Unloads a transient resource from a specified local slot.
    void                        unload(TransientResourceId index);
    
private:
    
    ResourceId*                 m_stackFrame;                   //!< An active render target stack frame.
    ResourceId                  m_identifiers[MaxStackSize];    //!< An array of intermediate render target handles.
};

// ** RenderingContext::TransientResourceStack::TransientResourceStack
RenderingContext::TransientResourceStack::TransientResourceStack( void )
    : m_stackFrame( m_identifiers )
{
    memset(m_identifiers, 0, sizeof(m_identifiers));
}

// ** RenderingContext::TransientResourceStack::pushFrame
void RenderingContext::TransientResourceStack::pushFrame( void )
{
    NIMBLE_ABORT_IF( (m_stackFrame + StackFrameSize) > (m_identifiers + MaxStackSize), "frame stack overflow" );
    m_stackFrame += StackFrameSize;
}

// ** RenderingContext::TransientResourceStack::popFrame
void RenderingContext::TransientResourceStack::popFrame( void )
{
    NIMBLE_ABORT_IF( m_stackFrame == m_identifiers, "stack underflow" );
    
    // Ensure that all render targets were released
    for( s32 i = 0; i < StackFrameSize; i++ )
    {
        if( m_stackFrame[i] )
        {
            LogWarning( "rvm", "%s", "an intermediate render target was not released before popping a stack frame\n" );
        }
    }
    
    // Pop a stack frame
    m_stackFrame -= StackFrameSize;
}

// ** RenderingContext::TransientResourceStack::get
ResourceId RenderingContext::TransientResourceStack::get(TransientResourceId index) const
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    return m_stackFrame[index - 1];
}

// ** RenderingContext::TransientResourceStack::load
void RenderingContext::TransientResourceStack::load(TransientResourceId index, ResourceId id)
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    m_stackFrame[index - 1] = id;
}

// ** RenderingContext::TransientResourceStack::unload
void RenderingContext::TransientResourceStack::unload(TransientResourceId index)
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    m_stackFrame[index - 1] = 0;
}
    
// -------------------------------------------------------------------------------- RenderingContext -------------------------------------------------------------------------------- //
    
// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext(RenderViewPtr view)
    : m_view(view)
    , m_shaderLibrary(*this)
    , m_frame(*this)
{
    LogDebug("renderingContext", "rendering context size is %d bytes\n", sizeof(RenderingContext));
    LogDebug("renderingContext", "rendering state size is %d bytes\n", sizeof(State));
    LogDebug("renderingContext", "rendering state block size is %d bytes\n", sizeof(StateBlock));
    LogDebug("renderingContext", "opcode size is %d bytes\n", sizeof(OpCode));
//    LogDebug("renderingContext", "drawCall size is %d bytes\n", sizeof(OpCode::drawCall));

    // Create a construction command buffer instance
    m_resourceCommandBuffer = DC_NEW ResourceCommandBuffer;
    
    // Create an intermediate target stack
    m_transientResources = DC_NEW TransientResourceStack;
    
    // Reset input layout cache
    memset(m_inputLayoutCache, 0, sizeof(m_inputLayoutCache));
    memset(&m_counters, 0, sizeof(m_counters));
    memset(&m_caps, 0, sizeof(m_caps));
    
    // Resize all resource index managers
    for (s32 i = 0; i < RenderResourceType::TotalTypes; i++)
    {
        m_identifiers[i].resize(32);
        m_identifiers[i].acquire();
    }
    
    // Initialize a default state block
    m_defaultStateBlock.setCullFace(TriangleFaceBack);
    m_defaultStateBlock.setDepthState(LessEqual, true);
    m_defaultStateBlock.disableAlphaTest();
    m_defaultStateBlock.disableBlending();
    m_defaultStateBlock.disableStencilTest();
    m_defaultStateBlock.setColorMask(ColorMaskAll);
    
    // Create a default program
    Program defaultProgram = requestProgram(ShaderProgramDescriptor());
    setDefaultProgram(defaultProgram);
    
    // Add shared functions
    m_shaderLibrary.addSharedFunction("f_schlick", NIMBLE_STRINGIFY(float f_schlick(highp float dp, float f0, float pow)
                                                                    {
                                                                        float facing = (1.0 - dp);
                                                                        return max(f0 + (1.0 - f0) * pow(facing, pow), 0.0);
                                                                    }
                                                                    ));
    
    m_shaderLibrary.addSharedFunction("f_cookTorrance", NIMBLE_STRINGIFY(float f_cookTorrance(float dp, float f0)
                                                                         {
                                                                             float n = (1.0 + sqrt(f0)) / (1.0 - sqrt(f0));
                                                                             float g = sqrt(n * n + dp * dp - 1.0);

                                                                             float part1 = (g - dp)/(g + dp);
                                                                             float part2 = ((g + dp) * dp - 1.0)/((g - dp) * dp + 1.0);

                                                                             return 0.5 * part1 * part1 * (1.0 + part2 * part2);
                                                                         }
                                                                         ));
    
    m_shaderLibrary.addSharedFunction("d_blinn", NIMBLE_STRINGIFY(float2 d_blinn(float4 products)
                                                                  {
                                                                      float diff = max(0.0, products.x);
                                                                      float spec = step(0.0, products.x) * max(0.0, products.z);
                                                                      return float2(diff, spec);
                                                                  }
                                                                  ));
    
    m_shaderLibrary.addSharedFunction("d_blinnPhong", NIMBLE_STRINGIFY(float2 d_blinnPhong(float4 products, float roughness)
                                                                        {
                                                                            float a = max(0.001, roughness * roughness);
                                                                            return float2((1.0 / ((3.1415926535897932384626433832795) * a * a)) * pow(products.z, 2.0 / (a * a) - 2.0), 0.0);
                                                                        }
                                                                        ));

    m_shaderLibrary.addSharedFunction("l_products", NIMBLE_STRINGIFY(float4 l_products(float3 n, float3 v, float3 l, float3 r)
                                                                     {
                                                                         float ndotl = dot(n, l);
                                                                         float ndotv = dot(n, v);
                                                                         float ndotr = dot(n, r);
                                                                         return vec4(ndotl, ndotv, ndotr, 1.0);
                                                                     }
                                                                     ));
    
    m_shaderLibrary.addSharedFunction("f_reflectance", NIMBLE_STRINGIFY(float f_reflectance(highp float eta)
                                                                        {
                                                                            float one_minus_eta = 1.0 - eta;
                                                                            float one_plus_eta  = 1.0 + eta;
                                                                            return (one_minus_eta * one_minus_eta) / (one_plus_eta * one_plus_eta);
                                                                        }
                                                                        ));
    
    m_shaderLibrary.addSharedFunction("directionFromHemisphere", NIMBLE_STRINGIFY(float3 directionFromHemisphere(float3 t, float3 b, float3 n, float2 spherical)
                                                                                  {
                                                                                      float4 angles = vec4(cos(spherical.x), sin(spherical.x), cos(spherical.y), sin(spherical.y));
                                                                                      return t * angles.x * angles.w + b * angles.y * angles.w + n * angles.z;
                                                                                  }
                                                                                  ));
    
    m_shaderLibrary.addSharedFunction("directionFromHemisphere", NIMBLE_STRINGIFY(float3 directionFromHemisphere(float3 t, float3 b, float3 n, float4 angles)
                                                                                  {
                                                                                      return t * angles.x * angles.w + b * angles.y * angles.w + n * angles.z;
                                                                                  }
                                                                                  ));
}
    
// ** RenderingContext::~RenderingContext
RenderingContext::~RenderingContext( void )
{
    delete m_resourceCommandBuffer;
}
    
// ** RenderingContext::setDefaultStateBlock
void RenderingContext::setDefaultStateBlock(const StateBlock12& value)
{
    m_defaultStateBlock = value;
}
    
// ** RenderingContext::defaultStateBlock
const StateBlock& RenderingContext::defaultStateBlock() const
{
    return m_defaultStateBlock;
}
    
// ** RenderingContext::defaultStateBlock
StateBlock& RenderingContext::defaultStateBlock()
{
    return m_defaultStateBlock;
}
    
// ** RenderingContext::setDefaultProgram
void RenderingContext::setDefaultProgram(Program value)
{
    m_defaultProgram = value;
}
    
// ** RenderingContext::frameCounters
const RenderingContext::FrameCounters& RenderingContext::frameCounters() const
{
    return m_counters;
}
    
// ** RenderingContext::caps
const RenderingContext::Caps& RenderingContext::caps() const
{
    return m_caps;
}
    
// ** RenderingContext::textureInfo
const TextureInfo& RenderingContext::textureInfo(Texture_ id) const
{
    NIMBLE_ABORT_IF(!id, "invalid texture id");
    return m_textures[id];
}
    
// ** RenderingContext::setTextureInfo
void RenderingContext::setTextureInfo(Texture_ id, TextureType type, u16 width, u16 height, u32 options)
{
    TextureInfo textureInfo;
    textureInfo.width   = width;
    textureInfo.height  = height;
    textureInfo.options = options;
    textureInfo.type    = type;
    m_textures.emplace(id, textureInfo);
}
    
// ** RenderingContext::allocateFrame
RenderFrame& RenderingContext::allocateFrame(s32 size)
{
    if (m_frame.allocationCapacity() != size)
    {
        m_frame.setAllocationCapacity(size);
    }
    m_frame.clear();
    return m_frame;
}

// ** RenderingContext::display
void RenderingContext::display(RenderFrame& frame, bool wait)
{
    // Begin frame
    if (m_view.valid())
    {
        m_view->beginFrame();
    }
    
    // Clear all counters.
    memset(&m_counters, 0, sizeof(m_counters));
    
    // Reset active rendering states
    memset(m_activeStates, 0, sizeof(m_activeStates));
    
    // First execute a construction command buffer
    construct();
    
    // Execute an entry point command buffer
    execute(frame.entryPoint());

    // End frame
    if (m_view.valid())
    {
        m_view->endFrame(wait);
    }
}
    
// ** RenderingContext::construct
void RenderingContext::construct()
{
    execute(*m_resourceCommandBuffer);
    m_resourceCommandBuffer->reset();
}

// ** RenderingContext::execute
void RenderingContext::execute(const CommandBuffer& commands)
{
    // Push a new frame to an intermediate target stack
    m_transientResources->pushFrame();
    
    // Execute a command buffer
    executeCommandBuffer(commands);
    
    // Pop a stack frame
    m_transientResources->popFrame();
}
    
// ** RenderingContext::loadTransientResource
void RenderingContext::loadTransientResource(TransientResourceId transient, ResourceId id)
{
    m_transientResources->load(transient, id);
}

// ** RenderingContext::unloadTransientResource
void RenderingContext::unloadTransientResource(TransientResourceId transient)
{
    m_transientResources->unload(transient);
}
    
// ** RenderingContext::transientTarget
ResourceId RenderingContext::transientResource(TransientResourceId transient) const
{
    return m_transientResources->get(transient);
}

// ** RenderingContext::allocateIdentifier
ResourceId RenderingContext::allocateIdentifier(RenderResourceType::Enum type)
{
    NIMBLE_ABORT_IF(!m_identifiers[type].hasFreeIndices(), "too much persistent identifiers allocated");
    return m_identifiers[type].acquire();
}

// ** RenderingContext::releaseIdentifier
void RenderingContext::releaseIdentifier(RenderResourceType::Enum type, ResourceId id)
{
    m_identifiers[type].release(id);
}

// ** RenderingContext::requestInputLayout
InputLayout RenderingContext::requestInputLayout(const VertexFormat& format)
{
    // First lookup a previously constucted input layout
    InputLayout id = m_inputLayoutCache[format];
    
    if( id )
    {
        return id;
    }
    
    // Allocate an input layout identifier
    id = allocateIdentifier(RenderResourceType::InputLayout);
    
    // Nothing found - construct a new one
    id = m_resourceCommandBuffer->createInputLayout(id, format);
    
    // Now put a new input layout to cache
    m_inputLayoutCache[format] = id;
    
    return id;
}

// ** RenderingContext::requestUniformLayout
UniformLayout RenderingContext::requestUniformLayout(const String& name, const UniformElement* elements)
{
    // Allocate next uniform layout id.
    UniformLayout id = allocateIdentifier(RenderResourceType::UniformLayout);

    // Construct a uniform instance
    m_uniformLayouts.emplace(id, UniformBufferLayout());

    for (const UniformElement* element = elements; element->name; element++)
    {
        m_uniformLayouts[id].push_back(*element);
    }

    // Now sort elements by an offset
    struct OrderBy
    {
        static bool offset(const UniformElement& first, const UniformElement& second)
        {
            return first.offset < second.offset;
        }
    };

    std::sort(m_uniformLayouts[id].begin(), m_uniformLayouts[id].end(), OrderBy::offset);
    
    // Finally add the sentinel
    UniformElement sentinel = { NULL };
    m_uniformLayouts[id].push_back(sentinel);

    // Save this layout to a name mapping
    m_uniformLayoutByName.insert(name, id);

    return id;
}

// ** RenderingContext::requestPipelineFeatureLayout
FeatureLayout RenderingContext::requestPipelineFeatureLayout(const PipelineFeature* features)
{
    // Create a pipeline feature layout
    PipelineFeatureLayout* layout = DC_NEW PipelineFeatureLayout;
    
    // Populate it with feature mappings
    for (; features->name; features++)
    {
        layout->addFeature(features->name.value(), features->bits);
    }
    
    // Allocate an input layout identifier
    FeatureLayout id = allocateIdentifier(RenderResourceType::FeatureLayout);
    
    // Put this layout instance to a pool
    m_pipelineFeatureLayouts.emplace(id, layout);

    return id;
}

// ** RenderingContext::requestVertexBuffer
VertexBuffer_ RenderingContext::requestVertexBuffer( const void* data, s32 size )
{
    VertexBuffer_ id = allocateIdentifier(RenderResourceType::VertexBuffer);
    return m_resourceCommandBuffer->createVertexBuffer(id, data, size);
}

// ** RenderingContext::requestIndexBuffer
IndexBuffer_ RenderingContext::requestIndexBuffer( const void* data, s32 size )
{
    IndexBuffer_ id = allocateIdentifier(RenderResourceType::IndexBuffer);
    return m_resourceCommandBuffer->createIndexBuffer(id, data, size);
}

// ** RenderingContext::requestConstantBuffer
ConstantBuffer_ RenderingContext::requestConstantBuffer(const void* data, s32 size, UniformLayout layout)
{
    ConstantBuffer_ id = allocateIdentifier(RenderResourceType::ConstantBuffer);
    return m_resourceCommandBuffer->createConstantBuffer(id, data, size, layout);
}
    
// ** RenderingContext::updateConstantBuffer
void RenderingContext::updateConstantBuffer(ConstantBuffer_ id, const void* data, s32 size)
{
    m_resourceCommandBuffer->uploadConstantBuffer(id, data, size);
}
    
// ** RenderingContext::requestConstantBuffer
ConstantBuffer_ RenderingContext::requestConstantBuffer(const void* data, s32 size, const String& name, const UniformElement* elements)
{
    UniformLayout layout = requestUniformLayout(name, elements);
    return requestConstantBuffer(data, size, layout);
}
    
// ** RenderingContext::deprecatedRequestConstantBuffer
ConstantBuffer_ RenderingContext::deprecatedRequestConstantBuffer(const void* data, s32 size, const UniformElement* elements)
{
    return 0;
}

// ** RenderingContext::requestTexture2D
Texture_ RenderingContext::requestTexture2D(const void* data, u16 width, u16 height, u32 options)
{
    // Allocate an identifier
    Texture_ id = allocateIdentifier(RenderResourceType::Texture);
    
    // Set a texture info
    setTextureInfo(id, TextureType2D, width, height, options);
    
    // Create a construction command
    return m_resourceCommandBuffer->createTexture2D(id, data, width, height, options);
}
    
// ** RenderingContext::requestTextureCube
Texture_ RenderingContext::requestTextureCube(const void* data, u16 size, u16 mipLevels, u32 options)
{
    // Allocate an identifier
    Texture_ id = allocateIdentifier(RenderResourceType::Texture);
    
    // Construct a texture info
    TextureInfo textureInfo;
    textureInfo.width   = size;
    textureInfo.height  = size;
    textureInfo.options = options;
    textureInfo.type    = TextureTypeCube;
    m_textures.emplace(id, textureInfo);
    
    // Create a construction command
    return m_resourceCommandBuffer->createTextureCube(id, data, size, mipLevels, options);
}
    
// ** RenderingContext::requestProgram
Program RenderingContext::requestProgram(const ShaderProgramDescriptor& descriptor)
{
    // Allocate a program identifier
    Program id = allocateIdentifier(RenderResourceType::Program);
    
    // Put this program to a pool
    m_programs.emplace(id, descriptor);
    
    return id;
}
    
// ** RenderingContext::precompilePermutations
void RenderingContext::precompilePermutations(Program id, FeatureLayout features)
{
    m_resourceCommandBuffer->precompilePermutation(id, 0);
}

// ** RenderingContext::requestProgram
Program RenderingContext::requestProgram(VertexShader vertexShader, FragmentShader fragmentShader)
{
    // Create a shader program descriptor
    ShaderProgramDescriptor descriptor;
    descriptor.vertexShader = vertexShader;
    descriptor.fragmentShader = fragmentShader;
    
    // Now request a program
    Program id = requestProgram(descriptor);
    
    return id;
}

// ** RenderingContext::requestProgram
Program RenderingContext::requestProgram(const String& vertex, const String& fragment)
{
    // Create vertex and fragment shaders
    VertexShader   vertexShader   = m_shaderLibrary.addVertexShader(vertex);
    FragmentShader fragmentShader = m_shaderLibrary.addFragmentShader(fragment);
    
    // Now request a program.
    Program id = requestProgram(vertexShader, fragmentShader);
    
    return id;
}
    
// ** RenderingContext::requestProgram
Program RenderingContext::requestProgram(const String& vertex)
{
    // Create vertex and fragment shaders
    VertexShader vertexShader = m_shaderLibrary.addVertexShader(vertex);
    
    // Now request a program.
    Program id = requestProgram(vertexShader, FragmentShader());
    
    return id;
}

// ** RenderingContext::deleteConstantBuffer
void RenderingContext::deleteConstantBuffer(ConstantBuffer_ id)
{
    m_resourceCommandBuffer->deleteConstantBuffer(id);
}

// ** RenderingContext::deleteConstantBuffer
void RenderingContext::deleteUniformLayout(UniformLayout id)
{
    m_uniformLayoutByName.remove(id);
    m_uniformLayouts.remove(id);
    releaseIdentifier(RenderResourceType::UniformLayout, id);
}

// ** RenderingContext::deleteConstantBuffer
void RenderingContext::deleteProgram(Program id)
{
    m_resourceCommandBuffer->deleteProgram(id);
}

// ** RenderingContext::findUniformLayout
const UniformElement* RenderingContext::findUniformLayout(const String& name) const
{
    if (m_uniformLayoutByName.has(name))
    {
        return &m_uniformLayouts[m_uniformLayoutByName.find(name)->second][0];
    }

    return NULL;
}

// ** RenderingContext::createVertexBufferLayout
VertexBufferLayoutUPtr RenderingContext::createVertexBufferLayout(VertexFormat vertexFormat) const
{
    // Create an input layout
    VertexBufferLayoutUPtr inputLayout = DC_NEW VertexBufferLayout(vertexFormat.vertexSize());
    
    // Add vertex attributes to an input layout
    if(vertexFormat & VertexFormat::Position)
    {
        inputLayout->attributeLocation(VertexPosition, 3, vertexFormat.attributeOffset(VertexFormat::Position));
    }
    if(vertexFormat & VertexFormat::Color)
    {
        inputLayout->attributeLocation(VertexColor, 4, vertexFormat.attributeOffset(VertexFormat::Color));
    }
    if(vertexFormat & VertexFormat::Normal)
    {
        inputLayout->attributeLocation(VertexNormal, 3, vertexFormat.attributeOffset(VertexFormat::Normal));
    }
    if(vertexFormat & VertexFormat::TexCoord0)
    {
        inputLayout->attributeLocation(VertexTexCoord0, 2, vertexFormat.attributeOffset(VertexFormat::TexCoord0));
    }
    if(vertexFormat & VertexFormat::TexCoord1)
    {
        inputLayout->attributeLocation(VertexTexCoord1, 2, vertexFormat.attributeOffset(VertexFormat::TexCoord1));
    }
    
    return inputLayout;
}

// ** RenderingContext::deprecatedRequestShader
Program RenderingContext::deprecatedRequestShader(const String& fileName)
{
    static CString vertexShaderMarker   = "[VertexShader]";
    static CString fragmentShaderMarker = "[FragmentShader]";
    
    // Read the code from an input stream
    String code = Io::DiskFileSystem::readTextFile( fileName );
    NIMBLE_ABORT_IF(code.empty(), "a shader source is empty or file not found");
    
    // Extract vertex/fragment shader code blocks
    size_t vertexBegin   = code.find( vertexShaderMarker );
    size_t fragmentBegin = code.find( fragmentShaderMarker );
    
    if( vertexBegin == String::npos && fragmentBegin == String::npos )
    {
        return 0;
    }
    
    String vertexShader, fragmentShader;
    
    if( vertexBegin != String::npos )
    {
        u32 vertexCodeStart = vertexBegin + strlen( vertexShaderMarker );
        vertexShader = code.substr( vertexCodeStart, fragmentBegin > vertexBegin ? fragmentBegin - vertexCodeStart : String::npos );
    }
    
    if( fragmentBegin != String::npos )
    {
        u32 fragmentCodeStart = fragmentBegin + strlen( fragmentShaderMarker );
        fragmentShader = code.substr( fragmentCodeStart, vertexBegin > fragmentBegin ? vertexBegin - fragmentCodeStart : String::npos );
    }
    
    return requestProgram(vertexShader, fragmentShader);
}
    
} // namespace Renderer

DC_END_DREEMCHEST
