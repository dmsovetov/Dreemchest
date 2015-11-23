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

#ifndef __DC_Fx_Renderers_H__
#define __DC_Fx_Renderers_H__

#include "Emitter.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

    // ** struct sParticleVertex
    struct sParticleVertex {
        Vec2        pos;
        Rgba        color;
    };

    // ** class Renderer
    class Renderer : public RefCounted {
    public:

                                    Renderer( void );
        virtual                     ~Renderer( void );

        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendingMode blendMode, const sParticle *particles, int count );
        virtual RenderingMode		type( void ) const { return TotalRenderingModes; }
        static Renderer*            createRenderer( RenderingMode type );
    };

    // ** class PointRenderer
    class PointRenderer : public Renderer {
    public:
        
                                    PointRenderer( void );

        // ** Renderer
        virtual RenderingMode		type( void ) const { return RenderPoints; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendingMode blendMode, const sParticle *particles, int count );
    };

    // ** class QuadRenderer
    class QuadRenderer : public Renderer {
    public:

                                    QuadRenderer( void );

        // ** Renderer
        virtual RenderingMode		type( void ) const { return RenderQuads; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendingMode blendMode, const sParticle *particles, int count );
    };

    // ** class LineRenderer
    class LineRenderer : public Renderer {
    public:

                                    LineRenderer( void );

        // ** Renderer
        virtual RenderingMode		type( void ) const { return RenderLines; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendingMode blendMode, const sParticle *particles, int count );
    };

    // ** class ThickLineRenderer
    class ThickLineRenderer : public Renderer {
    public:

                                    ThickLineRenderer( void );

        // ** Renderer
        virtual RenderingMode		type( void ) const { return RenderThickLines; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendingMode blendMode, const sParticle *particles, int count );
    };

    // ** class PathRenderer
    class PathRenderer : public Renderer {
    public:

                                    PathRenderer( void );

        // ** Renderer
        virtual RenderingMode	    type( void ) const { return RenderPaths; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendingMode blendMode, const sParticle *particles, int count );
    };

    // ** class ThickPathRenderer
    class ThickPathRenderer : public Renderer {
    public:

                                    ThickPathRenderer( void );

        // ** Renderer
        virtual RenderingMode		type( void ) const { return RenderThickPaths; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendingMode blendMode, const sParticle *particles, int count );
    };

} // namespace Fx

DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_Renderers_H__	*/