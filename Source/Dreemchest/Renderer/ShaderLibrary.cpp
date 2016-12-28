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

#include "ShaderLibrary.h"
#include "PipelineFeatureLayout.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ----------------------------------------------------------------- SharedFunctionPreprocessor ----------------------------------------------------------------- //

//! Performs shared function preprocessing
class ShaderLibrary::SharedFunctionPreprocessor : public ShaderPreprocessor
{
public:
    
                            //! Constructs an SharedFunctionPreprocessor instance.
                            SharedFunctionPreprocessor(const ShaderLibrary& shaderLibrary);
    
    //! Imports all used shared functions.
    virtual bool            preprocess(const RenderingContext& renderingContext, String& source) const NIMBLE_OVERRIDE;
    
private:
    
    const ShaderLibrary&    m_shaderLibrary;    //!< A parent shader library.
};
    
// ** ShaderLibrary::SharedFunctionPreprocessor::SharedFunctionPreprocessor
ShaderLibrary::SharedFunctionPreprocessor::SharedFunctionPreprocessor(const ShaderLibrary& shaderLibrary)
    : m_shaderLibrary(shaderLibrary)
{
    
}

// ** ShaderLibrary::SharedFunctionPreprocessor::preprocess
bool ShaderLibrary::SharedFunctionPreprocessor::preprocess(const RenderingContext& renderingContext, String& source) const
{
    const ShaderLibrary::SharedFunctions& sharedFunctions = m_shaderLibrary.sharedFunctions();
    
    for (ShaderLibrary::SharedFunctions::const_iterator i = sharedFunctions.begin(), end = sharedFunctions.end(); i != end; ++i)
    {
        if (source.find(i->name) == String::npos)
        {
            continue;
        }
        
        LogVerbose("shaderLibrary", "imporing shared function '%s'\n", i->name.c_str());
        
        source = i->code + source;
    }
    
    return true;
}
    
// ----------------------------------------------------------------- IncludePreprocessor ----------------------------------------------------------------- //

//! Performs include directive preprocessing
class ShaderLibrary::IncludePreprocessor : public ShaderPreprocessor
{
public:
 
                            //! Constructs an IncludePreprocessor instance.
                            IncludePreprocessor(const ShaderLibrary& shaderLibrary);
    
    //! Replaces all #include entries
    virtual bool            preprocess(const RenderingContext& renderingContext, String& source) const NIMBLE_OVERRIDE;
    
private:
    
    const ShaderLibrary&    m_shaderLibrary;    //!< A parent shader library.
};
    
// ** ShaderLibrary::IncludePreprocessor::IncludePreprocessor
ShaderLibrary::IncludePreprocessor::IncludePreprocessor(const ShaderLibrary& shaderLibrary)
    : m_shaderLibrary(shaderLibrary)
{
    
}
    
// ** ShaderLibrary::IncludePreprocessor::preprocess
bool ShaderLibrary::IncludePreprocessor::preprocess(const RenderingContext& renderingContext, String& source) const
{
    static const String s_include    = "#include";
    static const String s_whitespace = " \t\n";
    
    size_t offset = source.find(s_include);
    
    while (offset != String::npos)
    {
        // Save the start of a cbuffer expression
        size_t start = offset;
        
        // Skip a 'cbuffer' token
        offset += s_include.length();
        
        // Now skip spaces
        offset = source.find_first_not_of(s_whitespace, offset);
        NIMBLE_ABORT_IF(offset == String::npos, "shader parsing error");
        
        // Parse include file name
        String fileName;
        
        NIMBLE_BREAK_IF(source[offset] != '\'' && source[offset] != '"', "shader parsing error");
        offset++;
        
        while (source[offset] != '\'' && source[offset] != '"')
        {
            fileName += source[offset++];
        }
        
        // Get a shader code by name
        String include;
        
        if (!m_shaderLibrary.findSharedSource(fileName, include))
        {
            LogWarning("shaderLibrary", "an include file '%s' could not be found\n", fileName.c_str());
        }
        else
        {
            m_shaderLibrary.preprocessShader(include);
        }
        
        source.replace(start, offset - start + 1, include);
        
        // Search for a next one
        offset = source.find(s_include, start + include.length());
    }
    
    return true;
}
    
