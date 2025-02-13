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

#include "Instance.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

// -------------------------------------------------------- InstanceConst -------------------------------------------------------- //

// ** InstanceConst::InstanceConst
InstanceConst::InstanceConst( void )
    : m_class( NULL )
    , m_pointer( NULL )
{
}

// ** InstanceConst::InstanceConst
InstanceConst::InstanceConst( const Class* cls, const void* pointer )
    : m_class( cls ), m_pointer( pointer )
{
}

// ** InstanceConst::InstanceConst
InstanceConst::InstanceConst( const Instance& instance )
    : m_class( instance.m_class )
    , m_pointer( instance.m_pointer )
{
}

// ** InstanceConst::operator bool
InstanceConst::operator bool( void ) const
{
    return m_pointer != NULL && m_class != NULL;
}

// ------------------------------------------------------------ Instance ---------------------------------------------------------- //

// ** Instance::Instance
Instance::Instance( void )
{
}

// ** Instance::Instance
Instance::Instance( const Class* cls, void* pointer )
    : InstanceConst( cls, pointer )
{
}

} // namespace Reflection

DC_END_DREEMCHEST