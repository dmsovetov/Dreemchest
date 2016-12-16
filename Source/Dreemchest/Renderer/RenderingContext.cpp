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
#include "RenderingContextHal.h"
#include "CommandBuffer.h"
#include "VertexFormat.h"
#include "VertexBufferLayout.h"
#include "RenderFrame.h"

#include "../Io/DiskFileSystem.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// -------------------------------------------------------------------- RenderingContext::TransientTargetStack ------------------------------------------------------------------- //

/*!
 Transient target stack is used to convert local indices that are
 stored in commands to a global index of an intermediate render target.
 */
class RenderingContext::TransientTargetStack
{
public:
    
    //! A maximum number of intermediate render targets that can be hold by a single stack frame.
    enum { StackFrameSize = 8 };
    
    //! A maximum number of stack frames that can be pushed during rendering.
    enum { MaxStackFrames = 8 };
    
    //! A total size of an intermediate stack size.
    enum { MaxStackSize = MaxStackFrames * StackFrameSize };
    
    //! An intermediate render target slot index type.
    typedef u8 Index;
    
                                //! Constructs an TransientTargetStack instance.
                                TransientTargetStack( void );
    
    //! Pushes a new stack frame.
    void                        pushFrame( void );
    
    //! Pops an active stack frame.
    void                        popFrame( void );
    
    //! Returns a render target by a local index.
    TransientRenderTarget       get( Index index ) const;
    
    //! Loads an acquired intermediate render target to a specified local slot.
    void                        load( Index index, TransientRenderTarget id );
    
    //! Unloads an intermediate render target from a specified local slot.
    void                        unload( Index index );
    
private:
    
    TransientRenderTarget*   m_stackFrame;                   //!< An active render target stack frame.
    TransientRenderTarget    m_identifiers[MaxStackSize];    //!< An array of intermediate render target handles.
};

// ** RenderingContext::TransientTargetStack::TransientTargetStack
RenderingContext::TransientTargetStack::TransientTargetStack( void )
    : m_stackFrame( m_identifiers )
{
    memset( m_identifiers, 0, sizeof m_identifiers );
}

// ** RenderingContext::TransientTargetStack::pushFrame
void RenderingContext::TransientTargetStack::pushFrame( void )
{
    NIMBLE_ABORT_IF( (m_stackFrame + StackFrameSize) > (m_identifiers + MaxStackSize), "frame stack overflow" );
    m_stackFrame += StackFrameSize;
}

// ** RenderingContext::TransientTargetStack::popFrame
void RenderingContext::TransientTargetStack::popFrame( void )
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

// ** RenderingContext::TransientTargetStack::get
TransientRenderTarget RenderingContext::TransientTargetStack::get( Index index ) const
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    return m_stackFrame[index - 1];
}

// ** RenderingContext::TransientTargetStack::load
void RenderingContext::TransientTargetStack::load( Index index, TransientRenderTarget id )
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    m_stackFrame[index - 1] = id;
}

// ** RenderingContext::TransientTargetStack::unload
void RenderingContext::TransientTargetStack::unload( Index index )
{
    NIMBLE_ABORT_IF( index == 0, "invalid render target index" );
    m_stackFrame[index - 1] = 0;
}

// --------------------------------------------------------- RenderingContext::ConstructionCommandBuffer --------------------------------------------------------- //

//! An internal comamnd buffer type that is used for resource construction.
class RenderingContext::ConstructionCommandBuffer : public CommandBuffer
{
public:
    
    //! Emits an input layout creation command.
    InputLayout                 createInputLayout(InputLayout id, const VertexFormat& vertexFormat);
    
    //! Emits a vertex buffer creation command.
    VertexBuffer_               createVertexBuffer(VertexBuffer_ id, const void* data, s32 size);
    
    //! Emits an index buffer creation command.
    IndexBuffer_                createIndexBuffer(IndexBuffer_ id, const void* data, s32 size);
    
    //! Emits a constant buffer creation command.
    ConstantBuffer_             createConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, const ConstantBufferLayout* layout);
    
    //! Emits a texture creation command.
    Texture_                    createTexture(Texture_ id, const void* data, u16 width, u16 height, PixelFormat format);
};

