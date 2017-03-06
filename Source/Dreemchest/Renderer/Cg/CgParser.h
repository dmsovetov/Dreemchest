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
    //! An output message buffer.
    class OutputMessages
    {
    public:

        //! An output message type.
        enum MessageType
        {
              VerboseMessage
            , WarningMessage
            , ErrorMessage
        };

        //! A single message item.
        struct Message
        {
            s32         line;       //!< A line number where this message was emitted.
            u16         column;     //!< A column number where this message was emitted.
            MessageType type;       //!< A message type.
            const s8*   text;       //!< A message text string.
        };

        //! A container type to store recorded messages.
        typedef List<Message> Messages;

                        //! Constructs OutputMessage instance.
                        OutputMessages();

        //! Outputs an error message.
        void            error(s32 line, u16 column, const s8* text);

        //! Outputs a warning message.
        void            warning(s32 line, u16 column, const s8* text);

        //! Outputs a verbose messsage.
        void            verbose(s32 line, u16 column, const s8* text);

        //! Returns a total number of recorded error messages.
        s32             errorCount() const;

        //! Sorts a message list.
        void            sort();

        //! Returns a message list.
        const Messages& messages() const;

    private:

        //! Adds a new message of specified type.
        void            pushMessage(MessageType type, s32 line, u16 column, const s8* text);

    private:

        Messages        m_messages; //!< Recorded messages.
        s32             m_errors;   //!< A total number of errors recorded.
    };

    //! Parser takes a Cg program source as an input and outputs the AST.
    class Parser : public ExpressionParser
    {
    public:
        
                                //! Constructs a Parser instance.
                                Parser(LinearAllocator& allocator);
        
        //! Parses an input string and returns a Program instance.
        Program*                parseProgramSource(const s8* input);

        //! Returns recorded messages.
        const OutputMessages&   messages() const;
        
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
            , TokenPreprocessorPragma
            , TokenPreprocessorDefine
            , TokenPreprocessorIf
            , TokenPreprocessorElif
            , TokenPreprocessorElse
            , TokenPreprocessorEndif
        };

        //! Describes an operator priority and associativity.
        struct OperatorInfo
        {
            OperatorType        type;       //!< An operator type.
            s32                 precedence; //!< Operator precedence.
            u8                  flags;      //!< An operator flags.
        };
        
        //! Parses a variable declaration from an input stream.
        Variable*               parseVariableDeclaration(u8 flags = 0);
        
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

        //! Parses an object initializer.
        ObjectInitializer*      parseObjectInitializer();
        
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

        //! Parses a pragma definition.
        void                    parsePragma(Program* program);

        //! Returns an operator info by a current token
        bool                    checkOperator(OperatorInfo& op) const;
        
        //! Expects to read a built-in data type or an identifier.
        Type*                   expectType();
        
        //! Expects to read an identifier.
        Identifier*             expectIdentifier();

        //! Expects to read an identifier of a type name.
        Identifier*             expectFunctionIdentifier();
        
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

        //! Writes an error message to an output stream.
        void                    emitError(s32 line, u16 column, const s8* format, ...);
        
        //! Writes an error message that an unexpected token was read from input.
        void                    emitExpected(const s8* expected);
        
        //! Registers a register semantic type.
        void                    registerSemantic(const s8* name, const s8* shortName, SemanticType semantic);

        //! Searches for a semantic by it's name.
        SemanticType            findSemanticByToken(const Token& token) const;

        //! Searches for a declaration in a scope chain.
        const Declaration*      findDeclaration(const Identifier* identifier) const;

        //! Adds a new declaration to a topmost scope.
        void                    addDeclaration(const Declaration* declaration);

        //! Returns the topmost declaration scope.
        Scope*                  scope() const;

        //! Pushes a declaration scope.
        void                    pushDeclarationScope(Scope& scope);

        //! Pops a declaration scope.
        void                    popDeclarationScope();

        //! Registers built-in identifiers inside a given scope.
        void                    registerBuiltIns(Scope& scope);

        //! Registers built-in type inside a given scope.
        void                    registerBuiltInType(Scope& scope, const s8* name, BuiltInType type);

        //! Interns a null-terminated string as an identifier.
        const Identifier*       internString(const s8* value);

        
    private:

        //! A forward declaration of a visitor class that resolves declaration identifiers.
        class                   DeclarationResolver;
        
        //! A container type to map from an identifier to a semantic name.
        typedef HashMap<String64, SemanticType> RegisterSemanticTypes;
        
        LinearAllocator&        m_allocator;            //!< A linear allocator to be used for allocations.
        ExpressionTokenizer     m_tokenizer;            //!< Input stream tokenizer.
        RegisterSemanticTypes   m_registerSemantics;    //!< Mapping from a string to a semantic type.
        Stack<Scope*>           m_scopeStack;           //!< A scope stack.
        OutputMessages          m_messages;             //!< An output messages.
        static OperatorInfo     s_operators[TotalOperatorTypes + 1];
    };
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_ShaderParser_H__    */
