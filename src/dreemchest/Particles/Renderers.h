//
//  Filename:   Renderers.h
//	Created:	28:05:2011   18:32

#ifndef		__DC_Renderers_H__
#define		__DC_Renderers_H__

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include    "Emitter.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

    // ** struct sParticleVertex
    struct sParticleVertex {
        vec2        pos;
        rgba        color;
    };

    // ** class Renderer
    class Renderer {
    public:

                                    Renderer( void );
        virtual                     ~Renderer( void );

        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count );
        virtual RendererType		type( void ) const { return RendererType::Total; }
        static Renderer*            createRenderer( RendererType type );
    };

    // ** class PointRenderer
    class PointRenderer : public Renderer {
    public:
        
                                    PointRenderer( void );

        // ** Renderer
        virtual RendererType		type( void ) const { return RendererType::Points; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count );
    };

    // ** class QuadRenderer
    class QuadRenderer : public Renderer {
    public:

                                    QuadRenderer( void );

        // ** Renderer
        virtual RendererType		type( void ) const { return RendererType::Quads; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count );
    };

    // ** class LineRenderer
    class LineRenderer : public Renderer {
    public:

                                    LineRenderer( void );

        // ** Renderer
        virtual RendererType		type( void ) const { return RendererType::Lines; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count );
    };

    // ** class ThickLineRenderer
    class ThickLineRenderer : public Renderer {
    public:

                                    ThickLineRenderer( void );

        // ** Renderer
        virtual RendererType		type( void ) const { return RendererType::ThickLines; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count );
    };

    // ** class PathRenderer
    class PathRenderer : public Renderer {
    public:

                                    PathRenderer( void );

        // ** Renderer
        virtual RendererType	    type( void ) const { return RendererType::Paths; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count );
    };

    // ** class ThickPathRenderer
    class ThickPathRenderer : public Renderer {
    public:

                                    ThickPathRenderer( void );

        // ** Renderer
        virtual RendererType		type( void ) const { return RendererType::ThickPaths; }
        virtual void                render( dcBatchRenderer renderer, dcTextureAsset texture, BlendMode blendMode, const sParticle *particles, int count );
    };

} // namespace particles

} // namespace dreemchest

#endif		/*	!__DC_Parameter_H__	*/