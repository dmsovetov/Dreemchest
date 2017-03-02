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
#include "Declarations.h"
#include "AstVisitor.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{

// ---------------------------------------------------------------- Ast ---------------------------------------------------------------- //
    
// ** Ast::Ast
Ast::Ast(s32 line, u16 column)
    : m_line(line)
    , m_column(column)
{
    
}
    
// ** Ast::~Ast
Ast::~Ast()
{
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

// ---------------------------------------------------------------- Scope --------------------------------------------------------------- //

// ** Scope::Scope
Scope::Scope(const Scope* parent)
    : m_parent(parent)
{

}

// ** Scope::find
const Declaration* Scope::find(const StringView& name) const
{
    Declarations::const_iterator i = m_declarations.find(String64(name, name.length()));

    if (i != m_declarations.end())
    {
        return i->second;
    }

    return NULL;
}

// ** Scope::add
void Scope::add(const Declaration* declaration)
{
    const StringView& name = declaration->name();
    NIMBLE_ABORT_IF(find(name), "already declared");
    m_declarations[String64(name, name.length())] = declaration;
}

// ** Scope::findInScopeChain
const Declaration* Scope::findInScopeChain(const StringView& name) const
{
    if (const Declaration* declaration = find(name))
    {
        return declaration;
    }

    if (m_parent)
    {
        return m_parent->findInScopeChain(name);
    }

    return NULL;
}
    
// --------------------------------------------------------------- Program -------------------------------------------------------------- //
    
// ** Program::Program
Program::Program()
    : Ast(0, 0)
{
    memset(m_shaders, NULL, sizeof(m_shaders));
}
    
// ** Program::declarations
const Program::Declarations& Program::declarations() const
{
    return m_declarations;
}
    
// ** Program::declarations
Program::Declarations& Program::declarations()
{
    return m_declarations;
}

// ** Program::declarations
const Scope& Program::scope() const
{
    return m_scope;
}

// ** Program::declarations
Scope& Program::scope()
{
    return m_scope;
}
    
// ** Program::addDeclaration
void Program::addDeclaration(Declaration* declaration)
{
    m_declarations.push_back(declaration);
}

// ** Program::functionForShader
const Identifier* Program::functionForShader(ShaderType shader) const
{
    return m_shaders[shader];
}

// ** Program::setShaderFunction
void Program::setShaderFunction(ShaderType shader, const Identifier* name)
{
    m_shaders[shader] = name;
}

// ** Program::accept
void Program::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
// --------------------------------------------------------------- Identifier -------------------------------------------------------------- //
    
// ** Identifier::Identifier
Identifier::Identifier(const StringView& value, s32 line, u16 column)
    : Ast(line, column)
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
    : Ast(line, column)
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
