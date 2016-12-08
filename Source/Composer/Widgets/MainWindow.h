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

#ifndef __DC_Composer_MainWindow_H__
#define __DC_Composer_MainWindow_H__

#include "../Composer.h"

DC_BEGIN_COMPOSER

namespace Ui {

    //! Main application window Qt implementation
    class MainWindow : public QMainWindow {

        Q_OBJECT

    public:

                                        //! Constructs MainWindow instance.
                                        MainWindow( const QString& title );
                                    
        //! Adds a new toolbar to window.
        ToolBarQPtr                        addToolBar( void );
    
        //! Adds a new menu to window.
        MenuQPtr                        addMenu( const String& text );
    
        //! Performs the main window initialization.
        bool                            initialize( ComposerQPtr composer );

        //! Constructs a new asset editor dock window or brings to front the existing one.
        DocumentQPtr                    editDocument( Editors::AssetEditorQPtr assetEditor, const FileInfo& asset );

        //! Closes the document.
        bool                            closeDocument( DocumentQPtr document );

        //! Returns the opened document editor by asset.
        DocumentQPtr                    findDocument( const FileInfo& asset ) const;

        //! Returns an array of opened documents with a same type.
        QVector<DocumentQPtr>            findDocuments( const FileInfo& asset ) const;

        //! Shows the message box.
        void                            message( const String& text, MessageStatus status = MessageInfo ) const;

        //! Shows the message box with yes, no, cancel buttons.
        MessageBoxResult                messageYesNoCancel( const String& text, const String& info, MessageStatus status = MessageInfo ) const;

        //! Returns the asset tree instance.
        AssetTreeQPtr                    assetTree( void ) const;

        //! Returns the scene tree instance.
        SceneTreeQPtr                    sceneTree( void ) const;

        //! Returns the entity inspector instance.
        EntityInspectorQPtr                inspector( void ) const;

        //! Returns an active document.
        DocumentQPtr                    activeDocument( void ) const;

        //! Sets an active document.
        void                            setActiveDocument( DocumentQPtr dock );

        //! Returns the shared rendering context.
        RenderingFrameQPtr                sharedRenderingContext( void ) const;

    private:

        //! Ensures that the document was saved before closing.
        bool                            ensureSaved( DocumentQPtr document ) const;

        //! Creates the dock widget.
        QDockWidget*                    addDock( const QString& name, QWidget* widget, Qt::DockWidgetArea initialDockArea = Qt::LeftDockWidgetArea, Qt::DockWidgetAreas allowedDockAreas = Qt::AllDockWidgetAreas, QDockWidget* destination = NULL );

    private slots:

        //! Handles the ProjectOpened event.
        void                            createProjectInterface( Project* project );

        //! Handles the ProjectClosed event.
        void                            destroyProjectInterface( Project* project );
    
    private:

        QVector<DocumentQPtr>            m_documents;                //!< All opened documents reside here.
        DocumentQPtr                    m_activeDocument;            //!< An active document.
        RenderingFrameQPtr                m_sharedRenderingContext;    //!< The shared OpenGL context.
        AssetTreeQPtr                    m_assetTree;                //!< Asset tree instance.
        SceneTreeQPtr                    m_sceneTree;                //!< Scene tree instance.
        EntityInspectorQPtr                m_inspector;                //!< Entity inspector instance.
        ProjectQPtr                        m_project;                    //!< Opened project.
    };

} // namespace Ui

DC_END_COMPOSER

#endif    /*    !__DC_Composer_MainWindow_H__    */
