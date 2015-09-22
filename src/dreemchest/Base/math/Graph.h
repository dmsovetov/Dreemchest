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

#ifndef __Base_Graph_H__
#define __Base_Graph_H__

DC_BEGIN_DREEMCHEST

    //! Euclidean distance.
    template<typename T>
    struct EuclideanDistance2 {
        //! Calculates an euclidean distance between two points.
        f32 operator()( const T& a, const T& b ) const { return sqrtf( f32( (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) ) ); }
    };

    //! Mahnattan distance.
    template<typename T>
    struct ManhattanDistance2 {
        //! Calculates the Manhattan distance between two points.
        f32 operator()( const T& a, const T& b ) const { return fabs( f32( a.x - b.x ) ) + fabs( f32( a.y - b.y ) ); }
    };

    /*!
     A graph is a representation of a set of objects where some pairs of objects are connected by links.The interconnected
     objects are represented by mathematical abstractions called vertices, and the links that connect
     some pairs of vertices are called edges. (c) Wikipedia
    */
    template<typename TVertex, typename TCoordinates>
    class Graph {
    public:

        //! Allias the vertex coordinates type.
        typedef TCoordinates    Coordinates;

        //! Vertex index type.
        typedef s32             VIndex;

        //! Vertex outgoing edges container.
        typedef Set<VIndex>		LinkedVertices;

        //! Graph vertex.
        struct Vertex : public TVertex {
                                //! Constructs Vertex instance.
                                Vertex( const TCoordinates& coordinates, const TVertex& data )
                                    : TVertex( data ), m_coordinates( coordinates ) {}

            TCoordinates        m_coordinates;      //!< Vertex coordinates.
            LinkedVertices      m_links;            //!< Linked vertices.
        };

        //! A list of vertex indices representing a path on graph.
        typedef List<VIndex>    Path;

        //! Adds a new vertex to graph.
        VIndex                  add( const TCoordinates& coordinates, const TVertex& data );

        //! Links two vertices with an edge.
        void                    link( VIndex a, VIndex b );

        //! Returns the total number of vertices.
        s32                     vertexCount( void ) const;

        //! Returns the vertex by index.
        const Vertex*           vertex( VIndex index ) const;

    protected:

        Array<Vertex>			m_vertices; //!< Array of graph vertices.
    };

    // ** Graph::add
    template<typename TVertex, typename TCoordinates>
    typename Graph<TVertex, TCoordinates>::VIndex Graph<TVertex, TCoordinates>::add( const TCoordinates& coordinates, const TVertex& data )
    {
        VIndex idx = ( VIndex )m_vertices.size();
        m_vertices.push_back( Vertex( coordinates, data ) );
        return idx;
    }

    // ** Graph::link
    template<typename TVertex, typename TCoordinates>
    void Graph<TVertex, TCoordinates>::link( VIndex a, VIndex b )
    {
        DC_BREAK_IF( a >= ( VIndex )m_vertices.size() );
        DC_BREAK_IF( b >= ( VIndex )m_vertices.size() );

        m_vertices[a].m_links.insert( b );
        m_vertices[b].m_links.insert( a );
    }

    // ** Graph::vertex
    template<typename TVertex, typename TCoordinates>
    const typename Graph<TVertex, TCoordinates>::Vertex* Graph<TVertex, TCoordinates>::vertex( VIndex index ) const
    {
        return &m_vertices[index];
    }

    // ** Graph::vertex
    template<typename TVertex, typename TCoordinates>
    s32 Graph<TVertex, TCoordinates>::vertexCount( void ) const
    {
        return ( s32 )m_vertices.size();
    }

    //! A* path finder.
    template<typename TGraph, typename G = EuclideanDistance2<typename TGraph::Coordinates>, typename H = ManhattanDistance2<typename TGraph::Coordinates> >
    class AStarPathFinder {
    public:

        //! Alias the type for TGraph vertex.
        typedef typename TGraph::Vertex Vertex;

        //! Alias the type for TGraph vertex index.
        typedef typename TGraph::VIndex VIndex;

                                //! Constructs AStarPathFinder instance.
                                AStarPathFinder( const TGraph& graph )
                                    : m_graph( graph ) {}

        //! Finds the path between two points.
        typename TGraph::Path   find( VIndex start, VIndex end );


    private:

        //! Path node data.
        struct Node {
            //! Node flags masks.
            enum {
                Opened = BIT( 0 ),  //!< Indicates that a path node is opened.
                Closed = BIT( 1 ),  //!< Indicates that a path node is closed.
            };

                            //! Constructs the Node instance.
                            Node( const Vertex* vertex = NULL, VIndex index = -1 )
                                : m_vertex( vertex ), m_index( index ), m_parent( NULL ), m_mask( 0 ), m_g( 0.0f ), m_h( 0.0f ) {}

            //! Returns the calculated F score.
            f32             score( void ) const { return m_g + m_h; }

            //! Marks the node as opened.
            void            open( void ) { m_mask |= Opened; }

            //! Returns true if node is opened.
            bool            isOpened( void ) const { return (m_mask & Opened) != 0; }

            //! Marks the node as closed.
            void            close( void ) { m_mask |= Closed; }

            //! Returns true if node is closed.
            bool            isClosed( void ) const { return (m_mask & Closed) != 0; }  

            const Vertex*   m_vertex;   //!< The graph vertex.
            VIndex          m_index;    //!< The vertex index.
            const Node*     m_parent;   //!< The parent path node.
            u8              m_mask;     //!< The node flags.
            f32             m_g;        //!< G path score.
            f32             m_h;        //!< H path score.
        };

        //! Container type to map the graph vertex to a path node.
        typedef Map<VIndex, Node> NodeByVertex;

        //! Adds a path node to a sorted open list.
        bool                    open( Node* node, const Node* parent = NULL );

        //! Adds a path node to a closed list.
        bool                    close( Node* node ) const;

        //! Checks if we have a better path.
        void                    improve( Node* node, const Node* parent ) const;

        //! Returns the path node by graph vertex.
        Node*                   node( VIndex index );

        //! Calculates the H score between two graph vertices.
        f32                     hScore( const Vertex* a, const Vertex* b ) const { return H()( a->m_coordinates, b->m_coordinates ); }

        //! Calculates the G score between two graph vertices.
        f32                     gScore( const Vertex* a, const Vertex* b ) const { return G()( a->m_coordinates, b->m_coordinates ); }

        //! Traces the path.
        typename TGraph::Path   build( const Node* node ) const;

        const TGraph&           m_graph;    //!< Parent graph.
        VIndex                  m_end;      //!< The end point vertex.
        NodeByVertex            m_nodes;    //!< Stores the VIndex -> Node mapping.
        List<Node*>				m_open;     //!< The opened cells list.
    };

    // ** AStarPathFinder::find
    template<typename TGraph, typename G, typename H>
    typename TGraph::Path AStarPathFinder<TGraph, G, H>::find( typename TGraph::VIndex start, typename TGraph::VIndex end )
    {
        // ** Save the end point.
        m_end = end;

        // ** Add the starting cell to open list
        open( node( start ) );

        // ** Continue until there is no available nodes in open list
        do {
            // ** Get the node with lowest F score - in a sorted list it goes first.
            Node* current = m_open.front();
            m_open.pop_front();

 			// ** Add to closed list
			if( close( current ) ) {
                // ** Build the path when the end point reached.
				return build( current );
			}

            // ** Process each linked vertex
            for( typename TGraph::LinkedVertices::const_iterator i = current->m_vertex->m_links.begin(), end = current->m_vertex->m_links.end(); i != end; ++i ) {
                Node* linked = node( *i );

                // ** Already processed
                if( linked->isClosed() ) {
                    continue;
                }

				// ** Add to open list
				if( open( linked, current ) ) {
					continue;
				}

			    // ** Already in open list - check the cost and update the parent
			    improve( linked, current );
            }
        } while( m_open.empty() == false );

        return typename TGraph::Path();
    }

    // ** AStarPathFinder::node
    template<typename TGraph, typename G, typename H>
    typename AStarPathFinder<TGraph, G, H>::Node* AStarPathFinder<TGraph, G, H>::node( typename TGraph::VIndex index )
    {
        typename NodeByVertex::iterator i = m_nodes.find( index );

        if( i != m_nodes.end() ) {
            return &i->second;
        }

        m_nodes[index] = Node( m_graph.vertex( index ), index );
        return &m_nodes[index];
    }

    // ** AStarPathFinder::close
    template<typename TGraph, typename G, typename H>
    bool AStarPathFinder<TGraph, G, H>::open( Node* node, const Node* parent )
    {
		// ** The cell is already opened
		if( node->isOpened() ) {
			return false;
		}

        // ** Calculate the score.
        f32 h = hScore( node->m_vertex, m_graph.vertex( m_end ) );
        f32 g = parent ? hScore( node->m_vertex, parent->m_vertex ) : 0.0f;
			
		// ** Initialize the path node data.
        node->m_parent  = parent;
        node->m_h       = h;
        node->m_g       = parent ? parent->m_g + g : 0.0f;
        node->open();

		// ** Insert to a list and sort it
        struct Compare { static bool byScore( const Node* a, const Node* b ) { return a->score() < b->score(); } };

		m_open.push_back( node );
        m_open.sort( Compare::byScore );

		return true;
    }

    // ** AStarPathFinder::close
    template<typename TGraph, typename G, typename H>
    bool AStarPathFinder<TGraph, G, H>::close( Node* node ) const
    {
        node->close();
        return node->m_index == m_end;
    }

    // ** AStarPathFinder::improve
    template<typename TGraph, typename G, typename H>
    void AStarPathFinder<TGraph, G, H>::improve( Node* node, const Node* parent ) const
    {
        f32 g = gScore( node->m_vertex, parent->m_vertex );

		if( (parent->m_g + g) >= node->m_g ) {
			return;
		}
			
		node->m_parent  = parent;
		node->m_g       = parent->m_g + g;
    }

    // ** AStarPathFinder::build
    template<typename TGraph, typename G, typename H>
    typename TGraph::Path AStarPathFinder<TGraph, G, H>::build( const Node* node ) const
    {
        typename TGraph::Path result;

        while( node ) {
            result.push_front( node->m_index );
            node = node->m_parent;
        }

        return result;
    }

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_Graph_H__ )  */
