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

#ifndef __Base_Mesh_H__
#define __Base_Mesh_H__

DC_BEGIN_DREEMCHEST

    //! DCEL data struct to simplify access to a triangular mesh topology.
    template<typename TIndex = unsigned short>
    class DCEL {
    public:

        //! Index buffer type.
        typedef Array<TIndex> IndexBuffer;

        //! The edge struct.
        struct Edge {
            TIndex          m_face;     //!< The parent face.
            TIndex          m_vertex;   //!< The first edge vertex.
            Edge*           m_next;     //!< The next edge around the face.
            Edge*           m_twin;     //!< The pair edge.

            //! Returns true if this edge is a boundary.
            bool isBoundary( void ) const
            {
                return m_twin == NULL;
            }

            //! Returns edge twin.
            Edge* twin( void ) const
            {
                return m_twin;
            }

            //! Sets the edge twin.
            void setTwin( Edge* value )
            {
                m_twin        = value;
                value->m_twin = this;
            }
        };

                            ~DCEL( void );

        //! Returns edge count.
        int                 edgeCount( void ) const;

        //! Returns edge by index.
        const Edge*         edge( int index ) const;

        //! Constructs a half edge struct from a triangle mesh.
        static DCEL         create( const IndexBuffer& indexBuffer );

    private:

                            //! Constructs a HalfEdge instance.
                            DCEL( Edge* edges, int edgeCount );

    private:

        Edge*               m_edges;        //!< Mesh edges.
        int                 m_edgeCount;    //!< The total number of edges.
    };

    // ** DCEL::DCEL
    template<typename TIndex>
    DCEL<TIndex>::DCEL( Edge* edges, int edgeCount ) : m_edges( edges ), m_edgeCount( edgeCount )
    {
    }

    // ** DCEL::DCEL
    template<typename TIndex>
    DCEL<TIndex>::~DCEL( void )
    {
    //    delete[]m_edges;
    }

    // ** DCEL::edgeCount
    template<typename TIndex>
    int DCEL<TIndex>::edgeCount( void ) const
    {
        return m_edgeCount;
    }

    // ** DCEL::edgeCount
    template<typename TIndex>
    const typename DCEL<TIndex>::Edge* DCEL<TIndex>::edge( int index ) const
    {
        return &m_edges[index];
    }

    // ** DCEL::create
    template<typename TIndex>
    DCEL<TIndex> DCEL<TIndex>::create( const IndexBuffer& indexBuffer )
    {
        typedef std::pair<TIndex, TIndex>           EdgeVertices;
        typedef std::multimap<EdgeVertices, Edge*>  EdgeCache;

        int       faceCount  = indexBuffer.size() / 3;
        int       edgeCount  = faceCount * 3;
        Edge*     edges      = new Edge[edgeCount];
        EdgeCache cache;

        memset( edges, 0, sizeof( Edge ) * edgeCount );

        for( TIndex face = 0; face < faceCount; face++ ) {
            Edge*         faceEdges   = &edges[face * 3];
            const TIndex* faceIndices = &indexBuffer[face * 3];

            for( int i = 0; i < 3; i++ ) {
                faceEdges[i].m_face   = face;
                faceEdges[i].m_vertex = faceIndices[i];
            }

            for( int i = 0; i < 3; i++ ) {
                Edge& edge = faceEdges[i];

                edge.m_next = &faceEdges[(i + 1) % 3];
                cache.insert( std::pair<EdgeVertices, Edge*>( EdgeVertices( edge.m_vertex, edge.m_next->m_vertex ), &edge ) );
            }
        }

        for( int i = 0; i < edgeCount; i++ ) {
            Edge& e = edges[i];

            typename EdgeCache::iterator z = cache.find( EdgeVertices( e.m_next->m_vertex, e.m_vertex ) );
            if( z != cache.end() && &e != z->second ) {
                assert( &e != z->second );
                e.setTwin( z->second );
            }
        }

        return DCEL<TIndex>( edges, edgeCount );
    }

	//! Angle based chart builder.
	template<typename TMesh>
	class AngularChartifier {
	public:

		//! Container type to store face to chart mapping.
		typedef Map<int, int> ChartByFace;

		//! The result of a chart builder.
		struct Result {
			ChartByFace				m_chartByFace;	//!< A chart by face registry.
			typename TMesh::Charts	m_charts;	//!< Built charts;
		};

									//! Constructs AngularChartifier instance.
									AngularChartifier( float angle = 88.0f )
										: m_angle( angle ) {}

		//! Splits the input mesh into charts.
		Result						build( TMesh& mesh ) const;

	private:

		//! Adds faces to a chart.
		void						addToChart( Result& result, TMesh& mesh, const typename TMesh::Dcel::Edge* edge, const Vec3& axis, int index ) const;

	private:

		float						m_angle;	//!< The hard angle.
	};

    //! MeshIndexer helps to build a vertex/index buffer pair from an input stream of vertices.
    template<typename TVertex, typename TCompare, typename TIndex = unsigned short>
    class MeshIndexer {
    public:

        //! Container type to store the indices.
        typedef Array<TIndex>			IndexBuffer;

        //! Container type to store the vertices.
        typedef Array<TVertex>			VertexBuffer;

		//! Clears the mesh indexer.
		void							clear( void );

        //! Adds a new vertex and returns it's index.
        TIndex                          operator += ( const TVertex& vertex );

        //! Returns the built index buffer.
        const IndexBuffer&              indexBuffer( void ) const;
		IndexBuffer&					indexBuffer( void );

        //! Returns the built vertex buffer.
        const VertexBuffer&             vertexBuffer( void ) const;
		VertexBuffer&					vertexBuffer( void );

    private:

        //! Container type to store added vertices.
        typedef Map<TVertex, TIndex, TCompare>   VertexCache;

        VertexCache                     m_cache;            //!< Vertices added to an indexer.
        VertexBuffer                    m_vertexBuffer;     //!< Built vertex buffer.
        IndexBuffer                     m_indexBuffer;      //!< Built index buffer.
    };

    // ** MeshIndexer::clear
    template<typename TVertex, typename TCompare, typename TIndex>
    void MeshIndexer<TVertex, TCompare, TIndex>::clear( void )
    {
        m_cache.clear();
		m_vertexBuffer.clear();
		m_indexBuffer.clear();
    }

    // ** MeshIndexer::indexBuffer
    template<typename TVertex, typename TCompare, typename TIndex>
    const typename MeshIndexer<TVertex, TCompare, TIndex>::IndexBuffer& MeshIndexer<TVertex, TCompare, TIndex>::indexBuffer( void ) const
    {
        return m_indexBuffer;
    }

    // ** MeshIndexer::indexBuffer
    template<typename TVertex, typename TCompare, typename TIndex>
    typename MeshIndexer<TVertex, TCompare, TIndex>::IndexBuffer& MeshIndexer<TVertex, TCompare, TIndex>::indexBuffer( void )
    {
        return m_indexBuffer;
    }

    // ** MeshIndexer::vertexBuffer
    template<typename TVertex, typename TCompare, typename TIndex>
    const typename MeshIndexer<TVertex, TCompare, TIndex>::VertexBuffer& MeshIndexer<TVertex, TCompare, TIndex>::vertexBuffer( void ) const
    {
        return m_vertexBuffer;
    }

    // ** MeshIndexer::vertexBuffer
    template<typename TVertex, typename TCompare, typename TIndex>
    typename MeshIndexer<TVertex, TCompare, TIndex>::VertexBuffer& MeshIndexer<TVertex, TCompare, TIndex>::vertexBuffer( void )
    {
        return m_vertexBuffer;
    }

    // ** MeshIndexer::add
    template<typename TVertex, typename TCompare, typename TIndex>
    TIndex MeshIndexer<TVertex, TCompare, TIndex>::operator += ( const TVertex& vertex )
    {
        typename VertexCache::iterator i = m_cache.find( vertex );
        TIndex idx = 0;

        if( i != m_cache.end() ) {
            idx = i->second;
        } else {
            idx = m_vertexBuffer.size();
            m_cache[vertex] = idx;
            m_vertexBuffer.push_back( vertex );
        }

        m_indexBuffer.push_back( idx );
        return idx;
    }

	// ** AngularChartifier::build
	template<typename TMesh>
	typename AngularChartifier<TMesh>::Result AngularChartifier<TMesh>::build( TMesh& mesh ) const
	{
		Result result;

		typename TMesh::Dcel dcel = mesh.dcel();

		for( int i = 0, n = dcel.edgeCount(); i < n; i++ ) {
			const typename TMesh::Dcel::Edge* edge = dcel.edge( i );
			addToChart( result, mesh, edge, mesh.face( edge->m_face ).normal(), result.m_charts.size() );
		}

		return result;
	}

	// ** AngularChartifier::addToChart
	template<typename TMesh>
	void AngularChartifier<TMesh>::addToChart( Result& result, TMesh& mesh, const typename TMesh::Dcel::Edge* edge, const Vec3& axis, int index ) const
	{
		// ** Skip the processed faces.
		if( result.m_chartByFace.count( edge->m_face ) ) {
			return;
		}

		float angle = degrees( acosf( axis * mesh.face( edge->m_face ).normal() ) );

		if( angle > m_angle ) {
			return;
		}

		result.m_chartByFace[edge->m_face] = index;

		if( result.m_charts.size() <= index ) {
			result.m_charts.resize( index + 1 );
			result.m_charts[index] = typename TMesh::Chart( &mesh );
		}
		result.m_charts[index].add( edge->m_face );

		const typename TMesh::Dcel::Edge* i = edge;

		do {
			if( i->twin() ) {
				addToChart( result, mesh, i->twin(), axis, index );
			}
			i = i->m_next;
		} while( i != edge );
	}

	//! TriMesh represents an indexed triangular mesh.
	template< typename TVertex, typename TIndex = unsigned short, typename TVertexCompare = std::less<TVertex> >
	class TriMesh {
	public:

		//! Face index type.
		typedef unsigned int FaceIndex;

		//! Alias this type.
		typedef TriMesh<TVertex, TIndex, TVertexCompare> Mesh;

		//! Alias the indexer type.
		typedef MeshIndexer<TVertex, TVertexCompare> Indexer;

		//! Alias the vertex type.
		typedef TVertex Vertex;

		//! Alias the index type.
		typedef TIndex Index;

		//! Container type to store mesh vertices.
		typedef Array<TVertex> Vertices;

		//! Container type to store mesh indices.
		typedef Array<TIndex> Indices;

		//! Container type to store face indices.
		typedef Array<FaceIndex> FaceIndices;

        //! DCEL type.
        typedef DCEL<TIndex> Dcel;

		//! Triangular mesh face.
		struct Face {
							            //! Constructs Face instance.
							            Face( Vertices& vertices, Indices& indices, FaceIndex index )
								            : m_vertices( vertices ), m_indices( indices ), m_index( index ) {}

			//! Returns a vertex by index.
			TVertex&		            operator[]( TIndex index ) { return vertex( index ); }
			const TVertex&              operator[]( TIndex index ) const { return vertex( index ); }

			FaceIndex					m_index;	//!< Face index.
			Vertices&		            m_vertices;	//!< Mesh vertices.
			Indices&		            m_indices;	//!< Mesh indices.

			//! Calculates face normal.
			Vec3			            normal( void ) const;

            //! Flattens the triangle to a 2D space.
            void                        flatten( const Vec3& axis, Vec2& a, Vec2& b, Vec2& c ) const;

			//! Returns the face vertex by index.
			const TVertex&	            vertex( TIndex index ) const;
			TVertex&		            vertex( TIndex index );
		};

        //! Mesh chart.
        struct Chart {
                                        //! Constructs Chart instance.
                                        Chart( Mesh* mesh = NULL )
                                            : m_mesh( mesh ) {}

            //! Adds a new face to chart.
            void                        add( FaceIndex index );

            //! Returns face count.
            int                         faceCount( void ) const;

            //! Returns chart face by index.
            Face                        face( int index ) const;

            //! Calculates avarage chart normal.
            Vec3                        normal( void ) const;

			//! Returns chart faces.
			const FaceIndices&			faces( void ) const;

			//! Calculates the UV bounding rectangle.
			void						calculateUvRect( Vec2& min, Vec2& max, int layer = 0 ) const;

            Mesh*						m_mesh;     //!< Parent trimesh.
            FaceIndices					m_faces;    //!< Chart faces.
        };

		//! Container type to store mesh charts.
		typedef Array<Chart>	Charts;

						//! Constructs TriMesh instance.
						TriMesh( Vertices& vertices, Indices& indices );

        //! Returns a DCEL data structure for this mesh.
        Dcel            dcel( void ) const;   

		//! Returns the total number of mesh faces.
		int				faceCount( void ) const;

		//! Returns a mesh face by index.
		Face			face( int index ) const;

		//! Returns mesh vertices.
		Vertices&		vertices( void ) const;

		//! Returns mesh indices.
		Indices&		indices( void ) const;

		//! Splits the mesh into charts by a specified chart builder.
		template<typename TChartBuilder>
		typename TChartBuilder::Result charts( const TChartBuilder& chartBuilder ) { return chartBuilder.build( *this ); }

	private:

		//! Mesh vertices.
		Vertices&		m_vertices;

		//! Mesh indices.
		Indices&		m_indices;
	};

	// ** TriMesh::TriMesh
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	TriMesh<TVertex, TIndex, TVertexCompare>::TriMesh( Vertices& vertices, Indices& indices ) : m_vertices( vertices ), m_indices( indices )
	{
	}

	// ** TriMesh::dcel
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	typename TriMesh<TVertex, TIndex, TVertexCompare>::Dcel TriMesh<TVertex, TIndex, TVertexCompare>::dcel( void ) const
	{
		return Dcel::create( m_indices );
	}

	// ** TriMesh::faceCount
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	int TriMesh<TVertex, TIndex, TVertexCompare>::faceCount( void ) const
	{
		return ( int )m_indices.size() / 3;
	}

	// ** TriMesh::vertices
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	typename TriMesh<TVertex, TIndex, TVertexCompare>::Vertices& TriMesh<TVertex, TIndex, TVertexCompare>::vertices( void ) const
	{
		return m_vertices;
	}

	// ** TriMesh::indices
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	typename TriMesh<TVertex, TIndex, TVertexCompare>::Indices& TriMesh<TVertex, TIndex, TVertexCompare>::indices( void ) const
	{
		return m_indices;
	}

	// ** TriMesh::faceCount
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	typename TriMesh<TVertex, TIndex, TVertexCompare>::Face TriMesh<TVertex, TIndex, TVertexCompare>::face( int index ) const
	{
		return Face( m_vertices, m_indices, index );
	}

	// ** TriMesh::Chart::add
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	void TriMesh<TVertex, TIndex, TVertexCompare>::Chart::add( unsigned int index )
	{
        if( std::find( m_faces.begin(), m_faces.end(), index ) != m_faces.end() ) {
            return;
        }

        m_faces.push_back( index );
	}

	// ** TriMesh::Chart::faceCount
	template<typename TVertex, typename TIndex, typename TVertexCompare>
    int TriMesh<TVertex, TIndex, TVertexCompare>::Chart::faceCount( void ) const
	{
        return ( int )m_faces.size();
	}

	// ** TriMesh::Chart::face
	template<typename TVertex, typename TIndex, typename TVertexCompare>
    typename TriMesh<TVertex, TIndex, TVertexCompare>::Face TriMesh<TVertex, TIndex, TVertexCompare>::Chart::face( int index ) const
	{
        return m_mesh->face( m_faces[index] );
	}

	// ** TriMesh::Chart::faces
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	const typename TriMesh<TVertex, TIndex, TVertexCompare>::FaceIndices& TriMesh<TVertex, TIndex, TVertexCompare>::Chart::faces( void ) const
	{
		return m_faces;
	}

	// ** TriMesh::Chart::normal
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	Vec3 TriMesh<TVertex, TIndex, TVertexCompare>::Chart::normal( void ) const
	{
        Vec3 n( 0.0f, 0.0f, 0.0f );

        for( int i = 0; i < faceCount(); i++ ) {
            n += face( i ).normal();
        }

        n /= ( float )m_faces.size();
        n.normalize();

        return n;
	}

	// ** TriMesh::Chart::calculateUvRect
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	void TriMesh<TVertex, TIndex, TVertexCompare>::Chart::calculateUvRect( Vec2& min, Vec2& max, int layer ) const
	{
		min = Vec2(  FLT_MAX,  FLT_MAX );
		max = Vec2( -FLT_MAX, -FLT_MAX );

		for( int i = 0, n = faceCount(); i < n; i++ ) {
			const Face& f = face( i );

			for( int j = 0; j < 3; j++ ) {
				const TVertex& v = f.vertex( j );

				min.x = min2( v.uv[layer].x, min.x );
				min.y = min2( v.uv[layer].y, min.y );
				max.x = max2( v.uv[layer].x, max.x );
				max.y = max2( v.uv[layer].y, max.y );
			}
		}
	}

	// ** TriMesh::Face::flatten
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	void TriMesh<TVertex, TIndex, TVertexCompare>::Face::flatten( const Vec3& axis, Vec2& a, Vec2& b, Vec2& c ) const
	{
        Plane plane( axis );

        const Vec3& v0 = vertex( 0 ).position;
        const Vec3& v1 = vertex( 1 ).position;
        const Vec3& v2 = vertex( 2 ).position;
/*
        Vec3 N = normal();
        Vec3 T = v1 - v0; T.normalize();
        Vec3 B = N % T;   B.normalize();

        Matrix4 Tr;
        Tr.setRow( 0, Vec4( T.x, T.y, T.z, 0.0f ) );
        Tr.setRow( 1, Vec4( B.x, B.y, B.z, 0.0f ) );
        Tr.setRow( 2, Vec4( N.x, N.y, N.z, 0.0f ) );
        Tr.setRow( 3, Vec4( 0, 0, 0, 1.0f ) );

        Vec3 r0 = Tr.inversed().rotate( v0 );
        Vec3 r1 = Tr.inversed().rotate( v1 );
        Vec3 r2 = Tr.inversed().rotate( v2 );

        assert( fabs( r0.z - r1.z ) < 0.1f && fabs( r1.z - r2.z ) < 0.1f && fabs( r2.z - r0.z ) < 0.1f );

        a = Vec2( r0.x, r0.y );
        b = Vec2( r1.x, r1.y );
        c = Vec2( r2.x, r2.y );
*/

        Vec3 p0 = plane * v0;
        Vec3 p1 = plane * v1;
        Vec3 p2 = plane * v2;

        if( axis.x == 1.0f ) {
            a = Vec2( p0.y, p0.z );
            b = Vec2( p1.y, p1.z );
            c = Vec2( p2.y, p2.z );
        }
        else if( axis.y == 1.0f ) {
            a = Vec2( p0.x, p0.z );
            b = Vec2( p1.x, p1.z );
            c = Vec2( p2.x, p2.z );
        }
        else {
            a = Vec2( p0.x, p0.y );
            b = Vec2( p1.x, p1.y );
            c = Vec2( p2.x, p2.y );
        }
	}

	// ** TriMesh::Face::normal
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	Vec3 TriMesh<TVertex, TIndex, TVertexCompare>::Face::normal( void ) const
	{
		assert( m_index * 3 + 3 <= m_indices.size() );
		TIndex*	    indices = &m_indices[m_index * 3];
		const Vec3&	v0		= m_vertices[indices[0]].position;
		const Vec3&	v1		= m_vertices[indices[1]].position;
		const Vec3&	v2		= m_vertices[indices[2]].position;

		Vec3 normal = (v1 - v0) % (v2 - v0);
		normal.normalize();

		return normal;
	}

	// ** TriMesh::Face::vertex
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	const TVertex& TriMesh<TVertex, TIndex, TVertexCompare>::Face::vertex( TIndex index ) const
	{
		assert( m_index * 3 + 3 <= m_indices.size() );
		TIndex* indices = &m_indices[m_index * 3];
		return  m_vertices[indices[index]];
	}

	// ** TriMesh::Face::vertex
	template<typename TVertex, typename TIndex, typename TVertexCompare>
	TVertex& TriMesh<TVertex, TIndex, TVertexCompare>::Face::vertex( TIndex index )
	{
		assert( m_index * 3 + 3 <= m_indices.size() );
		TIndex* indices = &m_indices[m_index * 3];
		return  m_vertices[indices[index]];
	}

	//! Generates a second UV set for a lightmapping.
	template< typename TMesh, typename TChartifier = AngularChartifier<TMesh>, typename TRectanglePacker = RectanglePacker<float> >
	class UvGenerator {
	public:

		//! Mesh UV set type.
		typedef Array<Vec2>			UvSet;

									//! Constructs an instance if UvGenerator
									UvGenerator( TMesh& mesh, u32 uvLayer, const TChartifier& chartifier = TChartifier(), const TRectanglePacker& packer = TRectanglePacker() )
										: m_mesh( mesh ), m_chartifier( chartifier ), m_packer( packer ), m_uvLayer( uvLayer ) {}

		//! Generates a new UV set inplace.
		void						generate( typename TMesh::Vertices& vertices, typename TMesh::Indices& indices );

	private:

		//! Flattens the mesh chart and returns the resulting UV set.
		UvSet						flatten( const typename TMesh::Chart& chart ) const;

		//! Tries to rotate a UV set to minimize the occupied texture space.
		void						rotate( UvSet& uv ) const;

		//! Calculates the bounding rect of a UV set.
		Vec2						calculateBoundingRect( const UvSet& uv, Vec2& min, Vec2& max ) const;

		//! Calculates the bounding rect of a specified portion of a mesh.
		Vec2						calculateBoundingRect( const typename TMesh::Indices& indices, const typename TMesh::Vertices& vertices, Vec2& min, Vec2& max ) const;

		//! Packs the chart rectangles to a minimal needed space.
		Vec2						pack( void );

	private:

		TMesh&						m_mesh;			//!< The mesh being processed. 
		TChartifier					m_chartifier;	//!< The chartifier to split the mesh into set of charts.
		TRectanglePacker			m_packer;		//!< The rectangle packer to use.
		typename TMesh::Vertices	m_vertices;		//!< Mesh vertices.
		typename TMesh::Indices		m_indices;		//!< Mesh indices.
		u32							m_uvLayer;		//!< The UV layer index to generate.
	};

	// ** UvGenerator::flatten
	template<typename TMesh, typename TChartifier, typename TRectanglePacker>
	typename UvGenerator<TMesh, TChartifier, TRectanglePacker>::UvSet UvGenerator<TMesh, TChartifier, TRectanglePacker>::flatten( const typename TMesh::Chart& chart ) const
	{
		UvSet result;

		for( int j = 0; j < chart.faceCount(); j++ ) {
			typename TMesh::Face face = chart.face( j );
			Vec3 normal = chart.normal();

			Vec2 v[3];
			face.flatten( normal.ordinal(), v[0], v[1], v[2] );

			for( int i = 0; i < 3; i++ ) {
				result.push_back( v[i] );
			}
		}

		return result;
	}

	// ** UvGenerator::calculateBoundingRect
	template<typename TMesh, typename TChartifier, typename TRectanglePacker>
	Vec2 UvGenerator<TMesh, TChartifier, TRectanglePacker>::calculateBoundingRect( const Array<Vec2>& uv, Vec2& min, Vec2& max ) const
	{
		min = Vec2(  FLT_MAX,  FLT_MAX );
		max = Vec2( -FLT_MAX, -FLT_MAX );

		for( s32 i = 0, n = ( s32 )uv.size(); i < n; i++ ) {
			min.x = min2( min.x, uv[i].x );
			max.x = max2( max.x, uv[i].x );
			min.y = min2( min.y, uv[i].y );
			max.y = max2( max.y, uv[i].y );
		}

		return max - min;
	}

	// ** UvGenerator::calculateBoundingRect
	template<typename TMesh, typename TChartifier, typename TRectanglePacker>
	Vec2 UvGenerator<TMesh, TChartifier, TRectanglePacker>::calculateBoundingRect( const typename TMesh::Indices& indices, const typename TMesh::Vertices& vertices, Vec2& min, Vec2& max ) const
	{
		min = Vec2(  FLT_MAX,  FLT_MAX );
		max = Vec2( -FLT_MAX, -FLT_MAX );

		for( u32 i = 0, n = ( u32 )indices.size(); i < n; i++ ) {
			const Vec2& uv = vertices[indices[i]].uv[m_uvLayer];
			min.x = min2( uv.x, min.x );
			min.y = min2( uv.y, min.y );
			max.x = max2( uv.x, max.x );
			max.y = max2( uv.y, max.y );
		}

		return max - min;
	}

	// ** UvGenerator::pack
	template<typename TMesh, typename TChartifier, typename TRectanglePacker>
	Vec2 UvGenerator<TMesh, TChartifier, TRectanglePacker>::pack( void )
	{
		float w = 1;
		float h = 1;
		bool expandWidth = true;

		while( !m_packer.place( w, h ) ) {
			if( expandWidth ) {
				w += 0.1f;
				expandWidth = false;
			} else {
				h += 0.1f;
				expandWidth = true;
			}
		}

		return Vec2( w, h );
	}

	// ** UvGenerator::rotate
	template<typename TMesh, typename TChartifier, typename TRectanglePacker>
	void UvGenerator<TMesh, TChartifier, TRectanglePacker>::rotate( UvSet& uv ) const
	{
		Samples<Vec2>	 samples = Samples<Vec2>( uv ).centered();
		CovMatrix<float> cov	 = covarianceMatrix<float>( samples, 2 );
		Vector<float>	 e1( 2 ), e2( 2 );

		e1[0] = 1; e1[1] = 1;
		e1 = cov.converge( e1 );
		e1.normalize();

		e2[0] = e1[1]; e2[1] = -e1[0];

		UvSet rotated = uv;

		for( s32 i = 0, n = ( s32 )rotated.size(); i < n; i++ ) {
			Vec2& v = rotated[i];
			v = Vec2( e1[0] * v.x + e1[1] * v.y, e2[0] * v.x + e2[1] * v.y );
		}

		Vec2 beforeRotationMin, beforeRotationMax;
		Vec2 afterRotationMin,  afterRotationMax;

		Vec2 beforeSize = calculateBoundingRect( uv, beforeRotationMin, beforeRotationMax );
		Vec2 afterSize  = calculateBoundingRect( rotated, afterRotationMin, afterRotationMax );

		if( (afterSize.x * afterSize.y) < (beforeSize.x * beforeSize.y) ) {
			uv = rotated;
		}
	}

	// ** UvGenerator::generate
	template<typename TMesh, typename TChartifier, typename TRectanglePacker>
	void UvGenerator<TMesh, TChartifier, TRectanglePacker>::generate( typename TMesh::Vertices& vertices, typename TMesh::Indices& indices )
	{
		m_indices  = m_mesh.indices();
		m_vertices = m_mesh.vertices();

		TMesh					mesh( m_vertices, m_indices );
		typename TMesh::Indexer	indexer;
		Vec2					min, max;

		// ** Split the mesh into the charts
		typename TChartifier::Result charts = mesh.charts( m_chartifier );

		Array<typename TMesh::Indices> chartVertices;
		chartVertices.resize( charts.m_charts.size() );

		for( s32 i = 0, n = ( s32 )charts.m_charts.size(); i < n; i++ )
		{
			// ** Get the chart.
			typename TMesh::Chart& chart = charts.m_charts[i];

			// ** Flatten the mesh chart by projecting to the ordinal axis of it's normal.
			UvSet uv = flatten( chart );

			rotate( uv );

			// ** Calculate the resulting UV bounding rect.
			Vec2 size = calculateBoundingRect( uv, min, max );

			// ** Output a new set of vertices with this UV set.
			for( s32 j = 0; j < chart.faceCount(); j++ ) {
				typename TMesh::Face face = chart.face( j );

				for( int k = 0; k < 3; k++ ) {
					typename TMesh::Vertex vtx = face.vertex( k );

					Vec2 v = uv.front();
					uv.erase( uv.begin() );

					if( size.x > size.y ) {
						vtx.uv[m_uvLayer] = Vec2( v.x - min.x, v.y - min.y );
					} else {
						vtx.uv[m_uvLayer] = Vec2( v.y - min.y, v.x - min.x );
					}
					chartVertices[i].push_back( indexer += vtx );
				}
			}
		}

		m_vertices = indexer.vertexBuffer();
		m_indices  = indexer.indexBuffer();

		for( s32 i = 0, n = ( s32 )charts.m_charts.size(); i < n; i++ )
		{
			const typename TMesh::Chart& chart = charts.m_charts[i];
			Vec2 size = calculateBoundingRect( chartVertices[i], m_vertices, min, max );

			m_packer.add( max2( size.x, size.y ), min2( size.x, size.y ) );
		}

		// ** Pack the chart rectangles.
		pack();

		indexer = TMesh::Indexer();

		for( s32 i = 0, n = ( s32 )charts.m_charts.size(); i < n; i++ )
		{
			const typename TRectanglePacker::Rect& rect = m_packer.rect( i );

			for( u32 j = 0, nv = ( u32 )chartVertices[i].size(); j < nv; j++ ) {
				typename TMesh::Vertex vtx = m_vertices[chartVertices[i][j]];

				vtx.uv[m_uvLayer] = Vec2( rect.x + vtx.uv[m_uvLayer].x, rect.y + vtx.uv[m_uvLayer].y);
				indexer += vtx;
			}
		}

		vertices = indexer.vertexBuffer();
		indices  = indexer.indexBuffer();

		// ** Normalize the UV set.
		Vec2 size = calculateBoundingRect( indices, vertices, min, max );

		for( s32 i = 0, n = ( s32 )vertices.size(); i < n; i++ ) {
			Vec2& uv = vertices[i].uv[m_uvLayer];

			f32 u = uv.x;
			f32 v = uv.y;

			DC_BREAK_IF( u < min.x || v < min.y );
			DC_BREAK_IF( u > max.x || v > max.y );

			uv.x = (u - min.x) / (max.x - min.x);
			uv.y = (v - min.y) / (max.y - min.y);

			DC_BREAK_IF( uv.x < 0.0f || uv.x > 1.0f );
			DC_BREAK_IF( uv.y < 0.0f || uv.y > 1.0f );
		}
	}

DC_END_DREEMCHEST

#endif  /*  !defined( __Base_Mesh_H__ )  */
