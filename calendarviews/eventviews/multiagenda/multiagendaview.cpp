/*
  Copyright (c) 2007 Volker Krause <vkrause@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "multiagendaview.h"
#include "configdialoginterface.h"

#include <eventviews/prefs.h>
#include <eventviews/agenda/agenda.h>
#include <eventviews/agenda/agendaview.h>
#include <eventviews/agenda/timelabelszone.h>

#include <calendarsupport/calendar.h>
#include <calendarsupport/calendarmodel.h>
#include <calendarsupport/collectionselection.h>
#include <calendarsupport/collectionselectionproxymodel.h>
#include <calendarsupport/entitymodelstatesaver.h>
#include <calendarsupport/utils.h>

#include <KCalCore/Event>
#include <Akonadi/EntityTreeView>

#include <KHBox>
#include <KVBox>
#include <KLocale>
#include <KGlobalSettings>

#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QSplitter>
#include <QTimer>
#include <QScrollArea>
#include <QScrollBar>
#include <QStandardItem>
#include <QPointer>
#include <QStandardItemModel>

#include <algorithm>

using namespace KCalCore;
using namespace EventViews;

static QString generateColumnLabel( int c )
{
  return i18n( "Agenda %1", c + 1 );
}

MultiAgendaView::MultiAgendaView( QWidget *parent )
  : EventView( parent ),
    mUpdateOnShow( true ),
    mPendingChanges( true ),
    mCustomColumnSetupUsed( false ),
    mCustomNumberOfColumns( 2 )
{
  QHBoxLayout *topLevelLayout = new QHBoxLayout( this );
  topLevelLayout->setSpacing( 0 );
  topLevelLayout->setMargin( 0 );

  QFontMetrics fm( font() );
  int topLabelHeight = 2 * fm.height() + fm.lineSpacing();

  KVBox *topSideBox = new KVBox( this );

  QWidget *topSideSpacer = new QWidget( topSideBox );
  topSideSpacer->setFixedHeight( topLabelHeight );

  mLeftSplitter = new QSplitter( Qt::Vertical, topSideBox );
  mLeftSplitter->setOpaqueResize( KGlobalSettings::opaqueResize() );

  QLabel *label = new QLabel( i18n( "All Day" ), mLeftSplitter );
  label->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
  label->setWordWrap( true );

  KVBox *sideBox = new KVBox( mLeftSplitter );

  EventIndicator *eiSpacer = new EventIndicator( EventIndicator::Top, sideBox );
  eiSpacer->changeColumns( 0 );

  // compensate for the frame the agenda views but not the timelabels have
  QWidget *timeLabelTopAlignmentSpacer = new QWidget( sideBox );

  mTimeLabelsZone = new TimeLabelsZone( sideBox, PrefsPtr( new Prefs() ) );

  QWidget *timeLabelBotAlignmentSpacer = new QWidget( sideBox );

  eiSpacer = new EventIndicator( EventIndicator::Bottom, sideBox );
  eiSpacer->changeColumns( 0 );

  mLeftBottomSpacer = new QWidget( topSideBox );

  topLevelLayout->addWidget( topSideBox );

  mScrollArea = new QScrollArea( this );

// TODO_EVENTVIEWS
  //mScrollArea->setResizePolicy( Q3ScrollView::Manual );
  mScrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

  // asymetric since the timelabels
  timeLabelTopAlignmentSpacer->setFixedHeight( mScrollArea->frameWidth() - 1 );
  // have 25 horizontal lines
  timeLabelBotAlignmentSpacer->setFixedHeight( mScrollArea->frameWidth() - 2 );

  mScrollArea->setFrameShape( QFrame::NoFrame );
  topLevelLayout->addWidget( mScrollArea, 100 );
  mTopBox = new KHBox( mScrollArea->viewport() );
  mScrollArea->setWidget( mTopBox );

  topSideBox = new KVBox( this );

  topSideSpacer = new QWidget( topSideBox );
  topSideSpacer->setFixedHeight( topLabelHeight );

  mRightSplitter = new QSplitter( Qt::Vertical, topSideBox );
  mRightSplitter->setOpaqueResize( KGlobalSettings::opaqueResize() );

  new QWidget( mRightSplitter );
  sideBox = new KVBox( mRightSplitter );

  eiSpacer = new EventIndicator( EventIndicator::Top, sideBox );
  eiSpacer->setFixedHeight( eiSpacer->minimumHeight() );
  eiSpacer->changeColumns( 0 );
  mScrollBar = new QScrollBar( Qt::Vertical, sideBox );
  eiSpacer = new EventIndicator( EventIndicator::Bottom, sideBox );
  eiSpacer->setFixedHeight( eiSpacer->minimumHeight() );
  eiSpacer->changeColumns( 0 );

  mRightBottomSpacer = new QWidget( topSideBox );
  topLevelLayout->addWidget( topSideBox );
}

void MultiAgendaView::setCalendar( CalendarSupport::Calendar *cal )
{
  EventView::setCalendar( cal );
  Q_FOREACH ( CalendarSupport::CollectionSelectionProxyModel *const i, mCollectionSelectionModels ) {
    i->setSourceModel( cal->treeModel() );
  }
  recreateViews();
}

void MultiAgendaView::recreateViews()
{
  if ( !mPendingChanges ) {
    return;
  }
  mPendingChanges = false;

  deleteViews();

  if ( mCustomColumnSetupUsed ) {
    Q_ASSERT( mCollectionSelectionModels.size() == mCustomNumberOfColumns );
    for ( int i = 0; i < mCustomNumberOfColumns; ++i ) {
      addView( mCollectionSelectionModels[i], mCustomColumnTitles[i] );
    }
  } else {
    Q_FOREACH( const Akonadi::Collection &i, collectionSelection()->selectedCollections() ) {
      if ( i.contentMimeTypes().contains( Event::eventMimeType() ) ) {
        addView( i );
      }
    }
  }
  // no resources activated, so stop here to avoid crashing somewhere down the line
  // TODO: show a nice message instead
  if ( mAgendaViews.isEmpty() ) {
    return;
  }

  setupViews();
  QTimer::singleShot( 0, this, SLOT(slotResizeScrollView()) );
  mTimeLabelsZone->updateAll();

  QScrollArea *timeLabel = mTimeLabelsZone->timeLabels().first();
  connect( timeLabel->verticalScrollBar(), SIGNAL(valueChanged(int)),
           mScrollBar, SLOT(setValue(int)) );
  connect( mScrollBar, SIGNAL(valueChanged(int)),
           timeLabel->verticalScrollBar(), SLOT(setValue(int)) );

  connect( mLeftSplitter, SIGNAL(splitterMoved(int,int)), SLOT(resizeSplitters()) );
  connect( mRightSplitter, SIGNAL(splitterMoved(int,int)), SLOT(resizeSplitters()) );
  QTimer::singleShot( 0, this, SLOT(resizeSplitters()) );
  QTimer::singleShot( 0, this, SLOT(setupScrollBar()) );

  mTimeLabelsZone->updateTimeLabelsPosition();

}

void MultiAgendaView::deleteViews()
{
  Q_FOREACH ( AgendaView *const i, mAgendaViews ) {
    CalendarSupport::CollectionSelectionProxyModel *proxy =
      i->takeCustomCollectionSelectionProxyModel();
    if ( proxy && !mCollectionSelectionModels.contains( proxy ) ) {
      delete proxy;
    }
    delete i;
  }

  mAgendaViews.clear();
  qDeleteAll( mAgendaWidgets );
  mAgendaWidgets.clear();
}

void MultiAgendaView::setupViews()
{
  foreach ( AgendaView *agenda, mAgendaViews ) {
    connect( agenda, SIGNAL(newEventSignal(Akonadi::Collection::List)),
             SIGNAL(newEventSignal(Akonadi::Collection::List)) );
    connect( agenda, SIGNAL(editIncidenceSignal(Akonadi::Item)),
             SIGNAL(editIncidenceSignal(Akonadi::Item)) );
    connect( agenda, SIGNAL(showIncidenceSignal(Akonadi::Item)),
             SIGNAL(showIncidenceSignal(Akonadi::Item)) );
    connect( agenda, SIGNAL(deleteIncidenceSignal(Akonadi::Item)),
             SIGNAL(deleteIncidenceSignal(Akonadi::Item)) );
    connect( agenda, SIGNAL(startMultiModify(const QString &)),
             SIGNAL(startMultiModify(const QString &)) );
    connect( agenda, SIGNAL(endMultiModify()),
             SIGNAL(endMultiModify()) );

    connect( agenda, SIGNAL(incidenceSelected(Akonadi::Item, const QDate &)),
             SIGNAL(incidenceSelected(Akonadi::Item, const QDate &)) );

    connect( agenda, SIGNAL(cutIncidenceSignal(Akonadi::Item)),
             SIGNAL(cutIncidenceSignal(Akonadi::Item)) );
    connect( agenda, SIGNAL(copyIncidenceSignal(Akonadi::Item)),
             SIGNAL(copyIncidenceSignal(Akonadi::Item)) );
    connect( agenda, SIGNAL(pasteIncidenceSignal()),
             SIGNAL(pasteIncidenceSignal()) );
    connect( agenda, SIGNAL(toggleAlarmSignal(Akonadi::Item)),
             SIGNAL(toggleAlarmSignal(Akonadi::Item)) );
    connect( agenda, SIGNAL(dissociateOccurrencesSignal(Akonadi::Item, const QDate&)),
             SIGNAL(dissociateOccurrencesSignal(Akonadi::Item, const QDate&)) );

    connect( agenda, SIGNAL(newEventSignal(Akonadi::Collection::List,const QDate&)),
             SIGNAL(newEventSignal(Akonadi::Collection::List,const QDate&)) );
    connect( agenda, SIGNAL(newEventSignal(Akonadi::Collection::List,const QDateTime&)),
             SIGNAL(newEventSignal(Akonadi::Collection::List,const QDateTime&)) );
    connect( agenda,
             SIGNAL(newEventSignal(Akonadi::Collection::List,const QDateTime&,const QDateTime&)),
             SIGNAL(newEventSignal(Akonadi::Collection::List,const QDateTime&,const QDateTime&)) );

    connect( agenda, SIGNAL(newTodoSignal(const QDate&)),
             SIGNAL(newTodoSignal(const QDate&)) );

    connect( agenda, SIGNAL(incidenceSelected(Akonadi::Item, const QDate &)),
             SLOT(slotSelectionChanged()) );

    connect( agenda, SIGNAL(timeSpanSelectionChanged()),
             SLOT(slotClearTimeSpanSelection()) );

    disconnect( agenda->agenda(),
                SIGNAL(zoomView(const int,const QPoint&,const Qt::Orientation)),
                agenda, 0 );
    connect( agenda->agenda(),
             SIGNAL(zoomView(const int,const QPoint&,const Qt::Orientation)),
             SLOT(zoomView(const int,const QPoint&,const Qt::Orientation)) );
  }

  AgendaView *lastView = mAgendaViews.last();
  foreach ( AgendaView *agenda, mAgendaViews ) {
    if ( agenda != lastView ) {
      connect( agenda->agenda()->verticalScrollBar(), SIGNAL(valueChanged(int)),
               lastView->agenda()->verticalScrollBar(), SLOT(setValue(int)) );
    }
  }

  foreach ( AgendaView *agenda, mAgendaViews ) {
    agenda->readSettings();
  }

  int minWidth = 0;
  foreach ( QWidget *widget, mAgendaWidgets ) {
    minWidth = qMax( minWidth, widget->minimumSizeHint().width() );
  }
  foreach ( QWidget *widget, mAgendaWidgets ) {
    widget->setMinimumWidth( minWidth );
  }
}

MultiAgendaView::~MultiAgendaView()
{
}

Akonadi::Item::List MultiAgendaView::selectedIncidences() const
{
  Akonadi::Item::List list;
  foreach ( AgendaView *agendaView, mAgendaViews ) {
    list += agendaView->selectedIncidences();
  }
  return list;
}

DateList MultiAgendaView::selectedIncidenceDates() const
{
  DateList list;
  foreach ( AgendaView *agendaView, mAgendaViews ) {
    list += agendaView->selectedIncidenceDates();
  }
  return list;
}

int MultiAgendaView::currentDateCount() const
{
  foreach ( AgendaView *agendaView, mAgendaViews ) {
    return agendaView->currentDateCount();
  }
  return 0;
}

void MultiAgendaView::showDates( const QDate &start, const QDate &end )
{
  mStartDate = start;
  mEndDate = end;
  slotResizeScrollView();
  mTimeLabelsZone->updateAll();
  foreach ( AgendaView *agendaView, mAgendaViews ) {
    agendaView->showDates( start, end );
  }
}

void MultiAgendaView::showIncidences( const Akonadi::Item::List &incidenceList, const QDate &date )
{
  foreach ( AgendaView *agendaView, mAgendaViews ) {
    agendaView->showIncidences( incidenceList, date );
  }
}

void MultiAgendaView::updateView()
{
  recreateViews();
  foreach ( AgendaView *agendaView, mAgendaViews ) {
    agendaView->updateView();
  }
}

void MultiAgendaView::changeIncidenceDisplay( const Akonadi::Item &incidence, int mode )
{
  foreach ( AgendaView *agendaView, mAgendaViews ) {
    agendaView->changeIncidenceDisplay( incidence, mode );
  }
}

int MultiAgendaView::maxDatesHint() const
{
  // these maxDatesHint functions aren't used
  return 31;
}

void MultiAgendaView::slotSelectionChanged()
{
  foreach ( AgendaView *agenda, mAgendaViews ) {
    if ( agenda != sender() ) {
      agenda->clearSelection();
    }
  }
}

bool MultiAgendaView::eventDurationHint( QDateTime &startDt, QDateTime &endDt, bool &allDay ) const
{
  foreach ( AgendaView *agenda, mAgendaViews ) {
    bool valid = agenda->eventDurationHint( startDt, endDt, allDay );
    if ( valid ) {
      return true;
    }
  }
  return false;
}

void MultiAgendaView::slotClearTimeSpanSelection()
{
  foreach ( AgendaView *agenda, mAgendaViews ) {
    if ( agenda != sender() ) {
      agenda->clearTimeSpanSelection();
    }
  }
}

AgendaView *MultiAgendaView::createView( const QString &title )
{
  QWidget *box = new QWidget( mTopBox );
  QVBoxLayout *layout = new QVBoxLayout( box );
  layout->setMargin( 0 );
  QLabel *l = new QLabel( title );
  layout->addWidget( l );
  l->setAlignment( Qt::AlignVCenter | Qt::AlignHCenter );
  AgendaView *av = new AgendaView( true, this );
  layout->addWidget( av );
  av->setCalendar( calendar() );
  av->setIncidenceChanger( changer() );
  av->agenda()->scrollArea()->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  mAgendaViews.append( av );
  mAgendaWidgets.append( box );
  box->show();
  mTimeLabelsZone->setAgendaView( av );
  connect( av->splitter(), SIGNAL(splitterMoved(int,int)), SLOT(resizeSplitters()) );
  connect( av, SIGNAL(showIncidencePopupSignal(Akonadi::Item,QDate)),
           SIGNAL(showIncidencePopupSignal(Akonadi::Item,QDate)) );

  connect( av, SIGNAL(showNewEventPopupSignal()),
           SIGNAL(showNewEventPopupSignal()) );

  return av;
}

void MultiAgendaView::addView( const Akonadi::Collection &collection )
{
  AgendaView *av = createView( CalendarSupport::displayName( collection ) );
  av->setCollection( collection.id() );
}

void MultiAgendaView::addView( CalendarSupport::CollectionSelectionProxyModel *sm, const QString &title )
{
  AgendaView *av = createView( title );
  av->setCustomCollectionSelectionProxyModel( sm );
}

void MultiAgendaView::resizeEvent( QResizeEvent *ev )
{
  resizeScrollView( ev->size() );
  EventView::resizeEvent( ev );
}

void MultiAgendaView::resizeScrollView( const QSize &size )
{
  const int widgetWidth = size.width() - mTimeLabelsZone->width() - mScrollBar->width();
  const int width = qMax( mTopBox->sizeHint().width(), widgetWidth );
  int height = size.height();
  if ( width > widgetWidth ) {
    const int sbHeight = mScrollArea->horizontalScrollBar()->height();
    height -= sbHeight;
    mLeftBottomSpacer->setFixedHeight( sbHeight );
    mRightBottomSpacer->setFixedHeight( sbHeight );
  } else {
    mLeftBottomSpacer->setFixedHeight( 0 );
    mRightBottomSpacer->setFixedHeight( 0 );
  }
  // TODO_EVENTVIEWS: review
  //mScrollArea->resizeContents( width, height );
  mTopBox->resize( width, height );
}

void MultiAgendaView::setIncidenceChanger( CalendarSupport::IncidenceChanger *changer )
{
  EventView::setIncidenceChanger( changer );
  foreach ( AgendaView *agenda, mAgendaViews ) {
    agenda->setIncidenceChanger( changer );
  }
}

void MultiAgendaView::updateConfig()
{
  EventView::updateConfig();
  mTimeLabelsZone->updateAll();
  foreach ( AgendaView *agenda, mAgendaViews ) {
    agenda->updateConfig();
  }
}

void MultiAgendaView::resizeSplitters()
{
  if ( mAgendaViews.isEmpty() ) {
    return;
  }

  QSplitter *lastMovedSplitter = qobject_cast<QSplitter*>( sender() );
  if ( !lastMovedSplitter ) {
    lastMovedSplitter = mAgendaViews.first()->splitter();
  }
  foreach ( AgendaView *agenda, mAgendaViews ) {
    if ( agenda->splitter() == lastMovedSplitter ) {
      continue;
    }
    agenda->splitter()->setSizes( lastMovedSplitter->sizes() );
  }
  if ( lastMovedSplitter != mLeftSplitter ) {
    mLeftSplitter->setSizes( lastMovedSplitter->sizes() );
  }
  if ( lastMovedSplitter != mRightSplitter ) {
    mRightSplitter->setSizes( lastMovedSplitter->sizes() );
  }
}

void MultiAgendaView::zoomView( const int delta, const QPoint &pos, const Qt::Orientation ori )
{
  const int hourSz = preferences()->hourSize();
  if ( ori == Qt::Vertical ) {
    if ( delta > 0 ) {
      if ( hourSz > 4 ) {
        preferences()->setHourSize( hourSz - 1);
      }
    } else {
      preferences()->setHourSize( hourSz + 1 );
    }
  }

  foreach ( AgendaView *agenda, mAgendaViews ) {
    agenda->zoomView( delta, pos, ori );
  }

  mTimeLabelsZone->updateAll();
}

void MultiAgendaView::slotResizeScrollView()
{
  resizeScrollView( size() );
}

void MultiAgendaView::showEvent( QShowEvent *event )
{
  EventView::showEvent( event );
  if ( mUpdateOnShow ) {
    mUpdateOnShow = false;
    mPendingChanges = true; // force a full view recreation
    showDates( mStartDate, mEndDate );
  }
}

void MultiAgendaView::setChanges( Changes changes )
{
  EventView::setChanges( changes );
  foreach ( AgendaView *agenda, mAgendaViews ) {
    agenda->setChanges( changes );
  }
}

void MultiAgendaView::setupScrollBar()
{
  if ( !mAgendaViews.isEmpty() && mAgendaViews.first()->agenda() ) {
    QScrollBar *scrollBar = mAgendaViews.first()->agenda()->verticalScrollBar();
    mScrollBar->setMinimum( scrollBar->minimum() );
    mScrollBar->setMaximum( scrollBar->maximum() );
    mScrollBar->setSingleStep( scrollBar->singleStep() );
    mScrollBar->setPageStep( scrollBar->pageStep() );
    mScrollBar->setValue( scrollBar->value() );
  }
}

void MultiAgendaView::collectionSelectionChanged()
{
  kDebug();
  mPendingChanges = true;
  recreateViews();
}

bool MultiAgendaView::hasConfigurationDialog() const
{

  /** The wrapper in korg has the dialog. Too complicated to move to CalendarViews.
      Depends on korg/AkonadiCollectionView, and will be refactored some day
      to get rid of CollectionSelectionProxyModel/EntityStateSaver */
  return false;
}

