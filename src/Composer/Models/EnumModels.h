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

#ifndef __DC_Composer_EnumModels_H__
#define __DC_Composer_EnumModels_H__

#include "../Composer.h"

#define qDeclareEnumeration( name, items )                             \
	struct name##Model : public EnumerationModel {                          \
		name##Model( void ) { setStringList( QStringList() << items ); }    \
	};

DC_BEGIN_COMPOSER

	//! Base class for all enumeration models.
	class EnumerationModel : public QStringListModel {
	public:

		//! Returns the integer value from string.
		int					fromString( const QString& value ) const { return stringList().indexOf( value ); }

		//! Returns the string from enum value.
		QString				toString( int value ) const { return stringList().at( value ); }
	};

    qDeclareEnumeration( RenderingMode, "Opaque" << "Cutout" << "Translucent" << "Additive" )
    qDeclareEnumeration( Lighting, "Unlit" << "Ambient" << "Phong" )

DC_END_COMPOSER

#endif	/*	!__DC_Composer_EnumModels_H__	*/