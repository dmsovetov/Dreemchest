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

#include "UnitTests.h"

DC_USE_DREEMCHEST

using namespace io;

char data[] = "Hello world!";

TEST(ByteBuffer, SizeAfterCreation)
{
    ByteBufferPtr buffer = ByteBuffer::create();
    EXPECT_EQ( 0, buffer->length() );
}

TEST(ByteBuffer, PositionAfterCreation)
{
    ByteBufferPtr buffer = ByteBuffer::create();
    EXPECT_EQ( 0, buffer->position() );
}

TEST(ByteBuffer, SizeAfterInitilization)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    EXPECT_EQ( strlen( data ), buffer->length() );
}

TEST(ByteBuffer, PiositionAfterInitilization)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    EXPECT_EQ( 0, buffer->position() );
}

TEST(ByteBuffer, DataAfterInitialization)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    for( int i = 0; i < buffer->length(); i++ ) {
        EXPECT_EQ( data[i], buffer->buffer()[i] );
    }
}

TEST(ByteBuffer, SeekSet)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    for( int i = 0; i < buffer->length(); i++ ) {
        buffer->setPosition( i, SeekSet );
        EXPECT_EQ( i, buffer->position() );
    }
}

TEST(ByteBuffer, SeekCur)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    for( int i = 0; i < buffer->length(); i++ ) {
        buffer->setPosition( 1, SeekCur );
        EXPECT_EQ( i + 1, buffer->position() );
    }
}

TEST(ByteBuffer, SeekEnd)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    for( int i = 0; i < buffer->length(); i++ ) {
        buffer->setPosition( i, SeekEnd );
        EXPECT_EQ( buffer->length() - i, buffer->position() );
    }
}

TEST(ByteBuffer, DataAfterSeek)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    for( int i = 0; i < buffer->length(); i++ ) {
        buffer->setPosition( i );
        EXPECT_EQ( data[i], buffer->current()[0] );
    }
}


TEST(ByteBuffer, Fill)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    buffer->fill( 1 );

    for( int i = 0; i < buffer->length(); i++ ) {
        EXPECT_EQ( 1, buffer->buffer()[i] );
    }
}

TEST(ByteBuffer, TrimLeft)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    buffer->trimFromLeft( 2 );
    ASSERT_EQ( strlen( data ) - 2, buffer->length() );

    for( int i = 0; i < buffer->length(); i++ ) {
        ASSERT_EQ( data[i + 2], buffer->current()[i] );
    }
}

TEST(ByteBuffer, TrimRight)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    buffer->trimFromRight( 2 );
    ASSERT_EQ( strlen( data ) - 2, buffer->length() );

    for( int i = 0; i < buffer->length(); i++ ) {
        ASSERT_EQ( data[i], buffer->current()[i] );
    }
}

TEST(ByteBuffer, Read)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    char in[20] = { 0 };
    buffer->read( in, strlen( data ) );

    ASSERT_STREQ( data, in );
}

TEST(ByteBuffer, PositionAfterRead)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    char in[20] = { 0 };
    buffer->read( in, strlen( data ) );

    ASSERT_EQ( strlen( data ), buffer->position() );
}

TEST(ByteBuffer, SequentialRead)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    char c;

    for( int i = 0; i < buffer->length(); i++ ) {
        buffer->read( &c, 1 );
        ASSERT_EQ( i + 1, buffer->position() );
        ASSERT_EQ( data[i], c );
    }
}

TEST(ByteBuffer, Write)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    char* text = "written";

    buffer->write( text, strlen( text ) );
    for( int i = 0; i < strlen( text ); i++ ) {
         ASSERT_EQ( text[i], buffer->buffer()[i] );
    }
}

TEST(ByteBuffer, PositionAfterWrite)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    char* text = "written";

    buffer->write( text, strlen( text ) );
    ASSERT_EQ( strlen( text ), buffer->position() );
}

TEST(ByteBuffer, SequentialWrite)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    char c = 'x';

    for( int i = 0; i < buffer->length(); i++ ) {
        buffer->write( &c, 1 );
        ASSERT_EQ( i + 1, buffer->position() );
    }

    for( int i = 0; i < buffer->length(); i++ ) {
        ASSERT_EQ( c, buffer->buffer()[i] );
    }
}

TEST(ByteBuffer, BytesAbvailableAfterInit)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );
    EXPECT_EQ( strlen( data ), buffer->bytesAvailable() );
    EXPECT_EQ( buffer->length(), buffer->bytesAvailable() );
}

TEST(ByteBuffer, BytesAbvailableAfterRead)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    char c;

    for( int i = 0; i < buffer->length(); i++ ) {
        ASSERT_EQ( buffer->length() - i, buffer->bytesAvailable() );
        buffer->read( &c, 1 );
        ASSERT_EQ( buffer->length() - (i + 1), buffer->bytesAvailable() );
    }
}

TEST(ByteBuffer, WriteExpandsTheBuffer)
{
    ByteBufferPtr buffer = ByteBuffer::createFromData( ( u8* )data, strlen( data ) );
    ASSERT_EQ( strlen( data ), buffer->length() );

    buffer->setPosition( 0, SeekEnd );
    ASSERT_EQ( buffer->length(), buffer->position() );

    char* msg     = "message";
    int   before = buffer->length();

    buffer->write( msg, strlen( msg ) );
    ASSERT_EQ( buffer->length(), before + strlen( msg ) );
}