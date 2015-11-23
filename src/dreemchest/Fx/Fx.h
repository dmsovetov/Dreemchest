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

#ifndef __DC_Fx_H__
#define __DC_Fx_H__

#include "../Dreemchest.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

	class ParticleModel;
	class Zone;
	class Modifier;
	class Emitter;
	class Particles;
	class Renderer;
	class EmitterModel;
	class ParticleSystem;
	struct sParticle;

	typedef u32 dcTextureAsset;
	typedef u32 dcBatchRenderer;

	typedef Array<f32> FloatArray;
	typedef Array<Modifier*> ModifierArray;
	typedef Array<ParticleModel*> ParticleModelArray;
	typedef Array<Particles*> ParticlesArray;
	typedef Array<EmitterModel*> EmitterModelArray;
	typedef Array<Emitter*> EmitterArray;

	//! Modifier type.
	enum ModifierType {
		ModifierFriction,
		ModifierForce,
		ModifierForceField,

		TotalModifierTypes
	};
        
	//! Particle rendering types.
	enum RendererType {
		RenderPoints,
        RenderQuads,
        RenderLines,
        RenderThickLines,
        RenderPaths,
        RenderThickPaths,

        TotalRendererTypes,
	};

	//! Rendering blend modes.
	enum BlendMode {
		BlendAlpha,
		BlendAdditive,
		BlendPremultiplied,

		TotalBlendModes
	};

    //! Available particle emission zones.
    enum ZoneType {
        ZoneDisk,
        ZoneLine,

        TotalZoneTypes,
	};

} // namespace Fx

DC_END_DREEMCHEST

#endif	/*	!__DC_Fx_H__	*/