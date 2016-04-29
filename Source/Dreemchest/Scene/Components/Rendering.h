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
#include "../Assets/Mesh.h"
#include "../Assets/Image.h"
#include "../Assets/Material.h"
#include "../Rendering/RenderTarget.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! This component is attached to a camera to render unlit scene.
	class RenderUnlit : public Ecs::Component<RenderUnlit> {
	};

	//! This component is attached to a camera to render forward lit meshes.
	class RenderForwardLit : public Ecs::Component<RenderForwardLit> {
	};

	//! This component is attached to a camera to render scene wireframe.
	class RenderWireframe : public Ecs::Component<RenderWireframe> {
	};

	//! This component is attached to a camera to render debug grid.
	class RenderGrid : public Ecs::Component<RenderGrid> {
	};

	//! This component is attached to a camera to render vertex normals.
	class RenderVertexNormals : public Ecs::Component<RenderVertexNormals> {
	};

	//! This component is attached to a camera to render sprites.
	class RenderSprites : public Ecs::Component<RenderSprites> {
	};

	//! This component is attached to a camera to debug render sprite transforms.
	class RenderSpriteTransform : public Ecs::Component<RenderSpriteTransform> {
	};

	//! This component is attached to a camera to debug scene object bounding volumes
	class RenderBoundingVolumes : public Ecs::Component<RenderBoundingVolumes> {
	};

	//! This component is attached to a camera to render a depth complexity
	class RenderDepthComplexity : public Ecs::Component<RenderDepthComplexity> {
    public:

                                //! Constructs the RenderDepthComplexity instance with a same color for all rendering modes.
                                RenderDepthComplexity( const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f ), f32 intensity = 0.1f );

        //! Returns the output color for specified rendering mode.
        const Rgba&             colorForMode( RenderingMode mode ) const;

        //! Sets the output color for specified rendering mode.
        void                    setColorForMode( RenderingMode mode, const Rgba& value );

        //! Returns the color intensity.
        f32                     intensity( void ) const;

        //! Sets the color intensity.
        void                    setIntensity( f32 value );

    private:

        Rgba                    m_color[RenderingMode::Total];  //!< Output colors used for each of rendering modes.
        f32                     m_intensity;                    //!< The color intensity.
	};

	//! This component is attached to a camera to render an ambient lighting.
	class RenderAmbient : public Ecs::Component<RenderAmbient> {
	};

	//! This component is attached to a camera to render particles.
	class RenderParticles : public Ecs::Component<RenderParticles> {
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
									Light( LightType type = LightType::Point, const Rgb& color = Rgb( 1.0f, 1.0f, 1.0f ), f32 intensity = 1.0f, f32 range = 1.0f )
										: m_type( type ), m_color( color ), m_intensity( intensity ), m_range( range ) {}

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

	private:

		LightType				    m_type;			//!< Light type.
		Rgb							m_color;		//!< Light color.
		f32							m_intensity;	//!< Light intensity.
		f32							m_range;		//!< Light influence range.
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
                                    #if DEV_DEPRECATED_SCENE_RENDERER
                                        , m_visibility( ~0 )
                                    #endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */
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

    #if DEV_DEPRECATED_SCENE_RENDERER
		//! Returns true if the mesh is visible by camera.
		bool						isVisible( u8 camera ) const;

		//! Marks the mesh as visible from camera.
		void						setVisibilityMask( u16 mask, bool value );
    #endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */

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

    #if DEV_DEPRECATED_SCENE_RENDERER
		FlagSet16					m_visibility;		//!< Camera visibility mask.
    #endif  /*  #if DEV_DEPRECATED_SCENE_RENDERER   */
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
									Sprite( ImageHandle image = ImageHandle(), const Rgba& color = Rgba( 1.0f, 1.0f, 1.0f, 1.0f ) )
										: m_image( image ), m_color( color ) {}

		//! Returns the sprite image.
		ImageHandle				    image( void ) const;

		//! Returns the sprite color.
		const Rgba&					color( void ) const;

	private:

		ImageHandle					m_image;	//!< Sprite image.
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
									Camera( Projection projection = Projection::Perspective, RenderTargetWPtr target = RenderTargetWPtr(), const Rgba& clearColor = Rgba(), const Rect& ndc = Rect( 0.0f, 0.0f, 1.0f, 1.0f ) )
										: m_clearMask( ClearAll ), m_id( -1 ), m_projection( projection ), m_ndc( ndc ), m_target( target ), m_clearColor( clearColor ), m_fov( 60.0f ), m_near( 0.01f ), m_far( 1000.0f ) {}

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

		//! Returns the camera id.
		u8							id( void ) const;

		//! Sets camera id.
		void						setId( u8 value );

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

		//! Calculates the output viewport coordinates.
		Rect						viewport( void ) const;

		//! Sets the camera render view.
		void						setTarget( RenderTargetWPtr value );

		//! Returns the camera render view.
		RenderTargetWPtr		    target( void ) const;

		//! Calculates the projection matrix.
		Matrix4						calculateProjectionMatrix( void ) const;

		//! Calculates the view projection matrix.
		Matrix4						calculateViewProjection( const Matrix4& transform ) const;

		//! Converts from screen space to world space.
		bool						toWorldSpace( const Vec3& screen, Vec3& world, const Matrix4& transform ) const;

		//! Converts from world space to screen space.
		bool						pointToScreenSpace( const Vec3& world, Vec3& screen, const Matrix4& transform ) const;

		//! Projects the bounding sphere to a screen space.
		Circle						sphereToScreenSpace( const Sphere& sphere, const TransformWPtr& transform ) const;

	private:

		u8							m_clearMask;	//!< Camera clear flags.
		u8							m_id;			//!< Unique camera id.
		Projection					m_projection;	//!< Camera projection.
		Rect						m_ndc;			//!< Output normalized device coordinates.
		RenderTargetWPtr		    m_target;		//!< Rendering target.
		Rgba						m_clearColor;	//!< The clear color.
		f32							m_fov;			//!< Camera field of view.
		f32							m_near;			//!< Z-near value.
		f32							m_far;			//!< Z-far value.
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Component_Rendering_H__    */