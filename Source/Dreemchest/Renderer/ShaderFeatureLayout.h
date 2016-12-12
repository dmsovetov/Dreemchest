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

#ifndef __DC_Renderer_ShaderFeatureLayout_H__
#define __DC_Renderer_ShaderFeatureLayout_H__

#include "Renderer.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    
    //! Shader feature layout defines mappings from a feature mask to actual preprocessor definitions.
    class ShaderFeatureLayout
    {
    public:
        
        //! A single shader feature.
        struct Element
        {
            PipelineFeatures    mask;       //!< Pipeline feature mask that matches this element.
            String              name;       //!< Feature name.
            s32                 offset;     //!< A feature mask offset to extract actual element value.
        };
        
                                //! Construct ShaderFeatureLayout instance.
                                ShaderFeatureLayout( void );
        
        //! Returns a bitmask of supported pipeline features.
        PipelineFeatures        mask( void ) const;
        
        //! Returns a total number of shader feature elements.
        s32                     elementCount( void ) const;
        
        //! Returns an element at specified index.
        const Element&          elementAt(s32 index) const;
        
        //! Adds a new element to a layout.
        void                    addFeature(const String& name, PipelineFeatures mask);
        
    private:
        
        Array<Element>          m_elements; //!< An array of available shader features.
        PipelineFeatures        m_mask;     //!< A bitmask with supported pipeline features.
    };
    
} // namespace Renderer

DC_END_DREEMCHEST

#endif  /*  !__DC_Renderer_ShaderFeatureLayout_H__   */
