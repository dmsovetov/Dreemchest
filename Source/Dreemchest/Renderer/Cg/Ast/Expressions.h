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
    public:

        //! Returns an expression resulting type.
        BuiltInType         type() const;

        //! Sets an expression resulting type.
        void                setType(BuiltInType value);

        //! Returns true if an expression is constant.
        bool                isConstant() const;

        //! Sets expression constant flag.
        void                setConstant(bool value);

    protected:
        
                            //! Constructs an Expression instance.
                            Expression(s32 line, u16 column);

    private:

        BuiltInType         m_type;         //!< Expression resulting type.
        bool                m_isConstant;   //!< Indicates that an expression is constant and can't be modified by operator '=', '+=', etc.
    };
    
    //! A binary/unary operator node.
    class Operator : public Expression
    {
    friend class Parser;
    public:

        //! Returns an operator type.
        OperatorType        type() const;

        //! Returns a left hand side expression.
        const Expression*   lhs() const;
        Expression*         lhs();

        //! Returns a right hand side expression.
        const Expression*   rhs() const;
        Expression*         rhs();

        //! Invokes visitor's method to process this operator.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;

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
    public:

        //! Returns a constant term value.
        const StringView&   value() const;

        //! Invokes visitor's method to process this constant term.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;

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
    public:

        //! Invokes visitor's method to process this variable term.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;

        //! Returns a variable term name.
        const StringView&   name() const;

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
    public:

        //! A container type to store function call arguments.
        typedef List<Expression*> Arguments;

        //! Returns an identifier of a function being called.
        const StringView&   name() const;

        //! Returns a built-in type being constructed (if any).
        BuiltInType         builtInType() const;

        //! Returns a function call arguments.
        const Arguments&    arguments() const;
        Arguments&          arguments();

        //! Invokes visitor's method to process this function call.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;

    private:
        
                            //! Constructs a FunctionCall instance.
                            FunctionCall(const Identifier& identifier, BuiltInType builtInType, s32 line, u16 column);
        
        //! Adds a function call argument.
        void                addArgument(Expression* expression);
        
    private:
        
        const Identifier&   m_identifier;   //!< A function identifier.
        BuiltInType         m_builtInType;  //!< Indicates that this is a built-in type construction.
        Arguments           m_arguments;    //!< A list of expressions that are passed to a function call.
    };

    //! An object initializer expression.
    class ObjectInitializer : public Expression
    {
    friend class Parser;
    public:

        //! Invokes visitor's method to process this object initializer.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;

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
