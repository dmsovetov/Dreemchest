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

#ifndef __DC_Scene_DebugPasses_H__
#define __DC_Scene_DebugPasses_H__

#include "../RenderPass.h"
#include "../RenderingSystem.h"
#include "../Emitters/StaticMeshRopEmitter.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! Renders scene overdraw.
	class OverdrawPass : public RenderPassBase {
	public:

								//! Constructs OverdrawPass instance.
								OverdrawPass( Ecs::EcsWPtr ecs )
									{ addEmitter( DC_NEW StaticMeshRopEmitter( ecs ) ); }

	protected:

		//! Setups the RVM for rendering mesh overdraw.
		virtual void			setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection ) DC_DECL_OVERRIDE;
	};

	//! Reneders wireframe mesh overlay.
	class WireframePass : public RenderPassBase {
	public:

								//! Constructs WireframePass instance.
								WireframePass( Ecs::EcsWPtr ecs )
									{ addEmitter( DC_NEW StaticMeshRopEmitter( ecs ) ); }

	protected:

		//! Setups the RVM for rendering wireframe meshes.
		virtual void			setup( Rvm& rvm, ShaderCache& shaders, const Matrix4& viewProjection ) DC_DECL_OVERRIDE;
	};

	//! Renders bounding boxes of meshes.
	class MeshBoundsPass : public RenderPass<StaticMesh> {
	public:

								//! Constructs MeshBoundsPass instance.
								MeshBoundsPass( Ecs::EcsWPtr ecs )
									: RenderPass( ecs, "MeshBoundsPass" ) {}

	protected:

		//! Renders bounding box of a single static mesh.
		virtual void			render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const StaticMesh& staticMesh, const Transform& transform ) DC_DECL_OVERRIDE;
	};

	//! Renders bounding boxes of lights.
	class LightBoundsPass : public RenderPass<Light> {
	public:

								//! Constructs LightBoundsPass instance.
								LightBoundsPass( Ecs::EcsWPtr ecs )
									: RenderPass( ecs, "LightBoundsPass" ) {}

	protected:

		//! Renders bounding box of a single light.
		virtual void			render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const Light& light, const Transform& transform ) DC_DECL_OVERRIDE;
	};

	//! Renders bounding boxes of particle systems.
	class ParticlesBoundsPass : public RenderPass<Particles> {
	public:

								//! Constructs ParticlesBoundsPass instance.
								ParticlesBoundsPass( Ecs::EcsWPtr ecs )
									: RenderPass( ecs, "ParticlesBoundsPass" ) {}

	protected:

		//! Renders bounding box of a single light.
		virtual void			render( RenderingContextPtr context, Rvm& rvm, ShaderCache& shaders, const Particles& particles, const Transform& transform ) DC_DECL_OVERRIDE;
	};

	//! Renders bounding boxes of scene objects.
	class BoundingVolumesRenderer : public RenderingSystem<RenderBoundingVolumes> {
	public:

								BoundingVolumesRenderer( Ecs::EcsWPtr ecs )
									: RenderingSystem( ecs, "BoundingVolumesRenderer" )
								{
									addPass<MeshBoundsPass>();
									addPass<LightBoundsPass>();
									addPass<ParticlesBoundsPass>();
								}
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_DebugPasses_H__    */