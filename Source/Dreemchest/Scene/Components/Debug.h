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

#ifndef __DC_Scene_Components_Debug_H__
#define __DC_Scene_Components_Debug_H__

#include "../Scene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Grid component used for debug rendering of a grid.
    class Grid : public Ecs::Component<Grid> {
    public:

        //! Available grid features.
        enum Features {
              Cells        = BIT( 0 )    //!< Cells will be rendered.
            , Basis        = BIT( 1 )    //!< Basis will be rendered.
            , Outline    = BIT( 2 )    //!< Outline will be rendered
            , Axes        = BIT( 3 )    //!< Axes will be rendered.
        };

                            Grid( f32 size = 100.0f, f32 cellSize = 1.0f, u8 features = ~0 );

        //! Returns grid size.
        f32                    size( void ) const;

        //! Returns grid cell size.
        f32                    cellSize( void ) const;

        //! Returns grid features.
        const FlagSet8&        features( void ) const;

    private:

        f32                    m_size;        //!< Grid size.
        f32                    m_cellSize;    //!< Cell size.
        FlagSet8            m_features;    //!< Grid rendering features.
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Components_Debug_H__    */
