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

#ifndef __DC_Renderer_ShaderLibrary_H__
#define __DC_Renderer_ShaderLibrary_H__

#include "Renderer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! Shader source code preprocessor.
    class ShaderPreprocessor
    {
    public:

        virtual                         ~ShaderPreprocessor( void ) {}

        //1 Performs a shader source code preprocessing.
        virtual bool                    preprocess(const RenderingContext& renderingContext, String& shader) const NIMBLE_ABSTRACT;
    };

    //! Shader library contains all shader sources exposed to a rendering context.
    class ShaderLibrary
    {
    public:

        //! A shader preprocessor unique pointer type.
        typedef UPtr<ShaderPreprocessor> ShaderPreprocessorUPtr;
        
        //! A shared function name and code.
        struct SharedFunction
        {
            String                      name;   //!< A function name.
            String                      code;   //!< A function ncode.
        };
        
        typedef List<SharedFunction>    SharedFunctions;
        
                                        //! Constructs a ShaderLibrary instance.
                                        ShaderLibrary(const RenderingContext& renderingContext);
        
        //! Adds a new vertex shader
        VertexShader                    addVertexShader(const String& code, const String& name = "");
        
        //! Returns a vertex shader code by a handle.
        const String&                   vertexShader(VertexShader id) const;
        
        //! Adds a new geometry shader.
        GeometryShader                  addGeometryShader(const String& code, const String& name = "");
        
        //! Returns a geometry shader code by a handle.
        const String&                   geometryShader(GeometryShader id) const;
        
        //! Adds a new fragment shader.
        FragmentShader                  addFragmentShader(const String& code, const String& name = "");
        
        //! Returns a fragment shader code by a handle.
        const String&                   fragmentShader(FragmentShader id) const;
        
        //! Adds a new compute shader.
        ComputeShader                   addComputeShader(const String& code, const String& name = "");
        
        //! Returns a compute shader code by a handle.
        const String&                   computeShader(ComputeShader id) const;

        //! Generates a shader source code.
        bool                            generateShaderCode(const ShaderProgramDescriptor& program, PipelineFeatures features, const PipelineFeatureLayout* featureLayout, String result[TotalShaderTypes]) const;
        
        //! Adds a new shader preprocessor to this library.
        void                            addPreprocessor(ShaderPreprocessorUPtr preprocessor);
        
        //! Performs a shader preprocessing.
        void                            preprocessShader(String& source) const;
        
        //! Returns a shared source by an identifier.
        bool                            findSharedSource(const String& name, String& source) const;
        
        //! Adds a new shared source.
        void                            addSharedSource(const String& name, const String& value);
        
        //! Adds a new shared function.
        void                            addSharedFunction(const String& name, const String& source);
        
        //! Returns an array of shared functions.
        const SharedFunctions&          sharedFunctions() const;

    private:

        //! Generates a string of definitions from a pipeline feature mask.
        String                          generateOptionsString(PipelineFeatures features, const PipelineFeatureLayout* featureLayout) const;

    private:
        
        //! Allocates a new shader and returns it's identifier.
        ResourceId                      allocateShader(const String& name, const String& code, ShaderType type);
        
    private:
        
        //! Replaces an include directives with a shader code.
        class IncludePreprocessor;
        
        //! Searches for a shared function ussage and imports their code automatically.
        class SharedFunctionPreprocessor;

        const RenderingContext&         m_renderingContext;                 //!< Parent rendering context instance.
        ResourceIdentifiers             m_identifiers[TotalShaderTypes];    //!< An array of shader resource identifiers.
        FixedArray<String>              m_shaders[TotalShaderTypes];        //!< An array of shader containers.
        SharedFunctions                 m_sharedFunctions;                  //!< An container of shared functions.
        Map<String, String>             m_sharedSources;                    //!< A container to map from a shader name to a shared source code.
        List<ShaderPreprocessorUPtr>    m_preprocessors;                    //!< A list of shader preprocessors that are consequently invoked.
    };

    //! A base class for all shader preprocessors that rgenerate uniform buffer definitions.
    class UniformBufferPreprocessor : public ShaderPreprocessor
    {
    public:

        //! Extracts all uniform buffer declarations from a shader source code.
        virtual bool                    preprocess(const RenderingContext& renderingContext, String& source) const NIMBLE_OVERRIDE;

    protected:

        //! Generates a uniform buffer definition from it's type and name.
        virtual String                  generateBufferDefinition(const RenderingContext& renderingContext, const String& type, const String& name, s32 slot) const NIMBLE_ABSTRACT;
    };
    
    //! Adds a single preprocessor definition to a shader source code.
    class AddDefinitionPreprocessor : public ShaderPreprocessor
    {
    public:
        
                                        //! Constructs an instance of AddDefinitionPreprocessor.
                                        AddDefinitionPreprocessor(const String& name, const String& value = "");
        
    protected:
        
        //! Appends a preprocessor definition to a shader source code.
        virtual bool                    preprocess(const RenderingContext& renderingContext, String& source) const NIMBLE_OVERRIDE;
        
    private:
        
        String                          m_name;     //!< A definition name to be added.
        String                          m_value;    //!< A definition value (can be an empty string).
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_ShaderLibrary_H__    */
