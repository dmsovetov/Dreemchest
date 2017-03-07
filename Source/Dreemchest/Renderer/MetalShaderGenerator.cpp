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

#include "MetalShaderGenerator.h"
#include "Renderer/Cg/Ast/Ast.h"
#include "Renderer/Cg/Ast/Declarations.h"
#include "Renderer/Cg/Ast/Expressions.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

using namespace Cg;

//! Maps from a built-in type to a Metal type name.
const s8* metalTypeName(BuiltInType type)
{
	switch (type)
	{
	case TypeVoid:
		return "void";
	case TypeBool:
		return "bool";
	case TypeInt:
		return "int";
	case TypeFloat:
		return "float";
	case TypeFloat2:
		return "float2";
	case TypeFloat3:
		return "float3";
	case TypeFloat4:
		return "float4";
	case TypeHalf:
		return "half";
	case TypeHalf2:
		return "half2";
	case TypeHalf3:
		return "half3";
	case TypeHalf4:
		return "half4";
	case TypeFixed:
		return "char";
	case TypeFixed2:
		return "char2";
	case TypeFixed3:
		return "char3";
	case TypeFixed4:
		return "char4";
	case TypeFloat4x4:
		return "float4x4";
	case TypeHalf4x4:
		return "half4x4";
	case TypeFixed4x4:
		return "half4x4";
	}

	return NULL;
}

//! Maps from an input semantic type to a vertex attribute binding
const s8* metalAttribute(SemanticType semantic)
{
	switch (semantic)
	{
	case POSITION:
		return "attribute(0)";
	case COLOR:
		return "attribute(1)";
	case NORMAL:
		return "attribute(2)";
	case TEXCOORD0:
		return "attribute(3)";
	case TEXCOORD1:
		return "attribute(4)";
	case TEXCOORD2:
		return "attribute(5)";
	case TEXCOORD3:
		return "attribute(6)";
	case TEXCOORD4:
		return "attribute(7)";
	case TEXCOORD5:
		return "attribute(8)";
	case TEXCOORD6:
		return "attribute(9)";
	case TEXCOORD7:
		return "attribute(10)";
	}

	return NULL;
}

//! Maps from an output semantic to a Metal binding name.
const s8* metalSemantic(SemanticType semantic)
{
	switch (semantic)
	{
	case POSITION:
		return "position";
	}

	return NULL;
}

// ----------------------------------------------------- MTLArgumentGenerator -------------------------------------------------- //

//! A Cg visitor that generates a Metal shader input argument.
class MTLArgumentGenerator : public CodeGenerator
{
public:

								//! Constructs an argument generator instance.
								MTLArgumentGenerator(SemanticMapping inputSemantic, SemanticMapping outputSemantic, FixedStringBuffer& output);

private:

	//! Generates a shader function argument.
	virtual void				visit(Variable& node);
};

// ** MTLArgumentGenerator::MTLArgumentGenerator
MTLArgumentGenerator::MTLArgumentGenerator(SemanticMapping inputSemantic, SemanticMapping outputSemantic, FixedStringBuffer& output)
	: CodeGenerator(output)
{
	setTypeMapping(metalTypeName);
	setInputMapping(inputSemantic);
	setOutputMapping(outputSemantic);
}

// ** MTLArgumentGenerator::visit
void MTLArgumentGenerator::visit(Variable& node)
{
	const Type& type = node.type();

	if (type.structure())
	{
		output() << type.name() << " " << node.name() << " [[stage_in]]";
	}
	else
	{
		output() << builtInType(type.builtInType()) << " " << node.name() << " [[" << inputSemantic(node.semantic()) << "]]";
	}
}

// ----------------------------------------------------- MTLFunctionGenerator -------------------------------------------------- //

//! A Cg visitor that generates a generic Metal function.
class MTLFunctionGenerator : public CodeGenerator
{
public:

								//! Constructs a MTLFunctionGenerator instance.
								MTLFunctionGenerator(SemanticMapping inputSemantic, SemanticMapping outputSemantic, FixedStringBuffer& output);

protected:

	//! Generates a shader function declaration from an AST node.
	virtual void                visit(Function& node) NIMBLE_OVERRIDE;

	//! Generates a variable declaration.
	virtual void                visit(Variable& node) NIMBLE_OVERRIDE;

	//! Writes a type name to an output stream.
	FixedStringBuffer&          writeType(const Type& type);

	//! Writes a variable to an output.
	void                        writeVariable(const Variable& variable);

	//! Sets an argument generator.
	void						setArgumentGenerator(UPtr<Visitor> value);

private:

	UPtr<Visitor>				m_argument;	//!< A function argument visitor.
};

// ** MTLFunctionGenerator::MTLFunctionGenerator
MTLFunctionGenerator::MTLFunctionGenerator(SemanticMapping inputSemantic, SemanticMapping outputSemantic, FixedStringBuffer& output)
	: CodeGenerator(output)
{
	setExpression(DC_NEW ExpressionGenerator(metalTypeName, output));
	setArgumentGenerator(DC_NEW MTLArgumentGenerator(inputSemantic, outputSemantic, output));
	setTypeMapping(metalTypeName);
	setInputMapping(inputSemantic);
	setOutputMapping(outputSemantic);
}

// ** MTLFunctionGenerator::setArgumentGenerator
void MTLFunctionGenerator::setArgumentGenerator(UPtr<Visitor> value)
{
	m_argument = value;
}