// -------------------------------------------------------------------- ShaderLibrary -------------------------------------------------------------------- //

// ** ShaderLibrary::ShaderLibrary
ShaderLibrary::ShaderLibrary(const RenderingContext& renderingContext)
    : m_renderingContext(renderingContext)
{
    for (s32 i = 0; i < TotalShaderTypes; i++)
    {
        // Resize shader resource identifiers
        m_identifiers[i].resize(32);
    }
    
    // Now allocate default shaders
    addVertexShader("void main() { gl_Position = gl_Vertex; }");
    addFragmentShader("void main() { gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); }");
    addGeometryShader("");
    addComputeShader("");
    
    addPreprocessor(DC_NEW IncludePreprocessor(*this));
    addPreprocessor(DC_NEW SharedFunctionPreprocessor(*this));
}
    
// ** ShaderLibrary::addVertexShader
VertexShader ShaderLibrary::addVertexShader(const String& code, const String& name)
{
    return allocateShader(name, code, VertexShaderType);
}
    
// ** ShaderLibrary::vertexShader
const String& ShaderLibrary::vertexShader(VertexShader id) const
{
    return m_shaders[VertexShaderType][id];
}

// ** ShaderLibrary::addGeometryShader
GeometryShader ShaderLibrary::addGeometryShader(const String& code, const String& name)
{
    return allocateShader(name, code, GeometryShaderType);
}
    
// ** ShaderLibrary::geometryShader
const String& ShaderLibrary::geometryShader(GeometryShader id) const
{
    return m_shaders[GeometryShaderType][id];
}

// ** ShaderLibrary::addFragmentShader
FragmentShader ShaderLibrary::addFragmentShader(const String& code, const String& name)
{
    return allocateShader(name, code, FragmentShaderType);
}
    
// ** ShaderLibrary::fragmentShader
const String& ShaderLibrary::fragmentShader(FragmentShader id) const
{
    return m_shaders[FragmentShaderType][id];
}

// ** ShaderLibrary::addComputeShader
ComputeShader ShaderLibrary::addComputeShader(const String& code, const String& name)
{
    return allocateShader(name, code, ComputeShaderType);
}
    
// ** ShaderLibrary::computeShader
const String& ShaderLibrary::computeShader(ComputeShader id) const
{
    return m_shaders[ComputeShaderType][id];
}
 
// ** ShaderLibrary::allocateShader
ResourceId ShaderLibrary::allocateShader(const String& name, const String& code, ShaderType type)
{
    NIMBLE_ABORT_IF(!m_identifiers[type].hasFreeIndices(), "too much shaders allocated");
    
    // Acquire a next resource identifier
    ResourceId id = m_identifiers[type].acquire();

    // Emplace a shader instance with this identifier
    m_shaders[type].emplace(id, code);
    
    return id;
}

// ** ShaderLibrary::generateShaderCode
bool ShaderLibrary::generateShaderCode(const ShaderProgramDescriptor& program, PipelineFeatures features, const PipelineFeatureLayout* featureLayout, String result[TotalShaderTypes]) const
{
    // Generate an options string
    String options = generateOptionsString(features, featureLayout);
    //NIMBLE_BREAK_IF(!program.vertexShader, "a shader program should contain a vertex shader");
    
    if (program.vertexShader)
    {
        result[VertexShaderType] = options + vertexShader(program.vertexShader);
    }

    if (program.geometryShader)
    {
        result[GeometryShaderType] = options + geometryShader(program.geometryShader);
    }

    if (program.fragmentShader)
    {
        result[FragmentShaderType] = options + fragmentShader(program.fragmentShader);
    }

    if (program.computeShader)
    {
        result[ComputeShaderType] = options + computeShader(program.computeShader);
    }

    for (s32 i = 0; i < TotalShaderTypes; i++)
    {
        preprocessShader(result[i]);
    }

    return true;
}
    
