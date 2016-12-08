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

#include "AbstractTreeModel.h"

DC_BEGIN_COMPOSER

// --------------------------------------------------------- AbstractTreeModel --------------------------------------------------------- //

// ** AbstractTreeModel::AbstractTreeModel
AbstractTreeModel::AbstractTreeModel( int columnCount, QObject* parent ) : QAbstractItemModel( parent ), m_columnCount( columnCount )
{

}

AbstractTreeModel::~AbstractTreeModel( void )
{

}

// ** AbstractTreeModel::root
AbstractTreeModel::TreeItem* AbstractTreeModel::root( void ) const
{
    return &m_root;
}

// ** AbstractTreeModel::clear
void AbstractTreeModel::clear( void )
{
    QModelIndex index = indexFromItem( root() );

    beginRemoveRows( index, 0, root()->childCount() );
    root()->clear();
    endRemoveRows();
}

// ** AbstractTreeModel::index
QModelIndex AbstractTreeModel::index( int row, int column, const QModelIndex& parent ) const
{
    if( !hasIndex( row, column, parent ) ) {
        return QModelIndex();
    }

    const TreeItem* item = root();

    if( parent.isValid() ) {
        item = itemAtIndex( parent );
    }

    const TreeItem* child = item->child( row );
    QModelIndex        index = createIndex( row, column, ( void* )child );

    return index;
}

// ** AbstractTreeModel::supportedDropActions
Qt::DropActions AbstractTreeModel::supportedDropActions( void ) const
{
    return Qt::MoveAction;
}

// ** AbstractTreeModel::createMimeData
QMimeData* AbstractTreeModel::createMimeData( const QModelIndexList& indexes ) const
{
    return new QMimeData;
}

// ** AbstractTreeModel::mimeData
QMimeData* AbstractTreeModel::mimeData( const QModelIndexList& indexes ) const
{
    // Serialize TreeItem pointers to stream
    QByteArray encodedData;
    QDataStream stream( &encodedData, QIODevice::WriteOnly );

    foreach( const QModelIndex& idx, indexes ) {
        stream << ( quint64 )itemAtIndex( idx );
    }

    // Construct the MIME data
    QMimeData* data = createMimeData( indexes );
    data->setData( mimeTypes().at( 0 ), encodedData );

    return data;
}

// ** AbstractTreeModel::dropMimeData
bool AbstractTreeModel::dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent )
{
    if( action == Qt::IgnoreAction ) {
        return true;
    }

    if( !data->hasFormat( mimeTypes().at( 0 ) ) ) {
        return false;
    }

    if( column > 0 ) {
        return false;
    }

    QByteArray       encodedData = data->data( mimeTypes().at( 0 ) );
    QDataStream      stream( &encodedData, QIODevice::ReadOnly );
    QList<TreeItem*> items;

    while( !stream.atEnd() ) {
        quint64 ptr;
        stream >> ptr;
        items << ( TreeItem* )ptr;
    }

    // Calculate the parent row
    if( row == -1 ) {
        row = itemAtIndex( parent )->childCount();
    }

    // Move rows
    foreach( TreeItem* item, items ) {
        moveRow( parentIndexFromItem( item ), item->row(), parent, row );
    }

    return true;
}

// ** AbstractTreeModel::moveRows
bool AbstractTreeModel::moveRows( const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild )
{
    // Get the source parent item
    TreeItem* sourceParentItem = itemAtIndex( sourceParent );
    Q_ASSERT( sourceParentItem );

    // Get the destination parent item
    TreeItem* destinationParentItem    = itemAtIndex( destinationParent );
    Q_ASSERT( destinationParentItem );

    // Get item to be moved
    TreeItem* item = sourceParentItem->child( sourceRow );
    Q_ASSERT( item );

    // Perform checks
    Q_ASSERT( item != sourceParentItem && item != destinationParentItem );

    // Perform the item movement
    if( !moveItem( sourceParentItem == root() ? NULL : sourceParentItem, destinationParentItem == root() ? NULL : destinationParentItem, item, destinationChild ) ) {
        return false;
    }

    beginMoveRows( sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild );
    item->setParent( destinationParentItem, destinationChild );
    endMoveRows();

    return true;
}

#if DC_QT_VERSION == 4

// ** AbstractTreeModel::moveRow
bool AbstractTreeModel::moveRow( const QModelIndex& sourceParent, int sourceRow, const QModelIndex& destinationParent, int destinationChild )
{
    moveRows( sourceParent, sourceRow, 1, destinationParent, destinationChild );
    return true;
}

#endif    /*    #if DC_QT_VERSION == 4    */

// ** AbstractTreeModel::parent
QModelIndex AbstractTreeModel::parent( const QModelIndex& child ) const
{
    if( !child.isValid() ) {
        return QModelIndex();
    }

    TreeItem* c = itemAtIndex( child );
    TreeItem* p = c->parent();

    if( p == root() ) {
        return QModelIndex();
    }

    return indexFromItem( p );
}

