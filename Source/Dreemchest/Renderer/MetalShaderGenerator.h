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

#ifndef __DC_Renderer_MetalShaderGenerator_H__
#define __DC_Renderer_MetalShaderGenerator_H__

#include "Cg/Ast/AstVisitor.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

	//! The Cg AST visitor that generates a Metal shader code.
	class MTLShaderGenerator : public Cg::CodeGenerator
	{
	public:

									//! Constructs a MetalShaderGenerator instance.
									MTLShaderGenerator(FixedStringBuffer& output);

	private:

		//! Generates a variable declaration from an AST node.
		virtual void                visit(Cg::Variable& node) NIMBLE_OVERRIDE;

		//! An override for structure generator to skip structures with semantic bindings.
		virtual void                visit(Cg::Structure& node) NIMBLE_OVERRIDE;

		//! Writes a type name to an output stream.
		FixedStringBuffer&          writeType(const Cg::Type& type);

		//! Writes a variable to an output.
		void                        writeVariable(const Cg::Variable& variable);
	};

} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_MetalShaderGenerator_H__    */
