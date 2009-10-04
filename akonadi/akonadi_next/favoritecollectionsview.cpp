/*
    Copyright (c) 2006 - 2007 Volker Krause <vkrause@kde.org>
    Copyright (c) 2008 Stephen Kelly <steveire@gmail.com>
    Copyright (c) 2009 Kevin Ottens <ervin@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include "favoritecollectionsview.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QHeaderView>
#include <QtGui/QMenu>

#include <KAction>
#include <KLocale>
#include <KMessageBox>
#include <KUrl>
#include <KXMLGUIFactory>

#include <kxmlguiclient.h>

#include <akonadi/collection.h>
#include <akonadi/control.h>
#include <akonadi/item.h>
#include <akonadi/entitytreemodel.h>

#include <kdebug.h>

using namespace Akonadi;

/**
 * @internal
 */
class FavoriteCollectionsView::Private
{
public:
  Private( FavoriteCollectionsView *parent )
      : mParent( parent ),
      xmlGuiClient( 0 )
  {
  }

  void init();
  void itemClicked( const QModelIndex& );
  void itemDoubleClicked( const QModelIndex& );
  void itemCurrentChanged( const QModelIndex& );

  FavoriteCollectionsView *mParent;

  KXMLGUIClient *xmlGuiClient;
};

void FavoriteCollectionsView::Private::init()
{
  mParent->setEditTriggers( QAbstractItemView::EditKeyPressed );
  mParent->setAcceptDrops( true );
  mParent->setDropIndicatorShown( true );
  mParent->setDragDropMode( DragDrop );
  mParent->setDragEnabled( true );

  mParent->connect( mParent, SIGNAL( clicked( const QModelIndex& ) ),
                    mParent, SLOT( itemClicked( const QModelIndex& ) ) );
  mParent->connect( mParent, SIGNAL( doubleClicked( const QModelIndex& ) ),
                    mParent, SLOT( itemDoubleClicked( const QModelIndex& ) ) );

  Control::widgetNeedsAkonadi( mParent );
}

void FavoriteCollectionsView::Private::itemClicked( const QModelIndex &index )
{
  if ( !index.isValid() )
    return;

  const Collection collection = index.model()->data( index, EntityTreeModel::CollectionRole ).value<Collection>();
  if ( collection.isValid() ) {
    emit mParent->clicked( collection );
  }
}

void FavoriteCollectionsView::Private::itemDoubleClicked( const QModelIndex &index )
{
  if ( !index.isValid() )
    return;

  const Collection collection = index.model()->data( index, EntityTreeModel::CollectionRole ).value<Collection>();
  if ( collection.isValid() ) {
    emit mParent->doubleClicked( collection );
  }
}

void FavoriteCollectionsView::Private::itemCurrentChanged( const QModelIndex &index )
{
  if ( !index.isValid() )
    return;

  const Collection collection = index.model()->data( index, EntityTreeModel::CollectionRole ).value<Collection>();
  if ( collection.isValid() ) {
    emit mParent->currentChanged( collection );
  }
}

FavoriteCollectionsView::FavoriteCollectionsView( QWidget * parent ) :
    QListView( parent ),
    d( new Private( this ) )
{

  setSelectionMode( QAbstractItemView::SingleSelection );
  d->init();
}

FavoriteCollectionsView::FavoriteCollectionsView( KXMLGUIClient *xmlGuiClient, QWidget * parent ) :
    QListView( parent ),
    d( new Private( this ) )
{
  d->xmlGuiClient = xmlGuiClient;
  d->init();
}

FavoriteCollectionsView::~FavoriteCollectionsView()
{
  delete d;
}

void FavoriteCollectionsView::setModel( QAbstractItemModel * model )
{
  QListView::setModel( model );

  connect( selectionModel(), SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
           this, SLOT( itemCurrentChanged( const QModelIndex& ) ) );
}

void FavoriteCollectionsView::dragMoveEvent( QDragMoveEvent * event )
{
  QModelIndex index = indexAt( event->pos() );

  // Check if the collection under the cursor accepts this data type
  Collection col = model()->data( index, EntityTreeModel::CollectionRole ).value<Collection>();
  if (!col.isValid())
  {
    Item item = model()->data( index, EntityTreeModel::ItemRole ).value<Item>();
    if (item.isValid())
    {
      col = model()->data( index.parent(), EntityTreeModel::CollectionRole ).value<Collection>();
    }
  }
  if ( col.isValid() )
  {
    QStringList supportedContentTypes = col.contentMimeTypes();
    const QMimeData *data = event->mimeData();
    KUrl::List urls = KUrl::List::fromMimeData( data );
    foreach( const KUrl &url, urls ) {
      const Collection collection = Collection::fromUrl( url );
      if ( collection.isValid() ) {
        if ( !supportedContentTypes.contains( Collection::mimeType() ) )
          break;
      } else { // This is an item.
        QString type = url.queryItems()[ QString::fromLatin1( "type" )];
        if ( !supportedContentTypes.contains( type ) )
          break;
      }
      // All urls are supported. process the event.
      QListView::dragMoveEvent( event );
      return;
    }
  }

  event->setDropAction( Qt::IgnoreAction );
}

void FavoriteCollectionsView::dragLeaveEvent( QDragLeaveEvent * event )
{
  QListView::dragLeaveEvent( event );
}


void FavoriteCollectionsView::dropEvent( QDropEvent * event )
{
  QModelIndexList idxs = selectedIndexes();


  QMenu popup( this );
  QAction* moveDropAction;
  // TODO If possible, hide unavailable actions ...
  // Use the model to determine if a move is ok.
//   if (...)
//   {
  moveDropAction = popup.addAction( KIcon( QString::fromLatin1( "edit-rename" ) ), i18n( "&Move here" ) );
//   }

  //TODO: If dropping on one of the selectedIndexes, just return.
  // open a context menu offering different drop actions (move, copy and cancel)
  QAction* copyDropAction = popup.addAction( KIcon( QString::fromLatin1( "edit-copy" ) ), i18n( "&Copy here" ) );
  popup.addSeparator();
  popup.addAction( KIcon( QString::fromLatin1( "process-stop" ) ), i18n( "Cancel" ) );

  QAction *activatedAction = popup.exec( QCursor::pos() );

  if ( activatedAction == moveDropAction ) {
    event->setDropAction( Qt::MoveAction );
  } else if ( activatedAction == copyDropAction ) {
    event->setDropAction( Qt::CopyAction );
  }
  // TODO: Handle link action.
  else return;

  QListView::dropEvent( event );
}

void FavoriteCollectionsView::contextMenuEvent( QContextMenuEvent * event )
{
  if ( !d->xmlGuiClient )
    return;

  const QModelIndex index = indexAt( event->pos() );

  QMenu *popup = 0;

  // check if the index under the cursor is a collection or item
  const Collection collection = model()->data( index, EntityTreeModel::CollectionRole ).value<Collection>();
  if ( collection.isValid() )
    popup = static_cast<QMenu*>( d->xmlGuiClient->factory()->container(
                                 QLatin1String( "akonadi_favoriteview_contextmenu" ), d->xmlGuiClient ) );
  if ( popup )
    popup->exec( event->globalPos() );
}

void FavoriteCollectionsView::setXmlGuiClient( KXMLGUIClient * xmlGuiClient )
{
  d->xmlGuiClient = xmlGuiClient;
}

#include "favoritecollectionsview.moc"
