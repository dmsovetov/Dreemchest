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
#include "AstVisitor.h"
#include "Declarations.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

namespace Cg
{
    
// ------------------------------------------------------------- Expression ------------------------------------------------------------- //
 
// ** Expression::Expression
Expression::Expression(s32 line, u16 column)
    : Statement(line, column)
    , m_type(NULL)
    , m_isConstant(true)
{
    
}

// ** Expression::type
const Type* Expression::type() const
{
    return m_type;
}

// ** Expression::setType
void Expression::setType(const Type* value)
{
    m_type = value;
}

// ** Expression::isConstant
bool Expression::isConstant() const
{
    return m_isConstant;
}

// ** Expression::setConstant
void Expression::setConstant(bool value)
{
    m_isConstant = value;
}
    
// -------------------------------------------------------------- Operator -------------------------------------------------------------- //
    
// ** Operator::Operator
Operator::Operator(OperatorType type, Expression* lhs, Expression* rhs, s32 line, u16 column)
    : Expression(line, column)
    , m_type(type)
    , m_lhs(lhs)
    , m_rhs(rhs)
{
    
}

// ** Operator::type
OperatorType Operator::type() const
{
    return m_type;
}

// ** Operator::lhs
const Expression* Operator::lhs() const
{
    return m_lhs;
}

// ** Operator::lhs
Expression* Operator::lhs()
{
    return m_lhs;
}

// ** Operator::rhs
const Expression* Operator::rhs() const
{
    return m_rhs;
}

// ** Operator::rhs
Expression* Operator::rhs()
{
    return m_rhs;
}

// ** Operator::accept
void Operator::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
// ------------------------------------------------------------- ConstantTerm ----------------------------------------------------------- //
    
// ** ConstantTerm::ConstantTerm
ConstantTerm::ConstantTerm(const StringView& value, s32 line, u16 column)
    : Expression(line, column)
    , m_value(value)
{
    
}

// ** ConstantTerm::value
const StringView& ConstantTerm::value() const
{
    return m_value;
}

// ** ConstantTerm::accept
void ConstantTerm::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
// ------------------------------------------------------------- VariableTerm ----------------------------------------------------------- //
    
// ** VariableTerm::VariableTerm
VariableTerm::VariableTerm(const StringView& value, s32 line, u16 column)
    : Expression(line, column)
    , m_value(value)
    , m_variable(NULL)
{
        
}

// ** VariableTerm::flags
const FlagSet8& VariableTerm::flags() const
{
	return m_flags;
}

// ** VariableTerm::flags
FlagSet8& VariableTerm::flags()
{
	return m_flags;
}

// ** VariableTerm::variable
const Variable* VariableTerm::variable() const
{
    return m_variable;
}

// ** VariableTerm::setVariable
void VariableTerm::setVariable(const Variable* value)
{
    m_variable = value;

    if (value)
    {
		// Combine term flags with variable ones.
		m_flags = m_flags | value->flags();

		// Inherit type from a variable instance.
        setType(&value->type());
    }
}

// ** VariableTerm::name
const StringView& VariableTerm::name() const
{
    return m_value;
}

// ** VariableTerm::accept
void VariableTerm::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ** VariableTerm::isVariable
const VariableTerm* VariableTerm::isVariable() const
{
    return this;
}

// ** VariableTerm::isVariable
VariableTerm* VariableTerm::isVariable()
{
    return this;
}
    
// ------------------------------------------------------------- FunctionCall ----------------------------------------------------------- //
    
// ** FunctionCall::FunctionCall
FunctionCall::FunctionCall(const Identifier& identifier, s32 line, u16 column)
    : Expression(line, column)
    , m_identifier(identifier)
    , m_function(NULL)
{
    
}

// ** FunctionCall::function
const Function* FunctionCall::function() const
{
    return m_function;
}

// ** FunctionCall::setFunction
void FunctionCall::setFunction(const Function* value)
{
    m_function = value;

    if (value)
    {
        setType(&value->type());
    }
}

// ** FunctionCall::name
const StringView& FunctionCall::name() const
{
    return m_identifier.value();
}

// ** FunctionCall::arguments
const FunctionCall::Arguments& FunctionCall::arguments() const
{
    return m_arguments;
}

// ** FunctionCall::arguments
FunctionCall::Arguments& FunctionCall::arguments()
{
    return m_arguments;
}

// ** FunctionCall::addArgument
void FunctionCall::addArgument(Expression* expression)
{
    m_arguments.push_back(expression);
}

// ** FunctionCall::accept
void FunctionCall::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

// ----------------------------------------------------------- ObjectInitializer -------------------------------------------------------- //

// ** ObjectInitializer::ObjectInitializer
ObjectInitializer::ObjectInitializer(s32 line, u16 column)
    : Expression(line, column)
{
}
        
// ** ObjectInitializer::addFieldInitializer
void ObjectInitializer::addFieldInitializer(Expression* expression)
{
    m_initializers.push_back(expression);
}

// ** ObjectInitializer::accept
void ObjectInitializer::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST
