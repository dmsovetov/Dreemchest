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
    //! Shader library contains all shader sources exposed to a rendering context.
    class ShaderLibrary
    {
    public:
        
                                        //! Constructs a ShaderLibrary instance.
                                        ShaderLibrary( void );
        
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
        
    private:
        
        //! An internal structure to contain all info associated with a shader.
        struct Shader
        {
            String      name;       //!< A shader identifier.
            String      code;       //!< A shader source code.
        };
        
        //! Allocates a new shader and returns it's identifier.
        PersistentResourceId            allocateShader(const String& name, const String& code, ShaderType type);
        
    private:
        
        PersistentResourceIdentifiers   m_identifiers[TotalShaderTypes];  //!< An array of shader resource identifiers.
        FixedArray<Shader>              m_shaders[TotalShaderTypes];      //!< An array of shader containers.
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_ShaderLibrary_H__    */
