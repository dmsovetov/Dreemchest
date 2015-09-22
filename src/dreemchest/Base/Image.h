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

#ifndef __Base_Image_H__
#define __Base_Image_H__

DC_BEGIN_DREEMCHEST

    //! Packs a set of small rectangles to a bigger one.
	template<typename T>
    class RectanglePacker {
    public:

        //! A rectangular area that will be packed.
        struct Rect {
                        //! Constructs Rect instance.
                        Rect( T x, T y, T width, T height )
                            : x( x ), y( y ), width( width ), height( height ) {}

            T			x;      //!< Left side coordinate.
            T			y;      //!< Top side coordinate.
            T			width;  //!< Rectangle width.
            T			height; //!< Rectangle height;

            //! Compares two rectangles by size.
            bool        operator == ( const Rect& other ) const { return width == other.width && height == other.height; }

            //! Compares two rectangles by size.
            bool        operator <= ( const Rect& other ) const { return width <= other.width && height <= other.height; }

            //! Compares two rectangles by size.
            bool        operator >  ( const Rect& other ) const { return width > other.width || height > other.height; }

            //! Compares two rectangles by area.
            static bool compareByArea( const Rect* a, const Rect* b ) { return a->width * a->height > b->width * b->height; }
        };

        //! Container type to store rectangles being packed.
        typedef Array<Rect> Rectangles;

        //! Rectangle sorting predicate function.
        typedef bool (*SortPredicate)( const Rect* a, const Rect* b );

								//! Constructs RectanglePacker instance.
								RectanglePacker( T padding = 0, T margin = 0 )
									: m_padding( padding ), m_margin( 0 ) {}

        //! Adds a rectangle to set.
        /*!
         \param width Rectangle width.
         \param height Rectangle height.
         \return Rectangle index.
        */
        u32                     add( T width, T height );

        //! Packs added rectangles.
        /*!
         \param width Target area width.
         \param height Target area height.
         \return true if all rectangles can be placed to a destination area.
        */
        bool                    place( T width, T height, SortPredicate predicate = NULL );

        //! Returns the total number of rectangles.
        s32                     rectCount( void ) const;

        //! Returns the rectangle by index.
        const Rect&             rect( s32 index ) const;

    private:

		T						m_padding;		//!< The pading in pixels between the rectangles.
		T						m_margin;		//!< The margin from the border of root rect.
        Rectangles              m_rectangles;   //!< Rectangles being packed.
    };

    // ** RectanglePacker::add
	template<typename T>
    u32 RectanglePacker<T>::add( T width, T height )
    {
        m_rectangles.push_back( Rect( 0, 0, width + m_padding, height + m_padding ) );
        return m_rectangles.size() - 1;
    }

    // ** RectanglePacker::rectCount
	template<typename T>
    s32 RectanglePacker<T>::rectCount( void ) const
    {
        return ( s32 )m_rectangles.size();
    }

    // ** RectanglePacker::rect
	template<typename T>
    const typename RectanglePacker<T>::Rect& RectanglePacker<T>::rect( s32 index ) const
    {
        return m_rectangles[index];
    }

    // ** RectanglePacker::place
	template<typename T>
    bool RectanglePacker<T>::place( T width, T height, SortPredicate predicate )
    {
        //! Rectangle packer node.
        struct Node {
									//! Constructs Node instance.
									Node( const RectanglePacker* packer, T x, T y, T width, T height )
										: m_packer( packer ), m_isFree( true ), m_rect( x, y, width, height ) {}

            //! Returns true if this is a leaf node.
            bool					isLeaf( void ) const { return m_left == NULL && m_right == NULL; }

			const RectanglePacker*	m_packer;	//!< Parent rectangle packer.
            bool					m_isFree;   //!< Flag that marks this node as free.
            AutoPtr<Node>			m_left;     //!< Left child node.
            AutoPtr<Node>			m_right;    //!< Right child node.
            Rect					m_rect;     //!< Node rectangle.

            //! Assigns a rectangle to node.
            bool assign( Rect& placed )
            {
                // ** This is not a leaf node - go down the hierarchy.
                if( !isLeaf() ) return m_left->assign( placed ) ? true : m_right->assign( placed );

                // ** The packed rectangle is too big to place it inside this node.
                if( placed > m_rect || !m_isFree ) return false;

                // ** The packed rectangle perfectly fits to this node.
                if( placed == m_rect ) {
                    placed.x = m_rect.x;
                    placed.y = m_rect.y;
                    m_isFree = false;
                    return true;
                }

                // ** Store this rectangle inside the node.
                T dw = m_rect.width  - placed.width;
                T dh = m_rect.height - placed.height;

                if( dw > dh ) {
                    m_left  = new Node( m_packer, m_rect.x,                m_rect.y, placed.width,                m_rect.height );
                    m_right = new Node( m_packer, m_rect.x + placed.width, m_rect.y, m_rect.width - placed.width, m_rect.height );
                } else {
                    m_left  = new Node( m_packer, m_rect.x, m_rect.y,                 m_rect.width, placed.height );
                    m_right = new Node( m_packer, m_rect.x, m_rect.y + placed.height, m_rect.width, m_rect.height - placed.height );
                }

                return assign( placed );
            }
        };

		// ** Build an array of pointer to preserve the rectangle order.
		Array<Rect*> rectangles;

		for( s32 i = 0, n = ( s32 )m_rectangles.size(); i < n; i++ ) {
			rectangles.push_back( &m_rectangles[i] );
		}

        // ** Sort rectangles.
        std::sort( rectangles.begin(), rectangles.end(), predicate ? predicate : Rect::compareByArea );

        // ** Create a root node.
        Node root( this, m_margin, m_margin, width - m_margin, height - m_margin );

        // ** Place them to a destination area.
        for( s32 i = 0, n = ( s32 )rectangles.size(); i < n; i++ ) {
            if( !root.assign( *rectangles[i] ) ) {
                return false;
            }
        }

        return true;
    }

DC_END_DREEMCHEST

#endif	/*	!__Base_Image_H__	*/
