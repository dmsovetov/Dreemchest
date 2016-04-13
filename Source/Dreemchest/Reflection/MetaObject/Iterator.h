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

#ifndef __DC_Reflection_Iterator_H__
#define __DC_Reflection_Iterator_H__

#include "Class.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

    //! Iterators are used to access collection properties.
    class Iterator {
    public:

        virtual                     ~Iterator( void ) {}

        //! Returns a valid pointer if this iterator was issued by a list, array or set.
        virtual const ListIterator* isList( void ) const { return NULL; }
        virtual ListIterator*       isList( void ) { return NULL; }

        //! Returns a valid pointer if this iterator was issued by a map.
        virtual const MapIterator*  isMap( void ) const { return NULL; }
        virtual MapIterator*        isMap( void ) { return NULL; }

        //! Returns true if this iterator is valid.
        virtual bool                isValid( void ) const NIMBLE_ABSTRACT;

        //! Returns a value type.
        virtual const Type*         valueType( void ) const NIMBLE_ABSTRACT;

        //! Returns a current value.
        virtual Variant             value( void ) const NIMBLE_ABSTRACT;

        //! Iterates to a next element and returns true if an end of a collection was not reached.
        virtual bool                next( void ) const NIMBLE_ABSTRACT;

        //! Returns the total number of elements inside a collection.
        virtual s32                 count( void ) const NIMBLE_ABSTRACT;

        //! Removes the current element from a collection.
        virtual void                remove( void ) NIMBLE_ABSTRACT;
    };

    //! List iterators are used to access lists, arrays and sets.
    class ListIterator : public Iterator {
    public:

        //! Returns a valid pointer if this iterator was issued by a list, array or set.
        virtual const ListIterator* isList( void ) const NIMBLE_OVERRIDE { return this; }
        virtual ListIterator*       isList( void ) NIMBLE_OVERRIDE { return this; }

        //! Returns true if this iterator is valid.
        virtual bool                isValid( void ) const NIMBLE_ABSTRACT;

        //! Inserts a new element after a current value.
        virtual void                insertAfter( const Variant& value ) NIMBLE_ABSTRACT;

        //! Inserts a new element before a current value.
        virtual void                insertBefore( const Variant& value ) NIMBLE_ABSTRACT;
    };

    //! Map iterators are used to access dictionaries.
    class MapIterator : public Iterator {
    public:

        //! Returns a valid pointer if this iterator was issued by a map.
        virtual const MapIterator*  isMap( void ) const NIMBLE_OVERRIDE { return this; }
        virtual MapIterator*        isMap( void ) NIMBLE_OVERRIDE { return this; }

        //! Returns true if this iterator is valid.
        virtual bool                isValid( void ) const NIMBLE_ABSTRACT;

        //! Returns an current key.
        virtual Variant             key( void ) const NIMBLE_ABSTRACT;

        //! Returns a key type.
        virtual const Type*         keyType( void ) const NIMBLE_ABSTRACT;

        //! Inserts a new value with a specified key, returns false if a collection contained an element with a specified key.
        virtual bool                insert( const Variant& key, const Variant& value ) NIMBLE_ABSTRACT;
    };

    namespace Private {

        //! Generic list iterator type.
        template<typename TCollection> 
        class GenericListIterator : public ListIterator {
        public:

                                            //! Constructs GenericListIterator instance.
                                            GenericListIterator( TCollection& collection );

            //! Returns true if this iterator is valid.
            virtual bool                    isValid( void ) const NIMBLE_OVERRIDE;

            //! Returns a value type.
            virtual const Type*             valueType( void ) const NIMBLE_OVERRIDE;

            //! Returns a current value.
            virtual Variant                 value( void ) const NIMBLE_OVERRIDE;

            //! Iterates to a next element and returns true if an end of a collection was not reached.
            virtual bool                    next( void ) const NIMBLE_OVERRIDE;

            //! Returns the total number of elements inside a collection.
            virtual s32                     count( void ) const NIMBLE_OVERRIDE;

            //! Removes the current element from a collection.
            virtual void                    remove( void ) NIMBLE_OVERRIDE;

            //! Inserts a new element after a current value.
            virtual void                    insertAfter( const Variant& value ) NIMBLE_OVERRIDE;

            //! Inserts a new element before a current value.
            virtual void                    insertBefore( const Variant& value ) NIMBLE_OVERRIDE;

        private:

            TCollection&                            m_collection;   //!< Collection reference.
            mutable typename TCollection::iterator  m_iterator;     //!< Current iterator value.
        };

        // ** GenericListIterator::GenericListIterator
        template<typename TCollection> 
        GenericListIterator<TCollection>::GenericListIterator( TCollection& collection )
            : m_collection( collection )
            , m_iterator( collection.end() )
        {
        }

        // ** GenericListIterator::isValid
        template<typename TCollection>
        bool GenericListIterator<TCollection>::isValid( void ) const
        {
            return m_iterator != m_collection.end();
        }

        // ** GenericListIterator::valueType
        template<typename TCollection> 
        const Type* GenericListIterator<TCollection>::valueType( void ) const
        {
            return Type::fromClass<typename TCollection::value_type>();
        }

        // ** GenericListIterator::value
        template<typename TCollection> 
        Variant GenericListIterator<TCollection>::value( void ) const
        {
            DC_ABORT_IF( !isValid(), "invalid iterator" );
            return Variant::fromValue( *m_iterator );
        }

        // ** GenericListIterator::next
        template<typename TCollection> 
        bool GenericListIterator<TCollection>::next( void ) const
        {
            if( isValid() ) {
                m_iterator++;
            } else {
                m_iterator = m_collection.begin();
            }

            return isValid();
        }

        // ** GenericListIterator::count
        template<typename TCollection> 
        s32 GenericListIterator<TCollection>::count( void ) const
        {
            return static_cast<s32>( m_collection.size() );
        }

        // ** GenericListIterator::remove
        template<typename TCollection> 
        void GenericListIterator<TCollection>::remove( void )
        {
            DC_ABORT_IF( !isValid(), "invalid iterator" );
            m_iterator = m_collection.erase( m_iterator );
        }

        // ** GenericListIterator::insertAfter
        template<typename TCollection> 
        void GenericListIterator<TCollection>::insertAfter( const Variant& value )
        {
            typename TCollection::iterator i = m_iterator;
            m_iterator = m_collection.insert( i == m_collection.end() ? i : ++i, value.as<typename TCollection::value_type>() );
        }

        // ** GenericListIterator::insertBefore
        template<typename TCollection> 
        void GenericListIterator<TCollection>::insertBefore( const Variant& value )
        {
            DC_ABORT_IF( !isValid(), "invalid iterator" );
            m_iterator = m_collection.insert( m_iterator, value.as<typename TCollection::value_type>() );
        }

        //! Returns a null pointer for a types that are not collections.
        template<typename TCollection>
        struct NullIteratorFactory {
            //! Just returns a null pointer
            static Iterator* create( const TCollection& collection )
            {
                return NULL;
            }
        };

        //! Creates a list iterator instance.
        template<typename TCollection>
        struct ListIteratorFactory {
            //! Just returns a null pointer
            static Iterator* create( const TCollection& collection )
            {
                return DC_NEW GenericListIterator<TCollection>( const_cast<TCollection&>( collection ) );
            }
        };

        //! Creates a map iterator instance.
        template<typename TCollection>
        struct MapIteratorFactory {
            //! Just returns a null pointer
            static Iterator* create( const TCollection& collection )
            {
                DC_NOT_IMPLEMENTED;
                return NULL;
            }
        };

        //! Creates an iterator instance
        template<typename TCollection>
        Iterator* createIterator( const TCollection& collection )
        {
            enum {
                  IsIterable = IsIterable<TCollection>::value && !TypeEquals<TCollection, String>::value
                , IsMap      = IsKeyValueType<TCollection>::value && !IsSet<TCollection>::value
            };

            return NIMBLE_TERNARY( !IsIterable, NullIteratorFactory<TCollection>, NIMBLE_TERNARY( IsMap, MapIteratorFactory<TCollection>, ListIteratorFactory<TCollection> ) )::create( collection );
        }
    
    } // namespace Private

} // namespace Reflection

DC_END_DREEMCHEST

#endif    /*    !__DC_Reflection_Iterator_H__    */