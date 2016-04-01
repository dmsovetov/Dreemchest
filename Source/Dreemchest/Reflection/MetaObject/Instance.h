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

#ifndef __DC_Reflection_Instance_H__
#define __DC_Reflection_Instance_H__

#include "Class.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

    //! The InstanceConst class contains a const pointer to an instance along with an associated meta-object.
    class InstanceConst {
    public:

                                //! Constructs InstanceConst instance.
                                InstanceConst( void );

                                //! Constructs InstanceConst instance.
                                InstanceConst( const Class* cls, const void* pointer );

                                //! Constructs Instance instance from Instance.
                                InstanceConst( const Instance& instance );

                                //! Returns true if this meta-instance is valid.
                                operator bool( void ) const;

        //! Performs a down-cast of this instance.
        template<typename TClass>
        const TClass*           downCast( void ) const;

        //! Performs an up-cast of this instance.
        template<typename TClass>
        const TClass*           upCast( void ) const;

        //! Returns an instance pointer.
        const void*             pointer( void ) const;

        //! Returns meta-class instance pointer.
        const Class*            type( void ) const;

    protected:

        const Class*            m_class;    //!< Meta-class instance.
        const void*             m_pointer;  //!< Object instance pointer.
    };

    // ** InstanceConst::downCast
    template<typename TClass>
    const TClass* InstanceConst::downCast( void ) const
    {
        return NULL;
    }

    // ** InstanceConst::upCast
    template<typename TClass>
    const TClass* InstanceConst::upCast( void ) const
    {
        return NULL;
    }

    // ** InstanceConst::pointer
    NIMBLE_INLINE const void* InstanceConst::pointer( void ) const
    {
        return m_pointer;
    }

    // ** InstanceConst::type
    NIMBLE_INLINE const Class* InstanceConst::type( void ) const
    {
        return m_class;
    }

    //! The Instance class contains a pointer to an instance along with an associated meta-object.
    class Instance : public InstanceConst {
    public:

                                //! Constructs Instance instance.
                                Instance( void );

                                //! Constructs Instance instance.
                                Instance( const Class* cls, const void* pointer );

        //! Performs a down-cast of this instance.
        template<typename TClass>
        TClass*                 downCast( void ) const;

        //! Performs an up-cast of this instance.
        template<typename TClass>
        TClass*                 upCast( void ) const;

        //! Returns an instance pointer.
        void*                   pointer( void ) const;
    };

    // ** Instance::downCast
    template<typename TClass>
    NIMBLE_INLINE TClass* Instance::downCast( void ) const
    {
        return const_cast<TClass*>( InstanceConst::downCast<TClass>() );
    }

    // ** Instance::upCast
    template<typename TClass>
    NIMBLE_INLINE TClass* Instance::upCast( void ) const
    {
        return const_cast<TClass*>( InstanceConst::upCast<TClass>() );
    }

    // ** Instance::pointer
    NIMBLE_INLINE void* Instance::pointer( void ) const
    {
        return const_cast<void*>( m_pointer );
    }

} // namespace Reflection

DC_END_DREEMCHEST

#endif    /*    !__DC_Reflection_Instance_H__    */