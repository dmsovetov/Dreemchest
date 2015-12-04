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

#ifndef __DC_Composer_IDocument_H__
#define __DC_Composer_IDocument_H__

#include "IUserInterface.h"

namespace Ui {

	//! Document window interface.
	class IDocument : public IUserInterface {
	public:

		//! Returns the rendering frame used by this document.
		virtual IRenderingFrameWPtr			renderingFrame( void )							= 0;

		//! Sets the rendering frame.
		virtual void						setRenderingFrame( IRenderingFramePtr value )	= 0;

		//! Returns an attached asset editor.
		virtual Editors::AssetEditorWPtr	assetEditor( void ) const						= 0;
	};


} // namespace Ui

#endif	/*	!__DC_Composer_IDocument_H__	*/