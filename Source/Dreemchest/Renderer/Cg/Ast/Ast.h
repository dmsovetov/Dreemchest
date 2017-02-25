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
        
        //! Available AST node types.
        enum NodeType
        {
              ProgramNode       //!< An AST type for a root node.
            , StructureNode     //!< Structure declaration node type.
            , FunctionNode      //!< Function declaration node type.
            , VariableNode      //!< Variable declaration node type.
            , IdentifierNode    //!< Identifier node type.
            , TypeNode          //!< Data type node.
            , StatementsNode    //!< A statement block.
            , IfNode
            , WhileNode
            , ForNode
            , ReturnNode
            , DiscardNode
            , OperatorNode
            , ConstantTermNode
            , VariableTermNode
            , FunctionCallNode
            , ObjectInitializerNode
        };
        
        virtual             ~Ast();
        
        //! Returns a node type.
        NodeType            type() const;
        
        //! Returns node line number.
        s32                 line() const;
        
        //! Returns node column number.
        u16                 column() const;

        //! Passes this node to a visitor.
        virtual void        accept(Visitor& visitor) NIMBLE_ABSTRACT;
        
    protected:
        
                            //! Constructs an Ast node instance.
                            Ast(NodeType type, s32 line, u16 column);
        
    private:
        
        NodeType            m_type;     //!< Actual node type.
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
        
    private:
        
                            //! Constructs a Type node instance.
                            Type(const Identifier* name, BuiltInType builtInType, s32 line, u16 column);

        //! Invokes visitor's method to process this type.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
        const Identifier*   m_name;         //!< A user-defined type name, will be null for built-in types.
        BuiltInType         m_builtInType;  //!< A built-in type id.
    };
    
    //! Program is a root node of an AST.
    class Program : public Ast
    {
    friend class Parser;
    public:
        
        //! Returns a total number of declaration items.
        s32                 declarationCount() const;
        
        //! Returns a declaration at specified index.
        const Declaration*  declaration(s32 index) const;

        //! Invokes visitor's method to process this program.
        virtual void        accept(Visitor& visitor) NIMBLE_OVERRIDE;
        
    private:
        
                            //! Constructs a Program node instance.
                            Program();
        
        //! Adds a new declaration instance to program.
        void                addDeclaration(Declaration* declaration);
        
    private:
        
        Array<Declaration*> m_declarations; //!< An array of declarations.
    };
    
} // namespace Cg
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  __DC_Renderer_ShaderAst_H__    */