// ** AbstractTreeModel::rowCount
int AbstractTreeModel::rowCount( const QModelIndex& parent ) const
{
    if( parent.column() > 0 ) {
        return 0;
    }

    const TreeItem* parentItem = root();

    if( parent.isValid() ) {
        parentItem = itemAtIndex( parent );
    }

    return parentItem->childCount();
}

// ** AbstractTreeModel::columnCount
int AbstractTreeModel::columnCount( const QModelIndex& parent ) const
{
    return m_columnCount;
}

// ** AbstractTreeModel::itemAtIndex
AbstractTreeModel::TreeItem* AbstractTreeModel::itemAtIndex( const QModelIndex& index ) const
{
    if( !index.isValid() ) {
        return root();
    }

    return reinterpret_cast<TreeItem*>( index.internalPointer() );
}

// ** AbstractTreeModel::parentIndexFromItem
QModelIndex AbstractTreeModel::parentIndexFromItem( const TreeItem* item ) const
{
    if( item->parent() == root() ) {
        return QModelIndex();
    }

    return indexFromItem( item->parent() );
}

// ** AbstractTreeModel::indexFromItem
QModelIndex AbstractTreeModel::indexFromItem( const TreeItem* item ) const
{
    if( !item ) {
        return QModelIndex();
    }

    int row = item->row();
    return createIndex( row != -1 ? row : 0, 0, ( void* )item );
}

// ** AbstractTreeModel::moveItem
bool AbstractTreeModel::moveItem( TreeItem* sourceParent, TreeItem* destinationParent, TreeItem* item, int destinationRow ) const
{
    return true;
}

// ** AbstractTreeModel::addAsset
void AbstractTreeModel::addItem( TreeItem* item, TreeItem* parent )
{
    // Get index
    QModelIndex index = indexFromItem( parent );
    int count = parent ? parent->childCount() : root()->childCount();

    // Perform insertion
    beginInsertRows( index, count, count );
    {
        // Add it to parent
        if( parent ) {
            parent->addChild( item );
        } else {
            root()->addChild( item );
        }
    }
    endInsertRows();

    // Emit the signal
    emit dataChanged( index, index );
}

// ** AbstractTreeModel::removeItem
void AbstractTreeModel::removeItem( TreeItem* item )
{
    // Get index
    int row = item->row();
    QModelIndex index = parentIndexFromItem( item );

    // Perform removal
    beginRemoveRows( index, row, row );
    {
        item->parent()->removeChild( item );
        delete item;
    }
    endRemoveRows();

    // Emit the signal
    emit dataChanged( index, index );
}

// ------------------------------------------------------ AbstractTreeModel::TreeItem ------------------------------------------------------ //

// ** AbstractTreeModel::TreeItem::TreeItem
AbstractTreeModel::TreeItem::TreeItem( TreeItem* parent ) : m_parent( parent )
{
}

AbstractTreeModel::TreeItem::~TreeItem( void )
{
    qDeleteAll( m_children );
}

// ** AbstractTreeModel::TreeItem::clear
void AbstractTreeModel::TreeItem::clear( void )
{
    qDeleteAll( m_children );
    m_children.clear();
}

// ** AbstractTreeModel::TreeItem::row
int AbstractTreeModel::TreeItem::row( void ) const
{
    if( !m_parent ) {
        return -1;
    }

    return m_parent->m_children.indexOf( const_cast<TreeItem*>( this ) );
}

// ** AbstractTreeModel::TreeItem::child
AbstractTreeModel::TreeItem* AbstractTreeModel::TreeItem::child( int index ) const
{
    return m_children.at( index );
}

// ** AbstractTreeModel::TreeItem::addChild
void AbstractTreeModel::TreeItem::addChild( TreeItem* child )
{
    // Set this item as parent of child
    child->setParent( this, childCount() );
}

// ** AbstractTreeModel::TreeItem::removeChild
void AbstractTreeModel::TreeItem::removeChild( TreeItem* child )
{
    // Find child
    int index = m_children.indexOf( child );
    Q_ASSERT( index != -1 );

    // Remove from array
    m_children.remove( index );
}

// ** AbstractTreeModel::TreeItem::parent
AbstractTreeModel::TreeItem* AbstractTreeModel::TreeItem::parent( void ) const
{
    return m_parent;
}

// ** AbstractTreeModel::TreeItem::childCount
int AbstractTreeModel::TreeItem::childCount( void ) const
{
    return m_children.count();
}

// ** AbstractTreeModel::TreeItem::setRow
void AbstractTreeModel::TreeItem::setRow( int value )
{
    int row = this->row();

    if( row != -1 ) {
        m_parent->m_children.remove( row );
    }

    m_parent->m_children.insert( value, this );
}

// ** AbstractTreeModel::TreeItem::setParent
void AbstractTreeModel::TreeItem::setParent( TreeItem* parent, int row )
{
    // Remove from an old parent
    int oldRow = this->row();

    if( oldRow != -1 ) {
        m_parent->m_children.remove( oldRow );
    }

    // Set the new parent
    m_parent = parent;

    // Set the row
    setRow( row );
}

DC_END_COMPOSER
