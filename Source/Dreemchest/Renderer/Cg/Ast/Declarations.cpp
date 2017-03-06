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
Declaration::Declaration(DeclarationType declarationType, const Identifier& identifier, s32 line, u16 column)
    : Statement(line, column)
    , m_identifier(identifier)
    , m_declarationType(declarationType)
{
    
}

// ** Declaration::name
const StringView& Declaration::name() const
{
    return m_identifier.value();
}

// ** Declaration::declarationType
Declaration::DeclarationType Declaration::declarationType() const
{
    return m_declarationType;
}
    
// -------------------------------------------------------------- Variable -------------------------------------------------------------- //
    
// ** Variable::Variable
Variable::Variable(const Identifier& identifier, const Type& type, Expression* initializer, SemanticType semantic, u8 flags)
    : Declaration(VariableDeclaration, identifier, type.line(), type.column())
    , m_type(type)
    , m_initializer(initializer)
    , m_semantic(semantic)
    , m_flags(flags)
{
    
}

// ** Variable::type
const Type& Variable::type() const
{
    return m_type;
}

// ** Variable::flags
const FlagSet8& Variable::flags() const
{
    return m_flags;
}

// **  Variable::flags
FlagSet8& Variable::flags()
{
    return m_flags;
}

// ** Variable::initializer
const Expression* Variable::initializer() const
{
    return m_initializer;
}

// ** Variable::initializer
Expression* Variable::initializer()
{
    return m_initializer;
}   

// ** Variable::semantic
SemanticType Variable::semantic() const
{
    return m_semantic;
}

// ** Variable::accept
void Variable::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
// -------------------------------------------------------------- Structure ------------------------------------------------------------- //

// ** Structure::Structure
Structure::Structure(const Scope* scope, const Identifier& identifier)
    : Declaration(StructureDeclaration, identifier, identifier.line(), identifier.column())
    , m_declarations(scope)
{
    
}

// ** Structure::fields
const Structure::Fields& Structure::fields() const
{
    return m_fields;
}

// ** Structure::fields
Structure::Fields& Structure::fields()
{
    return m_fields;
}

// ** Structure::declarations
const Scope& Structure::declarations() const
{
    return m_declarations;
}

// ** Structure::declarations
Scope& Structure::declarations()
{
    return m_declarations;
}

// ** Structure::addField
void Structure::addField(Variable* field)
{
    m_fields.push_back(field);
}

// ** Structure::hasSemantics
bool Structure::hasSemantics() const
{
    for (Fields::const_iterator i = m_fields.begin(), end = m_fields.end(); i != end; ++i)
    {
        if ((*i)->semantic())
        {
            return true;
        }
    }
    return false;
}

// ** Structure::accept
void Structure::accept(Visitor& visitor)
{
    visitor.visit(*this);
}
    
// -------------------------------------------------------------- Function -------------------------------------------------------------- //
    
// ** Function::Function
Function::Function(const Scope* scope, const Identifier& identifier, const Type* type, BuiltInType constructor)
    : Declaration(FunctionDeclaration, identifier, type->line(), type->column())
    , m_type(type)
    , m_semantic(INVALID_SEMANTIC)
    , m_body(NULL)
    , m_declarations(scope)
    , m_constructor(constructor)
    , m_shader(TotalShaderTypes)
{
    
}

// ** Function::type
const Type& Function::type() const
{
    return *m_type;
}

// ** Function::arguments
const Function::Arguments& Function::arguments() const
{
    return m_arguments;
}

// ** Function::arguments
Function::Arguments& Function::arguments()
{
    return m_arguments;
}

// ** Function::semantic
SemanticType Function::semantic() const
{
    return m_semantic;
}

// ** Function::body
const Statement* Function::body() const
{
    return m_body;
}

// ** Function::body
Statement* Function::body()
{
    return m_body;
}

// ** Function::declarations
const Scope& Function::declarations() const
{
    return m_declarations;
}

// ** Function::declarations
Scope& Function::declarations()
{
    return m_declarations;
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

// ** Function::constructor
BuiltInType Function::constructor() const
{
    return m_constructor;
}

// ** Function::setShader
void Function::setShader(ShaderType value)
{
    m_shader = value;
}

// ** Function::setShader
ShaderType Function::shader() const
{
    return m_shader;
}

// ** Function::isShader
bool Function::isShader() const
{
    return shader() != TotalShaderTypes;
}

// ** Function::accept
void Function::accept(Visitor& visitor)
{
    visitor.visit(*this);
}

} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST
