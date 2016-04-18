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

#include "EntityInspector.h"
#include "PropertyInspector.h"
#include "../Actions.h"

#include "../../Models/PropertyModel.h"

DC_BEGIN_COMPOSER

namespace Ui {

// ------------------------------------------------------------ EntityInspector::HorizontalSeparator ------------------------------------------------------------ //

// ** EntityInspector::HorizontalSeparator
class EntityInspector::HorizontalSeparator : public QFrame {
public:

    //! Constructs HorizontalSeparator instance.
    HorizontalSeparator( void ) { setFrameStyle( QFrame::HLine | QFrame::Sunken ); }
};

// ------------------------------------------------------------------ EntityInspector::Header ------------------------------------------------------------------- //

// ** EntityInspector::Header
class EntityInspector::Header : public QWidget {
public:

                            //! Constructs Header instance.
                            Header( QMenu* menu, const QString& text, PropertyInspector* inspector );

protected:

    //! Handles mouse pressed event.
    virtual void            mousePressEvent( QMouseEvent* e ) Q_DECL_OVERRIDE;

    //! Handles mouse released event.
    virtual void            mouseReleaseEvent( QMouseEvent* e ) Q_DECL_OVERRIDE;

    //! Paints the header widget.
    virtual void            paintEvent( QPaintEvent* e ) Q_DECL_OVERRIDE;

    //! Set collapsed button.
    void                    setCollapseIndicator( bool collapsed );

    //! Paints horizontal triangle at specified point.
    void                    paintHorizontalTriangle( QPainter& painter, const QBrush& brush, qreal x, qreal y, qreal width, qreal height ) const;

    //! Paints vertical triangle at specified point.
    void                    paintVerticalTriangle( QPainter& painter, const QBrush& brush, qreal x, qreal y, qreal width, qreal height ) const;

private:

