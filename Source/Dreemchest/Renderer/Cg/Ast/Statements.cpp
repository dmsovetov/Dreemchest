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

#include "Statements.h"
#include "AstVisitor.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{
    
// ---------------------------------------------------------------- Statement ---------------------------------------------------------------- //

// ** Statement::Statement
Statement::Statement(s32 line, u16 column)
    : Ast(line, column)
{
    
}
    
// -------------------------------------------------------------- StatementBlock ------------------------------------------------------------- //
    
// ** StatementBlock::StatementBlock
StatementBlock::StatementBlock(const Scope* scope, s32 line, u16 column)
    : Statement(line, column)
    , m_declarations(scope)
{
    
}

// ** StatementBlock::statments
const StatementBlock::Statements& StatementBlock::statements() const
{
    return m_children;
}

// ** StatementBlock::statements
StatementBlock::Statements& StatementBlock::statements()
{
    return m_children;
}

// ** StatementBlock::declarations
const Scope& StatementBlock::declarations() const
{
    return m_declarations;
}

// ** StatementBlock::declarations
Scope& StatementBlock::declarations()
{
    return m_declarations;
}

// ** StatementBlock::addStatement
void StatementBlock::addStatement(Statement* statment)
{
    m_children.push_back(statment);
}

// ** StatementBlock::accept
void StatementBlock::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
// -------------------------------------------------------------------- If ------------------------------------------------------------------- //
    
// ** If::If
If::If(Expression* condition, Statement* then, Statement* otherwise, s32 line, u16 column)
    : Statement(line, column)
    , m_condition(condition)
    , m_then(then)
    , m_otherwise(otherwise)
{
    
}

// ** If::accept
void If::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ------------------------------------------------------------------- While ----------------------------------------------------------------- //

// ** While::While
While::While(Expression* condition, Statement* body, s32 line, u16 column)
    : Statement(line, column)
    , m_condition(condition)
    , m_body(body)
{
    
}

// ** While::accept
void While::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ------------------------------------------------------------------- For ------------------------------------------------------------------- //

// ** For::For
For::For(Expression* initial, Expression* condition, Expression* increment, Statement* body, s32 line, u16 column)
    : Statement(line, column)
    , m_initial(initial)
    , m_condition(condition)
    , m_increment(increment)
    , m_body(body)
{
    
}

// ** For::accept
void For::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
// ------------------------------------------------------------------ Return ---------------------------------------------------------------- //

// ** Return::Return
Return::Return(Expression* value, s32 line, u16 column)
    : Statement(line, column)
    , m_value(value)
{
    
}

// ** Return::value
Expression* Return::value()
{
    return m_value;
}

// ** Return::accept
void Return::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ------------------------------------------------------------------ Discard --------------------------------------------------------------- //

// ** Discard::Discard
Discard::Discard(s32 line, u16 column)
    : Statement(line, column)
{
    
}

// ** Discard::accept
void Discard::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
} // namespace Cg

} // namespace Renderer

DC_END_DREEMCHEST
