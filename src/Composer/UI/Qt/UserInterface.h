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

#ifndef __DC_Composer_Qt_UserInterface_H__
#define __DC_Composer_Qt_UserInterface_H__

#include "../IUserInterface.h"
#include "../IDocument.h"
#include "../IMainWindow.h"
#include "../IAssetTree.h"
#include "../IMainWindow.h"
#include "../IMenu.h"
#include "../IRenderingFrame.h"
#include "../IMimeData.h"

namespace Ui {

	class QAssetsModel;
	class Document;
	class RenderingFrame;

	//! Converts the Qt key index to engine key.
	extern Platform::Key convertKey( s32 key );

	//! Generic class to declare Qt interface implementations.
	template<typename TBase, typename TPrivate>
	class UserInterface : public TBase {
	public:

									//! Constructs the UserInterface instance.
									UserInterface( TPrivate* instance )
										: m_private( instance ) {}

		//! Returns the private interface.
		virtual void*				ptr( void ) const { return m_private.get(); }

	protected:

		AutoPtr<TPrivate>			m_private;	//!< Actual implementation instance.
	};

} // namespace Ui

#endif	/*	!__DC_Composer_Qt_UserInterface_H__	*/