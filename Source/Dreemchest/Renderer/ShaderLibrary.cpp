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
}
    
// ** ShaderLibrary::addVertexShader
VertexShader ShaderLibrary::addVertexShader(const String& code, const String& name)
{
    return allocateShader(name, code, VertexShaderType);
}
    
// ** ShaderLibrary::vertexShader
const String& ShaderLibrary::vertexShader(VertexShader id) const
{
    return m_shaders[VertexShaderType][id].code;
}

// ** ShaderLibrary::addGeometryShader
GeometryShader ShaderLibrary::addGeometryShader(const String& code, const String& name)
{
    return allocateShader(name, code, GeometryShaderType);
}
    
// ** ShaderLibrary::geometryShader
const String& ShaderLibrary::geometryShader(GeometryShader id) const
{
    return m_shaders[GeometryShaderType][id].code;
}

// ** ShaderLibrary::addFragmentShader
FragmentShader ShaderLibrary::addFragmentShader(const String& code, const String& name)
{
    return allocateShader(name, code, FragmentShaderType);
}
    
// ** ShaderLibrary::fragmentShader
const String& ShaderLibrary::fragmentShader(FragmentShader id) const
{
    return m_shaders[FragmentShaderType][id].code;
}

// ** ShaderLibrary::addComputeShader
ComputeShader ShaderLibrary::addComputeShader(const String& code, const String& name)
{
    return allocateShader(name, code, ComputeShaderType);
}
    
// ** ShaderLibrary::computeShader
const String& ShaderLibrary::computeShader(ComputeShader id) const
{
    return m_shaders[ComputeShaderType][id].code;
}
 
// ** ShaderLibrary::allocateShader
PersistentResourceId ShaderLibrary::allocateShader(const String& name, const String& code, ShaderType type)
{
    NIMBLE_ABORT_IF(!m_identifiers[type].hasFreeIndices(), "too much shaders allocated");
    
    // Acquire a next resource identifier
    PersistentResourceId id = m_identifiers[type].acquire();
    
    // Create shader
    Shader shader;
    shader.name = name;
    shader.code = code;
    
    // Emplace a shader instance with this identifier
    m_shaders[type].emplace(id, shader);
    
    return id;
}

// ** ShaderLibrary::generateShaderCode
bool ShaderLibrary::generateShaderCode(const ShaderProgramDescriptor& program, PipelineFeatures features, const PipelineFeatureLayout* featureLayout, String result[TotalShaderTypes]) const
{
    // Generate an options string
    String options = generateOptionsString(features, featureLayout);

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
        if (result[i].length())
        {
            preprocess(result[i]);
        }
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

// ** ShaderLibrary::preprocess
void ShaderLibrary::preprocess(String& shader) const
{
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

        while (!isspace(source[offset]) && source[offset] != ';')
        {
            name += source[offset++];
        }

        // Generate a cbuffer definition
        String definition = generateBufferDefinition(renderingContext, type, name);

        // And replace it.
        source.replace(start, offset - start, definition);

        // Search for a next one
        offset = source.find(s_cbuffer, start + definition.length());
    }

    return true;
}

} // namespace Renderer

DC_END_DREEMCHEST