    QHBoxLayout*            m_layout;       //!< Horizontal layout.
    PropertyInspector*      m_inspector;    //!< Child widget.
    QToolButton*            m_collapse;     //!< Collapse button.
    bool                    m_isPressed;    //!< Is the header pressed by user.
};

// ** EntityInspector::Header::Header
EntityInspector::Header::Header( QMenu* menu, const QString& text, PropertyInspector* inspector )
    : m_isPressed( false )
    , m_inspector( inspector )
{
    m_layout = new QHBoxLayout( this );
    m_layout->setMargin( 2 );
    m_layout->setSpacing( 0 );

    QLabel* label = new QLabel( text, this );
    label->setStyleSheet( "font-weight: bold" );

    m_collapse = new QToolButton;
    m_collapse->setAutoRaise( true );
    m_collapse->setDisabled( true );
    setCollapseIndicator( false );

    m_layout->addWidget( m_collapse );

    //QCheckBox* checkBox = new QCheckBox( this );
    //checkBox->setMaximumWidth( 20 );

    //m_layout->addSpacing( 15 );
    //m_layout->addWidget( checkBox );
    m_layout->addWidget( label );

    QToolButton* button = new QToolButton;
    button->setMenu( menu );
    button->setAutoRaise( true );
    button->setPopupMode( QToolButton::InstantPopup );
    button->setMaximumSize( QSize( 15, 15 ) );
    m_layout->addWidget( button );
}

// ** EntityInspector::Header::setCollapseIndicator
void EntityInspector::Header::setCollapseIndicator( bool collapsed )
{
    m_collapse->setArrowType( collapsed ? Qt::RightArrow : Qt::DownArrow );
    m_collapse->setIconSize( QSize( 10, 10 ) );
}

// ** EntityInspector::Header::mousePressEvent
void EntityInspector::Header::mousePressEvent( QMouseEvent* e )
{
    m_isPressed = true;
    update();
}

// ** EntityInspector::Header::mouseReleaseEvent
void EntityInspector::Header::mouseReleaseEvent( QMouseEvent* e )
{
    if( !m_isPressed ) {
        return;
    }

    m_isPressed = false;
    m_inspector->setVisible( !m_inspector->isVisible() );
    setCollapseIndicator( !m_inspector->isVisible() );

    update();
}

// ** EntityInspector::Header::paintEvent
void EntityInspector::Header::paintEvent( QPaintEvent* e )
{
    // Get the triangle brush
    QBrush brushTriangle = QApplication::palette().shadow();

    QPainter painter( this );
    painter.save();

    if( m_isPressed ) {
        painter.setPen( Qt::NoPen );
        painter.setBrush( QApplication::palette().light() );
        painter.drawRect( e->rect() ); 
    }

    // Restore the painter
    painter.restore();
}

// ** EntityInspector::Header::paintHorizontalTriangle
void EntityInspector::Header::paintHorizontalTriangle( QPainter& painter, const QBrush& brush, qreal x, qreal y, qreal width, qreal height ) const
{
    QPainterPath path;
    QRectF       rc( x, y, width, height );

    path.moveTo( rc.left(), rc.top() );
    path.lineTo( rc.right(), rc.top() );
    path.lineTo( rc.left() + rc.width() * 0.5f, rc.bottom() );
    path.lineTo( rc.left(), rc.top() );

    painter.fillPath( path, brush );
}

// ** EntityInspector::Header::paintVerticalTriangle
void EntityInspector::Header::paintVerticalTriangle( QPainter& painter, const QBrush& brush, qreal x, qreal y, qreal width, qreal height ) const
{
    QPainterPath path;
    QRectF       rc( x, y, height, width );

    path.moveTo( rc.left(), rc.top() );
    path.lineTo( rc.right(), rc.top() + rc.height() * 0.5f );
    path.lineTo( rc.left(), rc.bottom() );
    path.lineTo( rc.left(), rc.top() );

    painter.fillPath( path, brush );
}

// ---------------------------------------------------------------------- EntityInspector ----------------------------------------------------------------------- //

// ** EntityInspector::EntityInspector
EntityInspector::EntityInspector( QWidget* parent )
    : QWidget( parent )
    , m_layout( NULL )
    , m_hasChanges( false )
    , m_save( NULL )
    , m_reset( NULL )
{
    
}

// ** EntityInspector::entity
Ecs::EntityWPtr EntityInspector::entity( void ) const
{
    return m_entity;
}

// ** EntityInspector::bind
void EntityInspector::bind( Ecs::EntityWPtr entity )
{
    // Save entities
    m_entity = entity;

    // Rebuild layout
    refresh();

    // There are no changes by default
    setHasChanges( false );

    // Save current state
    m_previous = saveState();
}

// ** EntityInspector::hasChanges
bool EntityInspector::hasChanges( void ) const
{
    return m_hasChanges;
}

// ** EntityInspector::setHasChanges
void EntityInspector::setHasChanges( bool value )
{
    m_hasChanges = value;

    if( m_save ) {
        m_save->setEnabled( hasChanges() );
    }
    if( m_reset ) {
        m_reset->setEnabled( hasChanges() );
    }
}

// ** EntityInspector::saveState
Archive EntityInspector::saveState( void ) const
{
    Archive state;
#if DEV_DEPRECATED_SERIALIZATION
    Ecs::SerializationContext ctx( m_entity->ecs() );
    m_entity->serialize( ctx, state );
#else
    Ecs::Serializer serializer( m_entity->ecs(), Ecs::Aspect::expandComponentBits<Editors::SceneEditorInternal>() );
    Reflection::AssemblyPtr assembly = Reflection::Assembly::create();

    assembly->registerClass<Scene::Transform>();
    assembly->registerClass<Scene::Identifier>();
    assembly->registerClass<Scene::StaticMesh>();

    KeyValue ar;
    serializer.serialize( m_entity, ar );
    state = Archive::fromValue( ar );
    LogDebug( "entityInspector", "%s\n", Io::VariantTextStream().stringify( Variant::fromValue( ar ), true ).c_str() );

    Ecs::EntityPtr e = m_entity->ecs()->createEntity();
    serializer.deserialize( assembly, e, ar );

    {
        KeyValue ar;
        serializer.serialize( e, ar );
        LogDebug( "entityInspector", "%s\n", Io::VariantTextStream().stringify( Variant::fromValue( ar ), true ).c_str() );
    }
#endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */
    return state;
}

// ** EntityInspector::restoreState
void EntityInspector::restoreState( const Archive& state )
{
#if DEV_DEPRECATED_SERIALIZATION
    // Remove all components from entity
    m_entity->clear();

    // Deserialize entity
    Ecs::SerializationContext ctx( m_entity->ecs() );
    m_entity->deserialize( ctx, state );
#else
    LogError( "entityInspector", "entity deserialization is not implemented\n" );
#endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */
}

// ** EntityInspector::buildComponentList
Ecs::ComponentWeakList EntityInspector::buildComponentList( void ) const
{
    Ecs::ComponentWeakList list;

    // Add all components to a list
    Ecs::Entity::Components components = m_entity->components();
    for( Ecs::Entity::Components::iterator i = components.begin(), end = components.end(); i != end; ++i ) {
        list.push_back( i->second );
    }

#if 0
    // Declare the comparator
    struct LessThan {
        static bool compare( Ecs::ComponentWPtr a, Ecs::ComponentWPtr b ) { return strcmp( a->typeName(), b->typeName() ) < 0; }
    };

    // Sort list by a component name
    std::sort( list.begin(), list.end(), LessThan::compare );
#else
    LogWarning( "entityInspector", "component list was not sorted\n" );
#endif

    return list;
}

// ** EntityInspector::createComponentMenu
QMenu* EntityInspector::createComponentMenu( TypeIdx component ) const
{
    QMenu* menu = new QMenu;
    QAction* action = new DetachComponentAction( m_entity, component );
    action->setText( "Remove" );
    menu->addAction( action );

    connect( action, SIGNAL(triggered()), this, SLOT(detachComponent()) );

    return menu;
}

// ** EntityInspector::refresh
void EntityInspector::refresh( void )
{
	// Destroy old layout & mapper
	qDeleteLayout( m_layout ); m_layout = NULL;

	// Return if the model is not valid
	if( !m_entity.valid() ) {
		return;
	}

    // Clear object inspectors
    m_inspectors.clear();

	// Create root layout.
	m_layout = new QVBoxLayout( this );
    m_layout->setMargin( 0 );
    m_layout->setSpacing( 0 );

	// Construct component property inspectors
    Ecs::ComponentWeakList components = buildComponentList();

    foreach( Ecs::ComponentWPtr component, components ) {
        PropertyInspector* inspector  = new PropertyInspector( this );
        PropertyModel*     properties = new PropertyModel( component->metaInstance(), inspector );

        // Connect to a property modification signal
        connect( properties, SIGNAL(propertyChanged()), this, SLOT(propertyChanged()) );

        // Add header item
        m_layout->addWidget( new Header( createComponentMenu( component->typeIndex() ), component->metaObject()->name(), inspector ) );

        // Add component object inspector
        inspector->setModel( properties );
        inspector->setMargins( 10, 0, 0, 5 );
        m_layout->addWidget( inspector );

        // Add the horizontal separator
        m_layout->addWidget( new HorizontalSeparator );

        // Save this inspector
        m_inspectors.append( inspector );
    }

    // Add the horizontal separator
    m_layout->addWidget( new HorizontalSeparator );

    // Add the stretch
    m_layout->addStretch();
}

// ** EntityInspector::propertyChanged
void EntityInspector::propertyChanged( void )
{
    // Show that we have some changes
    setHasChanges( true );
}

// ** EntityInspector::save
void EntityInspector::save( void )
{
    // Save state
    m_previous = saveState();

    // There are no changes any more.
    setHasChanges( false );
}

// ** EntityInspector::reset
void EntityInspector::reset( void )
{
    // Restore entity from a previuos state
    restoreState( m_previous );

    // Rebuild layout.
    refresh();

    // There are no changes any more.
    setHasChanges( false );
}

// ** EntityInspector::attachComponent
void EntityInspector::attachComponent( const QString& type )
{
    // Create component by name
#if DEV_DEPRECATED_SERIALIZATION
    Ecs::ComponentPtr component = m_entity->ecs()->createComponentByName( type.toStdString() );
#else
    Ecs::ComponentPtr component = qComposer->assembly()->createInstance( type.toStdString() ).upCast<Ecs::ComponentBase>();
#endif  /*  #if DEV_DEPRECATED_SERIALIZATION    */
    DC_BREAK_IF( !component.valid() );

    // Attach it to an entity
    m_entity->attachComponent<Ecs::ComponentBase>( component.get() );

    // Refresh the layout
    refresh();

    // Mark that we have changes
    setHasChanges( true );
}

// ** EntityInspector::detachComponent
void EntityInspector::detachComponent( void )
{
    // Refresh the layout
    refresh();

    // Mark that we have changes
    setHasChanges( true );
}

} // namespace Ui

DC_END_COMPOSER