void MultiAgendaView::doRestoreConfig( const KConfigGroup &configGroup )
{
  mCustomColumnSetupUsed = configGroup.readEntry( "UseCustomColumnSetup", false );
  mCustomNumberOfColumns = configGroup.readEntry( "CustomNumberOfColumns", 2 );
  mCustomColumnTitles =  configGroup.readEntry( "ColumnTitles", QStringList() ).toVector();
  if ( mCustomColumnTitles.size() != mCustomNumberOfColumns ) {
    const int orig = mCustomColumnTitles.size();
    mCustomColumnTitles.resize( mCustomNumberOfColumns );
    for ( int i = orig; i < mCustomNumberOfColumns; ++i ) {
      mCustomColumnTitles[i] = generateColumnLabel( i );
    }
  }
  QVector<CalendarSupport::CollectionSelectionProxyModel*> oldModels = mCollectionSelectionModels;
  mCollectionSelectionModels.clear();
  mCollectionSelectionModels.resize( mCustomNumberOfColumns );
  for ( int i = 0; i < mCustomNumberOfColumns; ++i ) {
    const KConfigGroup g = configGroup.config()->group( configGroup.name() +
                                                        "_subView_" +
                                                        QByteArray::number( i ) );
    CalendarSupport::CollectionSelectionProxyModel *selection = new CalendarSupport::CollectionSelectionProxyModel;
    selection->setCheckableColumn( CalendarSupport::CalendarModel::CollectionTitle );
    selection->setDynamicSortFilter( true );
    if ( calendar() ) {
      selection->setSourceModel( calendar()->treeModel() );
    }

    QItemSelectionModel *qsm = new QItemSelectionModel( selection, selection );
    selection->setSelectionModel( qsm );
    CalendarSupport::EntityModelStateSaver *saver = new CalendarSupport::EntityModelStateSaver( selection, selection );
    saver->addRole( Qt::CheckStateRole, "CheckState" );
    saver->restoreConfig( g );
    mCollectionSelectionModels[i] = selection;
  }
  mPendingChanges = true;
  recreateViews();
  qDeleteAll( oldModels );
}

