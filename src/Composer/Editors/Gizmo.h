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

#ifndef __DC_Composer_Gizmo_H__
#define __DC_Composer_Gizmo_H__

#include "../Composer.h"

DC_BEGIN_COMPOSER

	//! Grid component used for grid rendering in visual editors.
	class Grid : public Ecs::Component<Grid> {
	public:

		//! Available grid features.
		enum Features {
			  Cells		= BIT( 0 )	//!< Cells will be rendered.
			, Basis		= BIT( 1 )	//!< Basis will be rendered.
			, Outline	= BIT( 2 )	//!< Outline will be rendered
			, Axes		= BIT( 3 )	//!< Axes will be rendered.
		};

							Grid( f32 size = 100.0f, f32 cellSize = 1.0f, u8 features = ~0 )
								: m_size( size ), m_cellSize( cellSize ), m_features( features ) {}

		//! Returns grid size.
		f32					size( void ) const;

		//! Returns grid cell size.
		f32					cellSize( void ) const;

		//! Returns grid features.
		const FlagSet8&		features( void ) const;

	private:

		f32					m_size;		//!< Grid size.
		f32					m_cellSize;	//!< Cell size.
		FlagSet8			m_features;	//!< Grid rendering features.
	};

	//! Shared transformation gizmo data.
	struct Gizmo {
		//! Transformation types.
		enum Transform {
			  Nothing			//!< Transform nothing by this gizmo.
			, X					//!< Transform X axis.
			, Y					//!< Transform Y axis.
			, Z					//!< Transform Z axis.
			, XY				//!< Transform X & Y axes.
			, YZ				//!< Transform Y & Z axes.
			, XZ				//!< Transfrom X & Z axes.
			
			, TotalTransforms	//!< The total number of gizmo transforms.
		};

		//! Available gizmo states.
		enum State {
			  Idle		//!< Gizmo does not have any highlighted parts & not locked.
			, Active	//!< Gizmo has a highlighted part.
			, Locked	//!< Gizmo is now being transformed.
		};

						//! Constructs Gizmo instance.
						Gizmo( void )
							: state( Idle ), transform( Nothing ) {}

		Vec3			offset;		//!< Gizmo offset.
		State			state;		//!< Current gizmo state.
		Transform		transform;	//!< Transformation applied by gizmo.
		Plane			plane;		//!< Plane to use for transformations.
	};

	//! Translation gizmo component.
	class TranslationGizmo : public Ecs::Component<TranslationGizmo> {
	public:

							//! Construct TranslationGizmo instance.
							TranslationGizmo( f32 orthRadius = 0.05f, f32 scalingFactor = 0.2f, f32 selectorScale = 0.35f );

		//! Returns the scaling factor of a gizmo.
		f32					scalingFactor( void ) const;

		//! Returns the orth radius.
		f32					orthRadius( void ) const;

		//! Returns the selector bounding box.
		Bounds				selectorBoundingBox( Gizmo::Transform transform ) const;

		//! Returns the active transform.
		Gizmo::Transform	activeTransform( void ) const;

		//! Sets active transform.
		void				setActiveTransform( Gizmo::Transform value );

		//! Returns gizmo state.
		Gizmo::State		state( void ) const;

		//! Locks this gizmo with specified origin point.
		void				lock( const Plane& plane, const Vec3& origin );

		//! Unlocks this gizmo.
		void				unlock( void );

		//! Returns grabbed offset from gizmo position to mouse point.
		const Vec3&			offset( void ) const;

		//! Returns active transformation plane.
		const Plane&		plane( void ) const;

	private:

		f32					m_scalingFactor;	//!< The gizmo scaling factor.
		f32					m_selectorScale;	//!< The selector plane size.
		f32					m_orthRadius;		//!< Orth radius.
		Gizmo				m_gizmo;			//!< Actual gizmo data.
	};

	//! Translates the transforms by a translation gizmo.
	class TranslationGizmoSystem : public Ecs::GenericEntitySystem<TranslationGizmo, Scene::Transform> {
	public:

									//! Constructs TranslationGizmoSystem instance
									TranslationGizmoSystem( Scene::SpectatorCameraWPtr camera );

		//! Sets cursor position.
		void						setCursor( u32 x, u32 y, u8 buttons );

		//! Processes the single translation gizmo.
		virtual void				process( u32 currentTime, f32 dt, Ecs::Entity& entity, TranslationGizmo& gizmo, Scene::Transform& transform ) DC_DECL_OVERRIDE;

	private:

		//! Returns the selection from a mouse ray.
		Gizmo::Transform			findTransformByRay( TranslationGizmo& gizmo, f32 scale, const Ray& ray ) const;

		//! Selects the best matching plane from a ray.
		Plane						findBestPlane( Gizmo::Transform transform, const Vec3& position, const Ray& ray ) const;

	private:

		Vec2						m_cursor;	//!< Last known cursor position.
		Scene::SpectatorCameraWPtr	m_camera;	//!< Camera instance used for ray casting.
		FlagSet8					m_buttons;	//!< Mouse buttons state.
	};

	//! Scene helpers rendering component.
	class RenderSceneHelpers : public Ecs::Component<RenderSceneHelpers> {
	public:
	};

	//! Renders the grid.
	class GridPass : public Scene::RenderPass<Grid> {
	public:

						//! Constructs GridPass instance.
						GridPass( Ecs::EcsWPtr ecs )
							: RenderPass( ecs, "GridPass" ) {}

	protected:

		//! Renders the mesh of a single grid instance.
		virtual void	render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const Grid& grid, const Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Renders grid with specified cell size and dimensions.
		void			renderGrid( Renderer::Renderer2DWPtr renderer, f32 cellSize, f32 size, const Rgba& color ) const;

		//! Renders origin cross.
		void			renderCross( Renderer::Renderer2DWPtr renderer, f32 size, const Rgba& color ) const;
	};

	//! Renders active translation gizmos.
	class TranslationGizmoPass : public Scene::RenderPass<TranslationGizmo> {
	public:

						//! Constructs TranslationGizmoPass instance.
						TranslationGizmoPass( Ecs::EcsWPtr ecs )
							: RenderPass( ecs, "TranslationGizmoPass" ) {}

	protected:

		//! Renders the mesh of a single translation gizmo.
		virtual void	render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const TranslationGizmo& gizmo, const Scene::Transform& transform ) DC_DECL_OVERRIDE;
	};

	//! Scene editor helpers renderer type.
	class SceneHelpersRenderer : public Scene::RenderingSystem<RenderSceneHelpers> {
	public:

								SceneHelpersRenderer( Ecs::EcsWPtr ecs )
									: RenderingSystem( ecs, "SceneHelpersRenderer" )
								{
									addPass<TranslationGizmoPass>();
									addPass<GridPass>();
								}
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Gizmo_H__	*/