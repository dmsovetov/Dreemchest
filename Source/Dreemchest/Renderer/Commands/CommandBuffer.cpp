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

#include "CommandBuffer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{

// ** CommandBuffer::CommandBuffer
CommandBuffer::CommandBuffer( void )
{
}
    
// ** CommandBuffer::reset
void CommandBuffer::reset( void )
{
    m_commands.clear();
    
    for (List<u8*>::const_iterator i = m_data.begin(), end = m_data.end(); i != end; ++i)
    {
        delete[]*i;
    }
    m_data.clear();
}

// ** CommandBuffer::execute
void CommandBuffer::execute( const CommandBuffer& commands )
{
    OpCode opCode;
    opCode.type = OpCode::Execute;
    opCode.sorting = 0;
    opCode.execute.commands = &commands;
    push( opCode );
}

// ** CommandBuffer::uploadConstantBuffer
void CommandBuffer::uploadConstantBuffer(ConstantBuffer_ id, const void* data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::UploadConstantBuffer;
    opCode.upload.id = id;
    opCode.upload.buffer = adoptDataBuffer(data, size);
    push( opCode );
}

// ** CommandBuffer::uploadConstantBuffer
void CommandBuffer::uploadConstantBuffer(ConstantBuffer_ id, const PersistentPointer& data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::UploadConstantBuffer;
    opCode.upload.id = id;
    opCode.upload.buffer.data = reinterpret_cast<const u8*>(data.value);
    opCode.upload.buffer.size = size;
    push( opCode );
}

// ** CommandBuffer::uploadVertexBuffer
void CommandBuffer::uploadVertexBuffer(VertexBuffer_ id, const void* data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::UploadVertexBuffer;
    opCode.upload.id = id;
    opCode.upload.buffer = adoptDataBuffer(data, size);
    push( opCode );
}

// ** CommandBuffer::uploadVertexBuffer
void CommandBuffer::uploadVertexBuffer(VertexBuffer_ id, const PersistentPointer& data, s32 size)
{
    OpCode opCode;
    opCode.type = OpCode::UploadVertexBuffer;
    opCode.upload.id = id;
    opCode.upload.buffer.data = reinterpret_cast<const u8*>(data.value);
    opCode.upload.buffer.size = size;
    push( opCode );
}
 
// ** CommandBuffer::adoptDataBuffer
OpCode::Buffer CommandBuffer::adoptDataBuffer(const void* data, s32 size)
{
    OpCode::Buffer buffer;
    
    if (data == NULL)
    {
        buffer.data = NULL;
        buffer.size = size;
        return buffer;
    }
    
    u8* adopted = new u8[size];
    memcpy(adopted, data, size);
    
    buffer.data    = adopted;
    buffer.size    = size;
    m_data.push_back(adopted);
    
    return buffer;
}

} // namespace Renderer

DC_END_DREEMCHEST
