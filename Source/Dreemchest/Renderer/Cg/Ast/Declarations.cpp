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

#include "Declarations.h"
#include "AstVisitor.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

namespace Cg
{

// ------------------------------------------------------------- Declaration ------------------------------------------------------------ //

// ** Declaration::Declaration
Declaration::Declaration(NodeType type, s32 line, u16 column)
    : Statement(type, line, column)
{
    
}
    
// -------------------------------------------------------------- Variable -------------------------------------------------------------- //
    
// ** Variable::Variable
Variable::Variable(const Identifier* identifier, const Type* type, Expression* initializer, SemanticType semantic)
    : Declaration(VariableNode, type->line(), type->column())
    , m_identifier(identifier)
    , m_type(type)
    , m_initializer(initializer)
    , m_semantic(semantic)
{
    
}
    
// ** Variable::name
const StringView& Variable::name() const
{
    return m_identifier->value();
}

// ** Variable::accept
void Variable::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
// -------------------------------------------------------------- Structure ------------------------------------------------------------- //

// ** Structure::Structure
Structure::Structure(const Identifier* identifier)
    : Declaration(StructureNode, identifier->line(), identifier->column())
{
    
}

// ** Structure::addField
void Structure::addField(Variable* field)
{
    m_fields.push_back(field);
}

// ** Structure::accept
void Structure::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
// -------------------------------------------------------------- Function -------------------------------------------------------------- //
    
// ** Function::Function
Function::Function(const Identifier* identifier, const Type* type)
    : Declaration(FunctionNode, type->line(), type->column())
    , m_identifier(identifier)
    , m_type(type)
    , m_semantic(INVALID_SEMANTIC)
    , m_body(NULL)
{
    
}

// ** Function::addArgument
void Function::addArgument(Variable* argument)
{
    m_arguments.push_back(argument);
}

// ** Function::setBody
void Function::setBody(Statement* value)
{
    m_body = value;
}
    
// ** Function::setSemantic
void Function::setSemantic(SemanticType value)
{
    m_semantic = value;
}

// ** Function::accept
void Function::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST
