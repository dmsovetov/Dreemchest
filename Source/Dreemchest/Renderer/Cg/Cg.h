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

#ifndef __DC_Renderer_Cg_H__
#define __DC_Renderer_Cg_H__

#include "../Renderer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{

    //! Available Cg operators ordered by precedence, operators with lower precedence go first.
    enum OperatorType
    {
          OpPlusEqual
        , OpMinusEqual
        , OpDevideEqual
        , OpMultiplyEqual
        , OpEqual
        , OpPlus
        , OpMinus
        , OpDivide
        , OpMultiply
        , OpCompare
        , OpLess
        , OpLessEqual
        , OpGreater
        , OpGreaterEqual
        , OpMember
        , TotalOperatorTypes
    };
    
    //! Available built-in types.
    enum BuiltInType
    {
          TypeUserDefined
        , TypeVoid
        , TypeBool
        , TypeInt
        , TypeFloat
        , TypeFloat2
        , TypeFloat3
        , TypeFloat4
        , TypeHalf
        , TypeHalf2
        , TypeHalf3
        , TypeHalf4
        , TypeFixed
        , TypeFixed2
        , TypeFixed3
        , TypeFixed4
        , TypeFloat4x4
        , TypeHalf4x4
        , TypeFixed4x4
        , TotalBuiltInTypes
    };

    //! Supported shader types.
    enum ShaderType
    {
          VertexShader
        , FragmentShader
        , GeometryShader
        , HullShader
        , DomainShader
        , TotalShaderTypes
    };
    
    //! Built-in input semantic types.
    enum SemanticType
    {
          INVALID_SEMANTIC
        
        // Input semantic types
        , POSITION
        , COLOR
        , NORMAL
        , TEXCOORD0
        , TEXCOORD1
        , TEXCOORD2
        , TEXCOORD3
        , TEXCOORD4
        , TEXCOORD5
        , TEXCOORD6
        , TEXCOORD7
        
        // Output semantic types
        , COLOR0
        , COLOR1
        , COLOR2
        , COLOR3
        , DEPTH
        
        // Texture unit semantic types
        , TEXUNIT0
        , TEXUNIT1
        , TEXUNIT2
        , TEXUNIT3
        , TEXUNIT4
        , TEXUNIT5
        , TEXUNIT6
        , TEXUNIT7
        , TEXUNIT8
        , TEXUNIT9
        , TEXUNIT10
        , TEXUNIT11
        , TEXUNIT12
        , TEXUNIT13
        , TEXUNIT14
        , TEXUNIT15
        
        // Constant buffer input semantic types.
        , CBUFFER0
        , CBUFFER1
        , CBUFFER2
        , CBUFFER3
        , CBUFFER4
        , CBUFFER5
        , CBUFFER6
        , CBUFFER7
    };
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_Cg_H__    */
