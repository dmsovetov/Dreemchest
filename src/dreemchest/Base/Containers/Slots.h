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

#ifndef __Base_Containers_Slots_H__
#define __Base_Containers_Slots_H__

DC_BEGIN_DREEMCHEST

    //! Opaque handle type, stores array index & generation.
    template<s32 TBitsIndex = 12, s32 TBitsGeneration = 20>
    class OpaqueHandle {
    public:

                                //! Constructs OpaqueHandle instance.
                                OpaqueHandle( u32 index = ~0, u32 generation = 0 )
                                    : m_index( index ), m_generation( generation ) {}

                                //! Copies an OpaqueHandle instance.
                                OpaqueHandle( const OpaqueHandle& other )
                                    : m_index( other.m_index ), m_generation( other.m_generation ) {}

                                //! Casts this OpaqueHandle to an integer value.
                                operator u32( void ) const { return m_index; }

        //! Returns true if this handle is valid.
        bool                    isValid( void ) const { return m_index != ~0; }

        //! Returns opaque handle generation.
        u32                     generation( void ) const { return m_generation; }

    private:

        u32                     m_index      : TBitsIndex;       //!< Slot index that is referenced by this handle.
        u32                     m_generation : TBitsGeneration;  //!< Generation of a slot at a time when this handle was issued.
    };

    //! Container issues opaque handles to it's slots that act as weak references.
    template<typename TValue, typename THandle>
    class Slots {
    public:

        typedef TValue          Value;  //!< Store the value type.
        typedef THandle         Handle; //!< Store the handle type.

                                //! Constructs Slots instance.
                                Slots( void );

        //! Adds the value to a container and returns it's ID.
        Handle                  add( const Value& value );

        //! Reserves the slot inside a container.
        Handle                  reserve( void );

        //! Removes item from a container with specified handle. Returns true if the removal succeed, otherwise returns false.
        bool                    remove( const Handle& handle );

        //! Returns true if the specified handle is valid.
        bool                    has( const Handle& handle ) const;

        //! Returns the value referenced by specified handle.
        const Value&            get( const Handle& handle ) const;
        Value&                  get( const Handle& handle );

        //! Returns the total number of used slots.
        s32                     size( void ) const;

        //! Returns the maxium capacity.
        s32                     capacity( void ) const;

        //! Returns the total number of free handles.
        s32                     freeCount( void ) const;

    #ifdef DC_DEBUG
        //! Used for debuging, traces list of free slots.
        void                    traceFreeSlots( void ) const;
    #endif  /*  DC_DEBUG    */

    private:

        //! Expands the opaque handle list by count elements.
        void                    expand( s32 count );

        //! Returns next free hande.
        Handle                  allocate( void );

    private:

        Array<Value>            m_data;         //!< Actual slot data.
        Array<Handle>           m_slots;        //!< Array of avaible slots.
        u32                     m_head;         //!< Head of a free list.
        s32                     m_count;        //!< The total number of allocated handles.
        s32                     m_capacity;     //!< The maximum capacity.
    };

    // ** Slots::FixedSlots
    template<typename TValue, typename THandle>
    Slots<TValue, THandle>::Slots( void )
         : m_head( 0 ), m_count( 0 ), m_capacity( 0 )
    {
        // Allocate handle pool.
        //if( size ) {
        //    expand( size );
        //}
    }

    // ** Slots::size
    template<typename TValue, typename THandle>
    s32 Slots<TValue, THandle>::size( void ) const
    {
        return m_count;
    }

    // ** Slots::add
    template<typename TValue, typename THandle>
    THandle Slots<TValue, THandle>::add( const TValue& value )
    {
        Handle handle = reserve();
        m_data[handle] = value;
        return handle;
    }

    // ** Slots::reserve
    template<typename TValue, typename THandle>
    THandle Slots<TValue, THandle>::reserve( void )
    {
        // Maximum capacity reached - expand
        if( freeCount() == 0 ) {
            s32 count = static_cast<s32>( capacity() * 0.25f );
            expand( count <= 0 ? 4 : count );
        }

        // Reserve the slot
        Handle handle = allocate();
        DC_BREAK_IF( !handle.isValid() );

        return handle;
    }

    // ** Slots::has
    template<typename TValue, typename THandle>
    bool Slots<TValue, THandle>::has( const THandle& handle ) const
    {
        if( !handle.isValid() ) {
            return false;
        }

        DC_BREAK_IF( handle >= static_cast<u32>( capacity() ) );
        return handle.generation() == m_slots[handle].generation();
    }

    // ** Slots::get
    template<typename TValue, typename THandle>
    const TValue& Slots<TValue, THandle>::get( const THandle& handle ) const
    {
        DC_BREAK_IF( !has( handle ) );
        return m_data[handle];
    }

    // ** Slots::get
    template<typename TValue, typename THandle>
    TValue& Slots<TValue, THandle>::get( const THandle& handle )
    {
        DC_BREAK_IF( !has( handle ) );
        return m_data[handle];
    }

    // ** Slots::expand
    template<typename TValue, typename THandle>
    void Slots<TValue, THandle>::expand( s32 count )
    {
        // Save previous capacity.
        s32 oldCapacity = capacity();
        m_capacity = oldCapacity + count;

        // Resize the block of slots
        m_slots.resize( capacity() );

        // Resize data
        m_data.resize( capacity() );

        // Add new handles to a free list.
        for( s32 i = oldCapacity; i < oldCapacity + count; i++ ) {
            m_slots[i] = i + 1;
        }
    }

    // ** Slots::allocate
    template<typename TValue, typename THandle>
    THandle Slots<TValue, THandle>::allocate( void )
    {
        // Return invalid handle if no more slots left.
        if( size() >= capacity() ) {
            return 0;
        }

        // Get the first free handle.
        THandle free = m_slots[m_head];

        // Save next free slot index.
        u32 next = free;

        // Fix the index.
        free = THandle( m_head, free.generation() );

        // Remove it from a list.
        m_head = next;

        // Increase the size of a container.
        m_count++;

        return free;
    }

    // ** SlotHandleList::remove
    template<typename TValue, typename THandle>
    bool Slots<TValue, THandle>::remove( const THandle& handle )
    {
        if( !has( handle ) ) {
            return false;
        }

        u32 idx = handle;

        // Place this handle to a free list and increase the generation counter to invalidate all living handles.
        m_slots[idx] = THandle( m_head, handle.generation() + 1 );
        m_head       = idx;

        // Decrease the allocated counter.
        m_count--;

        return true;
    }

    // ** SlotHandleList::freeCount
    template<typename TValue, typename THandle>
    s32 Slots<TValue, THandle>::freeCount( void ) const
    {
        return capacity() - size();
    }

    // ** SlotHandleList::capacity
    template<typename TValue, typename THandle>
    s32 Slots<TValue, THandle>::capacity( void ) const
    {
        return m_capacity;
    }

#ifdef DC_DEBUG
    // ** Slots::traceFreeSlots
    template<typename TValue, typename THandle>
    void Slots<TValue, THandle>::traceFreeSlots( void ) const
    {
        printf( "Free slots: " );

        for( s32 idx = m_head; idx < capacity(); ) {
            printf( "%d ", idx );
            idx = indexFromId( m_handles[idx] );
        }

        printf( "\n" );
    }
#endif  /*  DC_DEBUG    */

DC_END_DREEMCHEST

#endif  /*  !__Base_Containers_Slots_H__    */
