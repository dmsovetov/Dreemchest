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

#ifndef __DC_Scene_StaticMeshEmitter_H__
#define __DC_Scene_StaticMeshEmitter_H__

#include "RopEmitter.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

#if DEV_VIRTUAL_EMITTERS
	//! Emits render operations for static meshes in scene.
	class StaticMeshEmitter : public RopEmitter<StaticMesh> {
	public:

								//! Constructs StaticMeshEmitter instance.
								StaticMeshEmitter( RenderingContext& context, u32 renderModes = AllRenderModesBit );

	private:

		//! Emits render operations for all static meshes in scene.
		virtual void			emit( const Vec3& camera, const StaticMesh& staticMesh, const Transform& transform ) DC_DECL_OVERRIDE;

    private:

        u32                     m_renderModes;      //!< Material render modes that will emit render operations.
	};
#else
    //! Stores info about a single renderable mesh.
    struct StaticMeshRenderable {
        const Transform*        transform;          //!< Mesh transform component.
        const Matrix4*          matrix;             //!< Mesh transform affine matrix.
        MaterialHandle          material;           //!< Material used for rendering.
        MeshHandle              mesh;
        s32                     renderable;         //!< Renderable index.
        s32                     technique;          //!< Technique index.
    };

    //! Emits render operations for static meshes in scene.
    class StaticMeshEmitter : public FixedRopEmitter<StaticMesh, StaticMeshRenderable> {
 	public:

								        //! Constructs StaticMeshEmitter instance.
								        StaticMeshEmitter( RenderingContext& context, u32 renderModes = AllRenderModesBit );

    protected:

        //! Emits render operations for all visible meshes.
        virtual void			        emit( const Vec3& camera ) DC_DECL_OVERRIDE;

        //! Constructs mesh render entity from a scene entity.
        virtual StaticMeshRenderable    createRenderEntity( const Ecs::Entity& entity );

    private:

        u32                             m_renderModes;      //!< Material render modes that will emit render operations.
    };
#endif  /*  DEV_VIRTUAL_EMITTERS    */

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_StaticMeshEmitter_H__    */