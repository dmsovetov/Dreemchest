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

#include "GLShaderGenerator.h"
#include "Renderer/Cg/Ast/Ast.h"
#include "Renderer/Cg/Ast/Declarations.h"
#include "Renderer/Cg/Ast/Expressions.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

using namespace Cg;

// ------------------------------------------------------ GLSemanticMapping --------------------------------------------------- //

// ** GLSemanticMapping::attribute
const s8* GLSemanticMapping::attribute(SemanticType semantic)
{
    switch (semantic)
    {
    case POSITION:
        return "a_position";
    case COLOR:
        return "a_color";
    case NORMAL:
        return "a_normal";
    case TEXCOORD0:
        return "a_texCoord0";
    case TEXCOORD1:
        return "a_texCoord1";
    case TEXCOORD2:
        return "a_texCoord2";
    case TEXCOORD3:
        return "a_texCoord3";
    case TEXCOORD4:
        return "a_texCoord4";
    case TEXCOORD5:
        return "a_texCoord5";
    case TEXCOORD6:
        return "a_texCoord6";
    case TEXCOORD7:
        return "a_texCoord7";
    }

    return NULL;
}

// ** GLSemanticMapping::builtInVariable
const s8* GLSemanticMapping::builtInVariable(SemanticType semantic)
{
    switch (semantic)
    {
    case COLOR0:
        return "gl_FragColor[0]";
    case COLOR1:
        return "gl_FragColor[1]";
    case COLOR2:
        return "gl_FragColor[2]";
    case COLOR3:
        return "gl_FragColor[3]";
    case DEPTH:
        return "gl_FragDepth";
    }

    return NULL;
}

// **  GLSemanticMapping::varying
const s8* GLSemanticMapping::varying(SemanticType semantic)
{
    switch (semantic)
    {
    case POSITION:
        return "v_position";
    case COLOR:
        return "v_color";
    case NORMAL:
        return "v_normal";
    case TEXCOORD0:
        return "v_texCoord0";
    case TEXCOORD1:
        return "v_texCoord1";
    case TEXCOORD2:
        return "v_texCoord2";
    case TEXCOORD3:
        return "v_texCoord3";
    case TEXCOORD4:
        return "v_texCoord4";
    case TEXCOORD5:
        return "v_texCoord5";
    case TEXCOORD6:
        return "v_texCoord6";
    case TEXCOORD7:
        return "v_texCoord7";
    }

    return NULL;
}

// -------------------------------------------------------- GLTypeMapping ----------------------------------------------------- //

// ** GLTypeMapping::builtIn
const s8* GLTypeMapping::builtIn(BuiltInType type)
{
    switch (type)
    {
    case TypeVoid:
        return "void";
    case TypeBool:
        return "bool";
    case TypeInt:
        return "int";
    case TypeFloat:
        return "float";
    case TypeFloat2:
        return "vec2";
    case TypeFloat3:
        return "vec3";
    case TypeFloat4:
        return "vec4";
    case TypeHalf:
        return "float";
    case TypeHalf2:
        return "vec2";
    case TypeHalf3:
        return "vec3";
    case TypeHalf4:
        return "vec4";
    case TypeFixed:
        return "float";
    case TypeFixed2:
        return "vec2";
    case TypeFixed3:
        return "vec3";
    case TypeFixed4:
        return "vec4";
    case TypeFloat4x4:
        return "mat4";
    case TypeHalf4x4:
        return "mat4";
    case TypeFixed4x4:
        return "mat4";
    }

    return NULL;
}

// ---------------------------------------------------- GLExpressionGenerator ------------------------------------------------- //

// ** GLExpressionGenerator::GLExpressionGenerator
GLExpressionGenerator::GLExpressionGenerator(SemanticMapping inputSemantic, SemanticMapping outputSemantic, FixedStringBuffer& output)
    : ExpressionGenerator(GLTypeMapping::builtIn, output)
{
    setInputMapping(inputSemantic);
    setOutputMapping(outputSemantic);
}

// ** GLExpressionGenerator::visit
void GLExpressionGenerator::visit(VariableTerm& node)
{
    if (substituteBoundVariable(node))
    {
        return;
    }

    // Process this variable term as always
    ExpressionGenerator::visit(node);
}

// ** GLExpressionGenerator::visit
void GLExpressionGenerator::visit(Operator& node)
{
    // Handle all operators except the '.' one as always.
    if (node.type() != OpMember)
    {
        ExpressionGenerator::visit(node);
        return;
    }

    // This is a member operator, so we probably have to handle an input/output variable.
    NIMBLE_ABORT_IF(node.rhs() == NULL, "a member operator should have a left hand side expression")

    if (const VariableTerm* field = node.rhs()->isVariable())
    {
        if (substituteBoundVariable(*field))
        {
            return;
        }
    }

    // This member access operator should be processed as always
    ExpressionGenerator::visit(node);
}

