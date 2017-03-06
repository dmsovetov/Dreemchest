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

// ** ProgramVisitor::ProgramVisitor
ProgramVisitor::ProgramVisitor()
    : m_currentFunction(NULL)
    , m_currentStructure(NULL)
    , m_currentProgram(NULL)
{

}

// ** ProgramVisitor::setShaderFunction
void ProgramVisitor::setShaderFunction(ShaderType shader, UPtr<Visitor> value)
{
    m_shaderFunction[shader] = value;
}

// ** ProgramVisitor::setExpression
void ProgramVisitor::setExpression(UPtr<Visitor> value)
{
    m_expression = value;
}

// ** ProgramVisitor::currentFunction
const Function* ProgramVisitor::currentFunction() const
{
    return m_currentFunction;
}

// ** ProgramVisitor::currentStructure
const Structure* ProgramVisitor::currentStructure() const
{
    return m_currentStructure;
}

// ** ProgramVisitor::enterFunction
void ProgramVisitor::enterFunction(const Function& node)
{
    m_currentFunction = &node;
}

// ** ProgramVisitor::leaveFunction
void ProgramVisitor::leaveFunction()
{
    m_currentFunction = NULL;
}

// ** ProgramVisitor::visit
void ProgramVisitor::visit(Program& program)
{
    // Save current program
    m_currentProgram = &program;

    // Process each declaration
    Program::Declarations& declarations = program.declarations();

    for (Program::Declarations::iterator i = declarations.begin(), end = declarations.end(); i != end; ++i)
    {
        (*i)->accept(*this);
    }

    // Reset program
    m_currentProgram = NULL;
}

// ** ProgramVisitor::visit
void ProgramVisitor::visit(StatementBlock& node)
{
    StatementBlock::Statements& statements = node.statements();

    for (StatementBlock::Statements::iterator i = statements.begin(), end = statements.end(); i != end; ++i)
    {
        (*i)->accept(*this);
    }
}

// ** ProgramVisitor::visit
void ProgramVisitor::visit(Function& node)
{
    enterFunction(node);

    Visitor* shaderFunctionVisitor = m_shaderFunction[node.shader()].get();

    if (shaderFunctionVisitor)
    {
        // Dispatch shader functions to a separate visitor
        node.accept(*shaderFunctionVisitor);
    }
    else
    {
        if (Statement* body = node.body())
        {
            body->accept(*this);
        }
    }

    leaveFunction();
}

// ** ProgramVisitor::visit
void ProgramVisitor::visit(Operator& node)
{
    if (m_expression.get())
    {
        node.accept(*m_expression);
    }
}

// ** ProgramVisitor::visit
void ProgramVisitor::visit(Return& node)
{
    if (Expression* value = node.value())
    {
        value->accept(*this);
    }
}

// ----------------------------------------------------- ExpressionVisitor ----------------------------------------------------- //

// ** ExpressionVisitor::ExpressionVisitor
ExpressionVisitor::ExpressionVisitor()
    : m_operator(NULL)
{
}

// ** ExpressionVisitor::op
Operator* ExpressionVisitor::op() const
{
    return m_operator;
}

// ** ExpressionVisitor::visit
void ExpressionVisitor::visit(Operator& node)
{
    // Save current operator
    Operator* prevOp = m_operator;

    // Make this one active
    m_operator = &node;

    // Process leaf nodes.
    if (Expression* lhs = node.lhs())
    {
        lhs->accept(*this);
    }
    if (Expression* rhs = node.rhs())
    {
        rhs->accept(*this);
    }

    // Restore an operator
    m_operator = prevOp;
}

// ---------------------------------------------------- ExpressionGenerator ---------------------------------------------------- //

// ** ExpressionGenerator::s_operators
const s8* ExpressionGenerator::s_operators[TotalOperatorTypes] =
{
      "+="
    , "-="
    , "/="
    , "*="
    , "="
    , "+"
    , "-"
    , "/"
    , "*"
    , "=="
    , "<"
    , "<="
    , ">"
    , ">="
    , "."
};

// ** ExpressionGenerator::ExpressionGenerator
ExpressionGenerator::ExpressionGenerator(TypeMapping type, FixedStringBuffer& output)
    : CodeGenerator(output)
{
    setTypeMapping(type);
}