void MultiAgendaView::doSaveConfig( KConfigGroup &configGroup )
{
  configGroup.writeEntry( "UseCustomColumnSetup", mCustomColumnSetupUsed );
  configGroup.writeEntry( "CustomNumberOfColumns", mCustomNumberOfColumns );
  const QStringList titleList = mCustomColumnTitles.toList();
  configGroup.writeEntry( "ColumnTitles", titleList );
  int idx = 0;
  Q_FOREACH ( CalendarSupport::CollectionSelectionProxyModel *i, mCollectionSelectionModels ) {
    KConfigGroup g = configGroup.config()->group( configGroup.name() +
                                                  "_subView_" +
                                                  QByteArray::number( idx ) );
    ++idx;
    CalendarSupport::EntityModelStateSaver saver( i );
    saver.addRole( Qt::CheckStateRole, "CheckState" );
    saver.saveConfig( g );
  }
}

void MultiAgendaView::customCollectionsChanged( ConfigDialogInterface *dlg )
{
  if ( !mCustomColumnSetupUsed && !dlg->useCustomColumns() ) {
    // Config didn't change, no need to recreate views
    return;
  }

  mCustomColumnSetupUsed = dlg->useCustomColumns();
  mCustomNumberOfColumns = dlg->numberOfColumns();
  QVector<CalendarSupport::CollectionSelectionProxyModel*> newModels;
  newModels.resize( mCustomNumberOfColumns );
  mCustomColumnTitles.resize( mCustomNumberOfColumns );
  for ( int i = 0; i < mCustomNumberOfColumns; ++i ) {
    newModels[i] = dlg->takeSelectionModel( i );
    mCustomColumnTitles[i] = dlg->columnTitle( i );
  }
  mCollectionSelectionModels = newModels;
  mPendingChanges = true;
  recreateViews();
}

bool MultiAgendaView::customColumnSetupUsed() const
{
  return mCustomColumnSetupUsed;
}

int MultiAgendaView::customNumberOfColumns() const
{
  return mCustomNumberOfColumns;
}

QVector<CalendarSupport::CollectionSelectionProxyModel*>
MultiAgendaView::collectionSelectionModels() const
{
  return mCollectionSelectionModels;
}

QVector<QString> MultiAgendaView::customColumnTitles() const
{
  return mCustomColumnTitles;
}

#include "multiagendaview.moc"