// ** GLExpressionGenerator::substituteBoundVariable
bool GLExpressionGenerator::substituteBoundVariable(const VariableTerm& term)
{
	const Variable* variable = term.variable();
	NIMBLE_ABORT_IF(variable == NULL, "invalid variable");

    SemanticType semantic = variable->semantic();

    if (semantic == INVALID_SEMANTIC)
    {
        return false;
    }

    if (term.flags().is(VariableInput))
    {
        output() << inputSemantic(semantic);
    }
    else
    {
        output() << outputSemantic(semantic);
    }

    return true;
}

// ----------------------------------------------------- GLFunctionGenerator -------------------------------------------------- //

// ** GLFunctionGenerator::GLFunctionGenerator
GLFunctionGenerator::GLFunctionGenerator(SemanticMapping inputSemantic, SemanticMapping outputSemantic, FixedStringBuffer& output)
    : CodeGenerator(output)
{
    setExpression(DC_NEW GLExpressionGenerator(inputSemantic, outputSemantic, output));
    setTypeMapping(GLTypeMapping::builtIn);
    setInputMapping(inputSemantic);
    setOutputMapping(outputSemantic);
}

// ** GLFunctionGenerator::visit
void GLFunctionGenerator::visit(Function& node)
{
    writeType(node.type());
    output() << " " << node.name() << "(";

    Function::Arguments& arguments = node.arguments();
    size_t argumentsLeftToProcess = arguments.size();

    for (Function::Arguments::iterator i = arguments.begin(), end = arguments.end(); i != end; ++i)
    {
        writeVariable(**i);
        argumentsLeftToProcess--;

        if (argumentsLeftToProcess)
        {
            output() << ", ";
        }
    }

    output() << ")";

    startCurlyBraces(true);
    ProgramVisitor::visit(node);
    endCurlyBraces();
}

// ** GLFunctionGenerator::visit
void GLFunctionGenerator::visit(Variable& node)
{
    // Skip variables that have a structure type with semantic bindings
    if (const Structure* structure = node.type().structure())
    {
        if (structure->hasSemantics())
        {
            return;
        }
    }

    const StringView& name = node.name();

    writeVariable(node);

    if (Expression* initializer = node.initializer())
    {
        output() << " = ";
        initializer->accept(*this);
    }

    output() << ";" << endl();
}

// ** GLFunctionGenerator::writeType
FixedStringBuffer& GLFunctionGenerator::writeType(const Type& type)
{
    if (const s8* builtInName = builtInType(type.builtInType()))
    {
        output() << builtInName;
    }
    else
    {
        output() << type.name();
    }

    return output();
}

// ** GLFunctionGenerator::writeVariable
void GLFunctionGenerator::writeVariable(const Variable& variable)
{
    output() << ident();
    output() << writeType(variable.type()) << " " << variable.name();
}

// -------------------------------------------------- GLVertexFunctionGenerator ----------------------------------------------- //

// ** GLVertexFunctionGenerator::GLVertexFunctionGenerator
GLVertexFunctionGenerator::GLVertexFunctionGenerator(FixedStringBuffer& output)
    : GLFunctionGenerator(GLSemanticMapping::attribute, GLSemanticMapping::varying, output)
{
}

// ** GLVertexFunctionGenerator::visit
void GLVertexFunctionGenerator::visit(Function& node)
{
    enterFunction(node);

    // Generate vertex output variables
    const Type& type = node.type();

    if (const Structure* structure = type.structure())
    {
        // A vertex shader function returns structure, so emit a varying for each field
        const Structure::Fields& fields = structure->fields();

        for (Structure::Fields::const_iterator i = fields.begin(), end = fields.end(); i != end; ++i)
        {
            const Variable* field = *i;

            output() << "varying ";
            output() << writeType(field->type()) << " " << GLSemanticMapping::varying(field->semantic()) << ";" << endl();
        }
    }
    else
    {
        NIMBLE_BREAK
    }
    output() << endl();

    output() << "#ifdef VERTEX" << endl();

    // Generate input vertex attributes
    Function::Arguments& args = node.arguments();

    for (Function::Arguments::iterator i = args.begin(), end = args.end(); i != end; ++i)
    {
        (*i)->accept(*this);
    }
    output() << endl();

    // Generate function body
    output() << "void main()" << endl();

    startCurlyBraces();
    if (Statement* body = node.body())
    {
        body->accept(*this);
    }
    endCurlyBraces();

    output() << "#endif // VERTEX" << endl() << endl();

    leaveFunction();
}

// ** GLVertexFunctionGenerator::visit
void GLVertexFunctionGenerator::visit(Variable& node)
{
    // Get a variable data type
    const Type& type = node.type();

    // Is it a structure or a variable?
    if (const Structure* structure = type.structure())
    {
        GLFunctionGenerator::visit(node);
    }
    else
    {
        if (node.semantic())
        {
            output() << "attribute ";
            output() << writeType(type) << " " << GLSemanticMapping::attribute(node.semantic()) << ";" << endl();
        }
        else
        {
            GLFunctionGenerator::visit(node);
        }
    }
}

