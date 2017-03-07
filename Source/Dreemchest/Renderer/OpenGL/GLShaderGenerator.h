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

#ifndef __DC_Renderer_GLShaderGenerator_H__
#define __DC_Renderer_GLShaderGenerator_H__

#include "../Cg/Ast/AstVisitor.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! Maps from an input semantic to an attribute name.
    class GLSemanticMapping
    {
    public:

        //! Returns an attribute name for a specified semantic binding.
        static const s8*            attribute(Cg::SemanticType semantic);

        //! Returns an output GL variable for a given semantic binding.
        static const s8*            builtInVariable(Cg::SemanticType semantic);

        //! Returns a varying name for a given semantic binding.
        static const s8*            varying(Cg::SemanticType semantic);
    };

    //! Maps from a built-in type to a name.
    class GLTypeMapping
    {
    public:

        //! Returns a GLSL built-in type.
        static const s8*              builtIn(Cg::BuiltInType type);
    };

    //! The Cg visitor to generate GLSL expressions from AST.
    class GLExpressionGenerator : public Cg::ExpressionGenerator
    {
    public:

                                    //! Constructs a GLExpressionGenerator instance.
                                    GLExpressionGenerator(SemanticMapping inputSemantic, SemanticMapping outputSemantic, FixedStringBuffer& output);

    protected:

        //! Generates a variable term from a node.
        virtual void                visit(Cg::VariableTerm& node) NIMBLE_OVERRIDE;

        //! GLSL expression generator requires a specific handling of a member operator.
        virtual void                visit(Cg::Operator& node) NIMBLE_OVERRIDE;

        //! Substitutes a variable/attribute instead of a structure field or function argument.
        bool                        substituteBoundVariable(const Cg::VariableTerm& term);
    };

    //! The Cg AST visitor that generates a GLSL shader code.
    class GLShaderGenerator : public Cg::CodeGenerator
    {
    public:

                                    //! Constructs a GLShaderGenerator instance.
                                    GLShaderGenerator(FixedStringBuffer& output);

    private:

        //! Generates a variable declaration from an AST node.
        virtual void                visit(Cg::Variable& node) NIMBLE_OVERRIDE;

        //! An override for structure generator to skip structures with semantic bindings.
        virtual void                visit(Cg::Structure& node) NIMBLE_OVERRIDE;

        //! Writes a type name to an output stream.
        FixedStringBuffer&          writeType(const Cg::Type& type);

        //! Writes a variable to an output.
        void                        writeVariable(const Cg::Variable& variable);
    };

    //! A Cg visitor that generates a generic GLSL function.
    class GLFunctionGenerator : public Cg::CodeGenerator
    {
    public:

                                    //! Constructs a GLFunctionGenerator instance.
                                    GLFunctionGenerator(SemanticMapping inputSemantic, SemanticMapping outputSemantic, FixedStringBuffer& output);

    protected:

        //! Generates a shader function declaration from an AST node.
        virtual void                visit(Cg::Function& node) NIMBLE_OVERRIDE;

        //! Generates a variable declaration.
        virtual void                visit(Cg::Variable& node) NIMBLE_OVERRIDE;

        //! Writes a type name to an output stream.
        FixedStringBuffer&          writeType(const Cg::Type& type);

        //! Writes a variable to an output.
        void                        writeVariable(const Cg::Variable& variable);
    };

    //! A Cg visitor that generates GLSL vertex shader function.
    class GLVertexFunctionGenerator : public GLFunctionGenerator
    {
    public:

                                    //! Constructs a GLVertexFunctionGenerator instance.
                                    GLVertexFunctionGenerator(FixedStringBuffer& output);

    protected:

        //! Generates a shader function declaration from an AST node.
        virtual void                visit(Cg::Function& node) NIMBLE_OVERRIDE;

        //! Generates a vertex input from a variable.
        virtual void                visit(Cg::Variable& node) NIMBLE_OVERRIDE;

        //! Generates a return statement for a vertex function.
        virtual void                visit(Cg::Return& node) NIMBLE_OVERRIDE;
    };

    //! A Cg visitor that generates GLSL fragment shader function.
    class GLFragmentFunctionGenerator : public GLFunctionGenerator
    {
    public:

                                    //! Constructs a GLFragmentFunctionGenerator instance.
                                    GLFragmentFunctionGenerator(FixedStringBuffer& output);

    protected:

        //! Generates a shader function declaration from an AST node.
        virtual void                visit(Cg::Function& node) NIMBLE_OVERRIDE;

        //! Generates a fragment input from a variable.
        virtual void                visit(Cg::Variable& node) NIMBLE_OVERRIDE;

        //! Generates a return statement for a fragment function.
        virtual void                visit(Cg::Return& node) NIMBLE_OVERRIDE;
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_GLShaderGenerator_H__    */