// ** RenderingContext::ConstructionCommandBuffer::createVertexBuffer
InputLayout RenderingContext::ConstructionCommandBuffer::createInputLayout(InputLayout id, const VertexFormat& vertexFormat)
{
    OpCode opCode;
    opCode.type = OpCode::CreateInputLayout;
    opCode.createInputLayout.id = id;
    opCode.createInputLayout.format = vertexFormat;
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createVertexBuffer
VertexBuffer_ RenderingContext::ConstructionCommandBuffer::createVertexBuffer(VertexBuffer_ id, const void *data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::CreateVertexBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.data = data;
    opCode.createBuffer.size = size;
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createIndexBuffer
IndexBuffer_ RenderingContext::ConstructionCommandBuffer::createIndexBuffer(IndexBuffer_ id, const void *data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::CreateIndexBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.data = data;
    opCode.createBuffer.size = size;
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createConstantBuffer
ConstantBuffer_ RenderingContext::ConstructionCommandBuffer::createConstantBuffer(ConstantBuffer_ id, const void* data, s32 size, const ConstantBufferLayout* layout)
{
    OpCode opCode;
    opCode.type = OpCode::CreateConstantBuffer;
    opCode.createBuffer.id = id;
    opCode.createBuffer.data = data;
    opCode.createBuffer.size = size;
    opCode.createBuffer.userData = layout;
    push( opCode );
    return id;
}

// ** RenderingContext::ConstructionCommandBuffer::createTexture
Texture_ RenderingContext::ConstructionCommandBuffer::createTexture(Texture_ id, const void* data, u16 width, u16 height, PixelFormat format)
{
    OpCode opCode;
    opCode.type = OpCode::CreateTexture;
    opCode.createTexture.id = id;
    opCode.createTexture.data = data;
    opCode.createTexture.width = width;
    opCode.createTexture.height = height;
    opCode.createTexture.format = format;
    push( opCode );
    return id;
}
    
// -------------------------------------------------------------------------------- RenderingContext -------------------------------------------------------------------------------- //
    
// ** RenderingContext::RenderingContext
RenderingContext::RenderingContext(RenderViewPtr view)
    : m_view(view)
{
    // Create a construction command buffer instance
    m_constructionCommandBuffer = DC_NEW ConstructionCommandBuffer;
    
    // Create an intermediate target stack
    m_intermediateTargets = DC_NEW TransientTargetStack;
    
    // Reset input layout cache
    memset( m_inputLayoutCache, 0, sizeof( m_inputLayoutCache ) );
    
    // Resize all resource index managers
    for (s32 i = 0; i < RenderResourceType::TotalTypes; i++)
    {
        // Allocate space for 32 resource identifiers
        m_resourceIdentifiers[i].resize(32);
        
        // And reserve the zero id.
        m_resourceIdentifiers[i].acquire();
    }
    
    // Initialize a default state block
    m_defaultStateBlock.setCullFace(TriangleFaceBack);
    m_defaultStateBlock.setDepthState(LessEqual, true);
    m_defaultStateBlock.disableAlphaTest();
    //m_defaultStateBlock.setPolygonMode(PolygonFill);
    //m_defaultStateBlock.bindProgram(0);
    //m_defaultStateBlock.bindVertexBuffer(0);
    //for (s32 i = 0; i < State::MaxTextureSamplers; i++)
    //{
    //    m_defaultStateBlock.bindTexture(0, static_cast<State::TextureSampler>(i));
    //}
    
    // Create a default program
    Program defaultProgram = requestProgram(ShaderProgramDescriptor());
    setDefaultProgram(defaultProgram);
}
    
// ** RenderingContext::~RenderingContext
RenderingContext::~RenderingContext( void )
{
    delete m_constructionCommandBuffer;
}
    
// ** RenderingContext::setDefaultStateBlock
void RenderingContext::setDefaultStateBlock(const StateBlock& value)
{
    m_defaultStateBlock = value;
}
    
// ** RenderingContext::setDefaultProgram
void RenderingContext::setDefaultProgram(Program value)
{
    m_defaultProgram = value;
}
    
// ** RenderingContext::display
void RenderingContext::display( RenderFrame& frame )
{
    // Begin frame
    if (m_view.valid())
    {
        m_view->beginFrame();
    }
    
    // First execute a construction command buffer
    execute(frame, *m_constructionCommandBuffer);
    m_constructionCommandBuffer->reset();
    
    // Execute an entry point command buffer
    execute( frame, frame.entryPoint() );
    
    // Reset rendering states
    applyStateBlock(frame, m_defaultStateBlock);
    
    // End frame
    if (m_view.valid())
    {
        m_view->endFrame();
    }
}
    
// ** RenderingContext::execute
void RenderingContext::execute( const RenderFrame& frame, const CommandBuffer& commands )
{
    // Push a new frame to an intermediate target stack
    m_intermediateTargets->pushFrame();
    
    // Execute a command buffer
    executeCommandBuffer(frame, commands);
    
    // Pop a stack frame
    m_intermediateTargets->popFrame();
}
    
// ** RenderingContext::loadTransientTarget
void RenderingContext::loadTransientTarget(u8 index, TransientRenderTarget id)
{
    m_intermediateTargets->load(index, id);
}

// ** RenderingContext::unloadTransientTarget
void RenderingContext::unloadTransientTarget(u8 index)
{
    m_intermediateTargets->unload(index);
}
    
// ** RenderingContext::intermediateTarget
TransientRenderTarget RenderingContext::intermediateTarget(u8 index)
{
    return m_intermediateTargets->get(index);
}

// ** RenderingContext::allocateResourceIdentifier
u16 RenderingContext::allocateResourceIdentifier(RenderResourceType::Enum type)
{
    NIMBLE_ABORT_IF(!m_resourceIdentifiers[type].hasFreeIndices(), "too much resource indices allocated");
    return m_resourceIdentifiers[type].acquire();
}
    
// ** RenderingContext::requestInputLayout
InputLayout RenderingContext::requestInputLayout( const VertexFormat& format )
{
    // First lookup a previously constucted input layout
    InputLayout id = m_inputLayoutCache[format];
    
    if( id )
    {
        return id;
    }
    
    // Allocate an input layout identifier
    id = allocateResourceIdentifier(RenderResourceType::InputLayout);
    
    // Nothing found - construct a new one
    id = m_constructionCommandBuffer->createInputLayout(id, format);
    
    // Now put a new input layout to cache
    m_inputLayoutCache[format] = id;
    
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
    FeatureLayout id = allocateResourceIdentifier(RenderResourceType::FeatureLayout);
    
    // Put this layout instance to a pool
    m_pipelineFeatureLayouts.emplace(id, layout);

    return id;
}

// ** RenderingContext::requestVertexBuffer
VertexBuffer_ RenderingContext::requestVertexBuffer( const void* data, s32 size )
{
    VertexBuffer_ id = allocateResourceIdentifier(RenderResourceType::VertexBuffer);
    return m_constructionCommandBuffer->createVertexBuffer(id, data, size);
}

// ** RenderingContext::requestIndexBuffer
IndexBuffer_ RenderingContext::requestIndexBuffer( const void* data, s32 size )
{
    IndexBuffer_ id = allocateResourceIdentifier(RenderResourceType::IndexBuffer);
    return m_constructionCommandBuffer->createIndexBuffer(id, data, size);
}

// ** RenderingContext::requestConstantBuffer
ConstantBuffer_ RenderingContext::requestConstantBuffer( const void* data, s32 size, const ConstantBufferLayout* layout )
{
    ConstantBuffer_ id = allocateResourceIdentifier(RenderResourceType::ConstantBuffer);
    return m_constructionCommandBuffer->createConstantBuffer(id, data, size, layout);
}

// ** RenderingContext::requestConstantBuffer
Texture_ RenderingContext::requestTexture( const void* data, u16 width, u16 height, PixelFormat format )
{
    Texture_ id = allocateResourceIdentifier(RenderResourceType::Texture);
    return m_constructionCommandBuffer->createTexture(id, data, width, height, format);
}
    
// ** RenderingContext::requestProgram
Program RenderingContext::requestProgram(const ShaderProgramDescriptor& descriptor)
{
    // Allocate a program identifier
    Program id = allocateResourceIdentifier(RenderResourceType::Program);
    
    // Put this program to a pool
    m_programs.emplace(id, descriptor);
    
    return id;
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

// ** RenderingContext::mergeStateBlocks
s32 RenderingContext::mergeStateBlocks(const StateBlock* const * stateBlocks, s32 blockCount, State* states, s32 maxStates, PipelineFeatures& userDefined) const
{
    PipelineFeatures userFeatures = 0;
    PipelineFeatures userFeaturesMask = ~0;
    
    // A bitmask of states that were already set
    u32 activeStateMask = 0;
    
    // A total number of states written to an output array
    s32 statesWritten = 0;
    
    for( s32 i = 0; i < blockCount; i++ )
    {
        // Get a state block at specified index
        const StateBlock* block = stateBlocks[i];
        
        // No more state blocks in a stack - break
        if( block == NULL )
        {
            break;
        }
        
        // Update feature set
        userFeatures      = userFeatures     | block->features();
        userFeaturesMask  = userFeaturesMask & block->featureMask();
        
        // Skip redundant state blocks by testing a block bitmask against an active state mask
        if( (activeStateMask ^ block->mask()) == 0 )
        {
            continue;
        }
        
        // Apply all states in a block
        for( s32 j = 0, n = block->stateCount(); j < n; j++ )
        {
            // Get a render state bit
            u32 stateBit = block->stateBit( j );
            
            // Skip redundate state blocks by testing a state bitmask agains an active state mask
            if( activeStateMask & stateBit )
            {
                continue;
            }
            
            NIMBLE_ABORT_IF(statesWritten >= maxStates, "to much render states");
            
            // Write a render state at specified index to an output array
            states[statesWritten++] = block->state(j);
            
            // Update an active state mask
            activeStateMask = activeStateMask | stateBit;
        }
    }
    
    // Compose a user defined feature mask
    userDefined = userFeatures & userFeaturesMask;
    
    return statesWritten;
}

// ** RenderingContext::startPipelineConfiguration
s32 RenderingContext::startPipelineConfiguration(const StateBlock* const * stateBlocks, s32 blockCount, State* states, s32 maxStates, PipelineFeatures& userDefined)
{
    // Merge all state blocks to a single array of rendering states
    s32 stateCount = mergeStateBlocks(stateBlocks, blockCount, states, maxStates, userDefined);
    
    // This will start recording pipeline changes
    m_pipeline.beginStateBlock();
    
    return stateCount;
}

// ** RenderingContext::finishPipelineConfiguration
PipelineFeatures RenderingContext::finishPipelineConfiguration(PipelineFeatures userDefined)
{
    // Apply a user defined feature mask
    m_pipeline.activateUserFeatures(userDefined);
    
    // Finish applying state blocks
    m_pipeline.endStateBlock();
    
    //return features;
    return m_pipeline.features();
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
    
// ** RenderingContext::generateShaderCode
String RenderingContext::generateShaderCode(const String& source, PipelineFeatures features, const PipelineFeatureLayout* featureLayout) const
{
    // No feature layout, so just return a shader code
    if (!featureLayout)
    {
        return source;
    }
    
    // Generate macro definitions from features
    String macro = "";
    String debug = "";
    
    for (s32 i = 0, n = featureLayout->elementCount(); i < n; i++)
    {
        const PipelineFeatureLayout::Element& element = featureLayout->elementAt(i);
        
        if (element.mask & features)
        {
            macro += "#define " + element.name + " " + toString((element.mask & features) >> element.offset) + "\n";
            if(debug.length()) debug += ", ";
            debug += element.name;
        }
    }
    
    LogVerbose( "renderingContext", "compiling permutation %s\n", debug.empty() ? "" : ("(" + debug + ")").c_str() );
    
    return macro + source;
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
    
/*
// ** RenderingContext::requestShader
Program RenderingContext::requestShader(const String& vertex, const String& fragment)
{
    // Create a shader instance
    UbershaderPtr shader = DC_NEW Ubershader;
    
    // Set vertex and fragment shader code
    shader->setVertex(vertex);
    shader->setFragment(fragment);
    
    // ----------------------------------------------------
    shader->addInclude(
                       "                                                       \n\
                       struct CBufferScene    { vec4 ambient; };           \n\
                       struct CBufferView     { mat4 transform; float near; float far; vec3 position; };         \n\
                       struct CBufferInstance { mat4 transform; };         \n\
                       struct CBufferMaterial { vec4 diffuse; vec4 specular; vec4 emission; struct { vec3 color; float factor; float start; float end; } rim; };   \n\
                       struct CBufferLight    { vec3 position; float range; vec3 color; float intensity; vec3 direction; float cutoff; };         \n\
                       struct CBufferShadow   { mat4 transform; float invSize; };         \n\
                       struct CBufferClipPlanes { vec4 equation[6]; };         \n\
                       uniform CBufferScene    Scene;                      \n\
                       uniform CBufferView     View;                       \n\
                       uniform CBufferInstance Instance;                   \n\
                       uniform CBufferMaterial Material;                   \n\
                       uniform CBufferLight    Light;                      \n\
                       uniform CBufferShadow   Shadow;                     \n\
                       uniform CBufferClipPlanes ClipPlanes;               \n\
                       #define u_DiffuseTexture Texture0                   \n\
                       #define u_ShadowTexture  Texture1                   \n\
                       "
                       );
    
    Program id = allocateResourceIdentifier(RenderResourceType::Program);
    
    // Put this shader to a pool
    m_shaders.emplace(id, shader);

    return id;
}*/
    
} // namespace Renderer

DC_END_DREEMCHEST