// ** GLVertexFunctionGenerator::visit
void GLVertexFunctionGenerator::visit(Return& node)
{
    // Get an active function
    const Function* function = currentFunction();
    NIMBLE_ABORT_IF(function == NULL, "return statement is outside of a function");

    // Get a function return type
    const Type& type = function->type();

    // Output return statements only for primitive types
    if (!type.structure())
    {
        NIMBLE_BREAK;
    }
}

// -------------------------------------------------- GLFragmentFunctionGenerator ----------------------------------------------- //

// ** GLFragmentFunctionGenerator::GLFragmentFunctionGenerator
GLFragmentFunctionGenerator::GLFragmentFunctionGenerator(FixedStringBuffer& output)
    : GLFunctionGenerator(GLSemanticMapping::varying, GLSemanticMapping::builtInVariable, output)
{
}

// ** GLFragmentFunctionGenerator::visit
void GLFragmentFunctionGenerator::visit(Function& node)
{
    enterFunction(node);

    output() << "#ifdef FRAGMENT" << endl();

    // Generate fragment shader input.
    Function::Arguments& args = node.arguments();

    for (Function::Arguments::iterator i = args.begin(), end = args.end(); i != end; ++i)
    {
        (*i)->accept(*this);
    }

    // Generate function body
    output() << "void main()" << endl();

    startCurlyBraces();
    if (Statement* body = node.body())
    {
        body->accept(*this);
    }
    endCurlyBraces();

    output() << "#endif // FRAGMENT" << endl() << endl();

    leaveFunction();
}


// ** GLFragmentFunctionGenerator::visit
void GLFragmentFunctionGenerator::visit(Variable& node)
{
    // Get a variable data type
    const Type& type = node.type();

    // Is it a structure or a variable?
    if (const Structure* structure = type.structure())
    {
        GLFunctionGenerator::visit(node);
    }
    else
    {
        if (node.semantic())
        {
            output() << "attribute ";
            output() << writeType(type) << " " << GLSemanticMapping::attribute(node.semantic()) << ";" << endl();
        }
        else
        {
            GLFunctionGenerator::visit(node);
        }
    }
}

// ** GLFragmentFunctionGenerator::visit
void GLFragmentFunctionGenerator::visit(Return& node)
{
    // Get an active function
    const Function* function = currentFunction();
    NIMBLE_ABORT_IF(function == NULL, "return statement is outside of a function");

    // Get a function return type
    const Type& type = function->type();

    // Output return statements only for primitive types
    if (!type.structure())
    {
        // Get the returned expression
        Expression* value = node.value();
        NIMBLE_ABORT_IF(value == NULL, "shader function should return a value");

        output() << ident() << outputSemantic(function->semantic()) << " = ";
        value->accept(*this);
    }
}

// ----------------------------------------------------- GLShaderGenerator ---------------------------------------------------- //

// ** GLShaderGenerator::GLShaderGenerator
GLShaderGenerator::GLShaderGenerator(FixedStringBuffer& output)
    : CodeGenerator(output)
{
    setExpression(DC_NEW GLExpressionGenerator(NULL, NULL, output));
    setShaderFunction(Cg::VertexShader, DC_NEW GLVertexFunctionGenerator(output));
    setShaderFunction(Cg::FragmentShader, DC_NEW GLFragmentFunctionGenerator(output));
}

// ** GLShaderGenerator::visit
void GLShaderGenerator::visit(Structure& node)
{
    // Skip structures that contain input semantics
    if (node.hasSemantics())
    {
        return;
    }

    Structure::Fields& fields = node.fields();

    output() << ident() << "struct " << node.name();

    startCurlyBraces(true);
    for (Structure::Fields::iterator i = fields.begin(), end = fields.end(); i != end; ++i)
    {
        visit(**i);
    }
    endCurlyBraces(true);
}

// ** GLShaderGenerator::visit
void GLShaderGenerator::visit(Variable& node)
{
    const StringView& name = node.name();

    writeVariable(node);

    if (Expression* initializer = node.initializer())
    {
        output() << " = ";
        initializer->accept(*this);
    }

    output() << ";" << endl();
}

// ** GLShaderGenerator::writeType
FixedStringBuffer& GLShaderGenerator::writeType(const Type& type)
{
    if (const s8* builtInName = builtInType(type.builtInType()))
    {
        output() << builtInName;
    }
    else
    {
        output() << type.name();
    }

    return output();
}

// ** GLShaderGenerator::writeVariable
void GLShaderGenerator::writeVariable(const Variable& variable)
{
    output() << ident();
    output() << writeType(variable.type()) << " " << variable.name();
}

} // namespace Renderer

DC_END_DREEMCHEST