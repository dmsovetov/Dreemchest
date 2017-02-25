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

#include "CgParser.h"

#define newAst(T, ...) new (m_allocator.allocate(sizeof(T))) T(__VA_ARGS__)

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{
    
// ** Parser::s_operators
Parser::OperatorInfo Parser::s_operators[TotalOperatorTypes + 1] =
{
      { OpPlusEqual,         0, false, true }
    , { OpMinusEqual,        0, false, true }
    , { OpDevideEqual,       2, false, true }
    , { OpMultiplyEqual,     2, false, true }
    , { OpEqual,             3, false, true }
    , { OpPlus,              4, false, true }
    , { OpMinus,             4, false, true }
    , { OpDivide,            5, false, true }
    , { OpMultiply,          5, false, true }
    , { OpCompare,           6, false, true }
    , { OpLess,              7, false, true }
    , { OpLessEqual,         7, false, true }
    , { OpGreater,           7, false, true }
    , { OpGreaterEqual,      7, false, true }
    , { OpMember,            8, false, true }
    , { TotalOperatorTypes, -1, false, true }
};

// ** Parser::Parser
Parser::Parser(LinearAllocator& allocator)
    : ExpressionParser(m_tokenizer, 2)
    , m_allocator(allocator)
    , m_tokenizer(allocator)
{
    m_tokenizer.addKeyword("void", TokenBuiltInType);
    m_tokenizer.addKeyword("float", TokenBuiltInType);
    m_tokenizer.addKeyword("float2", TokenBuiltInType);
    m_tokenizer.addKeyword("float3", TokenBuiltInType);
    m_tokenizer.addKeyword("float4", TokenBuiltInType);
    m_tokenizer.addKeyword("float4x4", TokenBuiltInType);
    m_tokenizer.addKeyword("struct", TokenStruct);
    m_tokenizer.addKeyword("if", TokenIf);
    m_tokenizer.addKeyword("else", TokenElse);
    m_tokenizer.addKeyword("while", TokenWhile);
    m_tokenizer.addKeyword("for", TokenFor);
    m_tokenizer.addKeyword("discard", TokenDiscard);
    m_tokenizer.addKeyword("return", TokenReturn);
    m_tokenizer.addKeyword("register", TokenRegister);
    m_tokenizer.addKeyword("NORMAL", TokenInputSemantic);
    m_tokenizer.addKeyword("POSITION", TokenInputSemantic);
    m_tokenizer.addKeyword("TEXCOORD0", TokenInputSemantic);
    m_tokenizer.addKeyword("COLOR0", TokenOutputSemantic);
    m_tokenizer.addKeyword("COLOR1", TokenOutputSemantic);
    
    m_tokenizer.addPunctuation(";", TokenSemicolon);
    m_tokenizer.addPunctuation(":", TokenColon);
    m_tokenizer.addPunctuation("{", TokenBraceOpen);
    m_tokenizer.addPunctuation("}", TokenBraceClose);
    m_tokenizer.addPunctuation("(", TokenParenthesesOpen);
    m_tokenizer.addPunctuation(")", TokenParenthesesClose);
    m_tokenizer.addPunctuation(",", TokenComma);
    
    m_tokenizer.addOperator("+", OpPlus);
    m_tokenizer.addOperator("+=", OpPlusEqual);
    m_tokenizer.addOperator("=", OpEqual);
    m_tokenizer.addOperator(".", OpMember);
    m_tokenizer.addOperator("-", OpMinus);
    m_tokenizer.addOperator("/", OpDivide);
    m_tokenizer.addOperator("*", OpMultiply);
    m_tokenizer.addOperator("<", OpLess);
    
    registerSemantic("s0", TEXUNIT0);
}
    
// ** Parser::expect
void Parser::expect(TokenType type)
{
    if (check(type))
    {
        next();
        return;
    }
    
    const s8* token = "";
    
    switch (type)
    {
        case TokenSemicolon:
            token = ";";
            break;
        case TokenBraceOpen:
            token = "{";
            break;
        case TokenBraceClose:
            token = "}";
            break;
        case TokenParenthesesOpen:
            token = "(";
            break;
        case TokenParenthesesClose:
            token = ")";
            break;
        default:
            NIMBLE_BREAK;
    }

    emitExpected(token);
    next();
}
    
// ** Parser::parseProgramSource
Program* Parser::parseProgramSource(const s8* input)
{
    prepare(input);
    
    Program* program = newAst(Program);
    
    while (hasTokens())
    {
        const Token& token = current();
        
        switch (token.type())
        {
            case TokenIdentifier:
            case TokenBuiltInType:
                if (check(TokenIdentifier, 1) && check(TokenParenthesesOpen, 2))
                {
                    program->addDeclaration(parseFunctionDeclaration());
                }
                else
                {
                    program->addDeclaration(parseVariableDeclaration());
                    expect(TokenSemicolon);
                }
                break;
            case TokenStruct:
                program->addDeclaration(parseStructure());
                break;
            default:
                emitError("unexpected token '%s'", token.str().c_str());
                next();
        }
    }
    
    return program;
}

// ** Parser::parseVariableDeclaration
Variable* Parser::parseVariableDeclaration()
{
    // Type name is expected
    Type* type = expectType();
    
    if (!type)
    {
        return NULL;
    }
    
    // Identifier is expected
    Identifier* identifier = expectIdentifier();
    
    if (!identifier)
    {
        return NULL;
    }
    
    // Parse input semantic
    SemanticType semantic = parseSemantic();
    
    // Parse initializer
    Expression* initializer = NULL;
    
    if (parse("="))
    {
        initializer = parseExpression();
    }
    
    // Allocate a variable instance
    Variable* variable = newAst(Variable, identifier, type, initializer, semantic);

    return variable;
}
    
// ** Parser::parseFunctionDeclaration
Function* Parser::parseFunctionDeclaration()
{
    // Type name is expected
    Type* type = expectType();
    
    if (!type)
    {
        return NULL;
    }
    
    // Identifier is expected
    Identifier* identifier = expectIdentifier();
    
    if (!identifier)
    {
        return NULL;
    }
    
    // Allocate function instance
    Function* function = newAst(Function, identifier, type);
    
    // Parse function arguments
    expect(TokenParenthesesOpen);
    if (!parse(TokenParenthesesClose))
    {
        do
        {
            function->addArgument(parseVariableDeclaration());
        } while (parse(TokenComma));
        expect(TokenParenthesesClose);
    }
    
    // Parse an output semantic type
    SemanticType semantic = parseSemantic();
    function->setSemantic(semantic);
    
    // Parse function body
    StatementBlock* body = parseStatementBlock();
    function->setBody(body);

    return function;
}
    
// ** Parser::parseStructure
Structure* Parser::parseStructure()
{
    // Structure declaration starts with a 'struct' keyword
    expect(TokenStruct);
    
    // 'struct' keyword is followed by an identifier
    Identifier* identifier = expectIdentifier();
    
    if (!identifier)
    {
        return NULL;
    }
    
    // Allocate a structure instance
    Structure* structure = newAst(Structure, identifier);
    
    expect(TokenBraceOpen);
    while (!check(TokenBraceClose))
    {
        // Parse variable declaration
        structure->addField(parseVariableDeclaration());
        
        // Each field ends with a semicolon
        expect(TokenSemicolon);
    }
    expect(TokenBraceClose);
    
    // Structure declarations ends with a ';' token
    expect(TokenSemicolon);
    
    return structure;
}
    
// ** Parser::parseSemantic
SemanticType Parser::parseSemantic()
{
    SemanticType semantic = INVALID_SEMANTIC;
    
    if (parse(TokenColon))
    {
        semantic = expectSemantic();
    }
    
    return semantic;
}
    
// ** Parser::parseRegisterSemantic
SemanticType Parser::parseRegisterSemantic()
{
    // Register semantic starts from a 'register' keyword
    expect(TokenRegister);

    // 'register' keyword is followed by '(' token
    expect(TokenParenthesesOpen);
    
    // Parse the semantic from an identifier
    Token token = current();
    expect(TokenIdentifier);
    
    // Lookup semantic by name
    SemanticType semantic = INVALID_SEMANTIC;
    
    //const s8* name = token.text();
    RegisterSemanticTypes::const_iterator i = m_registerSemantics.find(String64(token.str().c_str()));
    
    if (i == m_registerSemantics.end())
    {
        emitError("unknown semantic '%s'", token.str().c_str());
    }
    else
    {
        semantic = i->second;
    }
    
    // Register semanti ends with a ')' token
    expect(TokenParenthesesClose);
    
    return semantic;
}
    
// ** Parser::parseStatement
Statement* Parser::parseStatement()
{
    const Token& token     = current();
    Statement*   statement = NULL;
    
    switch (token.type())
    {
        case TokenBuiltInType:
            statement = parseVariableDeclaration();
            expect(TokenSemicolon);
            break;
            
        case TokenIf:
            statement = parseIf();
            break;
            
        case TokenWhile:
            statement = parseWhile();
            break;
            
        case TokenFor:
            statement = parseFor();
            break;
            
        case TokenReturn:
            statement = parseReturn();
            break;
            
        case TokenDiscard:
            statement = parseDiscard();
            break;
            
        case TokenBraceOpen:
            statement = parseStatementBlock();
            break;

        case TokenSemicolon:
            next();
            break;
            
        default:
            if (check(TokenIdentifier) && check(TokenIdentifier, 1))
            {
                statement = parseVariableDeclaration();
            }
            else
            {
                statement = parseExpression();
            }
            expect(TokenSemicolon);
    }
    
    return statement;
}
    
// ** Parser::parseStatementBlock
StatementBlock* Parser::parseStatementBlock()
{
    // Save statment position
    s32 line   = current().line();
    u16 column = current().column();
    
    // Statement block starts with a '{' token
    expect(TokenBraceOpen);
    
    // Allocate statement block instance
    StatementBlock* block = newAst(StatementBlock, line, column);
    
    while (!check(TokenBraceClose))
    {
        block->addStatement(parseStatement());
    }
    
    // Statement block ends with a '}' token
    expect(TokenBraceClose);
    
    return block;
}

// ** Parser::parseReturn
Return* Parser::parseReturn()
{
    // Save statment position
    s32 line   = current().line();
    u16 column = current().column();
    
    expect(TokenReturn);
    
    Expression* value = NULL;
    
    if (!parse(TokenSemicolon))
    {
        value = parseExpression();
    }
    expect(TokenSemicolon);
    
    return newAst(Return, value, line, column);
}

// ** Parser::parseDiscard
Discard* Parser::parseDiscard()
{
    // Save statment position
    s32 line   = current().line();
    u16 column = current().column();
    
    expect(TokenDiscard);
    expect(TokenSemicolon);
    
    return newAst(Discard, line, column);
}
    
// ** Parser::parseIf
If* Parser::parseIf()
{
    // Save statment position
    s32 line   = current().line();
    u16 column = current().column();
    
    // Starts with an 'if' token
    expect(TokenIf);
    
    // Then goes a conditional expression
    Expression* condition = expectConditionalExpression();
    
    if (condition == NULL)
    {
        return NULL;
    }
    
    // Then statment body
    Statement* then = parseStatement();
    
    // And an optional else part
    Statement* otherwise = NULL;
    
    if (parse(TokenElse))
    {
        otherwise = parseStatement();
    }
    
    // Finally allocate if node
    If* statement = newAst(If, condition, then, otherwise, line, column);
    
    return statement;
}
    
// ** Parser::parseWhile
While* Parser::parseWhile()
{
    // Save statment position
    s32 line   = current().line();
    u16 column = current().column();
    
    // Starts with a 'while' token
    expect(TokenWhile);
    
    // Then goes a conditional expression
    Expression* condition = expectConditionalExpression();
    
    if (condition == NULL)
    {
        return NULL;
    }
    
    // While loop body
    Statement* body = parseStatement();
    
    // Finally allocate while node
    While* statement = newAst(While, condition, body, line, column);
    
    return statement;
}
    
// ** Parser::parseFor
For* Parser::parseFor()
{
    // Save statment position
    s32 line   = current().line();
    u16 column = current().column();
    
    // Starts with a 'for' token
    expect(TokenFor);
    
    // Now parse a for loop expressions
    expect(TokenParenthesesOpen);
    
    // Parse initial expression
    Expression* initial = NULL;
    
    if (!parse(TokenSemicolon))
    {
        initial = parseExpression();
        expect(TokenSemicolon);
    }
    
    // Parse conditional expression
    Expression* conditional = NULL;
    
    if (!parse(TokenSemicolon))
    {
        conditional = parseExpression();
        expect(TokenSemicolon);
    }
    
    // Parse an increment expression
    Expression* increment = NULL;
    
    if (!parse(TokenSemicolon))
    {
        increment = parseExpression();
        expect(TokenSemicolon);
    }
    
    // Parse for loop body
    Statement* body = parseStatement();
    
    // Allocate for loop node
    For* for_ = newAst(For, initial, conditional, increment, body, line, column);
    
    return for_;
}
    
// ** Parser::parseFunctionCall
FunctionCall* Parser::parseFunctionCall()
{
    Identifier* identifier = expectIdentifier();
    
    FunctionCall* call = newAst(FunctionCall, identifier, identifier->line(), identifier->column());
    
    expect(TokenParenthesesOpen);
    do
    {
        call->addArgument(parseExpression());
    } while (parse(TokenComma));
    expect(TokenParenthesesClose);
    
    return call;
}
    
// ** Parser::parseExpression
Expression* Parser::parseExpression(s32 precedence)
{
    // Parse a left hand side expression
    Expression* lhs = parseTerm();

    // Save an operator line and column
    s32 line = current().line();
    u16 column = current().column();

    // Parse a binary operator and rhs if possible
    OperatorInfo op;
    
    while (checkOperator(op))
    {
        // Should we consume the next operator?
        if (!op.binary || op.precedence < precedence)
        {
            break;
        }

        // Consume
        next();

        // Calculate next precedence value
        s32 nextPrecedence = op.left ? op.precedence + 1 : op.precedence;
        
        // Parse a right hand side expression
        Expression* rhs = parseExpression(nextPrecedence);
        
        // Compose an operator
        lhs = newAst(Operator, op.type, lhs, rhs, line, column);
    }
    
    return lhs;
}
    
// ** Parser::parseTerm
Expression* Parser::parseTerm()
{
    const Token& token = current();
    Expression*  term  = NULL;

    switch (token.type()) {
        case TokenIdentifier:
            if (check(TokenParenthesesOpen, 1))
            {
                term = parseFunctionCall();
            }
            else
            {
                term = newAst(VariableTerm, token.text(), token.line(), token.column());
                next();
            }
            break;

        case TokenNumber:
            term = newAst(ConstantTerm, token.text(), token.line(), token.column());
            next();
            break;
            
        default:
            emitExpected("term");
            next();
    }
    
    return term;
}

// ** Parser::checkOperator
bool Parser::checkOperator(OperatorInfo& info) const
{
    if (!check(TokenOperator))
    {
        return false;
    }

    // Extract an operator type
    OperatorType op = static_cast<OperatorType>(current().subtype());
    info = s_operators[op];

    return true;
}
    
// ** Parser::expectConditionalExpression
Expression* Parser::expectConditionalExpression()
{
    Expression* expression = NULL;
    
    if (parse(TokenParenthesesOpen))
    {
        expression = parseExpression();
        expect(TokenParenthesesClose);
        
        return expression;
    }
    
    emitExpected("conditional expression");
    return NULL;
}
    
// ** Parser::expectType
Type* Parser::expectType()
{
    Token token = current();
    
    if (parse(TokenBuiltInType))
    {
        return newAst(Type, newIdentifier(token), static_cast<BuiltInType>(token.subtype()), token.line(), token.column());
    }
    else if(parse(TokenIdentifier))
    {
        return newAst(Type, newIdentifier(token), TypeInvalid, token.line(), token.column());
    }
    
    emitExpected("type");
    next();
    
    return NULL;
}
    
// ** Parser::expectOperator
OperatorType Parser::expectOperator()
{
    if (check(TokenOperator))
    {
        OperatorType op = static_cast<OperatorType>(current().subtype());
        next();
        return op;
    }
    
    emitExpected("operator");
    return TotalOperatorTypes;
}
    
// ** Parser::expectIdentifier
Identifier* Parser::expectIdentifier()
{
    Token token = current();
    
    if (parse(TokenIdentifier))
    {
        return newIdentifier(token);
    }
    
    emitExpected("identifier");
    next();
    
    return NULL;
}
    
// ** Parser::expectSemantic
SemanticType Parser::expectSemantic()
{
    SemanticType type = INVALID_SEMANTIC;
    
    switch (current().type())
    {
        case TokenInputSemantic:
        case TokenOutputSemantic:
            type = static_cast<SemanticType>(current().subtype());
            next();
            return type;
            
        case TokenRegister:
            return parseRegisterSemantic();
    }

    emitExpected("input semantic");
    return type;
}
    
// ** Parser::registerSemantic
void Parser::registerSemantic(const s8* name, SemanticType semantic)
{
    m_registerSemantics[String64(name)] = semantic;
}

// ** Parser::emitError
void Parser::emitError(const s8* format, ...)
{
    NIMBLE_LOGGER_FORMAT(format);
    printf("ERROR: %d:%d: %s\n", current().line(), current().column(), buffer);
}
    
// ** Parser::emitExpected
void Parser::emitExpected(const s8* expected)
{
    emitError("expected '%s' after '%s', got '%s'", expected, previous().str().c_str(), current().str().c_str());
}
    
// ** Parser::newIdentifier
Identifier* Parser::newIdentifier(const Token& token)
{
    u8* pointer = m_allocator.allocate(sizeof(Identifier));
    return new (pointer) Identifier(token.text(), token.line(), token.column());
}
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST
