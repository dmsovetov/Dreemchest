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

#ifndef __DC_Scene_Component_Rendering_H__
#define __DC_Scene_Component_Rendering_H__

#include "../Scene.h"
#include "../Viewport.h"
#include "../Assets/Mesh.h"
#include "../Assets/Image.h"
#include "../Assets/Material.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! This component is attached to a camera to render sprites.
	class SpriteRenderer : public Ecs::Component<SpriteRenderer> {
	public:

										//! Constructs a SpriteRenderer instance.
										SpriteRenderer( f32 scaleFactor = 1.0f );

		//! Returns a sprite rendering scale factor.
		f32								scaleFactor( void ) const;

		//! Sets a sprite rendering scale factor.
		void							setScaleFactor( f32 value );

	private:

		f32								m_scaleFactor;		//!< A global scale factor that is applied to rendered sprites.
	};

	//! This component is attached to a camera to render forward lit meshes.
	class ForwardRenderer : public Ecs::Component<ForwardRenderer> {
    public:

                                        //! Constructs a ForwardRenderer instance.
                                        ForwardRenderer( s32 shadowSize = 1024.0f, s32 shadowCascadeCount = 3 );

        //! Returns a shadow texture size.
        s32                             shadowSize( void ) const;

        //! Sets a shadow texture size.
        void                            setShadowSize( s32 value );

        //! Returns a total number of cascades used for directional light sources.
        s32                             shadowCascadeCount( void ) const;

        //! Sets a total number of cascades used for directinal light sources.
        void                            setShadowCascadeCount( s32 value );

        //! Returns true if the debug rendering of cascade shadows is enabled.
        bool                            isDebugCascadeShadows( void ) const;

        //! Enables or disables the debug rendering of a cascade shadows.
        void                            setDebugCascadeShadows( bool value );

        //! A parameter value used by a splitting planes calculation function.
        f32                             shadowCascadeLambda( void ) const;

        //! Sets a parameter value used by a splitting planes calculation function, should be in [0, 1] range.
        void                            setShadowCascadeLambda( f32 value );

    private:

        s32                             m_shadowSize;           //!< A shadow texture size.
        s32                             m_shadowCascadeCount;   //!< A total number of cascades a camera frustum is split to.
        f32                             m_shadowCascadeLambda;  //!< An interpolation factor between linear and logarithmic splitting schemes.
        bool                            m_debugCascadeShadows;  //!< Enables a debug rendering of cascaded shadowmaps.
	};

    //! This component is attached to a camera to render a debug info.
    class DebugRenderer : public Ecs::Component<DebugRenderer> {
    public:

                                        //! Constructs a DebugRenderer instance.
                                        DebugRenderer( void );

        //! Returns a static mesh color.
        const Rgba&                     staticMeshColor( void ) const;

        //! Sets a static mesh color.
        void                            setStaticMeshColor( const Rgba& value );

        //! Returns a light color.
        const Rgba&                     lightColor( void ) const;

        //! Sets a light color.
        void                            setLightColor( const Rgba& value );

        //! Returns a camera color.
        const Rgba&                     cameraColor( void ) const;

        //! Sets a camera color.
        void                            setCameraColor( const Rgba& value );

    private:

        Rgba                            m_staticMeshColor;  //!< A color of all static mesh bounding boxes.
        Rgba                            m_lightColor;       //!< A color of all light fixtures.
        Rgba                            m_cameraColor;      //!< A color of all camera frustums.
    };

	//! Available light types.
    NIMBLE_DECLARE_ENUM( LightType, Point, Spot, Directional )

	//! Holds the light information.
	class Light : public Ecs::Component<Light> {

        INTROSPECTION_SUPER( Light, Ecs::ComponentBase
            , PROPERTY( type,      type,      setType,      "The light type."            )
            , PROPERTY( color,     color,     setColor,     "The light color."           )
            , PROPERTY( intensity, intensity, setIntensity, "The light intensity."       )
            , PROPERTY( range,     range,     setRange,     "The light influence range." )
            )

	public:

									//! Constructs Light instance.
									Light( LightType type = LightType::Point, const Rgb& color = Rgb( 1.0f, 1.0f, 1.0f ), f32 intensity = 1.0f, f32 range = 1.0f );

		//! Returns the light type.
		LightType				    type( void ) const;

		//! Sets the light type.
		void						setType( LightType value );

		//! Returns the light color.
		const Rgb&					color( void ) const;

		//! Sets the light color.
		void						setColor( const Rgb& value );

		//! Returns the light intensity.
		f32							intensity( void ) const;

		//! Sets the light intensity.
		void						setIntensity( f32 value );

		//! Returns the light influence range.
		f32							range( void ) const;

		//! Sets the light influence range.
		void						setRange( f32 value );

        //! Returns a light cutoff value.
        f32                         cutoff( void ) const;

        //! Sets a light cutoff value.
        void                        setCutoff( f32 value );

        //! Returns true if a light casts shadows.
        bool                        castsShadows( void ) const;

        //! Sets a shadow casting flag.
        void                        setCastsShadows( bool value );

	private:

		LightType				    m_type;			//!< Light type.
		Rgb							m_color;		//!< Light color.
		f32							m_intensity;	//!< Light intensity.
		f32							m_range;		//!< Light influence range.
        f32                         m_cutoff;       //!< Light spot cutoff value.
        bool                        m_castsShadow;  //!< Indicates that a light casts shadows.
	};

	//! Holds the static mesh data with per-instance materials.
	class StaticMesh : public Ecs::Component<StaticMesh> {

        INTROSPECTION_SUPER( StaticMesh, Ecs::ComponentBase
            , PROPERTY( mesh, mesh, setMesh, "The mesh asset ")
            )

	public:

									//! Constructs StaticMesh instance.
									StaticMesh( const MeshHandle mesh = MeshHandle() )
										: m_mesh( mesh )
                                        {
                                        }

		//! Returns mesh to be rendered.
		const MeshHandle&		    mesh( void ) const;

		//! Sets a mesh to be rendered.
		void						setMesh( const MeshHandle& value );

		//! Returns the mesh world space bounding box.
		const Bounds&				worldSpaceBounds( void ) const;

		//! Sets the mesh world space bounding box.
		void						setWorldSpaceBounds( const Bounds& value );

		//! Returns the total number of materials.
		u32							materialCount( void ) const;

		//! Returns the material by index.
		const MaterialHandle&	    material( u32 index ) const;

		//! Sets the material by index.
		void						setMaterial( u32 index, MaterialHandle value );

		//! Returns a lightmap texture.
		const Renderer::TexturePtr&	lightmap( void ) const;

		//! Sets a lightmap texture.
		void						setLightmap( const Renderer::TexturePtr& value );

	private:

		MeshHandle				    m_mesh;				//!< Mesh to be rendered.
		Bounds						m_worldSpaceBounds;	//!< Mesh world space bounding box.
		Array<MaterialHandle>	    m_materials;		//!< Mesh materials array.
		Renderer::TexturePtr		m_lightmap;			//!< Lightmap texture that is rendered for this mesh.
	};

    // ** StaticMesh::materialCount
    NIMBLE_INLINE u32 StaticMesh::materialCount( void ) const
    {
	    return static_cast<u32>( m_materials.size() );
    }

    // ** StaticMesh::material
    NIMBLE_INLINE const MaterialHandle& StaticMesh::material( u32 index ) const
    {
        static MaterialHandle Invalid;
	    return index < materialCount() ? m_materials[index] : Invalid;
    }

    //! Holds a point cloud.
    class PointCloud : public Ecs::Component<PointCloud> {
    public:

                                //! Constructs PointCloud instance.
                                PointCloud( s32 vertexCount = 0, const VertexFormat& format = VertexFormat::Color | VertexFormat::Normal );
                                ~PointCloud( void );

        //! Resizes a point cloud.
        void                    resize( s32 vertexCount );

        //! Clears a point cloud.
        void                    clear( void );

        //! Returns a point cloud material.
        const MaterialHandle&   material( void ) const;

        //! Sets a point cloud material.
        void                    setMaterial( const MaterialHandle& value );

        //! Returns a vertex format.
        const VertexFormat&     vertexFormat( void ) const;

        //! Sets a vertex format.
        void                    setVertexFormat( const VertexFormat& value );

        //! Returns a total number of points inside a cloud.
        s32                     vertexCount( void ) const;

        //! Returns point cloud vertices.
        const void*             vertices( void ) const;
        void*                   vertices( void );

    private:

        VertexFormat            m_format;       //!< A point cloud vertex format.
        u8*                     m_vertices;     //!< A point cloud vertices.
        s32                     m_vertexCount;  //!< A total number of vertices inside a point cloud.
        MaterialHandle          m_material;     //!< A point cloud material instance.
    };

	//! Holds the sprite rendering info.
	class Sprite : public Ecs::Component<Sprite> {
	public:

									//! Constructs the Sprite instance.
									Sprite( s32 width = 0, s32 height = 0, MaterialHandle material = MaterialHandle(), const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) )
										: m_width( width )
										, m_height( height )
										, m_material( material )
										, m_color( color ) {}

		//! Returns a sprite width.
		s32							width( void ) const;

		//! Returns a sprite height.
		s32							height( void ) const;

		//! Returns the sprite material.
		MaterialHandle				material( void ) const;

		//! Returns the sprite color.
		const Rgba&					color( void ) const;

	private:

		s32							m_width;	//!< A sprite width.
		s32							m_height;	//!< A sprite height.
		MaterialHandle				m_material;	//!< Sprite material.
		Rgba						m_color;	//!< Sprite color.
	};

	//! Holds the particle system instance to be rendered.
	class Particles : public Ecs::Component<Particles> {
	public:

										//! Constructs Particles instance.
										Particles( const Fx::ParticleSystemPtr& particleSystem = Fx::ParticleSystemPtr(), const Fx::ParticleSystemInstancePtr& instance = Fx::ParticleSystemInstancePtr() );

		//! Returns particle system instance.
		Fx::ParticleSystemInstanceWPtr	instance( void ) const;

		//! Returns particle system.
		Fx::ParticleSystemWPtr			particles( void ) const;

		//! Returns the material particles material.
		MaterialHandle					material( void ) const;

		//! Sets the particles material.
		void							setMaterial( MaterialHandle value );

	private:

		
		Fx::ParticleSystemPtr			m_particleSystem;	//!< Particle system.
		Fx::ParticleSystemInstancePtr	m_instance;			//!< Particle system instance.
		MaterialHandle				    m_material;			//!< Particle system material.
	};

    //! Available camera projections.
    NIMBLE_DECLARE_ENUM( Projection, Perspective, Ortho, OrthoCenter )

	//! Camera component.
	class Camera : public Ecs::Component<Camera> {

        INTROSPECTION_SUPER( Camera, Ecs::ComponentBase
            , PROPERTY( projection,  projection, setProjection, "The camera projection."            )
            , PROPERTY( clearColor,  clearColor, setClearColor, "The viewport clear color."         )
            , PROPERTY( fieldOfView, fov,        setFov,        "The camera field of view."         )
            , PROPERTY( near,        near,       setNear,       "The camera near clipping plane."   )
            , PROPERTY( far,         far,        setFar,        "The camera far clipping plane."    )
            )

	public:

		//! Camera clear flags.
		enum ClearFlags {
			  ClearColor	= BIT( 0 )					//!< Camera will clear the color buffer.
			, ClearDepth	= BIT( 1 )					//!< Camera will clear the depth buffer.
			, ClearAll		= ClearColor | ClearDepth	//!< Camera will clear all buffers.
		};

									//! Constructs Camera instance.
									Camera( Projection projection = Projection::Perspective, const Rgba& clearColor = Rgba(), const Rect& ndc = Rect( 0.0f, 0.0f, 1.0f, 1.0f ) );

		//! Returns camera clear mask.
		u8							clearMask( void ) const;

		//! Sets camera clear mask.
		void						setClearMask( u8 value );

		//! Sets the clear color.
		void						setClearColor( const Rgba& value );

		//! Returns the clear color.
		const Rgba&					clearColor( void ) const;

        //! Returns camera projection.
        Projection                  projection( void ) const;

        //! Sets camera projection.
        void                        setProjection( Projection value );

		//! Returns camera field of view.
		f32							fov( void ) const;

		//! Sets camera field of view.
		void						setFov( f32 value );

		//! Returns the Z-near value.
		f32							near( void ) const;

		//! Sets the Z-near value.
		void						setNear( f32 value );

		//! Returns the Z-far value.
		f32							far( void ) const;

		//! Sets the Z-far value.
		void						setFar( f32 value );

		//! Sets the normalized device coordinates to render frame to.
		void						setNdc( const Rect& value );

		//! Returns the normalized device coordinates to render frame to.
		const Rect&					ndc( void ) const;

		//! Calculates the projection matrix.
		static Matrix4			    calculateProjectionMatrix( const Camera& camera, const Viewport& viewport );

		//! Calculates the view projection matrix.
		static Matrix4			    calculateViewProjection( const Camera& camera, const Viewport& viewport, const Matrix4& transform );

    #if 0
		//! Converts from screen space to world space.
		bool						toWorldSpace( const Vec3& screen, Vec3& world, const Matrix4& transform ) const;

		//! Converts from world space to screen space.
		bool						pointToScreenSpace( const Vec3& world, Vec3& screen, const Matrix4& transform ) const;

		//! Projects the bounding sphere to a screen space.
		Circle					sphereToScreenSpace( const Sphere& sphere, const TransformWPtr& transform ) const;
    #endif

	private:

		u8							m_clearMask;	//!< Camera clear flags.
		Projection					m_projection;	//!< Camera projection.
		Rect						m_ndc;			//!< Output normalized device coordinates.
		Rgba						m_clearColor;	//!< The clear color.
		f32							m_fov;			//!< Camera field of view.
		f32							m_near;			//!< Z-near value.
		f32							m_far;			//!< Z-far value.
	};

    //! A viewport component is attached to camera entities and used to handle a user input.
    class Viewport : public Ecs::Component<Viewport> {
    public:

                                    //! Constructs a Viewport instance.
                                    Viewport( ViewportWPtr viewport = ViewportWPtr() );

                                    ~Viewport( void );

        //! Returns a viewport width.
        s32                         width( void ) const;

        //! Returns a viewport height.
        s32                         height( void ) const;

        //! Returns a viewport apsect ratio.
        f32                         aspect( void ) const;

        //! Returns a viewport instance.
        ViewportWPtr                get( void ) const;

        //! Calculates a viewport rectangle from a normalized viewport.
        Rect                        denormalize( const Rect& normalized ) const;

        //! Queues a new touch began event.
        void                        touchBegan( s32 id, s32 x, s32 y, u8 flags = 0 );

        //! Queues a touch ended event.
        void                        touchEnded( s32 id, s32 x, s32 y, u8 flags = 0 );

        //! Queues a touch moved event.
        void                        touchMoved( s32 id, s32 x, s32 y, s32 dx, s32 dy, u8 flags = 0 );

        //! Returns a total number of queued events.
        s32                         eventCount( void ) const;

        //! Returns an event instance at specified index.
        const InputEvent&           eventAt( s32 index ) const;

        //! Clears all queued events.
        void                        clearEvents( void );

        //! Returns the viewport split by it's coordinates.
        static Rect					calculateSplitRect( u32 x, u32 y, u32 nx, u32 ny );

    private:

        //! Handles a viewport touch began event and queues it.
        void                            handleTouchBegan( const AbstractViewport::TouchBegan& e );

        //! Handles a viewport touch began event and queues it.
        void                            handleTouchMoved( const AbstractViewport::TouchMoved& e );

        //! Handles a viewport touch began event and queues it.
        void                            handleTouchEnded( const AbstractViewport::TouchEnded& e );

    private:

        ViewportWPtr                m_viewport; //!< A scene viewport that is used to render a frame.
        Array<InputEvent>           m_events;   //!< All events recorded by this viewport instance.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Rendering_H__    */