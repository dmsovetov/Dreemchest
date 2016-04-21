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

#ifndef __DC_Ecs_EntitySerializer_H__
#define __DC_Ecs_EntitySerializer_H__

#include "Ecs.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

    //! Performs writing/reading entities to a key-value storage.
    class Serializer : public Reflection::Serializer {
    public:

		//! Function type used by a component converters.
		typedef cClosure<KeyValue(const Variant&)> ComponentConverter;

                                            //! Constructs Serializer instance.
                                            Serializer( EcsWPtr ecs, const Bitset& excluded = Bitset() );

        //! Writes entity instance to a key-value storage.
        virtual bool                        serialize( EntityWPtr entity, KeyValue& ar ) const;

        //! Creates entity and reads it from a key-value storage.
        virtual bool                        deserialize( Reflection::AssemblyWPtr assembly, EntityWPtr entity, const KeyValue& ar );

        //! Writes component instance to a key-value storage.
        void                                serializeComponent( ComponentWPtr component, KeyValue& ar ) const;

        //! Reads component instance from a key-value storage.
        void                                deserializeComponent( ComponentWPtr component, const KeyValue& ar ) const;

		//! Registers a type converter.
		template<typename TComponent>
		void					            registerComponentConverter( const ComponentConverter& callback );

	private:

        //! Returns a component converter.
        ComponentConverter                  findComponentConverter( const String& name ) const;

		//! Callback to access the default value of Entity flags property.
		Variant								defaultEntityFlags( const KeyValue& ar ) const;

        //! Converts a Guid value to an entity reference.
        Variant                             convertGuidToEntity( const Reflection::Class& cls, const Reflection::Property& property, const Variant& value ) const;

        //! Converts an entity reference to a Guid variant value.
        Variant                             convertEntityToGuid( const Reflection::Class& cls, const Reflection::Property& property, const Variant& value ) const;

        // ------------------------------------- workaround for strong pointers

        //! Converts a Guid value to an entity reference.
        Variant                             convertGuidToEntityPtr( const Reflection::Class& cls, const Reflection::Property& property, const Variant& value ) const;

        //! Converts an entity reference to a Guid variant value.
        Variant                             convertEntityPtrToGuid( const Reflection::Class& cls, const Reflection::Property& property, const Variant& value ) const;

    private:

		//! Container type to store component conversions.
		typedef HashMap<String32, ComponentConverter> ComponentConverters;

        EcsWPtr                             m_ecs;                  //!< Parent Ecs instance.
        Bitset                              m_excluded;             //!< List of excluded components.
        ComponentConverters                 m_componentConverters;  //!< Registered component converters.
    };

    // ** Serializer::registerComponentConverter
	template<typename TComponent>
	void Serializer::registerComponentConverter( const ComponentConverter& callback )
    {
        String32 hash( TypeInfo<TComponent>::name() );
        m_componentConverters[hash] = callback;
    }

} // namespace Ecs

DC_END_DREEMCHEST

#endif    /*    !__DC_Ecs_EntitySerializer_H__    */