// ** ExpressionGenerator::visit
void ExpressionGenerator::visit(Operator& node)
{
    if (Cg::Expression* lhs = node.lhs())
    {
        lhs->accept(*this);
    }

    Cg::OperatorType op = node.type();

    if (op != Cg::OpMember)
    {
        output() << " ";
    }

    output() << s_operators[op];

    if (op != Cg::OpMember)
    {
        output() << " ";
    }

    if (Cg::Expression* rhs = node.rhs())
    {
        rhs->accept(*this);
    }
}

// ** ExpressionGenerator::visit
void ExpressionGenerator::visit(ConstantTerm& node)
{
    output() << node.value();
}

// ** ExpressionGenerator::visit
void ExpressionGenerator::visit(VariableTerm& node)
{
    output() << node.name();
}

// ** ExpressionGenerator::visit
void ExpressionGenerator::visit(FunctionCall& node)
{
    const Function* function = node.function();

    if (const s8* type = builtInType(function->constructor()))
    {
        output() << type;
    }
    else
    {
        output() << node.name();
    }
    output() << "(";

    FunctionCall::Arguments& args = node.arguments();
    size_t argumentsLeftToProcess = args.size();

    for (FunctionCall::Arguments::iterator i = args.begin(), end = args.end(); i != end; ++i)
    {
        (*i)->accept(*this);
        argumentsLeftToProcess--;

        if (argumentsLeftToProcess)
        {
            output() << ", ";
        }
    }

    output() << ")";
}

// ---------------------------------------------------------- CodeGenerator ---------------------------------------------------------- //

// ** CodeGenerator::s_indentations
const s8* CodeGenerator::s_indentations[MaxIndent] =
{
      ""
    , "  "
    , "    "
    , "      "
    , "        "
    , "          "
    , "            "
    , "              "
};

// ** CodeGenerator::CodeGenerator
CodeGenerator::CodeGenerator(FixedStringBuffer& output)
    : m_output(output)
    , m_indent(0)
    , m_inputSemantic(NULL)
    , m_outputSemantic(NULL)
    , m_type(NULL)
{

}

// ** CodeGenerator::visit
void CodeGenerator::visit(Cg::Operator& node)
{
    output() << ident();
    ProgramVisitor::visit(node);
    output() << ";" << endl();
}

// ** CodeGenerator::visit
void CodeGenerator::visit(Return& node)
{
    output() << ident() << "return";

    if (Cg::Expression* value = node.value())
    {
        output() << " ";
        ProgramVisitor::visit(node);
    }

    output() << ";" << endl();
}

// ** CodeGenerator::output
FixedStringBuffer& CodeGenerator::output()
{
    return m_output;
}

// ** CodeGenerator::output
const s8* CodeGenerator::inputSemantic(SemanticType semantic) const
{
    NIMBLE_ABORT_IF(m_inputSemantic == NULL, "no input semantic mapping set");
    return m_inputSemantic(semantic);
}

// ** CodeGenerator::output
const s8* CodeGenerator::outputSemantic(SemanticType semantic) const
{
    NIMBLE_ABORT_IF(m_outputSemantic == NULL, "no output semantic mapping set");
    return m_outputSemantic(semantic);
}

// ** CodeGenerator::output
const s8* CodeGenerator::builtInType(BuiltInType type) const
{
    NIMBLE_ABORT_IF(m_type == NULL, "no built-in type mapping set");
    return m_type(type);
}

// ** CodeGenerator::endl
const s8* CodeGenerator::endl() const
{
    return "\n";
}

// ** CodeGenerator::startCurlyBraces
void CodeGenerator::startCurlyBraces(bool newLine)
{
    if (newLine)
    {
        output() << endl();
    }
    output() << "{" << endl();
    m_indent++;
}

// ** CodeGenerator::endCurlyBraces
void CodeGenerator::endCurlyBraces(bool semicolon)
{
    m_indent--;
    output() << "}";

    if (semicolon)
    {
        output() << ";";
    }

    output() << endl() << endl();
}

// ** CodeGenerator::setInputMapping
void CodeGenerator::setInputMapping(SemanticMapping value)
{
    m_inputSemantic = value;
}

// ** CodeGenerator::setOutputMapping
void CodeGenerator::setOutputMapping(SemanticMapping value)
{
    m_outputSemantic = value;
}

// ** CodeGenerator::setTypeMapping
void CodeGenerator::setTypeMapping(TypeMapping value)
{
    m_type = value;
}

// ** CodeGenerator::ident
const s8* CodeGenerator::ident() const
{
    return s_indentations[min2<s32>(m_indent, MaxIndent)];
}
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST