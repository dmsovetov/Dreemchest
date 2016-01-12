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

    //! Forward declaration of a slot list.
    template<typename TSlot> class SlotHandleList;

    //! This handles are issued by slot containers to outer world.
    template<s32 TBitsIndex = 12, s32 TBitsGeneration = 20>
    class SlotHandle {
    friend class SlotHandleList< SlotHandle<TBitsIndex, TBitsGeneration> >;
    public:

                    //! Copies an SlotHandle instance.
                    SlotHandle( const SlotHandle& other )
                        : index( other.index ), generation( other.generation ) {}

                    //! Casts this slot handle to an integer value.
                    operator u32( void ) const { return index; }

                    //! Returns true if this slot is valid.
        bool        isValid( void ) const { return index != -1; }

    private:

                    //! Constructs SlotHandle instance.
                    SlotHandle( u32 index = -1, u32 generation = 0 )
                        : index( index ), generation( generation ) {}

        u32         index       : TBitsIndex;       //!< Slot index that is referenced by this slot handle.
        u32         generation  : TBitsGeneration;  //!< Generation of a slot at a time when this handle was issued.
    };

    //! Linked list of slot handles.
    template<typename THandle>
    class SlotHandleList {
    public:

                        //! Constructs SlotHandleList instance.
                        SlotHandleList( s32 size );
                        ~SlotHandleList( void );

        //! Expands the opaque handle list by count elements.
        void            expand( s32 count );

        //! Returns next free hande.
        THandle         allocate( void );

        //! Frees the previously allocated handle.
        bool            free( const THandle& handle );

        //! Returns true if the specified handle is still valid.
        bool            isAlive( const THandle& handle ) const;

        //! Returns the total number of allocated handles.
        s32             allocatedCount( void ) const;

        //! Returns the maxium capacity.
        s32             capacity( void ) const;

        //! Returns the total number of free handles.
        s32             freeCount( void ) const;

    #ifdef DC_DEBUG
        //! Used for debuging, traces list of free slots.
        void            trace( void ) const;
    #endif  /*  DC_DEBUG    */

    private:

        THandle*        m_handles;  //!< An array of opaque handles.
        u32             m_head;     //!< Head of a free list.
        s32             m_count;    //!< The total number of allocated handles.
        s32             m_capacity; //!< The maximum capacity.
    };

    // ** SlotHandleList::SlotHandleList
    template<typename THandle>
    SlotHandleList<THandle>::SlotHandleList( s32 size ) : m_handles( NULL ), m_head( 0 ), m_count( 0 ), m_capacity( 0 )
    {
        // Allocate handle pool.
        if( size ) {
            expand( size );
        }
    }

    // ** SlotHandleList::~SlotHandleList
    template<typename THandle>
    SlotHandleList<THandle>::~SlotHandleList( void )
    {
        delete[]m_handles;
    }

    // ** SlotHandleList::expand
    template<typename THandle>
    void SlotHandleList<THandle>::expand( s32 count )
    {
        // Save previous capacity.
        s32 oldCapacity = capacity();
        m_capacity = oldCapacity + count;

        // Allocate the block of handles.
        THandle* handles = new THandle[capacity()];

        // Copy old handles.
        if( m_handles ) {
            memcpy( handles, m_handles, sizeof( THandle ) * oldCapacity );
        }

        // Add added handles to a free list.
        for( s32 i = oldCapacity; i < oldCapacity + count; i++ ) {
            handles[i].index = i + 1;
        }

        // Reset handles.
        delete[]m_handles;
        m_handles = handles;
    }

    // ** SlotHandleList::allocate
    template<typename THandle>
    THandle SlotHandleList<THandle>::allocate( void )
    {
        // Return invalid handle if no more slots left.
        if( allocatedCount() >= capacity() ) {
            DC_BREAK;
            return THandle();
        }

        // Get the first free handle.
        THandle handle = m_handles[m_head];

        // Save next free slot index.
        u32 next = handle.index;

        // Fix the index.
        handle.index = m_head;

        // Remove it from a list.
        m_head = next;

        // Increase the size of a container.
        m_count++;

        return handle;
    }

    // ** SlotHandleList::free
    template<typename THandle>
    bool SlotHandleList<THandle>::free( const THandle& handle )
    {
        if( !isAlive( handle ) ) {
            return false;
        }

        u32 idx = handle.index;

        // Increase the generation counter to invalidate all living handles.
        m_handles[idx].generation++;

        // Place this handle to a free list.
        m_handles[idx].index = m_head;
        m_head               = idx;

        // Decrease the allocated counter.
        m_count--;

        return true;
    }

    // ** SlotHandleList::isAlive
    template<typename THandle>
    bool SlotHandleList<THandle>::isAlive( const THandle& handle ) const
    {
        DC_BREAK_IF( handle.index >= static_cast<u32>( capacity() ) );
        return handle.generation == m_handles[handle].generation;
    }

    // ** SlotHandleList::allocatedCount
    template<typename THandle>
    s32 SlotHandleList<THandle>::allocatedCount( void ) const
    {
        return m_count;
    }

    // ** SlotHandleList::freeCount
    template<typename THandle>
    s32 SlotHandleList<THandle>::freeCount( void ) const
    {
        return capacity() - allocatedCount();
    }

    // ** SlotHandleList::capacity
    template<typename THandle>
    s32 SlotHandleList<THandle>::capacity( void ) const
    {
        return m_capacity;
    }

