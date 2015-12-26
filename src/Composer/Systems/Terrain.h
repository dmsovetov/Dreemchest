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

#ifndef __DC_Composer_Terrain_H__
#define __DC_Composer_Terrain_H__

#include "../Composer.h"

DC_BEGIN_COMPOSER

	//! Terrain editing tool.
	class TerrainTool : public Ecs::Component<TerrainTool> {
	public:

							//! Constructs TerrainTool instance.
							TerrainTool( Scene::TerrainWPtr terrain = Scene::TerrainWPtr(), f32 radius = 1.0f );

		//! Returns tool radius.
		f32					radius( void ) const;

		//! Sets tool radius.
		void				setRadius( f32 value );

		//! Returns the affected terrain.
		Scene::TerrainWPtr	terrain( void ) const;

	private:

		f32					m_radius;	//!< Terrain tool radius.
		Scene::TerrainWPtr	m_terrain;	//!< Affected terrain instance.
	};

	//! Used for terrain heightmap editing.
	class TerrainHeightmapSystem : public Ecs::GenericEntitySystem<Editors::TerrainChunk, Scene::StaticMesh, Scene::Transform> {
	public:

							//! Constructs TerrainHeightmapSystem instance.
							TerrainHeightmapSystem( Scene::SceneObjectWPtr tool, Editors::CursorWPtr cursor );

	protected:

		//! Edits the terrain chunk.
		virtual void		process( u32 currentTime, f32 dt, Ecs::Entity& entity, Editors::TerrainChunk& chunk, Scene::StaticMesh& mesh, Scene::Transform& transform ) DC_DECL_OVERRIDE;

		//! Finds the heightmap & ray intersection point.
		Vec3				findTerrainIntersection( Scene::TerrainWPtr terrain, const Ray& ray, f32 epsilon = 0.001f ) const;

	private:

		Editors::CursorWPtr		m_cursor;	//!< Cursor binding.
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

#endif	/*	!__DC_Composer_Terrain_H__	*/