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

#ifndef __DC_Renderer_Ast_AstVisitor_H__
#define __DC_Renderer_Ast_AstVisitor_H__

#include "Ast.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{

    //! A visitor pattern to process generated AST.
    class Visitor
    {
    public:

        virtual                 ~Visitor() {}

        //! Visits a program node.
        virtual void            visit(Program& node) {}

        //! Visits a function declaration node.
        virtual void            visit(Function& node) {}

        //! Visits a variable declaration node.
        virtual void            visit(Variable& node) {}

        //! Visits a structure declaration node.
        virtual void            visit(Structure& node) {}

        //! Visits a identifier node.
        virtual void            visit(Identifier& node) {}

        //! Visits a type node.
        virtual void            visit(Type& node) {}

        //! Visits a operator node.
        virtual void            visit(Operator& node) {}

        //! Visits a constant term node.
        virtual void            visit(ConstantTerm& node) {}

        //! Visits a variable term node.
        virtual void            visit(VariableTerm& node) {}

        //! Visits an object initializer node.
        virtual void            visit(ObjectInitializer& node) {}

        //! Visits a function call node.
        virtual void            visit(FunctionCall& node) {}

        //! Visits a statement block node.
        virtual void            visit(StatementBlock& node) {}

        //! Visits a 'if' statement node.
        virtual void            visit(If& node) {}

        //! Visits a 'while' statement node.
        virtual void            visit(While& node) {}

        //! Visits a 'for' statement node.
        virtual void            visit(For& node) {}

        //! Visits a 'return' statement node.
        virtual void            visit(Return& node) {}

        //! Visits a 'discard' statement node.
        virtual void            visit(Discard& node) {}
    };

    //! A Cg visitor that traverses a program tree and visits each declaration and each function body with all it's statements.
    class ProgramVisitor : public Visitor
    {
    public:

                                //!< Constructs ProgramVisitor instance.
                                ProgramVisitor();

    protected:

        //! Sets a special shader function visitor.
        void                    setShaderFunction(ShaderType shader, UPtr<Visitor> value);

        //! Sets an expression visitor.
        void                    setExpression(UPtr<Visitor> value);

        //! Returns current function.
        const Function*         currentFunction() const;

        //! Returns current structure.
        const Structure*        currentStructure() const;

        //! Enters a function node.
        void                    enterFunction(const Function& node);

        //! Leaves a function node.
        void                    leaveFunction();

        //! Visits all declarations nested inside a program.
        virtual void            visit(Program& node) NIMBLE_OVERRIDE;

        //! Visits all statments nested inside a statement block.
        virtual void            visit(StatementBlock& node) NIMBLE_OVERRIDE;

        //! Visits all lhs and rhs operands
        virtual void            visit(Operator& node) NIMBLE_OVERRIDE;

        //! Visits a function body statement block.
        virtual void            visit(Function& node) NIMBLE_OVERRIDE;

        //! Generates a return expression.
        virtual void            visit(Return& node) NIMBLE_OVERRIDE;

    private:

        const Function*         m_currentFunction;                      //!< A current function being visited.
        const Structure*        m_currentStructure;                     //!< A current structure being visited.
        const Program*          m_currentProgram;                       //!< A current program being processed.
        UPtr<Visitor>           m_expression;                           //!< An expression visitor to be used.
        UPtr<Visitor>           m_shaderFunction[TotalShaderTypes + 1]; //!< A shader entry-point visitor.
    };

    //! A Cg visitor that walks through an expression tree.
    class ExpressionVisitor : public Visitor
    {
    public:

                                //! Constructs an ExpressionVisitor instance.
                                ExpressionVisitor();

        //! Returns an active operator being processed.
        Operator*               op() const;

        //! Visits an operator node and dispatches visit to leaf nodes.
        virtual void            visit(Operator& node) NIMBLE_OVERRIDE;

    private:

        Operator*               m_operator; //!< An active operator.
    };

    //! A Cg program visitor that generates target shader code from an AST.
    class CodeGenerator : public ProgramVisitor
    {
    protected:

        //! A function type that mapps from a semantic type to a string.
        typedef const s8* (*SemanticMapping)(SemanticType semantic);

        //! A function type that mapps from a built-in type to a type name.
        typedef const s8* (*TypeMapping)(BuiltInType type);

                                //!< Constructs a CodeGenerator instance.
                                CodeGenerator(FixedStringBuffer& output);

        //! Visits an operator node and generates an expression from it.
        virtual void            visit(Operator& node) NIMBLE_OVERRIDE;

        //! Generates a return expression.
        virtual void            visit(Return& node) NIMBLE_OVERRIDE;

        //! Starts a curly braces block by writing '{\n' to an output and increasing the indentation level.
        void                    startCurlyBraces(bool newLine = false);

        //! Ends a curly braces block by writing '}[;]\n\n' to an output and decreasing the indentation level.
        void                    endCurlyBraces(bool semicolon = false);

        //! Sets an input semantic mapping.
        void                    setInputMapping(SemanticMapping value);

        //! Sets an output semantic mapping.
        void                    setOutputMapping(SemanticMapping value);

        //! Sets a built-in type mapping.
        void                    setTypeMapping(TypeMapping value);

        //! Returns current indentation string.
        const s8*               ident() const;

        //! Returns reference to an output string buffer.
        FixedStringBuffer&      output();

        //! Returns an input semantic name.
        const s8*               inputSemantic(SemanticType semantic) const;

        //! Returns an output semantic name.
        const s8*               outputSemantic(SemanticType semantic) const;

        //! Returns a built-in type name.
        const s8*               builtInType(BuiltInType type) const;

        //! Returns an end line string.
        const s8*               endl() const;

    private:

        //! A maximum number of indentation levels.
        enum { MaxIndent = 8 };

        static const s8*        s_indentations[MaxIndent];  //!< Indentation levels.
        FixedStringBuffer&      m_output;                   //!< An output string buffer.
        s32                     m_indent;                   //!< Current indentation level.
        SemanticMapping         m_inputSemantic;            //!< Maps an input semantic to a string value.
        SemanticMapping         m_outputSemantic;           //!< Maps an output semantic to a string value.
        TypeMapping             m_type;                     //!< Maps a built-in type to a string value.
    };

    //! A Cg visitor that generates an expression string from an input AST.
    class ExpressionGenerator : public CodeGenerator
    {
    public:

                                //! Constructs the ExpressionGenerator instance.
                                ExpressionGenerator(TypeMapping type, FixedStringBuffer& output);

    protected:

        //! Generates code for an operator node.
        virtual void            visit(Operator& node) NIMBLE_OVERRIDE;

        //! Generates code for a constant term leaf.
        virtual void            visit(ConstantTerm& node) NIMBLE_OVERRIDE;

        //! Generates code for a variable term leaf.
        virtual void            visit(VariableTerm& node) NIMBLE_OVERRIDE;

        //! Generates code for a function call term leaf.
        virtual void            visit(FunctionCall& node) NIMBLE_OVERRIDE;

    private:

        static const s8*        s_operators[Cg::TotalOperatorTypes];    //!< An array to map from built-in operator to operator string.
    };
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_Ast_AstVisitor_H__    */
