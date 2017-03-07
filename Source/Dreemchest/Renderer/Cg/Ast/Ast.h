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

#ifndef __DC_Renderer_ShaderAst_H__
#define __DC_Renderer_ShaderAst_H__

#include "../Cg.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
namespace Cg
{    
    // A forward declaration of a AST visitor
    class Visitor;

    // Forward declarations that refelect an AST architecture
    class Ast;
        class Program;
        class Type;
        class Identifier;
        class Declaration;
            class Variable;
            class Function;
            class Structure;
        class Statement;
            class StatementBlock;
            class If;
            class For;
            class While;
            class Return;
            class Discard;
            class Expression;
                class Operator;
                class VariableTerm;
                class ConstantTerm;
                class FunctionCall;
                class ObjectInitializer;
    
    //! Base class for all abstract syntax tree nodes.
    class Ast
    {
    public:
        
        virtual             ~Ast();

        //! Returns node line number.
        s32                 line() const;
        
        //! Returns node column number.
        u16                 column() const;

        //! Passes this node to a visitor.
        virtual void        accept(Visitor& visitor) NIMBLE_ABSTRACT;
        
    protected:
        
                            //! Constructs an Ast node instance.
                            Ast(s32 line, u16 column);
        
    private:
        
        s32                 m_line;     //!< A line number of this node within a source code.
        u16                 m_column;   //!< A column number of this node within a source code.
    };
    
    //! An identifier node.
    class Identifier : public Ast
    {
    friend class Parser;
    public:
        
        //! Returns an identifier text.
        const StringView&   value() const;
        
    private:
        
                            //! Constructs an Identifier instance.
                            Identifier(const StringView& value, s32 line, u16 column);

        //! Invokes visitor's method to process this identifier.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
        StringView          m_value; //!< An identifier text.
    };
    
    //! A data type node.
    class Type : public Ast
    {
    friend class Parser;
    public:
        
        //! Returns a type name.
        const StringView&   name() const;
        
        //! Returns a built-in type id.
        BuiltInType         builtInType() const;

        //! Returns a user-defined structure pointer.
        const Structure*    structure() const;
        
    private:
        
                            //! Constructs a built-in Type node instance.
                            Type(const Identifier& name, BuiltInType builtInType, s32 line, u16 column);

                            //! Constructs a user-defined Type node instance.
                            Type(const Identifier& name, const Structure* structure, s32 line, u16 column);

        //! Invokes visitor's method to process this type.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
        const Identifier&   m_name;         //!< A user-defined type name, will be null for built-in types.
        BuiltInType         m_builtInType;  //!< A built-in type id.
        const Structure*    m_structure;    //!< A structure pointer for user-defined types.
    };

    //! Declaration scope contains a parent declaration scope and mapping from a string to declaration.
    class Scope
    {
    public:

                                //!< Constructs a Scope instance.
                                Scope(const Scope* parent = NULL);

        //! Searches for a declaration with a given name inside this scope only.
        const Declaration*      find(const StringView& name) const;

        //! Adds a new declaration.
        void                    add(const Declaration* declaration);

        //! Searches for a declaration with a given name first inside this scope and then in a parent scopes.
        const Declaration*      findInScopeChain(const StringView& name) const;

    private:

        //! A container type to store declarations.
        typedef HashMap<String64, const Declaration*> Declarations;

        const Scope*            m_parent;       //!< Parent scope.
        Declarations            m_declarations; //!< Exposed declarations.
    };
    
    //! Program is a root node of an AST.
    class Program : public Ast
    {
    friend class Parser;
    public:

        //! A container type to store nested declarations.
        typedef List<Declaration*> Declarations;

        //! Returns a list of nested declarations.
        const Declarations&     declarations() const;
        Declarations&           declarations();

        //! Returns a declaration scope.
        const Scope&            scope() const;
        Scope&                  scope();

        //! Returns a shader function.
        const Identifier*       functionForShader(ShaderType shader) const;

        //! Invokes visitor's method to process this program.
        virtual void            accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
                                //! Constructs a Program node instance.
                                Program();
        
        //! Adds a new declaration instance to program.
        void                    addDeclaration(Declaration* declaration);

        //! Sets a shader function name.
        void                    setShaderFunction(ShaderType shader, const Identifier* name);
        
    private:
        
        Declarations            m_declarations;                 //!< An list of declarations.
        const Identifier*       m_shaders[TotalShaderTypes];    //!< A vertex shader function.
        Scope                   m_scope;                        //!< Root declaration scope.
    };
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_ShaderAst_H__    */
