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

#ifndef __DC_Renderer_Ast_Expressions_H__
#define __DC_Renderer_Ast_Expressions_H__

#include "Statements.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{
    //! An expression is a base class for a function call, operator, constant or variable term.
    class Expression : public Statement
    {
    protected:
        
                            //! Constructs an Expression instance.
                            Expression(NodeType type, s32 line, u16 column);
    };
    
    //! A binary/unary operator node.
    class Operator : public Expression
    {
    friend class Parser;
    private:
        
                            //! Constructs an Operator instance.
                            Operator(OperatorType type, Expression* lhs, Expression* rhs, s32 line, u16 column);
        
    private:
        
        OperatorType        m_type; //!< An operator type.
        Expression*         m_lhs;  //!< A left hand side.
        Expression*         m_rhs;  //!< A right hand side.
    };
    
    //! A constant term represents a numeric value.
    class ConstantTerm : public Expression
    {
    friend class Parser;
    private:
        
                            //! Constructs a ConstantTerm instance.
                            ConstantTerm(const StringView& value, s32 line, u16 column);
        
    private:
        
        StringView          m_value;    //!< A constant term value.
    };
    
    //! A variable term represents an identifier that points to a variable of strucure field.
    class VariableTerm : public Expression
    {
    friend class Parser;
    private:
        
                            //! Constructs a VariableTerm instance.
                            VariableTerm(const StringView& value, s32 line, u16 column);
        
    private:
        
        StringView          m_value;    //!< A constant term value.
    };
    
    //! A function call wrapps an identifier of function being called and a list of passed arguments.
    class FunctionCall : public Expression
    {
    friend class Parser;
    private:
        
                            //! Constructs a FunctionCall instance.
                            FunctionCall(const Identifier* identifier, s32 line, u16 column);
        
        //! Adds a function call argument.
        void                addArgument(Expression* expression);
        
    private:
        
        const Identifier*   m_identifier;   //!< A function identifier.
        Array<Expression*>  m_arguments;    //!< A list of expressions that are passed to a function call.
    };

    //! An object initializer expression.
    class ObjectInitializer : public Expression
    {
    friend class Parser;
    private:

                            //! Constructs a ObjectInitializer instance.
                            ObjectInitializer(s32 line, u16 column);
        
        //! Adds a new field initializer expression.
        void                addFieldInitializer(Expression* expression);
        
    private:

        Array<Expression*>  m_initializers; //!< A list of field initializer expressions.
    };
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_Ast_Expressions_H__    */
