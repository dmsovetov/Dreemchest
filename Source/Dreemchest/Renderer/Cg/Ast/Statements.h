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

#ifndef __DC_Renderer_Ast_Statements_H__
#define __DC_Renderer_Ast_Statements_H__

#include "Ast.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{
    //! Statement is a building block of a function body.
    class Statement : public Ast
    {
    protected:
        
                            //! Constructs a Statement node instance.
                            Statement(NodeType type, s32 line, u16 column);
    };
    
    //! Statement block containes a list of child statements.
    class StatementBlock : public Statement
    {
    friend class Parser;
    protected:
        
                            //! Constructs a statement block node.
                            StatementBlock(s32 line, u16 column);
        
    private:
        
        //! Adds a new statement to this block.
        void                addStatement(Statement* statment);

        //! Invokes visitor's method to process this statement block.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
        Array<Statement*>   m_children; //!< Child statement nodes.
    };
    
    //! If control statement node.
    class If : public Statement
    {
    friend class Parser;
    private:
        
                            //! Constructs If statement instance.
                            If(Expression* condition, Statement* then, Statement* otherwise, s32 line, u16 column);

        //! Invokes visitor's method to process this 'if' statement.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
        Expression*         m_condition;    //!< Conditional expression.
        Statement*          m_then;         //!< Then statement.
        Statement*          m_otherwise;    //!< Else statement.
    };
    
    //! While loop statement node: while (condition) body
    class While : public Statement
    {
    friend class Parser;
    private:
        
                            //! Constructs While statement instance.
                            While(Expression* condition, Statement* body, s32 line, u16 column);

        //! Invokes visitor's method to process this 'while' statement.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
        Expression*         m_condition;    //!< Conditional expression.
        Statement*          m_body;         //!< Loop body.
    };
    
    //! For loop statement node: for (initial; condition; increment) body
    class For : public Statement
    {
    friend class Parser;
    private:
        
                            //! Constructs For statement instance.
                            For(Expression* initial, Expression* condition, Expression* increment, Statement* body, s32 line, u16 column);

        //! Invokes visitor's method to process this 'for' statement.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
        Expression*         m_initial;      //!< Initial expression.
        Expression*         m_condition;    //!< Conditional expression.
        Expression*         m_increment;    //!< An increment expression.
        Statement*          m_body;         //!< Loop body.
    };
    
    //! Return statement node: return [expression]
    class Return : public Statement
    {
    friend class Parser;
    private:
        
                            //! Constructs Return statement instance.
                            Return(Expression* value, s32 line, u16 column);

        //! Invokes visitor's method to process this 'return' statement.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
        Expression*         m_value;    //!< Return expression value.
    };
    
    //! Discard statement node: discard
    class Discard : public Statement
    {
    friend class Parser;
    private:
        
                            //! Constructs Discard statement instance.
                            Discard(s32 line, u16 column);

        //! Invokes visitor's method to process this 'discard' statement.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
    };
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_Ast_Expressions_H__    */
