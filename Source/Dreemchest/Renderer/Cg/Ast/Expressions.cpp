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

#include "Expressions.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

namespace Cg
{
    
// ------------------------------------------------------------- Expression ------------------------------------------------------------- //
 
// ** Expression::Expression
Expression::Expression(NodeType type, s32 line, u16 column)
    : Statement(type, line, column)
{
    
}
    
// -------------------------------------------------------------- Operator -------------------------------------------------------------- //
    
// ** Operator::Operator
Operator::Operator(OperatorType type, Expression* lhs, Expression* rhs, s32 line, u16 column)
    : Expression(OperatorNode, line, column)
    , m_type(type)
    , m_lhs(lhs)
    , m_rhs(rhs)
{
    
}
    
// ------------------------------------------------------------- ConstantTerm ----------------------------------------------------------- //
    
// ** ConstantTerm::ConstantTerm
ConstantTerm::ConstantTerm(const StringView& value, s32 line, u16 column)
    : Expression(ConstantTermNode, line, column)
    , m_value(value)
{
    
}
    
// ------------------------------------------------------------- VariableTerm ----------------------------------------------------------- //
    
// ** VariableTerm::VariableTerm
VariableTerm::VariableTerm(const StringView& value, s32 line, u16 column)
    : Expression(VariableTermNode, line, column)
    , m_value(value)
{
        
}
    
// ------------------------------------------------------------- FunctionCall ----------------------------------------------------------- //
    
// ** FunctionCall::FunctionCall
FunctionCall::FunctionCall(const Identifier* identifier, s32 line, u16 column)
    : Expression(FunctionCallNode, line, column)
    , m_identifier(identifier)
{
    
}

// ** FunctionCall::addArgument
void FunctionCall::addArgument(Expression* expression)
{
    m_arguments.push_back(expression);
}
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST
