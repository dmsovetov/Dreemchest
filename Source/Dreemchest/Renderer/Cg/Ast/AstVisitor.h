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

        virtual             ~Visitor() {}

        //! Visits a program node.
        virtual void        visit(Program& node) {}

        //! Visits a function declaration node.
        virtual void        visit(Function& node) {}

        //! Visits a variable declaration node.
        virtual void        visit(Variable& node) {}

        //! Visits a structure declaration node.
        virtual void        visit(Structure& node) {}

        //! Visits a identifier node.
        virtual void        visit(Identifier& node) {}

        //! Visits a type node.
        virtual void        visit(Type& node) {}

        //! Visits a operator node.
        virtual void        visit(Operator& node) {}

        //! Visits a constant term node.
        virtual void        visit(ConstantTerm& node) {}

        //! Visits a variable term node.
        virtual void        visit(VariableTerm& node) {}

        //! Visits an object initializer node.
        virtual void        visit(ObjectInitializer& node) {}

        //! Visits a function call node.
        virtual void        visit(FunctionCall& node) {}

        //! Visits a statement block node.
        virtual void        visit(StatementBlock& node) {}

        //! Visits a 'if' statement node.
        virtual void        visit(If& node) {}

        //! Visits a 'while' statement node.
        virtual void        visit(While& node) {}

        //! Visits a 'for' statement node.
        virtual void        visit(For& node) {}

        //! Visits a 'return' statement node.
        virtual void        visit(Return& node) {}

        //! Visits a 'discard' statement node.
        virtual void        visit(Discard& node) {}
    };

    //! A Cg visitor that traverses a program tree.
    class TreeVisitor : public Visitor
    {
    public:

        //! Visits all declarations nested inside a program.
        virtual void        visit(Program& node) NIMBLE_OVERRIDE;

        //! Visits all statments nested inside a statement block.
        virtual void        visit(StatementBlock& node) NIMBLE_OVERRIDE;

        //! Visits all lhs and rhs operands
        virtual void        visit(Operator& node) NIMBLE_OVERRIDE;

        //! Visits a function body statement block.
        virtual void        visit(Cg::Function& node) NIMBLE_OVERRIDE;
    };
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_Ast_AstVisitor_H__    */
