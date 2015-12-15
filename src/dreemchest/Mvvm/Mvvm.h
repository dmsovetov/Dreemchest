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

#include "../Io/Bson.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

	DECLARE_LOG( log )

	//! Generic class to convert between the value type & BSON.
	template<typename TValue>
	struct BsonConverter {
		//! Converts to BSON from a value.
		static Io::Bson	to( const TValue& value ) { return Io::Bson( value ); }

		//! Converts from a value to a BSON.
		static TValue	from( const Io::Bson& value ) { return static_cast<TValue>( value ); }
	};

	//! Converts between the GUID and BSON.
	struct GuidBsonConverter {
		//! Converts to BSON from a value.
		static Io::Bson	to( const ::Guid& value ) { return Io::Bson( value.toString() ); }

		//! Converts from a value to a BSON.
		static ::Guid	from( const Io::Bson& value ) { return value.asString(); }	
	};

	//! Converts between the Vec2 and BSON.
	struct Vec2BsonConverter {
		//! Converts to BSON from a value.
		static Io::Bson	to( const Vec2& value );

		//! Converts from a value to a BSON.
		static Vec2		from( const Io::Bson& value );			
	};

	//! Primitive value type forward declaration.
	template< typename TValue, typename TBsonConverter = BsonConverter<TValue> > class PrimitiveValue;

	//! Array value type forward declaration.
	template<typename TValue> class ArrayValue;

	//! Value type index.
	typedef TypeIdx ValueTypeIdx;

	//! Widget type index.
	typedef u32		WidgetTypeIdx;

	//! The widget type chain.
	typedef Array<WidgetTypeIdx>	WidgetTypeChain;

	//! The set of value types.
	typedef Array<ValueTypeIdx>		ValueTypes;

	//! Widget that is bound to a value.
	typedef void* Widget;

	dcDeclareNamedPtrs( IBinding, Binding )
	dcDeclareNamedPtrs( ObjectValue, Object )
	dcDeclarePtrs( Value )
	dcDeclarePtrs( BindingFactory )
	dcDeclarePtrs( Bindings )

	typedef PrimitiveValue<bool>						Boolean;		//!< Boolean value type.
	typedef PrimitiveValue<s32>							Integer;		//!< Integer value type.
	typedef PrimitiveValue<f32>							Float;			//!< Floating point value type.
	typedef PrimitiveValue<String>						Text;			//!< String value type.
	typedef PrimitiveValue<::Guid, GuidBsonConverter>	Guid;			//!< Guid value type.	
	typedef PrimitiveValue<Vec2, Vec2BsonConverter>		Point2;			//!< 2-dimensional point type.
	typedef ArrayValue<Text>							TextArray;		//!< Array of text values.
	typedef ArrayValue<Point2>							Point2Array;	//!< Array of 2-dimensional points.

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