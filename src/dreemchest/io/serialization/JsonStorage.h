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

#ifndef __DC_Io_JsonStorage_H__
#define __DC_Io_JsonStorage_H__

#include "TextStorage.h"

#ifndef HAVE_JSONCPP
    #error HAVE_JSONCPP should be defined when building with JsonStorage
#endif

#include <json/json.h>

DC_BEGIN_DREEMCHEST

namespace io {

    //! A JSON storage interface to use in serialization
    class JsonStorage : public TextStorage {
    public:

                                    JsonStorage( const StreamPtr& stream );

    private:

        //! Writes a number value.
        virtual void                writeNumber( CString key, double value );

        //! Reads a number value.
        virtual void                readNumber( CString key, double& value ) const;

        //! Writes a boolean value.
        virtual void                writeBoolean( CString key, const bool& value );

        //! Reads a boolean value.
        virtual void                readBoolean( CString key, bool& value ) const;

        //! Writes a string value.
        virtual void                writeString( CString key, const String& value );

        //! Reads a string value.
        virtual void                readString( CString key, String& value ) const;


        //! Begins writing of an array.
        virtual void                pushArrayWrite( CString key, u32 size );

        //! End writing of an array.
        virtual void                popArrayWrite( void );

        //! Begins writing of an object.
        virtual void                pushObjectWrite( CString key );

        //! Ends writing of an object.
        virtual void                popObjectWrite( void );

        //! Begins writing of an array item.
        virtual void                pushItemWrite( u32 index );

        //! Ends writing of an array item.
        virtual void                popItemWrite( void );

        //! Begins reading of an array.
        virtual u32                 pushArrayRead( CString key ) const;

        //! Ends reading of an array.
        virtual void                popArrayRead( void ) const;

        //! Begins reading of an object.
        virtual void                pushObjectRead( CString key ) const;

        //! Ends reading of an object.
        virtual void                popObjectRead( void ) const;

        //! Returns a current JSON node (top of the node stack).
        Json::Value*                current( void );

        //! Returns a current JSON node (top of the node stack).
        const Json::Value*          current( void ) const;

    protected:

        //! Returns a writable JSON node
        Json::Value&                writeTo( CString key );

    private:

        //! Stream
        StreamPtr                   m_stream;

        //! Stack of JSON nodes to handle nesting.
        mutable std::stack<Json::Value*>    m_stack;

        //! Array index.
        mutable u32                 m_index;
    };

} // namespace io

DC_END_DREEMCHEST

#endif  /*  !defined(__DC_Io_JsonStorage_H__)   */
