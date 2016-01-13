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

#ifndef __Base_Containers_FixedSlots_H__
#define __Base_Containers_FixedSlots_H__

DC_BEGIN_DREEMCHEST

    //! Slot container with a fixed size. Issues opaque handles that act as weak references to slots.
    template<typename TValue, s32 TSize>
    class FixedSlots {
    public:

        //! Store the container size.
        enum { Capacity = TSize };

        //! Store the value type.
        typedef TValue  Value;

        //! This IDs are issued by a container to outer world.
        class ID {
        friend class FixedSlots<TValue, TSize>;
        public:

                        //! Copies an ID instance.
                        ID( const ID& other )
                            : index( other.index ), generation( other.generation ) {}

                        //! Casts this ID to an integer value.
                        operator u32( void ) const { return index; }

                        //! Returns true if this ID is valid.
            bool        isValid( void ) const { return index != -1; }

        private:

                        //! Constructs ID instance.
                        ID( u32 index = -1, u32 generation = 0 )
                            : index( index ), generation( generation ) {}

            union {
                u32     index;          //!< Slot index that is referenced by this handle.
                u32     next;           //!< This field is used by a free list.
            };
            u32         generation;     //!< Generation of a slot at a time when this handle was issued.
        };

                        //! Constructs FixedSlots instance.
                        FixedSlots( void );

        //! Reserves the first free slot and returns it's ID. Returns invalid reference when maximum capacity is reached.
        ID              reserve( void );

        //! Adds the value to a container and returns it's ID.
        ID              add( const TValue& value );

        //! Removes item from a container with specified ID. Returns true if the removal succeed, otherwise returns false.
        bool            remove( const ID& id );

        //! Returns true if the specified ID is valid.
        bool            has( const ID& id ) const;

        //! Returns the value referenced by specified ID.
        const TValue&   get( const ID& id ) const;
        TValue&         get( const ID& id );

        //! Returns the total number of used slots.
        s32             size( void ) const;

    private:

        TValue          m_data[TSize];      //!< Actual slot data.
        ID              m_slots[TSize];     //!< Array of available slots.
        s32             m_size;             //!< The total number of used slots.
        u32             m_firstFree;        //!< Index of a first free slot.
    };

    // ** FixedSlots::FixedSlots
    template<typename TValue, s32 TSize>
    FixedSlots<TValue, TSize>::FixedSlots( void ) : m_size( 0 ), m_firstFree( 0 )
    {
        // Initialize free list of slots
        for( s32 i = 0; i < TSize; i++ ) {
            m_slots[i].next = i + 1;
        }
    }

    // ** FixedSlots::size
    template<typename TValue, s32 TSize>
    s32 FixedSlots<TValue, TSize>::size( void ) const
    {
        return m_size;
    }

    // ** FixedSlots::reserve
    template<typename TValue, s32 TSize>
    typename FixedSlots<TValue, TSize>::ID FixedSlots<TValue, TSize>::reserve( void )
    {
        // Return invalid ID if no more slots left.
        if( m_size >= TSize ) {
            return ID();
        }

        // Get the first free slot.
        ID id = m_slots[m_firstFree];

        // Save next free slot index.
        u32 next = id.next;

        // Fix the index.
        id.index = m_firstFree;

        // Remove it from a list.
        m_firstFree = next;

        // Increase the size of a container.
        m_size++;

        return id;
    }

    // ** FixedSlots::add
    template<typename TValue, s32 TSize>
    typename FixedSlots<TValue, TSize>::ID FixedSlots<TValue, TSize>::add( const TValue& value )
    {
        // Reserve the slot
        ID id = reserve();

        // Copy value to a slot
        if( id.isValid() ) {
            m_data[id] = value;
        }

        return id;
    }

    // ** FixedSlots::remove
    template<typename TValue, s32 TSize>
    bool FixedSlots<TValue, TSize>::remove( const ID& id )
    {
        if( !has( id ) ) {
            return false;
        }

        u32 idx = id.index;

        // Increase the generation counter to invalidate all living IDs.
        m_slots[idx].generation++;

        // Place this slot to a free list.
        m_slots[idx].next = m_firstFree;
        m_firstFree       = idx;

        // Decrease the size of a container.
        m_size--;

        return true;
    }

    // ** FixedSlots::has
    template<typename TValue, s32 TSize>
    bool FixedSlots<TValue, TSize>::has( const ID& id ) const
    {
        if( !id.isValid() ) {
            return false;
        }

        DC_BREAK_IF( id.index >= TSize );
        return id.generation == m_slots[id].generation;
    }

    // ** FixedSlots::get
    template<typename TValue, s32 TSize>
    const TValue& FixedSlots<TValue, TSize>::get( const ID& id ) const
    {
        DC_BREAK_IF( !has( id ) );
        return m_data[id];
    }

    // ** FixedSlots::get
    template<typename TValue, s32 TSize>
    TValue& FixedSlots<TValue, TSize>::get( const ID& id )
    {
        DC_BREAK_IF( !has( id ) );
        return m_data[id];
    }

DC_END_DREEMCHEST

#endif  /*  !__Base_Containers_Slots_H__    */
