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

#ifndef __DC_Io_IOPath_H__
#define __DC_Io_IOPath_H__

#include "Io.h"

DC_BEGIN_DREEMCHEST

namespace Io {

    //! A helper class to work with file paths.
    class Path {
    public:

                            Path( void );
                            Path( const String& str );
                            Path( CString str );
                            Path( const Path& other );

        //! Sets a path from string.
        const Path&         operator =  ( CString str );

        //! Sets a path from string.
        const Path&         operator =  ( const String& str );

        //! Compares two paths and returns true if they are equal.
        bool                operator == ( const Path& other ) const;

        //! Compares two paths.
        bool                operator < ( const Path& other ) const;

        //! Compares a path object with string.
        bool                operator != ( CString str ) const;

        //! Appends a path component to this path.
        const Path&         operator += ( CString str );

        //! Append a path to this path.
        const Path&         operator += ( const Path& other );

        //! Concatenates a path with string.
        Path                operator +  ( const String& str ) const;

        //! Concatenates a path with string.
        Path                operator +  ( CString str ) const;

        //! Returns true if this path is empty.
        bool                isEmpty( void ) const;

        //! Returns a path's parent folder.
        Path                folder( void ) const;

        //! Returns a last path component.
        CString             last( void ) const;

        //! Returns a C string representation of this path.
        CString             c_str( void ) const;

        //! Returns a string representation of this path.
        const String&       str( void ) const;

        //! Splits a path string to list.
        static StringList   split( CString str );

    private:

        //! Rebuilds an internal path data.
        void                build( void );

    private:

        //! Full path string.
        String              m_str;

        //! Path components.
        StringList          m_items;
    };

} // namespace Io

DC_END_DREEMCHEST

#endif		/*	!__DC_Io_IOPath_H__	*/