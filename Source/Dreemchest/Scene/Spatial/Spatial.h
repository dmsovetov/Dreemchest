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

#ifndef __DC_Scene_Spatial_H__
#define __DC_Scene_Spatial_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Spatial class performs ray/sphere/box scene queries and returns scene objects that match a specified aspect.
    class Spatial {
    friend class Scene;
    public:

        //! Spatial query feature mask.
	    enum SceneQueryFlags {
		      QuerySingle		= BIT( 0 )	//!< Scene query will result a sinle best matching result.
		    , QueryBackToFront	= BIT( 1 )	//!< Scene query will return results in back to front order. 
	    };

        //! Spatial query result contains weak pointer to a scene object and intersection info.
        struct Result {
 		    SceneObjectWPtr		sceneObject;	//!< The scene object hit.

            union {
                struct {
		            f32		    point[3];	    //!< The intersection world space point.
		            f32			time;			//!< The distance value in range [0, 1] from ray origin to a hit point.
                } ray;
            };

                                //! Constructs invalid query result.
                                Result( void );

							    //! Constructs ray query Result instance.
							    Result( SceneObjectWPtr sceneObject, const Vec3& point, f32 time );

							    //! Returns true if this is a valid result (contains a hit scene object).
							    operator bool( void ) const { return sceneObject.valid(); }          
        };


	    //! Array of spatial query results.
	    typedef Array<Result>   Results;

		//! Performs the ray tracing.
		Results                 queryRay( const Ray& ray, const FlagSet8& flags = QuerySingle ) const;

    private:

                                //! Constructs Spatial instance.
                                Spatial( SceneWPtr scene );

        //! Compares two ray tracing results.
        static bool             rayTracingResultLess( const Result& a, const Result& b );

        //! Compares two ray tracing results.
        static bool             rayTracingResultGreater( const Result& a, const Result& b );

    private:

        SceneWPtr               m_scene;    //!< Parent scene instance.
        Ecs::IndexPtr		    m_meshes;   //!< All queries will be made to this mesh index.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Spatial_H__    */