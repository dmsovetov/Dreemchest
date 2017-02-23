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

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{
    
// ---------------------------------------------------------------- Statement ---------------------------------------------------------------- //

// ** Statement::Statement
Statement::Statement(NodeType type, s32 line, u16 column)
    : Ast(type, line, column)
{
    
}
    
// -------------------------------------------------------------- StatementBlock ------------------------------------------------------------- //
    
// ** StatementBlock::StatementBlock
StatementBlock::StatementBlock(s32 line, u16 column)
    : Statement(StatementsNode, line, column)
{
    
}

// ** StatementBlock::addStatement
void StatementBlock::addStatement(Statement* statment)
{
    m_children.push_back(statment);
}
    
// -------------------------------------------------------------------- If ------------------------------------------------------------------- //
    
// ** If::If
If::If(Expression* condition, Statement* then, Statement* otherwise, s32 line, u16 column)
    : Statement(IfNode, line, column)
    , m_condition(condition)
    , m_then(then)
    , m_otherwise(otherwise)
{
    
}

// ------------------------------------------------------------------- While ----------------------------------------------------------------- //

// ** While::While
While::While(Expression* condition, Statement* body, s32 line, u16 column)
    : Statement(WhileNode, line, column)
    , m_condition(condition)
    , m_body(body)
{
    
}

// ------------------------------------------------------------------- For ------------------------------------------------------------------- //

// ** For::For
For::For(Expression* initial, Expression* condition, Expression* increment, Statement* body, s32 line, u16 column)
    : Statement(ForNode, line, column)
    , m_initial(initial)
    , m_condition(condition)
    , m_increment(increment)
    , m_body(body)
{
    
}
    
// ------------------------------------------------------------------ Return ---------------------------------------------------------------- //

// ** Return::Return
Return::Return(Expression* value, s32 line, u16 column)
    : Statement(ReturnNode, line, column)
    , m_value(value)
{
    
}

// ------------------------------------------------------------------ Discard --------------------------------------------------------------- //

// ** Discard::Discard
Discard::Discard(s32 line, u16 column)
    : Statement(DiscardNode, line, column)
{
    
}
    
} // namespace Cg

} // namespace Renderer

DC_END_DREEMCHEST
