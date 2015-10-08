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

#ifndef __DC_Mvvm_H__
#define __DC_Mvvm_H__

#include "../Dreemchest.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

	DECLARE_LOG( log )

    template<typename T> class Property;
    template<typename T> class ArrayProperty;

	dcDeclareNamedPtrs( IBinding, Binding )
	dcDeclareNamedPtrs( IProperty, Property )
	dcDeclarePtrs( Data )
	dcDeclarePtrs( View )
	dcDeclarePtrs( ActionHandler )

    typedef Property<bool>          BoolProperty;
    typedef Property<s32>           IntProperty;
    typedef Property<f32>			FloatProperty;
    typedef Property<String>        StringProperty;
    typedef ArrayProperty<String>   StringArrayProperty;
    typedef List<BindingPtr>        BindingsList;

	//! Performs the property type cast.
	template<typename TTargetType>
	inline StrongPtr< Property<TTargetType> > castTo( const PropertyPtr& property )
	{
		if( GroupedTypeIndex<TTargetType, IProperty>::idx() == property->type() ) {
			return StrongPtr< Property<TTargetType> >( static_cast<Property<TTargetType>*>( property.get() ) );
		}

		return StrongPtr< Property<TTargetType> >();
	}

} // namespace mvvm

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
    #include "View.h"
    #include "Data.h"
    #include "ActionHandler.h"
    #include "Property.h"
    #include "Validation.h"

    #ifdef DC_MVVM_MYGUI_ENABLED
        #include "MyGUI/MyGUIView.h"
        #include "MyGUI/BindingsMyGUI.h"
    #endif
#endif

#endif  /*  !__DC_Mvvm_H__   */