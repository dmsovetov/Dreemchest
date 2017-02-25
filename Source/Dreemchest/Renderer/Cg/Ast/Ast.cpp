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

#include "Ast.h"
#include "AstVisitor.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{

// ---------------------------------------------------------------- Ast ---------------------------------------------------------------- //
    
// ** Ast::Ast
Ast::Ast(NodeType type, s32 line, u16 column)
    : m_type(type)
    , m_line(line)
    , m_column(column)
{
    
}
    
// ** Ast::~Ast
Ast::~Ast()
{
}

// ** Ast::type
Ast::NodeType Ast::type() const
{
    return m_type;
}

// ** Ast::type
s32 Ast::line() const
{
    return m_line;
}

// ** Ast::type
u16 Ast::column() const
{
    return m_column;
}
    
// --------------------------------------------------------------- Program -------------------------------------------------------------- //
    
// ** Program::Program
Program::Program()
    : Ast(Ast::ProgramNode, 0, 0)
{
    
}
    
// ** Program::Program
s32 Program::declarationCount() const
{
    return static_cast<s32>(m_declarations.size());
}
    
// ** Program::Program
const Declaration* Program::declaration(s32 index) const
{
    NIMBLE_ABORT_IF(index < 0 || index >= declarationCount(), "index is out of range");
    return m_declarations[index];
}
    
// ** Program::addDeclaration
void Program::addDeclaration(Declaration* declaration)
{
    m_declarations.push_back(declaration);
}

// ** Program::accept
void Program::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
// --------------------------------------------------------------- Identifier -------------------------------------------------------------- //
    
// ** Identifier::Identifier
Identifier::Identifier(const StringView& value, s32 line, u16 column)
    : Ast(Ast::IdentifierNode, line, column)
    , m_value(value)
{
    
}

// **  Identifier::value
const StringView& Identifier::value() const
{
    return m_value;
}

// ** Identifier::accept
void Identifier::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
// ------------------------------------------------------------------ Type ---------------------------------------------------------------- //

// ** Type::Type
Type::Type(const Identifier* name, BuiltInType builtInType, s32 line, u16 column)
    : Ast(TypeNode, line, column)
    , m_name(name)
    , m_builtInType(builtInType)
{
    NIMBLE_ABORT_IF(m_name == NULL, "invalid type name");
}

// ** Type::name
const StringView& Type::name() const
{
    return m_name->value();
}

// ** Type::builtInType
BuiltInType Type::builtInType() const
{
    return m_builtInType;
}

// ** Type::accept
void Type::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
} // namespace Cg

} // namespace Renderer

DC_END_DREEMCHEST
