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

#ifndef __DC_Renderer_Ast_Declarations_H__
#define __DC_Renderer_Ast_Declarations_H__

#include "Statements.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{
    //! A base class for all declaration nodes like function or variable.
    class Declaration : public Statement
    {
    protected:
                            //! Constructs a Declaration node instance.
                            Declaration(NodeType type, s32 line, u16 column);
    };
    
    //! Variable declaration node.
    class Variable : public Declaration
    {
    friend class Parser;
    public:
        
        //! Returns a variable name.
        const StringView&   name() const;

        //! Returns a variable type.
        const Type*         type() const;

        //! Returns a variable initializer expression.
        const Expression*   initializer() const;
        Expression*         initializer();
        
        //! Returns variable binding semantic.
        SemanticType        semantic() const;

    private:
        
                            //! Constructs Variable node instance.
                            Variable(const Identifier* identifier, const Type* type, Expression* initializer, SemanticType semantic);

        //! Invokes visitor's method to process this variable.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
        const Identifier*   m_identifier;   //!< A variable name identifier.
        const Type*         m_type;         //!< A variable data type.
        Expression*         m_initializer;  //!< An initializer expression.
        SemanticType        m_semantic;     //!< An input semantic name.
    };
    
    //! Structure declaration node.
    class Structure : public Declaration
    {
    friend class Parser;
    private:
        
                            //! Constructs a Structure node instance.
                            Structure(const Identifier* identifier);
        
        //! Adds new field to a structure.
        void                addField(Variable* field);

        //! Invokes visitor's method to process this structure.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
        const Identifier*   m_identifier;   //!< A structure identifier.
        Array<Variable*>    m_fields;       //!< Fields declared inside this structure.
    };
    
    //! Function declaration node.
    class Function : public Declaration
    {
    friend class Parser;
    public:

        //! A container type to store function arguments.
        typedef Array<Variable*> Arguments;

        //! Returns a function name.
        const StringView&   name() const;

        //! Returns a function type.
        const Type&         type() const;

        //! Returns function arguments.
        const Arguments&    arguments() const;
        Arguments&          arguments();

        //! Returns return value semantic binding.
        SemanticType        semantic() const;

        //! Returns a function body.
        const Statement*    body() const;
        Statement*          body();

    private:
        
                            //! Constructs a function instance.
                            Function(const Identifier* identifier, const Type* type);
        
        //! Adds a function argument.
        void                addArgument(Variable* argument);
        
        //! Sets a function body.
        void                setBody(Statement* value);
        
        //! Sets a function semantic.
        void                setSemantic(SemanticType value);

        //! Invokes visitor's method to process this function.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
        const Identifier*   m_identifier;   //!< A function identifier.
        const Type*         m_type;         //!< Return type of a function.
        Arguments           m_arguments;    //!< Function arguments.
        SemanticType        m_semantic;     //!< An output function semantic.
        Statement*          m_body;         //!< A function body instance.
    };
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_Ast_Declarations_H__    */
