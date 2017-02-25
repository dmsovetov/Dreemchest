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

#ifndef __DC_Renderer_CgParser_H__
#define __DC_Renderer_CgParser_H__

#include "Cg.h"
#include "Ast/Expressions.h"
#include "Ast/Declarations.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

namespace Cg
{

    //! Parser takes a Cg program source as an input and outputs the AST.
    class Parser : public ExpressionParser
    {
    public:
        
                        //! Constructs a Parser instance.
                        Parser(LinearAllocator& allocator);
        
        //! Parses an input string and returns a Program instance.
        Program*        parseProgramSource(const s8* input);
        
    private:
        
        //! A Cg program source can consist from the following list of tokens.
        enum TokenType
        {
              TokenKeyword = ExpressionTokenizer::TokenKeyword
            , TokenPunctuation
            , TokenOperator
            , TokenIdentifier
            , TokenNumber
            
            , TokenBuiltInType = UserDefinedToken
            , TokenStruct
            , TokenInputSemantic
            , TokenOutputSemantic
            , TokenReturn
            , TokenRegister
            , TokenIf
            , TokenElse
            , TokenFor
            , TokenWhile
            , TokenDiscard
            , TokenBraceOpen
            , TokenBraceClose
            , TokenParenthesesOpen
            , TokenParenthesesClose
            , TokenColon
            , TokenSemicolon
            , TokenComma
        };

        //! Describes an operator priority and associativity.
        struct OperatorInfo
        {
            OperatorType        type;       //!< An operator type.
            s32                 precedence; //!< Operator precedence.
            bool                left;       //!< Operator associativity.
            bool                binary;     //!< Indicates that an operator is binary.
        };
        
        //! Parses a variable declaration from an input stream.
        Variable*               parseVariableDeclaration();
        
        //! Parses a function declaration from an input stream.
        Function*               parseFunctionDeclaration();
        
        //! Parses a structure from an input stream.
        Structure*              parseStructure();
        
        //! Parses an expression from an input stream.
        Expression*             parseExpression(s32 precedence = 0);
        
        //! Parses an expression term from an input stream.
        Expression*             parseTerm();
        
        //! Parses a statement from an input stream.
        Statement*              parseStatement();
        
        //! Parses an optional seamantic specification.
        SemanticType            parseSemantic();
        
        //! Parses a register semantic specification.
        SemanticType            parseRegisterSemantic();
        
        //! Parses a function call.
        FunctionCall*           parseFunctionCall();
        
        //! Parses an if statement.
        If*                     parseIf();
        
        //! Parses a while statement.
        While*                  parseWhile();
        
        //! Parses for loop.
        For*                    parseFor();
        
        //! Parses a return statement.
        Return*                 parseReturn();
        
        //! Parses a discard statement.
        Discard*                parseDiscard();
        
        //! Parses a statement block.
        StatementBlock*         parseStatementBlock();

        //! Returns an operator info by a current token
        bool                    checkOperator(OperatorInfo& op) const;
        
        //! Expects to read a built-in data type or an identifier.
        Type*                   expectType();
        
        //! Expects to read an identifier.
        Identifier*             expectIdentifier();
        
        //! Expects to read an operator.
        OperatorType            expectOperator();
        
        //! Expects an input semantic name.
        SemanticType            expectSemantic();
        
        //! Expects a conditional expression '(' expr ')'
        Expression*             expectConditionalExpression();
        
        //! Allocates a new identifier from token.
        Identifier*             newIdentifier(const Token& token);
        
        //! Expects to read a token of a specified type.
        void                    expect(TokenType type);
    
        //! Writes an error message to an output stream.
        void                    emitError(const s8* format, ...);
        
        //! Writes an error message that an unexpected token was read from input.
        void                    emitExpected(const s8* expected);
        
        //! Registers a register semantic type.
        void                    registerSemantic(const s8* name, SemanticType semantic);
        
    private:
        
        //! A container type to map from an identifier to a semantic name.
        typedef HashMap<String64, SemanticType> RegisterSemanticTypes;
        
        LinearAllocator&        m_allocator;
        ExpressionTokenizer     m_tokenizer;
        RegisterSemanticTypes   m_registerSemantics;
        static OperatorInfo     s_operators[TotalOperatorTypes + 1];
    };
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_ShaderParser_H__    */