// ** ShaderLibrary::generateOptionsString
String ShaderLibrary::generateOptionsString(PipelineFeatures features, const PipelineFeatureLayout* featureLayout) const
{
    // No feature layout, so just return an empty string
    if (!featureLayout)
    {
        return "";
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

    LogVerbose( "shaderLibrary", "compiling permutation %s\n", debug.empty() ? "" : ("(" + debug + ")").c_str() );
    
    return macro;
}

// ** ShaderLibrary::preprocessShader
void ShaderLibrary::preprocessShader(String& shader) const
{
    if (shader.empty())
    {
        return;
    }
    
    shader = "\n#define M_PI 3.1415926535897932384626433832795\n" + shader;
    
    for (List<ShaderPreprocessorUPtr>::const_iterator i = m_preprocessors.begin(); i != m_preprocessors.end(); ++i)
    {
        (*i)->preprocess(m_renderingContext, shader);
    }
}

// ** ShaderLibrary::addPreprocessor
void ShaderLibrary::addPreprocessor(ShaderPreprocessorUPtr preprocessor)
{
    m_preprocessors.push_back(preprocessor);
}
    
// ** ShaderLibrary::findSharedSource
bool ShaderLibrary::findSharedSource(const String& name, String& source) const
{
    Map<String, String>::const_iterator i = m_sharedSources.find(name);
    
    if (i == m_sharedSources.end())
    {
        return false;
    }
    
    source = i->second;
    return true;
}
    
// ** ShaderLibrary::addSharedSource
void ShaderLibrary::addSharedSource(const String& name, const String& source)
{
    NIMBLE_ABORT_IF(m_sharedSources.count(name) != 0, "a shader library already contains a shared source with the same name");
    m_sharedSources[name] = source;
}
    
// ** ShaderLibrary::addSharedFunction
void ShaderLibrary::addSharedFunction(const String& name, const String& source)
{
    SharedFunction sharedFunction;
    sharedFunction.name = name;
    sharedFunction.code = source;
    m_sharedFunctions.push_back(sharedFunction);
}

// ** ShaderLibrary::sharedFunctions
const ShaderLibrary::SharedFunctions& ShaderLibrary::sharedFunctions() const
{
    return m_sharedFunctions;
}

// --------------------------------------------------- UniformBufferPreprocessor --------------------------------------------------- //

// ** UniformBufferPreprocessor::preprocess
bool UniformBufferPreprocessor::preprocess(const RenderingContext& renderingContext, String& source) const
{
    static const String s_cbuffer = "cbuffer";
    static const String s_whitespace = " \t\n";

    size_t offset = source.find(s_cbuffer);

    while (offset != String::npos)
    {
        // Save the start of a cbuffer expression
        size_t start = offset;

        // Skip a 'cbuffer' token
        offset += s_cbuffer.length();

        // Now skip spaces
        offset = source.find_first_not_of(s_whitespace, offset);
        NIMBLE_ABORT_IF(offset == String::npos, "shader parsing error");

        // Parse uniform buffer type
        String type;

        while (!isspace(source[offset]))
        {
            type += source[offset++];
        }

        // Skip spaces again
        offset = source.find_first_not_of(s_whitespace, offset);
        NIMBLE_ABORT_IF(offset == String::npos, "shader parsing error");

        // Parse uniform name.
        String name;

        while (!isspace(source[offset]))
        {
            name += source[offset++];
        }

        // Skip spaces
        offset = source.find_first_not_of(s_whitespace, offset);
        NIMBLE_ABORT_IF(offset == String::npos, "shader parsing error");

        NIMBLE_ABORT_IF(source[offset] != ':', "shader parsing error");
        offset++;

        offset = source.find_first_not_of(s_whitespace, offset);
        NIMBLE_ABORT_IF(offset == String::npos, "shader parsing error");

        // Parse slot index
        String slot;

        while (!isspace(source[offset]) && source[offset] != ';')
        {
            slot += source[offset++];
        }

        // Generate a cbuffer definition
        String definition = generateBufferDefinition(renderingContext, type, name, atoi(slot.c_str()));
        
        // And replace it.
        source.replace(start, offset - start + 1, definition);

        // Search for a next one
        offset = source.find(s_cbuffer, start + definition.length());
    }

    return true;
}

} // namespace Renderer

DC_END_DREEMCHEST
