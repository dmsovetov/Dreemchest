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

	//! Performs the property type cast.
	template<typename T, typename S>
	inline WeakPtr<T> castTo( WeakPtr<S> ptr ) {
		return ptr.valid() && ptr->is( Value::valueType<T>() ) ? static_cast<T*>( ptr.get() ) : NULL;
	}

	//! Performs the property type cast.
	template<typename T, typename S>
	inline StrongPtr<T> castTo( StrongPtr<S> ptr ) {
		return ptr.valid() && ptr->is( Value::valueType<T>() ) ? static_cast<T*>( ptr.get() ) : NULL;
	}

	//! Primitive value type forward declaration.
	template<typename TValue> class PrimitiveValue;

	//! Array value type forward declaration.
	template<typename TValue> class ArrayValue;

	//! Value type index.
	typedef TypeIdx ValueTypeIdx;

	//! Widget type index.
	typedef u32		WidgetTypeIdx;

	//! The widget prototype chain.
	typedef Array<WidgetTypeIdx>	WidgetPrototypeChain;

	//! Widget that is bound to a value.
	typedef void* Widget;

	dcDeclareNamedPtrs( IBinding, Binding )
	dcDeclareNamedPtrs( ObjectValue, Object )
	dcDeclarePtrs( Value )
	dcDeclarePtrs( BindingFactory )
	dcDeclarePtrs( Bindings )

	typedef PrimitiveValue<bool>	Boolean;	//!< Boolean value type.
	typedef PrimitiveValue<s32>		Integer;	//!< Integer value type.
	typedef PrimitiveValue<f32>		Float;		//!< Floating point value type.
	typedef PrimitiveValue<String>	Text;		//!< String value type.
	typedef PrimitiveValue<::Guid>	Guid;		//!< Guid value type.	
	typedef ArrayValue<Text>		TextArray;	//!< Array of text values.

} // namespace mvvm

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
	#include "Value.h"
	#include "Converter.h"
	#include "Binding.h"

    #ifdef DC_MVVM_MYGUI_ENABLED
        #include "MyGUI/MyGUIView.h"
        #include "MyGUI/BindingsMyGUI.h"
    #endif
#endif

#endif  /*  !__DC_Mvvm_H__   */