#ifdef DC_DEBUG
    // ** SlotHandleList::trace
    template<typename THandle>
    void SlotHandleList<THandle>::trace( void ) const
    {
        printf( "Free slots: " );

        for( s32 idx = m_head; idx < capacity(); ) {
            printf( "%d ", idx );
            idx = m_handles[idx].index;
        }

        printf( "\n" );
    }
#endif  /*  DC_DEBUG    */

    //! Container issues opaque handles to it's slots that act as weak references.
    template<typename TValue, s32 TBitsIndex = 12, s32 TBitsGeneration = 20>
    class Slots {
    public:

        //! Store the value type.
        typedef TValue  Value;

        //! Alias the SlotHandle type.
        typedef SlotHandle<TBitsIndex, TBitsGeneration> ID;

                                //! Constructs Slots instance of specified size.
                                Slots( s32 size = 0 );

        //! Adds the value to a container and returns it's ID.
        ID                      add( const TValue& value );

        //! Removes item from a container with specified ID. Returns true if the removal succeed, otherwise returns false.
        bool                    remove( const ID& id );

        //! Returns true if the specified ID is valid.
        bool                    has( const ID& id ) const;

        //! Returns the value referenced by specified ID.
        const TValue&           get( const ID& id ) const;
        TValue&                 get( const ID& id );

        //! Returns the total number of used slots.
        s32                     size( void ) const;

    #ifdef DC_DEBUG
        //! Used for debuging, traces list of free slots.
        void                    traceFreeSlots( void ) const { m_handles.trace(); }
    #endif  /*  DC_DEBUG    */

    protected:

        Array<TValue>           m_data;         //!< Actual slot data.
        SlotHandleList<ID>      m_handles;      //!< Array of avaible slot handles.
    };

    // ** Slots::FixedSlots
    template<typename TValue, s32 TBitsIndex, s32 TBitsGeneration>
    Slots<TValue, TBitsIndex, TBitsGeneration>::Slots( s32 size ) : m_handles( size )
    {
        m_data.resize( size );
    }

    // ** Slots::size
    template<typename TValue, s32 TBitsIndex, s32 TBitsGeneration>
    s32 Slots<TValue, TBitsIndex, TBitsGeneration>::size( void ) const
    {
        return m_handles.allocatedCount();
    }

    // ** Slots::add
    template<typename TValue, s32 TBitsIndex, s32 TBitsGeneration>
    typename Slots<TValue, TBitsIndex, TBitsGeneration>::ID Slots<TValue, TBitsIndex, TBitsGeneration>::add( const TValue& value )
    {
        // Maximum capacity reached - expand
        if( m_handles.freeCount() == 0 ) {
            s32 count = static_cast<s32>( m_handles.capacity() * 0.25f );
            m_handles.expand( count <= 0 ? 1 : count );
            m_data.resize( m_handles.capacity() );
        }

        // Reserve the slot
        ID id = m_handles.allocate();

        // Copy value to a slot
        if( id.isValid() ) {
            m_data[id] = value;
        }

        return id;
    }

    // ** Slots::remove
    template<typename TValue, s32 TBitsIndex, s32 TBitsGeneration>
    bool Slots<TValue, TBitsIndex, TBitsGeneration>::remove( const ID& id )
    {
        return m_handles.free( id );
    }

    // ** Slots::has
    template<typename TValue, s32 TBitsIndex, s32 TBitsGeneration>
    bool Slots<TValue, TBitsIndex, TBitsGeneration>::has( const ID& id ) const
    {
        if( !id.isValid() ) {
            return false;
        }

        return m_handles.isAlive( id );
    }

    // ** Slots::get
    template<typename TValue, s32 TBitsIndex, s32 TBitsGeneration>
    const TValue& Slots<TValue, TBitsIndex, TBitsGeneration>::get( const ID& id ) const
    {
        DC_BREAK_IF( !has( id ) );
        return m_data[id];
    }

    // ** Slots::get
    template<typename TValue, s32 TBitsIndex, s32 TBitsGeneration>
    TValue& Slots<TValue, TBitsIndex, TBitsGeneration>::get( const ID& id )
    {
        DC_BREAK_IF( !has( id ) );
        return m_data[id];
    }

DC_END_DREEMCHEST

#endif  /*  !__Base_Containers_Slots_H__    */
