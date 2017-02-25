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

#include "AstVisitor.h"
#include "Ast.h"
#include "Declarations.h"
#include "Expressions.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{

// ** TreeVisitor::visit
void TreeVisitor::visit(Program& program)
{
    Program::Declarations& declarations = program.declarations();

    for (Program::Declarations::iterator i = declarations.begin(), end = declarations.end(); i != end; ++i)
    {
        (*i)->accept(*this);
    }
}

// ** TreeVisitor::visit
void TreeVisitor::visit(StatementBlock& node)
{
    StatementBlock::Statements& statements = node.statements();

    for (StatementBlock::Statements::iterator i = statements.begin(), end = statements.end(); i != end; ++i)
    {
        (*i)->accept(*this);
    }
}

// ** TreeVisitor::visit
void TreeVisitor::visit(Operator& node)
{
    if (Expression* lhs = node.lhs())
    {
        lhs->accept(*this);
    }

    if (Expression* rhs = node.rhs())
    {
        rhs->accept(*this);
    }
}
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST