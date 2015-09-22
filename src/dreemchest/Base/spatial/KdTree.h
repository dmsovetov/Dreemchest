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

#ifndef __Base_KdTree_H__
#define __Base_KdTree_H__

DC_BEGIN_DREEMCHEST

	//! k-d tree node.
	template<typename T>
	struct KdNode {

								//! Constructs KdNode instance.
								KdNode( u16 dimension = 0 )
									: m_dimension( dimension ), m_splitter( 0.0f ) {}

		//! Returns true if node is a leaf.
		bool					isLeaf( void ) const { return m_left == NULL || m_right == NULL; }

		AutoPtr<KdNode>			m_left;			//!< Left child node.
		AutoPtr<KdNode>			m_right;		//!< Right child node.
		Array<T>				m_items;		//!< Items stored in this node.
		float					m_splitter;		//!< The splitting value.
		u16						m_dimension;	//!< The splitting dimension.
	};

	//! A simple k-d tree balancer that splits along the specified dimension axis.
	template<typename TItem>
	class KdCapacityBalancer {
	public:

							//! Constructs the KdCapacityBalancer instance.
							KdCapacityBalancer( s32 threshold = 5 )
								: m_threshold( threshold ) {}

		//! Splits the node if it has more than a threshold value items inside.
		bool				split( const KdNode<TItem>* node, float& splitter, u16 dimension );

	private:

		s32					m_threshold;	//!< A threshold value to split the node.
	};

	// ** KdCapacityBalancer::insert
	template<typename TItem>
	bool KdCapacityBalancer<TItem>::split( const KdNode<TItem>* root, float& splitter, u16 dimension )
	{
		s32 count = root->m_items.size();

		if( count <= m_threshold ) {
			return false;
		}

		float mean = 0.0f;

		for( s32 i = 0, n = count; i < n; i++ ) {
			mean += RemovePointer<TItem>::cast( root->m_items[i] )[dimension];
		}

		splitter = mean / count;

		return true;
	}

	//! Template k-d tree implementation.
	/*!
	 k-d tree is a space-partitioning data structure for organizing points in a k-dimensional space. k-d trees are a useful
	 data structure for several applications, such as searches involving a multidimensional search key (e.g. range searches and nearest
	 neighbor searches). k-d trees are a special case of binary space partitioning trees.
	*/
	template< typename TItem, u16 K, typename TBalancer = KdCapacityBalancer<TItem> >
	class KdTree {
	public:

		//! Alias the item type.
		typedef TItem							Item;

		//! Alias the item type without a pointer.
		typedef typename RemovePointer<TItem>::Type&	ItemRef;

		//! Alias the item type without a pointer.
		typedef const typename RemovePointer<TItem>::Type&	ConstItemRef;

		//! Alias this type.
		typedef KdTree<TItem, K, TBalancer>		Tree;

		//! Alias the array of items.
		typedef Array<TItem>					Items;

		//! Alias the node type.
		typedef KdNode<TItem>					Node;

												//! Constructs KdTree instance.
												KdTree( const TBalancer& balancer = TBalancer() )
												: m_balancer( balancer ) {}

		//! Inserts a new item into the tree.
		void									insert( TItem& item );

		//! Builds a tree from an array of items
		void									set( const Items& items );

		//! Returns the root node.
		const Node&								root( void ) const;

		//! Returns all nearest neighbors that fall into the specified radius.
		Items									neighbors( const f32 point[K], f32 radius ) const;

		//! Returns the nearest item to a specified point.
		const TItem&							nearest( const f32 point[K] ) const;

	private:

		//! Find a suitable node for item.
		const Node*								find( const Node* root, ConstItemRef item ) const;
		Node*									find( Node* root, ConstItemRef item );

		//! Checks whether or not the node should be splitted.
		void									balance( Node* node );

		//! Locates nearest photons to a specified point.
		void									neighbors( Items& items, const f32 point[K], f32 radius, const Node* root ) const;

	private:
		
		TBalancer								m_balancer;	//!< The used tree balancer.
		Node									m_root;		//!< The root tree node.
	};

	// ** KdTree::root
	template<typename TItem, u16 K, typename TBalancer>
	typename const KdTree<TItem, K, TBalancer>::Node& KdTree<TItem, K, TBalancer>::root( void ) const
	{
		return m_root;
	}

	// ** KdTree::nearest
	template<typename TItem, u16 K, typename TBalancer>
	typename const TItem& KdTree<TItem, K, TBalancer>::nearest( const f32 point[K] ) const
	{
		const Node* node = &m_root;

		while( !node->isLeaf() ) {
			const Node* a = node->m_left.get();
			const Node* b = node->m_right.get();

			if( point[node->m_dimension] < node->m_splitter ) {
				node = !a->isLeaf() || !a->m_items.empty() ? a : b;
			} else {
				node = !b->isLeaf() || !b->m_items.empty() ? b : a;
			}
		}

		DC_BREAK_IF( node->m_items.empty() );

		return node->m_items[0];
	}

	// ** KdTree::neighbors
	template<typename TItem, u16 K, typename TBalancer>
	void KdTree<TItem, K, TBalancer>::neighbors( Items& items, const f32 point[K], f32 radius, const Node* node ) const
	{
		// ** Calculate the signed distance to a splitting plane.
		f32 distance = point[node->m_dimension] - node->m_splitter;

		// ** Search the subtree.
		const Node* a = distance < 0 ? node->m_left.get()  : node->m_right.get();
		const Node* b = distance < 0 ? node->m_right.get() : node->m_left.get();

		if( a && b ) {
			neighbors( items, point, radius, a );

			if( distance * distance < radius ) {
				neighbors( items, point, radius, b );
			}
		}

		for( s32 i = 0, n = ( s32 )node->m_items.size(); i < n; i++ ) {
			ConstItemRef itemr2 = RemovePointer<TItem>::cast( node->m_items[i] );

			f32 dist = 0.0f;

			for( s32 j = 0; j < K; j++ ) {
				f32 d = point[j] - itemr2[j];
				dist += d * d;
			}

			if( dist < radius ) {
				items.push_back( node->m_items[i] );
			}
		}
	}

	// ** KdTree::neighbors
	template<typename TItem, u16 K, typename TBalancer>
	typename KdTree<TItem, K, TBalancer>::Items KdTree<TItem, K, TBalancer>::neighbors( const f32 point[K], f32 radius ) const
	{
		typename Items items;
		neighbors( items, point, radius * radius, &m_root );
		return items;
	}

	// ** KdTree::insert
	template<typename TItem, u16 K, typename TBalancer>
	void KdTree<TItem, K, TBalancer>::insert( TItem& item )
	{
		ConstItemRef  itemr = RemovePointer<TItem>::cast( item );
		Node*		  node  = find( &m_root, itemr );
		DC_BREAK_IF( node == NULL );

		node->m_items.push_back( item );
		balance( node );
	}

	// ** KdTree::insert
	template<typename TItem, u16 K, typename TBalancer>
	void KdTree<TItem, K, TBalancer>::set( const Items& items )
	{
		m_root.m_items = items;
		balance( &m_root );
	}

	// ** KdTree::find
	template<typename TItem, u16 K, typename TBalancer>
	const typename KdTree<TItem, K, TBalancer>::Node* KdTree<TItem, K, TBalancer>::find( const Node* root, ConstItemRef item ) const
	{
		if( root->m_left == NULL || root->m_right == NULL ) {
			return root;
		}

		return item[root->m_dimension] < root->m_splitter ? find( root->m_left.get(), item ) : find( root->m_right.get(), item );
	}

	// ** KdTree::find
	template<typename TItem, u16 K, typename TBalancer>
	typename KdTree<TItem, K, TBalancer>::Node* KdTree<TItem, K, TBalancer>::find( Node* root, ConstItemRef item )
	{
		if( root->m_left == NULL || root->m_right == NULL ) {
			return root;
		}

		return item[root->m_dimension] < root->m_splitter ? find( root->m_left.get(), item ) : find( root->m_right.get(), item );
	}

	// ** KdTree::balance
	template<typename TItem, u16 K, typename TBalancer>
	void KdTree<TItem, K, TBalancer>::balance( Node* node )
	{
		float splitter;
		u16   dimension = node->m_dimension;

		if( !m_balancer.split( node, splitter, dimension ) ) {
			return;
		}

		node->m_dimension = dimension;
		node->m_splitter  = splitter;
		node->m_left	  = new Node( (dimension + 1) % K );
		node->m_right	  = new Node( (dimension + 1) % K );
	
		for( s32 i = 0, n = ( s32 )node->m_items.size(); i < n; i++ ) {
			TItem&		 item  = node->m_items[i];
			ConstItemRef itemr = RemovePointer<TItem>::cast( item );

			if( itemr[dimension] < splitter ) {
				node->m_left->m_items.push_back( item );
			} else {
				node->m_right->m_items.push_back( item );
			}
		}

		node->m_items.clear();

		balance( node->m_left.get() );
		balance( node->m_right.get() );
	}

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_KdTree_H__ )  */
