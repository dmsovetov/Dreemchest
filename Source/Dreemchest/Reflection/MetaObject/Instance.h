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

        //! Performs an up-cast of this instance.
        template<typename TClass>
        const TClass*           upCast( void ) const;

        //! Returns an instance pointer if it matches the specified class.
        template<typename TClass>
        const TClass*           pointer( void ) const;

        //! Returns meta-class instance pointer.
        const Class*            type( void ) const;

    protected:

        const Class*            m_class;    //!< Meta-class instance.
        const void*             m_pointer;  //!< Object instance pointer.
    };

    // ** InstanceConst::upCast
    template<typename TClass>
    const TClass* InstanceConst::upCast( void ) const
    {
        DC_ABORT_IF( m_pointer == NULL, "invalid instance" );
        DC_ABORT_IF( m_class == NULL, "invalid instance" );

        // Start from a current class and instance pointer.
        const Class*  current  = m_class;
        InstanceConst instance = *this;

        // Keep going up until the target class encountered
        const Class* target   = TClass::staticMetaObject();
        
        // Move up until the top of a hill reached
        while( instance ) {
            // Target class reached - return current instance
            if( target == current ) {
                return instance.pointer<TClass>();
            }

            // Cast to a base class
            instance = current->upCast( instance );
            current  = instance.type();
        }

        return NULL;
    }

    // ** InstanceConst::type
    NIMBLE_INLINE const Class* InstanceConst::type( void ) const
    {
        return m_class;
    }

    // ** InstanceConst::pointer
    template<typename TClass>
    NIMBLE_INLINE const TClass* InstanceConst::pointer( void ) const
    {
        DC_ABORT_IF( m_class != TClass::staticMetaObject(), "unexpected instance type" );
        return reinterpret_cast<const TClass*>( m_pointer );
    }

    //! The Instance class contains a pointer to an instance along with an associated meta-object.
    class Instance : public InstanceConst {
    public:

                                //! Constructs Instance instance.
                                Instance( void );

                                //! Constructs Instance instance.
                                Instance( const Class* cls, void* pointer );

        //! Performs an up-cast of this instance.
        template<typename TClass>
        TClass*                 upCast( void ) const;

        //! Returns an instance pointer if it matches the specified class.
        template<typename TClass>
        TClass*                 pointer( void ) const;
    };

    // ** Instance::upCast
    template<typename TClass>
    NIMBLE_INLINE TClass* Instance::upCast( void ) const
    {
        return const_cast<TClass*>( InstanceConst::upCast<TClass>() );
    }

    // ** Instance::pointer
    template<typename TClass>
    NIMBLE_INLINE TClass* Instance::pointer( void ) const
    {
        return const_cast<TClass*>( InstanceConst::pointer<TClass>() );
    }

    namespace Private {
    
        //! Performs an type casting between types.
        template<typename TFrom, typename TTo>
        InstanceConst typeCast( const InstanceConst& instance )
        {
            return InstanceConst( TTo::staticMetaObject(), static_cast<const TTo*>( instance.pointer<TFrom>() ) );
        }

    } // namespace Private

} // namespace Reflection

DC_END_DREEMCHEST

#endif    /*    !__DC_Reflection_Instance_H__    */