// ** MTLFunctionGenerator::visit
void MTLFunctionGenerator::visit(Function& node)
{
	writeType(node.type());
	output() << " " << node.name() << "(";

	Function::Arguments& arguments = node.arguments();
	size_t argumentsLeftToProcess = arguments.size();

	for (Function::Arguments::iterator i = arguments.begin(), end = arguments.end(); i != end; ++i)
	{
		if (Visitor* argument = m_argument.get())
		{
			(*i)->accept(*argument);
		}
		else
		{
			writeVariable(**i);
		}
		argumentsLeftToProcess--;

		if (argumentsLeftToProcess)
		{
			output() << ", ";
		}
	}

	output() << ")";

	if (node.semantic())
	{

	}

	startCurlyBraces(true);
	ProgramVisitor::visit(node);
	endCurlyBraces();
}

// ** MTLFunctionGenerator::visit
void MTLFunctionGenerator::visit(Variable& node)
{
	const StringView& name = node.name();

	writeVariable(node);

	if (Expression* initializer = node.initializer())
	{
		output() << " = ";
		initializer->accept(*this);
	}

	output() << ";" << endl();
}

// ** MTLFunctionGenerator::writeType
FixedStringBuffer& MTLFunctionGenerator::writeType(const Type& type)
{
	if (const s8* builtInName = builtInType(type.builtInType()))
	{
		output() << builtInName;
	}
	else
	{
		output() << type.name();
	}

	return output();
}

// ** MTLFunctionGenerator::writeVariable
void MTLFunctionGenerator::writeVariable(const Variable& variable)
{
	output() << ident();
	output() << writeType(variable.type()) << " " << variable.name();
}

// -------------------------------------------------- MTLVertexFunctionGenerator ----------------------------------------------- //

//! A Cg visitor that generates Metal vertex shader function.
class MTLVertexFunctionGenerator : public MTLFunctionGenerator
{
public:

								//! Constructs a MTLVertexFunctionGenerator instance.
								MTLVertexFunctionGenerator(FixedStringBuffer& output);

protected:

	//! Generates a shader function declaration from an AST node.
	virtual void                visit(Cg::Function& node) NIMBLE_OVERRIDE;
};

// ** MTLVertexFunctionGenerator::MTLVertexFunctionGenerator
MTLVertexFunctionGenerator::MTLVertexFunctionGenerator(FixedStringBuffer& output)
	: MTLFunctionGenerator(metalAttribute, NULL, output)
{
}

// ** MTLVertexFunctionGenerator::visit
void MTLVertexFunctionGenerator::visit(Function& node)
{
	output() << "vertex ";
	MTLFunctionGenerator::visit(node);
}

// -------------------------------------------------- MTLFragmentFunctionGenerator ----------------------------------------------- //

//! A Cg visitor that generates Metal fragment shader function.
class MTLFragmentFunctionGenerator : public MTLFunctionGenerator
{
public:

								//! Constructs a MTLFragmentFunctionGenerator instance.
								MTLFragmentFunctionGenerator(FixedStringBuffer& output);

protected:

	//! Generates a shader function declaration from an AST node.
	virtual void                visit(Cg::Function& node) NIMBLE_OVERRIDE;
};

// ** MTLFragmentFunctionGenerator::MTLFragmentFunctionGenerator
MTLFragmentFunctionGenerator::MTLFragmentFunctionGenerator(FixedStringBuffer& output)
	: MTLFunctionGenerator(NULL, NULL, output)
{
}

// ** MTLFragmentFunctionGenerator::visit
void MTLFragmentFunctionGenerator::visit(Function& node)
{
	output() << "fragment ";
	MTLFunctionGenerator::visit(node);
}

// ----------------------------------------------------- MTLShaderGenerator ---------------------------------------------------- //

// ** MTLShaderGenerator::MTLShaderGenerator
MTLShaderGenerator::MTLShaderGenerator(FixedStringBuffer& output)
	: CodeGenerator(output)
{
	setTypeMapping(metalTypeName);
	setInputMapping(metalAttribute);
	setOutputMapping(metalSemantic);
	setExpression(DC_NEW ExpressionGenerator(metalTypeName, output));
	setShaderFunction(Cg::VertexShader, DC_NEW MTLVertexFunctionGenerator(output));
	setShaderFunction(Cg::FragmentShader, DC_NEW MTLFragmentFunctionGenerator(output));
}

// ** MTLShaderGenerator::visit
void MTLShaderGenerator::visit(Structure& node)
{
	Structure::Fields& fields = node.fields();

	output() << ident() << "struct " << node.name();

	startCurlyBraces(true);
	for (Structure::Fields::iterator i = fields.begin(), end = fields.end(); i != end; ++i)
	{
		visit(**i);
	}
	endCurlyBraces(true);
}

// ** MTLShaderGenerator::visit
void MTLShaderGenerator::visit(Variable& node)
{
	const StringView& name = node.name();

	writeVariable(node);

	// Write variable semantic
	SemanticType semantic = node.semantic();

	if (semantic)
	{
		output() << " [[ " << inputSemantic(semantic) << " ]]";
	}

	// Write variable initializer
	if (Expression* initializer = node.initializer())
	{
		output() << " = ";
		initializer->accept(*this);
	}

	output() << ";" << endl();
}

// ** MTLShaderGenerator::writeType
FixedStringBuffer& MTLShaderGenerator::writeType(const Type& type)
{
	if (const s8* builtInName = builtInType(type.builtInType()))
	{
		output() << builtInName;
	}
	else
	{
		output() << type.name();
	}

	return output();
}

// ** MTLShaderGenerator::writeVariable
void MTLShaderGenerator::writeVariable(const Variable& variable)
{
	output() << ident();
	output() << writeType(variable.type()) << " " << variable.name();
}

} // namespace Renderer

DC_END_DREEMCHEST
