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

#ifndef __DC_Composer_TerrainEditing_H__
#define __DC_Composer_TerrainEditing_H__

#include "../../Composer.h"

DC_BEGIN_COMPOSER

	//! Terrain editing tool.
	class TerrainTool : public Ecs::Component<TerrainTool> {
	public:

		//! Available tool types.
		enum Type {
			  Raise
			, Lower
			, Flatten
			, Level
			, Smooth
		};

							//! Constructs TerrainTool instance.
							TerrainTool( Scene::TerrainWPtr terrain = Scene::TerrainWPtr(), f32 radius = 1.0f, f32 strength = 0.1f, f32 exp = 1.0f );

		//! Returns tool radius.
		f32					radius( void ) const;

		//! Sets tool radius.
		void				setRadius( f32 value );

		//! Returns tool strength.
		f32					strength( void ) const;

		//! Sets tool strength.
		void				setStrength( f32 value );

		//! Returns the affected terrain.
		Scene::TerrainWPtr	terrain( void ) const;

		//! Returns the influence at specified distance.
		f32					influence( f32 distance ) const;

		//! Returns tool type.
		Type				type( void ) const;

		//! Sets tool type.
		void				setType( Type value );

        //! Returns tool exponentiation factor.
        f32                 exp( void ) const;

        //! Sets tool exponentiation factor.
        void                setExp( f32 value );

	private:

		f32					m_radius;	//!< Terrain tool radius.
		f32					m_strength;	//!< Terrain tool strength.
		Type				m_type;		//!< Tool type.
        f32                 m_exp;      //!< Tool exponentiation factor.

		Scene::TerrainWPtr	m_terrain;	//!< Affected terrain instance.
	};

	//! Used for terrain heightmap editing.
	class TerrainHeightmapSystem : public Scene::GenericTouchSystem<TerrainHeightmapSystem, Editors::TerrainChunk, Scene::StaticMesh, Scene::Transform> {
	public:

							//! Constructs TerrainHeightmapSystem instance.
							TerrainHeightmapSystem( Scene::SceneObjectWPtr tool, Scene::ViewportWPtr viewport );

	protected:

		//! Edits the terrain chunk.
		virtual void		touchMovedEvent( Scene::Viewport::TouchMoved& e, Ecs::Entity& entity, Editors::TerrainChunk& chunk, Scene::StaticMesh& mesh, Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Returns true if the point is inside the chunk.
		bool				isPointInside( const Vec3& point, f32 radius, const Editors::TerrainChunk& chunk ) const;

        //! Applies the lowering tool.
        f32                 applyLowering( const Vec3& vertex, s32 x, s32 z, Scene::TerrainWPtr terrain, f32 influence, f32 strength ) const;

        //! Applies the raising tool.
        f32                 applyRaising( const Vec3& vertex, s32 x, s32 z, Scene::TerrainWPtr terrain, f32 influence, f32 strength ) const;

        //! Applies the flattening tool.
        f32                 applyFlattening( const Vec3& vertex, s32 x, s32 z, Scene::TerrainWPtr terrain, f32 influence, f32 strength ) const;

        //! Applies the leveling tool.
        f32                 applyLeveling( const Vec3& vertex, s32 x, s32 z, Scene::TerrainWPtr terrain, f32 influence, f32 strength ) const;

        //! Applies the smoothing tool.
        f32                 applySmoothing( const Vec3& vertex, s32 x, s32 z, Scene::TerrainWPtr terrain, f32 influence, f32 strength ) const;

        //! Returns an average terrain height around the specified point with given radius.
        f32                 calculateAverageHeight( Scene::TerrainWPtr terrain, s32 x, s32 z, s32 radius ) const;

	private:

		Scene::SceneObjectWPtr	m_tool;		//!< Terrain tool instance.
	};

	//! Renders the terrain tool circle.
	class TerrainToolPass : public Scene::RenderPass<TerrainTool> {
	public:

						//! Constructs TerrainToolPass instance.
						TerrainToolPass( Ecs::EcsWPtr ecs )
							: RenderPass( ecs, "TerrainToolPass" ) {}

	protected:

		//! Renders terrain tool circle
		virtual void	render( Scene::RenderingContextPtr context, Scene::Rvm& rvm, Scene::ShaderCache& shaders, const TerrainTool& tool, const Scene::Transform& transform ) DC_DECL_OVERRIDE;
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_TerrainEditing_H__	*/