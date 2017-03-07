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
    public:

        //! Declaration type
        enum DeclarationType
        {
              VariableDeclaration
            , FunctionDeclaration
            , StructureDeclaration
        };

        //! Returns a declaration name.
        const StringView&   name() const;

        //! Returns a declaration type.
        DeclarationType     declarationType() const;

    protected:
                            //! Constructs a Declaration node instance.
                            Declaration(DeclarationType declarationType, const Identifier& identifier, s32 line, u16 column);

    private:

        const Identifier&   m_identifier;   //!< A variable name identifier.
        DeclarationType     m_declarationType; //!< A declaration type. Is it a variable, function or a structure.
    };
    
    //! Variable declaration node.
    class Variable : public Declaration
    {
    friend class Parser;
    public:

        //! Returns a variable type.
        const Type&         type() const;

        //! Returns variable flags.
        const FlagSet8&     flags() const;
        FlagSet8&           flags();

        //! Returns a variable initializer expression.
        const Expression*   initializer() const;
        Expression*         initializer();
        
        //! Returns variable binding semantic.
        SemanticType        semantic() const;

        //! Invokes visitor's method to process this variable.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;

    private:
        
                            //! Constructs Variable node instance.
                            Variable(const Identifier& identifier, const Type& type, Expression* initializer, SemanticType semantic, u8 flags = 0);
        
    private:

        const Type&         m_type;         //!< A variable data type.
        Expression*         m_initializer;  //!< An initializer expression.
        SemanticType        m_semantic;     //!< An input semantic name.
        FlagSet8            m_flags;        //!< Variable flags.
    };
    
    //! Structure declaration node.
    class Structure : public Declaration
    {
    friend class Parser;
    public:

        //! A container type to store structure fields.
        typedef List<Variable*> Fields;

        //! Returns structure fields.
        const Fields&       fields() const;
        Fields&             fields();

        //! Returns a declaration scope.
        const Scope&        declarations() const;
        Scope&              declarations();

        //! Returns true if any field has a semantic binding.
        bool                hasSemantics() const;

    private:
        
                            //! Constructs a Structure node instance.
                            Structure(const Scope* scope, const Identifier& identifier);
        
        //! Adds new field to a structure.
        void                addField(Variable* field);

        //! Invokes visitor's method to process this structure.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
  
        Fields              m_fields;       //!< Fields declared inside this structure.
        Scope               m_declarations; //!< Variable declaration scope.
    };
    
    //! Function declaration node.
    class Function : public Declaration
    {
    friend class Parser;
    public:

        //! A container type to store function arguments.
        typedef Array<Variable*> Arguments;

        //! Returns a function type.
        const Type&         type() const;

        //! Returns function arguments.
        const Arguments&    arguments() const;
        Arguments&          arguments();

        //! Returns a declaration scope.
        const Scope&        declarations() const;
        Scope&              declarations();

        //! Returns return value semantic binding.
        SemanticType        semantic() const;

        //! Returns a function body.
        const Statement*    body() const;
        Statement*          body();

        //! Returns a constructed type.
        BuiltInType         constructor() const;

        //! Sets a shader function type.
        void                setShader(ShaderType value);

        //! Returns a shader type associated with this function with #pragma option.
        ShaderType          shader() const;

        //! Returns true if this function is marked as a shader entry point.
        bool                isShader() const;

        //! Invokes visitor's method to process this function.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;

    private:
        
                            //! Constructs a function instance.
                            Function(const Scope* scope, const Identifier& identifier, const Type* type, BuiltInType constructor = TypeUserDefined);
        
        //! Adds a function argument.
        void                addArgument(Variable* argument);
        
        //! Sets a function body.
        void                setBody(Statement* value);
        
        //! Sets a function semantic.
        void                setSemantic(SemanticType value);
        
    private:
        
        const Type*         m_type;         //!< Return type of a function.
        Arguments           m_arguments;    //!< Function arguments.
        SemanticType        m_semantic;     //!< An output function semantic.
        Statement*          m_body;         //!< A function body instance.
        Scope               m_declarations; //!< Variable declaration scope.
        BuiltInType         m_constructor;  //!< Valid only for constructor functions.
        ShaderType          m_shader;       //!< A shader function type.
    };
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_Ast_Declarations_H__    */
