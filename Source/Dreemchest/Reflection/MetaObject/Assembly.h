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

#ifndef __DC_Reflection_Assembly_H__
#define __DC_Reflection_Assembly_H__

#include "Class.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

    //! Assembly instance contains a list of classes along with factories used for their construction.
    class Assembly : public RefCounted {
    public:

        //! Creates a new class instance.
        Instance            createInstance( const String& name ) const;

        //! Adds new class to this assembly.
        template<typename TClass>
        bool                registerClass( void );

        //! Creates a toplevel assembly instance.
        static AssemblyPtr  create( void );

        //! Creates a nested assembly instance.
        static AssemblyPtr  create( const String& name, AssemblyWPtr parent );

    private:

                            //! Constructs Assembly instance.
                            Assembly( void );

                            //! Constructs Assembly instance.
                            Assembly( const String& name, AssemblyWPtr parent );

    private:

        //! Maps from a class name to an associated meta-object.
        typedef HashMap<String64, const Class*> Classes;

        String              m_name;     //!< Assembly name.
        AssemblyWPtr        m_parent;   //!< Parrent assembly instance.
        Classes             m_classes;  //!< All registered classes reside here.
    };

    // ** Assembly::registerClass
    template<typename TClass>
    bool Assembly::registerClass( void )
    {
        //! Get the static meta-object from a class
        const Class* metaObject = TClass::staticMetaObject();
        DC_ABORT_IF( metaObject == NULL, "the specified class does not declare a meta-object" );

        //! Get the class name
        CString name = metaObject->name();

        // Constructs a hash string from a class name
        String64 hash( name );

        // Ensure that this class was not registered before.
        Classes::iterator i = m_classes.find( hash );

        if( i != m_classes.end() ) {
            DC_BREAK;
            LogWarning( "assembly", "class '%s' was already added to the '%s' assembly\n", name, m_name == "" ? "toplevel" : m_name.c_str() );
            return false;
        }

        // Add this class to container.
        m_classes[hash] = metaObject;

        return true;
    }

} // namespace Reflection

DC_END_DREEMCHEST

#endif    /*    !__DC_Reflection_Assembly_H__    */