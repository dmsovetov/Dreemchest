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

#include "GLShaderGenerator.h"
#include "Renderer/Cg/Ast/Ast.h"
#include "Renderer/Cg/Ast/Declarations.h"
#include "Renderer/Cg/Ast/Expressions.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ** GLShaderGenerator::visit
void GLShaderGenerator::visit(Cg::Function& node)
{
    writeType(node.type());
}

// ** GLShaderGenerator::visit
void GLShaderGenerator::visit(Cg::Variable& node)
{
    const StringView& name = node.name();

    writeType(*node.type());
    printf(" ");
    printf("%s", name.str().c_str());

    if (Cg::Expression* initializer = node.initializer())
    {
        printf(" = ");
        initializer->accept(*this);
    }

    printf(";\n");
}

// ** GLShaderGenerator::writeType
void GLShaderGenerator::writeType(const Cg::Type& type)
{
    printf("%s", type.name().str().c_str());
}

} // namespace Renderer

DC_END_DREEMCHEST