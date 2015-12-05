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

#ifndef __DC_Composer_MimeData_H__
#define __DC_Composer_MimeData_H__

#include "UserInterface.h"

namespace Ui {

	//! Composer MIME data.
	class QComposerMime : public QMimeData {
		
		Q_OBJECT

	public:

								//! Constructs QComposerMime instance.
								QComposerMime( const QString& format );

		//! Returns an attached MIME data.
		IMimeDataWPtr			mime( void ) const;

		//! Sets an attached MIME data.
		void					setMime( IMimeDataPtr value );

	private:

		IMimeDataPtr			m_mime;	//!< Attached MIME.
	};

	//! MIME data Qt implementation.
	class MimeData : public IMimeData {
	public:

							//! Constructs the MimeData instance.
							MimeData( QComposerMime* mime );

		//! Returns true if the MIME data contains the specified format.
		virtual bool		hasFormat( const String& value ) const;

	public:

		QComposerMime*		m_mime;	//!< Internal MIME data instance.
	};

} // namespace Ui

#endif	/*	!__DC_Composer_MimeData_H__	*/