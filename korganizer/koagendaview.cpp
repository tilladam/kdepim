/*
    This file is part of KOrganizer.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>
    Copyright (C) 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlayout.h>
#ifndef KORG_NOSPLITTER
#include <qsplitter.h>
#endif
#include <qfont.h>
#include <qfontmetrics.h>
#include <qpopupmenu.h>
#include <qtooltip.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qcursor.h>
#include <qbitarray.h>

#include <kapplication.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kholidays.h>

#include <libkcal/calendar.h>
#include <libkcal/icaldrag.h>
#include <libkcal/dndfactory.h>
#include <libkcal/calfilter.h>

#include <kcalendarsystem.h>

#include "koglobals.h"
#ifndef KORG_NOPLUGINS
#include "kocore.h"
#endif
#include "koprefs.h"
#include "koagenda.h"
#include "koagendaitem.h"
#include "timelabels.h"

#include "koincidencetooltip.h"
#include "kogroupware.h"
#include "kodialogmanager.h"
#include "koeventpopupmenu.h"

#include "koagendaview.h"
#include "koagendaview.moc"

using namespace KOrg;


EventIndicator::EventIndicator(Location loc,QWidget *parent,const char *name)
  : QFrame(parent,name)
{
  mColumns = 1;
  mEnabled.resize( mColumns );
  mLocation = loc;

  if (mLocation == Top) mPixmap = KOGlobals::self()->smallIcon("upindicator");
  else mPixmap = KOGlobals::self()->smallIcon("downindicator");

  setMinimumHeight(mPixmap.height());
}

EventIndicator::~EventIndicator()
{
}

void EventIndicator::drawContents(QPainter *p)
{
//  kdDebug(5850) << "======== top: " << contentsRect().top() << "  bottom "
//         << contentsRect().bottom() << "  left " << contentsRect().left()
//         << "  right " << contentsRect().right() << endl;

  int i;
  for(i=0;i<mColumns;++i) {
    if (mEnabled[i]) {
      int cellWidth = contentsRect().right()/mColumns;
      int xOffset = KOGlobals::self()->reverseLayout() ?
               (mColumns - 1 - i)*cellWidth + cellWidth/2 -mPixmap.width()/2 :
               i*cellWidth + cellWidth/2 -mPixmap.width()/2;
      p->drawPixmap(QPoint(xOffset,0),mPixmap);
    }
  }
}

void EventIndicator::changeColumns(int columns)
{
  mColumns = columns;
  mEnabled.resize(mColumns);

  update();
}

void EventIndicator::enableColumn(int column, bool enable)
{
  mEnabled[column] = enable;
}


#include <libkcal/incidence.h>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


KOAlternateLabel::KOAlternateLabel(const QString &shortlabel, const QString &longlabel,
    const QString &extensivelabel, QWidget *parent, const char *name )
  : QLabel(parent, name), mTextTypeFixed(false), mShortText(shortlabel),
    mLongText(longlabel), mExtensiveText(extensivelabel)
{
  setSizePolicy(QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ));
  if (mExtensiveText.isEmpty()) mExtensiveText = mLongText;
  squeezeTextToLabel();
}

KOAlternateLabel::~KOAlternateLabel()
{
}

void KOAlternateLabel::useShortText()
{
  mTextTypeFixed = true;
  QLabel::setText( mShortText );
  QToolTip::remove( this );
  QToolTip::add( this, mExtensiveText );
  update(); // for kolab/issue4350
}

void KOAlternateLabel::useLongText()
{
  mTextTypeFixed = true;
  QLabel::setText( mLongText );
  QToolTip::remove( this );
  QToolTip::add( this, mExtensiveText );
  update(); // for kolab/issue4350
}

void KOAlternateLabel::useExtensiveText()
{
  mTextTypeFixed = true;
  QLabel::setText( mExtensiveText );
  QToolTip::remove( this );
  QToolTip::add( this, "" );
  update(); // for kolab/issue4350
}

void KOAlternateLabel::useDefaultText()
{
  mTextTypeFixed = false;
  squeezeTextToLabel();
}

KOAlternateLabel::TextType KOAlternateLabel::largestFittingTextType() const
{
  QFontMetrics fm( fontMetrics() );
  const int labelWidth = size().width();
  const int longTextWidth = fm.width( mLongText );
  const int extensiveTextWidth = fm.width( mExtensiveText );
  if ( extensiveTextWidth <= labelWidth )
    return Extensive;
  else if ( longTextWidth <= labelWidth )
    return Long;
  else
    return Short;
}

void KOAlternateLabel::setFixedType( TextType type )
{
  switch ( type )
  {
    case Extensive: useExtensiveText(); break;
    case Long: useLongText(); break;
    case Short: useShortText(); break;
  }
}

void KOAlternateLabel::squeezeTextToLabel()
{
  if ( mTextTypeFixed )
    return;

  const TextType type = largestFittingTextType();
  switch ( type )
  {
    case Extensive:
      QLabel::setText( mExtensiveText );
      QToolTip::remove( this );
      QToolTip::add( this, "" );
      break;
    case Long:
      QLabel::setText( mLongText );
      QToolTip::remove( this );
      QToolTip::add( this, mExtensiveText );
      break;
    case Short:
      QLabel::setText( mShortText );
      QToolTip::remove( this );
      QToolTip::add( this, mExtensiveText );
      break;
  }
  update(); // for kolab/issue4350
}

void KOAlternateLabel::resizeEvent( QResizeEvent * )
{
  squeezeTextToLabel();
}

QSize KOAlternateLabel::minimumSizeHint() const
{
  QSize sh = QLabel::minimumSizeHint();
  sh.setWidth(-1);
  return sh;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

KOAgendaView::KOAgendaView( Calendar *cal,
                            CalendarView *calendarView,
                            QWidget *parent,
                            const char *name,
                            bool isSideBySide ) :
  KOrg::AgendaView (cal, parent,name), mExpandButton( 0 ),
  mAllowAgendaUpdate( true ),
  mUpdateItem( 0 ),
  mIsSideBySide( isSideBySide ),
  mPendingChanges( true ),
  mAreDatesInitialized( false ),
  mProcessingDrop( false )
{
  mSelectedDates.append(QDate::currentDate());

  mLayoutDayLabels = 0;
  mDayLabelsFrame = 0;
  mDayLabels = 0;

  bool isRTL = KOGlobals::self()->reverseLayout();

  if ( KOPrefs::instance()->compactDialogs() ) {
    if ( KOPrefs::instance()->mVerticalScreen ) {
      mExpandedPixmap = KOGlobals::self()->smallIcon( "1downarrow" );
      mNotExpandedPixmap = KOGlobals::self()->smallIcon( "1uparrow" );
    } else {
      mExpandedPixmap = KOGlobals::self()->smallIcon( isRTL ? "1leftarrow" : "1rightarrow" );
      mNotExpandedPixmap = KOGlobals::self()->smallIcon( isRTL ? "1rightarrow" : "1leftarrow" );
    }
  }

  QBoxLayout *topLayout = new QVBoxLayout(this);

  // Create day name labels for agenda columns
  mDayLabelsFrame = new QHBox(this);
  topLayout->addWidget(mDayLabelsFrame);

  // Create agenda splitter
#ifndef KORG_NOSPLITTER
  mSplitterAgenda = new QSplitter(Vertical,this);
  topLayout->addWidget(mSplitterAgenda);

#if KDE_IS_VERSION( 3, 1, 93 )
  mSplitterAgenda->setOpaqueResize( KGlobalSettings::opaqueResize() );
#else
  mSplitterAgenda->setOpaqueResize();
#endif

  mAllDayFrame = new QHBox(mSplitterAgenda);

  QWidget *agendaFrame = new QWidget(mSplitterAgenda);
#else
  QVBox *mainBox = new QVBox( this );
  topLayout->addWidget( mainBox );

  mAllDayFrame = new QHBox(mainBox);

  QWidget *agendaFrame = new QWidget(mainBox);
#endif

  // Create all-day agenda widget
  mDummyAllDayLeft = new QVBox( mAllDayFrame );
  if ( isSideBySide )
    mDummyAllDayLeft->hide();

  if ( KOPrefs::instance()->compactDialogs() ) {
    mExpandButton = new QPushButton(mDummyAllDayLeft);
    mExpandButton->setPixmap( mNotExpandedPixmap );
    mExpandButton->setSizePolicy( QSizePolicy( QSizePolicy::Fixed,
                                  QSizePolicy::Fixed ) );
    connect( mExpandButton, SIGNAL( clicked() ), SIGNAL( toggleExpand() ) );
  } else {
    QLabel *label = new QLabel( i18n("All Day"), mDummyAllDayLeft );
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter | Qt::WordBreak );
  }

  mAllDayAgenda = new KOAgenda( 1, calendarView, this, mAllDayFrame );
  mAllDayAgenda->setCalendar( calendar() );
  QWidget *dummyAllDayRight = new QWidget(mAllDayFrame);

  // Create agenda frame
  QGridLayout *agendaLayout = new QGridLayout(agendaFrame,3,3);
//  QHBox *agendaFrame = new QHBox(splitterAgenda);

  // create event indicator bars
  mEventIndicatorTop = new EventIndicator(EventIndicator::Top,agendaFrame);
  agendaLayout->addWidget(mEventIndicatorTop,0,1);
  mEventIndicatorBottom = new EventIndicator(EventIndicator::Bottom,
                                             agendaFrame);
  agendaLayout->addWidget(mEventIndicatorBottom,2,1);
  QWidget *dummyAgendaRight = new QWidget(agendaFrame);
  agendaLayout->addWidget(dummyAgendaRight,0,2);

  // Create time labels
  mTimeLabels = new TimeLabels(24,agendaFrame);
  agendaLayout->addWidget(mTimeLabels,1,0);

  // Create agenda
  mAgenda = new KOAgenda( 1, 96, KOPrefs::instance()->mHourSize, calendarView, this, agendaFrame );
  mAgenda->setCalendar( calendar() );
  agendaLayout->addMultiCellWidget(mAgenda,1,1,1,2);
  agendaLayout->setColStretch(1,1);

  // Create event context menu for agenda
  mAgendaPopup = eventPopup();

  // Create event context menu for all day agenda
  mAllDayAgendaPopup = eventPopup();

  // make connections between dependent widgets
  mTimeLabels->setAgenda(mAgenda);
  if ( isSideBySide )
    mTimeLabels->hide();

  // Update widgets to reflect user preferences
//  updateConfig();

  createDayLabels( true );

  if ( !isSideBySide ) {
    // these blank widgets make the All Day Event box line up with the agenda
    dummyAllDayRight->setFixedWidth(mAgenda->verticalScrollBar()->width());
    dummyAgendaRight->setFixedWidth(mAgenda->verticalScrollBar()->width());
  }

  updateTimeBarWidth();

  // Scrolling
  connect(mAgenda->verticalScrollBar(),SIGNAL(valueChanged(int)),
          mTimeLabels, SLOT(positionChanged()));

  connect( mAgenda,
    SIGNAL( zoomView( const int, const QPoint & ,const Qt::Orientation ) ),
    SLOT( zoomView( const int, const QPoint &, const Qt::Orientation ) ) );

  connect(mTimeLabels->verticalScrollBar(),SIGNAL(valueChanged(int)),
          SLOT(setContentsPos(int)));

  // Create Events, depends on type of agenda
  connect( mAgenda, SIGNAL(newTimeSpanSignal(const QPoint &, const QPoint &)),
                    SLOT(newTimeSpanSelected(const QPoint &, const QPoint &)));
  connect( mAllDayAgenda, SIGNAL(newTimeSpanSignal(const QPoint &, const QPoint &)),
                          SLOT(newTimeSpanSelectedAllDay(const QPoint &, const QPoint &)));

  // event indicator update
  connect( mAgenda, SIGNAL(lowerYChanged(int)),
                    SLOT(updateEventIndicatorTop(int)));
  connect( mAgenda, SIGNAL(upperYChanged(int)),
                    SLOT(updateEventIndicatorBottom(int)));

  if ( !readOnly() ) {
    connectAgenda( mAgenda, mAgendaPopup, mAllDayAgenda );
    connectAgenda( mAllDayAgenda, mAllDayAgendaPopup, mAgenda);
  }

  if ( cal ) {
    cal->registerObserver( this );
  }
}


KOAgendaView::~KOAgendaView()
{
  if ( calendar() )
    calendar()->unregisterObserver( this );
  delete mAgendaPopup;
  delete mAllDayAgendaPopup;
}

void KOAgendaView::connectAgenda( KOAgenda *agenda, QPopupMenu *popup,
                                  KOAgenda *otherAgenda )
{
  connect( agenda, SIGNAL(showIncidencePopupSignal(Calendar *,Incidence *,const QDate &)),
           popup, SLOT(showIncidencePopup(Calendar *,Incidence *,const QDate &)) );

  connect( agenda, SIGNAL(showNewEventPopupSignal()),
           SLOT(showNewEventPopup()) );


  // Create/Show/Edit/Delete Event
  connect( agenda, SIGNAL(newEventSignal(ResourceCalendar *,const QString &)),
           SIGNAL(newEventSignal(ResourceCalendar *,const QString &)) );

  connect( agenda, SIGNAL(newStartSelectSignal()),
           otherAgenda, SLOT(clearSelection()) );
  connect( agenda, SIGNAL(newStartSelectSignal()),
           SIGNAL(timeSpanSelectionChanged()) );

  connect( agenda, SIGNAL(editIncidenceSignal(Incidence *,const QDate &)),
           SIGNAL(editIncidenceSignal(Incidence *,const QDate &)) );
  connect( agenda, SIGNAL(showIncidenceSignal(Incidence *,const QDate &)),
           SIGNAL(showIncidenceSignal(Incidence *,const QDate &)) );
  connect( agenda, SIGNAL(deleteIncidenceSignal(Incidence *)),
           SIGNAL(deleteIncidenceSignal(Incidence *)) );

  connect( agenda, SIGNAL(startMultiModify(const QString &)),
           SIGNAL(startMultiModify(const QString &)) );
  connect( agenda, SIGNAL(endMultiModify()),
           SIGNAL(endMultiModify()) );

  // drag signals
  connect( agenda, SIGNAL(startDragSignal(Incidence *)),
           SLOT(startDrag(Incidence *)) );

  // synchronize selections
  connect( agenda, SIGNAL(incidenceSelected(Incidence *,const QDate &)),
           otherAgenda, SLOT(deselectItem()) );
  connect( agenda, SIGNAL(incidenceSelected(Incidence *,const QDate &)),
           SIGNAL(incidenceSelected(Incidence *,const QDate &)) );

  // rescheduling of todos by d'n'd
  connect( agenda, SIGNAL(droppedIncidence(Incidence *,const QPoint &,bool)),
           SLOT(slotIncidenceDropped(Incidence *,const QPoint &,bool)) );

}

void KOAgendaView::zoomInVertically( )
{
  if ( !mIsSideBySide )
    KOPrefs::instance()->mHourSize++;
  mAgenda->updateConfig();
  mAgenda->checkScrollBoundaries();

  mTimeLabels->updateConfig();
  mTimeLabels->positionChanged();
  mTimeLabels->repaint();

  updateView();
}

void KOAgendaView::zoomOutVertically( )
{

  if ( KOPrefs::instance()->mHourSize > 4 || mIsSideBySide ) {

    if ( !mIsSideBySide )
      KOPrefs::instance()->mHourSize--;
    mAgenda->updateConfig();
    mAgenda->checkScrollBoundaries();

    mTimeLabels->updateConfig();
    mTimeLabels->positionChanged();
    mTimeLabels->repaint();

    updateView();
  }
}

void KOAgendaView::zoomInHorizontally( const QDate &date)
{
  QDate begin;
  QDate newBegin;
  QDate dateToZoom = date;
  int ndays,count;

  begin = mSelectedDates.first();
  ndays = begin.daysTo( mSelectedDates.last() );

  // zoom with Action and are there a selected Incidence?, Yes, I zoom in to it.
  if ( ! dateToZoom.isValid () )
    dateToZoom=mAgenda->selectedIncidenceDate();

  if( !dateToZoom.isValid() ) {
    if ( ndays > 1 ) {
      newBegin=begin.addDays(1);
      count = ndays-1;
      emit zoomViewHorizontally ( newBegin , count );
    }
  } else {
    if ( ndays <= 2 ) {
      newBegin = dateToZoom;
      count = 1;
    } else  {
      newBegin = dateToZoom.addDays( -ndays/2 +1  );
      count = ndays -1 ;
    }
    emit zoomViewHorizontally ( newBegin , count );
  }
}

void KOAgendaView::zoomOutHorizontally( const QDate &date )
{
  QDate begin;
  QDate newBegin;
  QDate dateToZoom = date;
  int ndays,count;

  begin = mSelectedDates.first();
  ndays = begin.daysTo( mSelectedDates.last() );

  // zoom with Action and are there a selected Incidence?, Yes, I zoom out to it.
  if ( ! dateToZoom.isValid () )
    dateToZoom=mAgenda->selectedIncidenceDate();

  if ( !dateToZoom.isValid() ) {
    newBegin = begin.addDays(-1);
    count = ndays+3 ;
  } else {
    newBegin = dateToZoom.addDays( -ndays/2-1 );
    count = ndays+3;
  }

  if ( abs( count ) >= 31 )
    kdDebug(5850) << "change to the mounth view?"<<endl;
  else
    //We want to center the date
    emit zoomViewHorizontally( newBegin, count );
}

void KOAgendaView::zoomView( const int delta, const QPoint &pos,
  const Qt::Orientation orient )
{
  static QDate zoomDate;
  static QTimer *t = new QTimer( this );


  //Zoom to the selected incidence, on the other way
  // zoom to the date on screen after the first mousewheel move.
  if ( orient == Qt::Horizontal ) {
    QDate date=mAgenda->selectedIncidenceDate();
    if ( date.isValid() )
      zoomDate=date;
    else{
      if ( !t->isActive() ) {
        zoomDate= mSelectedDates[pos.x()];
      }
      t->start ( 1000,true );
    }
    if ( delta > 0 )
      zoomOutHorizontally( zoomDate );
    else
      zoomInHorizontally( zoomDate );
  } else {
    // Vertical zoom
    QPoint posConstentsOld = mAgenda->gridToContents(pos);
    if ( delta > 0 ) {
      zoomOutVertically();
    } else {
      zoomInVertically();
    }
    QPoint posConstentsNew = mAgenda->gridToContents(pos);
    mAgenda->scrollBy( 0, posConstentsNew.y() - posConstentsOld.y() );
  }
}

void KOAgendaView::createDayLabels( bool force )
{
//  kdDebug(5850) << "KOAgendaView::createDayLabels()" << endl;

  // Check if mSelectedDates has changed, if not just return
  // Removes some flickering and gains speed (since this is called by each updateView())
  if ( !force && mSaveSelectedDates == mSelectedDates ) {
    return;
  }
  mSaveSelectedDates = mSelectedDates;

  delete mDayLabels;
  mDateDayLabels.clear();

  mDayLabels = new QFrame (mDayLabelsFrame);
  mLayoutDayLabels = new QHBoxLayout(mDayLabels);
  if ( !mIsSideBySide )
    mLayoutDayLabels->addSpacing(mTimeLabels->width());

  const KCalendarSystem*calsys=KOGlobals::self()->calendarSystem();

  DateList::ConstIterator dit;
  for( dit = mSelectedDates.begin(); dit != mSelectedDates.end(); ++dit ) {
    QDate date = *dit;
    QBoxLayout *dayLayout = new QVBoxLayout(mLayoutDayLabels);
    mLayoutDayLabels->setStretchFactor(dayLayout, 1);
//    dayLayout->setMinimumWidth(1);

    int dW = calsys->dayOfWeek(date);
    QString veryLongStr = KGlobal::locale()->formatDate( date );
    QString longstr = i18n( "short_weekday date (e.g. Mon 13)","%1 %2" )
        .arg( calsys->weekDayName( dW, true ) )
        .arg( calsys->day(date) );
    QString shortstr = QString::number(calsys->day(date));

    KOAlternateLabel *dayLabel = new KOAlternateLabel(shortstr,
                                                      longstr, veryLongStr, mDayLabels);
    dayLabel->useShortText(); // will be recalculated in updateDayLabelSizes() anyway
    dayLabel->setMinimumWidth(1);
    dayLabel->setAlignment(QLabel::AlignHCenter);
    if (date == QDate::currentDate()) {
      QFont font = dayLabel->font();
      font.setBold(true);
      dayLabel->setFont(font);
    }
    dayLayout->addWidget(dayLabel);
    mDateDayLabels.append( dayLabel );

    // if a holiday region is selected, show the holiday name
    QStringList texts = KOGlobals::self()->holiday( date );
    QStringList::ConstIterator textit = texts.begin();
    for ( ; textit != texts.end(); ++textit ) {
      // use a KOAlternateLabel so when the text doesn't fit any more a tooltip is used
      KOAlternateLabel*label = new KOAlternateLabel( (*textit), (*textit), QString::null, mDayLabels );
      label->setMinimumWidth(1);
      label->setAlignment(AlignCenter);
      dayLayout->addWidget(label);
    }

#ifndef KORG_NOPLUGINS
    CalendarDecoration::List cds = KOCore::self()->calendarDecorations();
    CalendarDecoration *it;
    for(it = cds.first(); it; it = cds.next()) {
      QString text = it->shortText( date );
      if ( !text.isEmpty() ) {
        // use a KOAlternateLabel so when the text doesn't fit any more a tooltip is used
        KOAlternateLabel*label = new KOAlternateLabel( text, text, QString::null, mDayLabels );
        label->setMinimumWidth(1);
        label->setAlignment(AlignCenter);
        dayLayout->addWidget(label);
      }
    }

    for(it = cds.first(); it; it = cds.next()) {
      QWidget *wid = it->smallWidget(mDayLabels,date);
      if ( wid ) {
//      wid->setHeight(20);
        dayLayout->addWidget(wid);
      }
    }
#endif
  }

  if ( !mIsSideBySide )
    mLayoutDayLabels->addSpacing(mAgenda->verticalScrollBar()->width());
  mDayLabels->show();
  QTimer::singleShot( 0, this, SLOT( updateDayLabelSizes() ) );
}

void KOAgendaView::enableAgendaUpdate( bool enable )
{
  mAllowAgendaUpdate = enable;
}

int KOAgendaView::maxDatesHint()
{
  // Not sure about the max number of events, so return 0 for now.
  return 0;
}

int KOAgendaView::currentDateCount()
{
  return mSelectedDates.count();
}

Incidence::List KOAgendaView::selectedIncidences()
{
  Incidence::List selected;
  Incidence *incidence;

  incidence = mAgenda->selectedIncidence();
  if (incidence) selected.append(incidence);

  incidence = mAllDayAgenda->selectedIncidence();
  if (incidence) selected.append(incidence);

  return selected;
}

DateList KOAgendaView::selectedIncidenceDates()
{
  DateList selected;
  QDate qd;

  qd = mAgenda->selectedIncidenceDate();
  if (qd.isValid()) selected.append(qd);

  qd = mAllDayAgenda->selectedIncidenceDate();
  if (qd.isValid()) selected.append(qd);

  return selected;
}

bool KOAgendaView::eventDurationHint( QDateTime &startDt, QDateTime &endDt,
                                      bool &allDay )
{
  if ( selectionStart().isValid() ) {
    QDateTime start = selectionStart();
    QDateTime end = selectionEnd();

    if ( start.secsTo( end ) == 15*60 ) {
      // One cell in the agenda view selected, e.g.
      // because of a double-click, => Use the default duration
      QTime defaultDuration( KOPrefs::instance()->mDefaultDuration.time() );
      int addSecs = ( defaultDuration.hour()*3600 ) +
                    ( defaultDuration.minute()*60 );
      end = start.addSecs( addSecs );
    }

    startDt = start;
    endDt = end;
    allDay = selectedIsAllDay();
    return true;
  }
  return false;
}

/** returns if only a single cell is selected, or a range of cells */
bool KOAgendaView::selectedIsSingleCell()
{
  if ( !selectionStart().isValid() || !selectionEnd().isValid() ) return false;

  if (selectedIsAllDay()) {
    int days = selectionStart().daysTo(selectionEnd());
    return ( days < 1 );
  } else {
    int secs = selectionStart().secsTo(selectionEnd());
    return ( secs <= 24*60*60/mAgenda->rows() );
  }
}


void KOAgendaView::updateView()
{
//  kdDebug(5850) << "KOAgendaView::updateView()" << endl;
  fillAgenda();
}


/*
  Update configuration settings for the agenda view. This method is not
  complete.
*/
void KOAgendaView::updateConfig()
{
//  kdDebug(5850) << "KOAgendaView::updateConfig()" << endl;

  // update config for children
  mTimeLabels->updateConfig();
  mAgenda->updateConfig();
  mAllDayAgenda->updateConfig();

  // widget synchronization
  // FIXME: find a better way, maybe signal/slot
  mTimeLabels->positionChanged();

  // for some reason, this needs to be called explicitly
  mTimeLabels->repaint();

  updateTimeBarWidth();

  // ToolTips displaying summary of events
  KOAgendaItem::toolTipGroup()->setEnabled(KOPrefs::instance()
                                           ->mEnableToolTips);

  setHolidayMasks();

  createDayLabels( true );

  updateView();
}

void KOAgendaView::updateTimeBarWidth()
{
  int width;

  width = mDummyAllDayLeft->fontMetrics().width( i18n("All Day") );
  width = QMAX( width, mTimeLabels->width() );

  mDummyAllDayLeft->setFixedWidth( width );
  mTimeLabels->setFixedWidth( width );
}

void KOAgendaView::updateDayLabelSizes()
{
  // First, calculate the maximum text type that fits for all labels
  KOAlternateLabel::TextType overallType = KOAlternateLabel::Extensive;
  QPtrList<KOAlternateLabel>::const_iterator it = mDateDayLabels.constBegin();
  for( ; it != mDateDayLabels.constEnd(); it++ ) {
    KOAlternateLabel::TextType type = (*it)->largestFittingTextType();
    if ( type < overallType )
      overallType = type;
  }

  // Then, set that maximum text type to all the labels
  it = mDateDayLabels.constBegin();
  for( ; it != mDateDayLabels.constEnd(); it++ ) {
    (*it)->setFixedType( overallType );
  }
}

void KOAgendaView::resizeEvent( QResizeEvent *resizeEvent )
{
  updateDayLabelSizes();
  KOrg::AgendaView::resizeEvent( resizeEvent );
}

void KOAgendaView::updateEventDates( KOAgendaItem *item,
                                     bool useLastGroupwareDialogAnswer,
                                     ResourceCalendar *res,
                                     const QString &subRes,
                                     bool addIncidence )
{
  kdDebug(5850) << "KOAgendaView::updateEventDates(): " << item->text()
                << "; item->cellXLeft(): " << item->cellXLeft()
                << "; item->cellYTop(): " << item->cellYTop()
                << "; item->lastMultiItem(): " << item->lastMultiItem()
                << "; item->itemPos(): " << item->itemPos()
                << "; item->itemCount(): " << item->itemCount()
                << endl;

  QDateTime startDt, endDt;

  // Start date of this incidence, calculate the offset from it (so recurring and
  // non-recurring items can be treated exactly the same, we never need to check
  // for doesRecur(), because we only move the start day by the number of days the
  // agenda item was really moved. Smart, isn't it?)
  QDate thisDate;
  if ( item->cellXLeft() < 0 ) {
    thisDate = ( mSelectedDates.first() ).addDays( item->cellXLeft() );
  } else {
    thisDate = mSelectedDates[ item->cellXLeft() ];
  }
  const QDate oldThisDate( item->itemDate() );
  const int daysOffset = oldThisDate.daysTo( thisDate );
  int daysLength = 0;

  Incidence *incidence = item->incidence();

  if ( !incidence || !mChanger ||
       ( !addIncidence && !mChanger->beginChange( incidence,
                                                  resourceCalendar(),
                                                  subResourceCalendar() ) ) ) {
    kdDebug() << "Weird, application has a bug?" << endl;
    return;
  }
  incidence->startUpdates();
  Incidence *oldIncidence = incidence->clone();

  QTime startTime( 0, 0, 0 ), endTime( 0, 0, 0 );
  if ( incidence->doesFloat() ) {
    daysLength = item->cellWidth() - 1;
  } else {
    startTime = mAgenda->gyToTime( item->cellYTop() );
    if ( item->lastMultiItem() ) {
      endTime = mAgenda->gyToTime( item->lastMultiItem()->cellYBottom() + 1 );
      daysLength = item->lastMultiItem()->cellXLeft() - item->cellXLeft();
      kdDebug(5850) << "item->lastMultiItem()->cellXLeft(): " << item->lastMultiItem()->cellXLeft()
                    << endl;
    } else if ( item->itemPos() == item->itemCount() && item->itemCount() > 1 ) {
      /* multiitem handling in agenda assumes two things:
         - The start (first KOAgendaItem) is always visible.
         - The first KOAgendaItem of the incidence has a non-null item->lastMultiItem()
             pointing to the last KOagendaItem.

        But those aren't always met, for example when in day-view.
        kolab/issue4417
       */

      // Cornercase 1: - Resizing the end of the event but the start isn't visible
      endTime = mAgenda->gyToTime( item->cellYBottom() + 1 );
      daysLength = item->itemCount() - 1;
      startTime = incidence->dtStart().time();
    } else if ( item->itemPos() == 1 && item->itemCount() > 1 ) {
      // Cornercase 2: - Resizing the start of the event but the end isn't visible
      endTime = incidence->dtEnd().time();
      daysLength = item->itemCount() - 1;
    } else {
      endTime = mAgenda->gyToTime( item->cellYBottom() + 1 );
    }
  }

  kdDebug(5850) << "daysLength: " << daysLength << "; startTime: " << startTime
                << "; endTime: " << endTime << "; thisDate: " << thisDate
                << "; incidence->dtStart(): " << incidence->dtStart() << endl;

  if ( incidence->type() == "Event" ) {
    startDt = incidence->dtStart();
    startDt = startDt.addDays( daysOffset );
    startDt.setTime( startTime );
    endDt = startDt.addDays( daysLength );
    endDt.setTime( endTime );
    Event* ev = static_cast<Event*>( incidence );
    if ( incidence->dtStart() == startDt && ev->dtEnd() == endDt ) {
      // No change
      delete oldIncidence;
      incidence->cancelUpdates();
      return;
    }
    incidence->setDtStart( startDt );
    ev->setDtEnd( endDt );
  } else if ( incidence->type() == "Todo" ) {
    // To-do logic must be reviewed.

    Todo *td = static_cast<Todo*>( incidence );
    startDt = td->hasStartDate() ? td->dtStart() : td->dtDue();
    startDt = thisDate.addDays( td->dtDue().daysTo( startDt ) );
    startDt.setTime( startTime );
    endDt.setDate( thisDate );
    endDt.setTime( endTime );

    if ( td->dtDue() == endDt ) {
      // No change
      delete oldIncidence;
      incidence->cancelUpdates();
      return;
    }

    if ( td->hasStartDate() ) {
      td->setDtStart( startDt );
    }
    td->setDtDue( endDt );
  }

  item->setItemDate( startDt.date() );

  KOIncidenceToolTip::remove( item );
  KOIncidenceToolTip::add( item, calendar(), incidence, thisDate, KOAgendaItem::toolTipGroup() );


  bool result;
  kdDebug() << "New date is " << incidence->dtStart() << endl;
  if ( addIncidence ) {
    result = mChanger->addIncidence( incidence, res, subRes, this, useLastGroupwareDialogAnswer );
  } else {
    result = mChanger->changeIncidence( oldIncidence, incidence,
                                                   KOGlobals::DATE_MODIFIED, this, useLastGroupwareDialogAnswer );
    mChanger->endChange( incidence, resourceCalendar(), subResourceCalendar() );
  }

  delete oldIncidence;

  if ( !result ) {
    mPendingChanges = true;
    QTimer::singleShot( 0, this, SLOT(updateView()) );
    incidence->cancelUpdates();
    return;
  } else {
    incidence->endUpdates();
  }

  // don't update the agenda as the item already has the correct coordinates.
  // an update would delete the current item and recreate it, but we are still
  // using a pointer to that item! => CRASH
  enableAgendaUpdate( false );

  // We need to do this in a timer to make sure we are not deleting the item
  // we are currently working on, which would lead to crashes
  // Only the actually moved agenda item is already at the correct position and mustn't be
  // recreated. All others have to!!!
  if ( incidence->doesRecur() ) {
    mUpdateItem = incidence;
    QTimer::singleShot( 0, this, SLOT( doUpdateItem() ) );
  }

  enableAgendaUpdate( true );
}

void KOAgendaView::doUpdateItem()
{
  if ( mUpdateItem ) {
    changeIncidenceDisplay( mUpdateItem, KOGlobals::INCIDENCEEDITED );
    mUpdateItem = 0;
  }
}



void KOAgendaView::showDates( const QDate &start, const QDate &end )
{
//  kdDebug(5850) << "KOAgendaView::selectDates" << endl;
  if ( !mSelectedDates.isEmpty() && mSelectedDates.first() == start
        && mSelectedDates.last() == end && !mPendingChanges )
    return;

  mSelectedDates.clear();

  QDate d = start;
  while ( d <= end ) {
    mSelectedDates.append( d );
    d = d.addDays( 1 );
  }

  mAreDatesInitialized = true;

  // and update the view
  fillAgenda();
}


void KOAgendaView::showIncidences( const Incidence::List &, const QDate & )
{
  kdDebug(5850) << "KOAgendaView::showIncidences( const Incidence::List & ) is not yet implemented" << endl;
}

void KOAgendaView::insertIncidence( Incidence *incidence, const QDate &curDate )
{
  if ( !filterByResource( incidence ) ) {
    return;
  }

  // FIXME: Use a visitor here, or some other method to get rid of the dynamic_cast's
  Event *event = dynamic_cast<Event *>( incidence );
  Todo  *todo  = dynamic_cast<Todo  *>( incidence );

  int curCol = mSelectedDates.first().daysTo( curDate );

  // In case incidence->dtStart() isn't visible (crosses bounderies)
  if ( curCol < 0 ) {
    curCol = 0;
  }

  // The date for the event is not displayed, just ignore it
  if ( curCol >= static_cast<int>( mSelectedDates.count() ) ) {
    return;
  }

  // Default values, which can never be reached
  mMinY[curCol] = mAgenda->timeToY( QTime( 23, 59 ) ) + 1;
  mMaxY[curCol] = mAgenda->timeToY( QTime( 0, 0 ) ) - 1;

  int beginX;
  int endX;
  QDate columnDate;
  if ( event ) {
    QDate firstVisibleDate = mSelectedDates.first();
    // its crossing bounderies, lets calculate beginX and endX
    if ( curDate < firstVisibleDate ) {
      beginX = curCol + firstVisibleDate.daysTo( curDate );
      endX   = beginX + event->dtStart().daysTo( event->dtEnd() );
      columnDate = firstVisibleDate;
    } else {
      beginX = curCol;
      endX   = beginX + event->dtStart().daysTo( event->dtEnd() );
      columnDate = curDate;
    }
  } else if ( todo ) {
    if ( !todo->hasDueDate() ) {
      return;  // todo shall not be displayed if it has no date
    }
    columnDate = curDate;
    beginX = endX = curCol;

  } else {
    return;
  }
  if ( todo && todo->isOverdue() ) {
    mAllDayAgenda->insertAllDayItem( incidence, columnDate, curCol, curCol );
  } else if ( incidence->doesFloat() ||
              ( todo &&
                  !todo->dtDue().isValid() ) ) {
      mAllDayAgenda->insertAllDayItem( incidence, columnDate, beginX, endX );
  } else if ( event && event->isMultiDay() ) {
    int startY = mAgenda->timeToY( event->dtStart().time() );
    QTime endtime = event->dtEnd().time();
    if ( endtime == QTime( 0, 0, 0 ) ) {
      endtime = QTime( 23, 59, 59 );
    }
    int endY = mAgenda->timeToY( endtime ) - 1;
    if ( ( beginX <= 0 && curCol == 0 ) || beginX == curCol ) {
      mAgenda->insertMultiItem( event, columnDate, beginX, endX, startY, endY );

    }
    if ( beginX == curCol ) {
      mMaxY[curCol] = mAgenda->timeToY( QTime( 23, 59 ) );
      if ( startY < mMinY[curCol] ) {
        mMinY[curCol] = startY;
      }
    } else if ( endX == curCol ) {
      mMinY[curCol] = mAgenda->timeToY( QTime( 0, 0 ) );
      if ( endY > mMaxY[curCol] ) {
        mMaxY[curCol] = endY;
      }
    } else {
      mMinY[curCol] = mAgenda->timeToY( QTime( 0, 0 ) );
      mMaxY[curCol] = mAgenda->timeToY( QTime( 23, 59 ) );
    }
  } else {
    int startY = 0, endY = 0;
    if ( event ) {
      startY = mAgenda->timeToY( incidence->dtStart().time() );
      QTime endtime = event->dtEnd().time();
      if ( endtime == QTime( 0, 0, 0 ) ) {
        endtime = QTime( 23, 59, 59 );
      }
      endY = mAgenda->timeToY( endtime ) - 1;
    }
    if ( todo ) {
      QTime t = todo->dtDue().time();

      if ( t == QTime( 0, 0 ) ) {
        t = QTime( 23, 59 );
      }

      int halfHour = 1800;
      if ( t.addSecs( -halfHour ) < t ) {
        startY = mAgenda->timeToY( t.addSecs( -halfHour ) );
        endY   = mAgenda->timeToY( t ) - 1;
      } else {
        startY = 0;
        endY   = mAgenda->timeToY( t.addSecs( halfHour ) ) - 1;
      }
    }
    if ( endY < startY ) {
      endY = startY;
    }
    mAgenda->insertItem( incidence, columnDate, curCol, startY, endY, 1, 1 );
    if ( startY < mMinY[curCol] ) {
      mMinY[curCol] = startY;
    }
    if ( endY > mMaxY[curCol] ) {
      mMaxY[curCol] = endY;
    }
  }
}

void KOAgendaView::changeIncidenceDisplayAdded( Incidence *incidence )
{
  Todo *todo = dynamic_cast<Todo *>(incidence);
  CalFilter *filter = calendar()->filter();
  if ( ( filter && !filter->filterIncidence( incidence ) ) ||
       ( ( todo && !KOPrefs::instance()->showAllDayTodo() ) ) ) {
    return;
  }

  displayIncidence( incidence );
}

void KOAgendaView::changeIncidenceDisplay( Incidence *incidence, int mode )
{
  switch ( mode ) {
    case KOGlobals::INCIDENCEADDED:
    {
      // Add an event. No need to recreate the whole view!
      // recreating everything even causes troubles: dropping to the
      // day matrix recreates the agenda items, but the evaluation is
      // still in an agendaItems' code, which was deleted in the mean time.
      // Thus KOrg crashes...
      changeIncidenceDisplayAdded( incidence );
      updateEventIndicators();
      break;
    }
    case KOGlobals::INCIDENCEEDITED:
    {
      if ( mAllowAgendaUpdate ) {
        /**
         * No need to relayout neighbours, if we do, they will be re-placed
         * and occupy the space where the item we're deleting && re-adding is.
         * Fixes: https://issues.kolab.org/issue4113
         * "Parallel events in the agenda view: last changed events are always on the right side"
         **/
        removeIncidence( incidence, false /** relayout neighbours */ );
        changeIncidenceDisplayAdded( incidence );
      }
      updateEventIndicators();
      break;
    }
    case KOGlobals::INCIDENCEDELETED:
    {
      removeIncidence( incidence );
      updateEventIndicators();
      break;
    }
    default:
      return;
  }

  // HACK: Update the view if the all-day agenda has been modified.
  // Do this because there are some layout problems in the
  // all-day agenda that are not easily solved, but clearing
  // and redrawing works ok.
  if ( incidence->doesFloat() ) {
    updateView();
  }
}

void KOAgendaView::fillAgenda( const QDate & )
{
  fillAgenda();
}

void KOAgendaView::fillAgenda()
{
  if ( !mAreDatesInitialized ) {
    return;
  }

  mPendingChanges = false;

  /* Remember the uids of the selected items. In case one of the
   * items was deleted and re-added, we want to reselect it. */
  const QString &selectedAgendaUid = mAgenda->lastSelectedUid();
  const QString &selectedAllDayAgendaUid = mAllDayAgenda->lastSelectedUid();

  enableAgendaUpdate( true );
  clearView();

  mAllDayAgenda->changeColumns( mSelectedDates.count() );
  mAgenda->changeColumns( mSelectedDates.count() );
  mEventIndicatorTop->changeColumns( mSelectedDates.count() );
  mEventIndicatorBottom->changeColumns( mSelectedDates.count() );

  createDayLabels( false );
  setHolidayMasks();

  mMinY.resize( mSelectedDates.count() );
  mMaxY.resize( mSelectedDates.count() );

  mAgenda->setDateList( mSelectedDates );

  bool somethingReselected = false;
  Incidence::List incidences = calendar()->incidences();

  for ( Incidence::List::ConstIterator it = incidences.begin(); it!=incidences.constEnd(); ++it ) {
    Incidence *incidence = (*it);
    displayIncidence( incidence );

    if( incidence->uid() == selectedAgendaUid && !selectedAgendaUid.isNull() ) {
      mAgenda->selectItemByUID( incidence->uid() );
      somethingReselected = true;
    }

    if( incidence->uid() == selectedAllDayAgendaUid && !selectedAllDayAgendaUid.isNull() ) {
      mAllDayAgenda->selectItemByUID( incidence->uid() );
      somethingReselected = true;
    }

  }

  mAgenda->checkScrollBoundaries();
  updateEventIndicators();

  //  mAgenda->viewport()->update();
  //  mAllDayAgenda->viewport()->update();

  // make invalid
  deleteSelectedDateTime();

  if( !somethingReselected ) {
    emit incidenceSelected( 0, QDate() );
  }
}

void KOAgendaView::displayIncidence( Incidence *incidence )
{
  QDate today = QDate::currentDate();
  DateTimeList::iterator t;

  // FIXME: use a visitor here
  Todo *todo = dynamic_cast<Todo *>( incidence );
  Event *event = dynamic_cast<Event *>( incidence );

  QDateTime firstVisibleDateTime = mSelectedDates.first();
  QDateTime lastVisibleDateTime = mSelectedDates.last();

  lastVisibleDateTime.setTime( QTime( 23, 59, 59, 59 ) );
  firstVisibleDateTime.setTime( QTime( 0, 0 ) );
  DateTimeList dateTimeList;

  QDateTime incDtStart = incidence->dtStart();
  QDateTime incDtEnd   = incidence->dtEnd();

  if ( todo &&
       ( !KOPrefs::instance()->showAllDayTodo() || !todo->hasDueDate() ) ) {
    return;
  }

  if ( incidence->doesRecur() ) {
    int eventDuration = event ? incDtStart.daysTo( incDtEnd ) : 0;

    // if there's a multiday event that starts before firstVisibleDateTime but ends after
    // lets include it. timesInInterval() ignores incidences that aren't totaly inside
    // the range
    QDateTime startDateTimeWithOffset = firstVisibleDateTime.addDays( -eventDuration );
    dateTimeList =
      incidence->recurrence()->timesInInterval( startDateTimeWithOffset,
                                                lastVisibleDateTime );
  } else {
    QDateTime dateToAdd; // date to add to our date list
    QDateTime incidenceStart;
    QDateTime incidenceEnd;

    if ( todo && todo->hasDueDate() && !todo->isOverdue() ) {
      // If it's not overdue it will be shown at the original date (not today)
      dateToAdd = todo->dtDue();

      // To-dos are drawn with the bottom of the rectangle at dtDue
      // if dtDue is at 00:00, then it should be displayed in the previous day, at 23:59
      if ( !todo->doesFloat() && dateToAdd.time() == QTime( 0, 0 ) ) {
        dateToAdd = dateToAdd.addSecs( -1 );
      }

      incidenceEnd = dateToAdd;
    } else if ( event ) {
      dateToAdd = incDtStart;
      incidenceEnd = incDtEnd;
    }

    if ( incidence->doesFloat() ) {
      // so comparisons with < > actually work
      dateToAdd.setTime( QTime( 0, 0 ) );
      incidenceEnd.setTime( QTime( 23, 59, 59, 59 ) );
    }

    if ( dateToAdd <= lastVisibleDateTime && incidenceEnd > firstVisibleDateTime ) {
      dateTimeList += dateToAdd;
    }
  }

  // ToDo items shall be displayed today if they are already overdude
  QDateTime dateTimeToday = today;
  if ( todo &&
       todo->isOverdue() &&
       dateTimeToday >= firstVisibleDateTime &&
       dateTimeToday <= lastVisibleDateTime ) {

    bool doAdd = true;

    if ( todo->doesRecur() ) {
      /* If there's a recurring instance showing up today don't add "today" again
       * we don't want the event to appear duplicated */
      for ( t = dateTimeList.begin(); t != dateTimeList.end(); ++t ) {
        if ( (*t).date() == today ) {
          doAdd = false;
          break;
       }
      }
    }

    if ( doAdd ) {
      dateTimeList += dateTimeToday;
    }
  }

  const bool makesDayBusy = KOEventView::makesWholeDayBusy( incidence );
  for ( t = dateTimeList.begin(); t != dateTimeList.end(); ++t ) {
    if ( makesDayBusy ) {
      Event::List &busyEvents = mBusyDays[(*t).date()];
      busyEvents.append( event );
    }
    insertIncidence( incidence, (*t).date() );
  }

  // Can be multiday
  if ( event && makesDayBusy && event->isMultiDay() ) {
    const QDate lastVisibleDate = mSelectedDates.last();
    for ( QDate date = event->dtStart().date();
          date <= event->dtEnd().date() && date <= lastVisibleDate ;
          date = date.addDays( 1 ) ) {
      Event::List &busyEvents = mBusyDays[date];
      busyEvents.append( event );
    }
  }
}

void KOAgendaView::clearView()
{
//  kdDebug(5850) << "ClearView" << endl;
  mAllDayAgenda->clear();
  mAgenda->clear();
  mBusyDays.clear();
}

CalPrinterBase::PrintType KOAgendaView::printType()
{
  if ( currentDateCount() == 1 ) return CalPrinterBase::Day;
  else return CalPrinterBase::Week;
}

void KOAgendaView::updateEventIndicatorTop( int newY )
{
  uint i;
  for( i = 0; i < mMinY.size(); ++i ) {
    mEventIndicatorTop->enableColumn( i, newY > mMinY[i] );
  }
  mEventIndicatorTop->update();
}

void KOAgendaView::updateEventIndicatorBottom( int newY )
{
  uint i;
  for( i = 0; i < mMaxY.size(); ++i ) {
    mEventIndicatorBottom->enableColumn( i, newY <= mMaxY[i] );
  }
  mEventIndicatorBottom->update();
}

void KOAgendaView::slotIncidenceDropped( Incidence *incidence, const QPoint &gpos, bool allDay )
{
  struct BoolChanger {
    BoolChanger( bool &bb ) : b( bb )
    {
      b = true;
    }
    
    ~BoolChanger()
    {
      b = false;
    }
    bool &b;
  };
  
  BoolChanger boolChanger( mProcessingDrop );

  if ( gpos.x()<0 || gpos.y()<0 ) return;
  QDate day = mSelectedDates[gpos.x()];
  QTime time = mAgenda->gyToTime( gpos.y() );
  QDateTime newTime( day, time );

  Event *event = 0;
  Todo *todo = 0;
  
  if ( incidence->type() == "Event" )
    event = static_cast<Event*>( incidence );

  if ( incidence->type() == "Todo" )
    todo = static_cast<Todo*>( incidence );
  
  if ( !event && !todo )
    return;
  
  if ( todo ) {
    Todo *existingTodo = calendar()->todo( todo->uid() );
    if ( existingTodo ) {
      kdDebug(5850) << "Drop existing Todo" << endl;
      Todo *oldTodo = existingTodo->clone();
      if ( mChanger &&
           mChanger->beginChange( existingTodo, resourceCalendar(), subResourceCalendar() ) ) {
        
        existingTodo->setDtDue( newTime );
        existingTodo->setFloats( allDay );
        existingTodo->setHasDueDate( true );
        mChanger->changeIncidence( oldTodo, existingTodo,
                                   KOGlobals::DATE_MODIFIED, this );
        mChanger->endChange( existingTodo, resourceCalendar(), subResourceCalendar() );
      } else {
        KMessageBox::sorry( this, i18n("Unable to modify this to-do, "
                            "because it cannot be locked.") );
      }
      delete oldTodo;
    } else {
      kdDebug(5850) << "Drop new Todo" << endl;
      todo->setDtDue( newTime );
      todo->setFloats( allDay );
      todo->setHasDueDate( true );
      if ( !mChanger->addIncidence( todo, 0, QString(), this ) ) {
        KODialogManager::errorSaveIncidence( this, todo );
      }
    }
  } else if ( event ) {
    Event *existingEvent = calendar()->event( event->uid() );
    Event *existingEventInSameResource = 0;

    if ( existingEvent ) {
      // If it comes from another calendar, create a new.
      // Otherwise reuse the same one
      if ( !resourceCalendar() || resourceCalendar()->incidence( incidence->uid() ) ) {
        existingEventInSameResource = existingEvent;
      }
    }

    if ( existingEventInSameResource ) {
      kdDebug(5850) << "Drop existing Event" << endl;
      Event *oldEvent = existingEventInSameResource->clone();
      if ( mChanger &&
           mChanger->beginChange( existingEvent, resourceCalendar(), subResourceCalendar() ) ) {
        existingEventInSameResource->setDtStart( newTime );
      
        const int duration = ( existingEventInSameResource->doesFloat() && !allDay ) ?
                              3600 : oldEvent->dtStart().secsTo( oldEvent->dtEnd() );

        existingEventInSameResource->setFloats( allDay );
        existingEventInSameResource->setDtEnd( newTime.addSecs( duration ) );
        mChanger->changeIncidence( oldEvent, existingEventInSameResource,
                                   KOGlobals::DATE_MODIFIED, this );
        mChanger->endChange( existingEventInSameResource, resourceCalendar(), subResourceCalendar() );
      } else {
        KMessageBox::sorry( this, i18n("Unable to modify this event, "
                            "because it cannot be locked.") );
      }
      delete oldEvent;
    } else {
      kdDebug(5850) << "Drop new Event" << endl;
      const int duration = event->dtStart().secsTo( event->dtEnd() );
      event->setDtStart( newTime );
      event->setFloats( allDay );
      event->setUid( CalFormat::createUniqueId() );
      event->setDtEnd( newTime.addSecs( duration ) );
      if ( mChanger->addIncidence( event, resourceCalendar(), subResourceCalendar(), this ) ) {
        if ( existingEvent && !existingEventInSameResource ) {
          // It's not a drag from another application, it's a drag from another agenda.
          mChanger->deleteIncidence( existingEvent, this );
        }
      } else {
        KODialogManager::errorSaveIncidence( this, event );
      }
    }
  }
}

void KOAgendaView::startDrag( Incidence *incidence )
{
#ifndef KORG_NODND
  DndFactory factory( calendar() );
  ICalDrag *vd = factory.createDrag( incidence, this );
  if ( vd->drag() ) {
    kdDebug(5850) << "KOAgendaView::startDrag(): Delete drag source" << endl;
  }
#endif
}

void KOAgendaView::readSettings()
{
  readSettings(KOGlobals::self()->config());
}

void KOAgendaView::readSettings(KConfig *config)
{
//  kdDebug(5850) << "KOAgendaView::readSettings()" << endl;

  config->setGroup("Views");

#ifndef KORG_NOSPLITTER
  QValueList<int> sizes = config->readIntListEntry("Separator AgendaView");
  if (sizes.count() == 2) {
    mSplitterAgenda->setSizes(sizes);
  }
#endif

  updateConfig();
}

void KOAgendaView::writeSettings(KConfig *config)
{
//  kdDebug(5850) << "KOAgendaView::writeSettings()" << endl;

  config->setGroup("Views");

#ifndef KORG_NOSPLITTER
  QValueList<int> list = mSplitterAgenda->sizes();
  config->writeEntry("Separator AgendaView",list);
#endif
}

void KOAgendaView::setHolidayMasks()
{
  if ( mSelectedDates.isEmpty() || !mSelectedDates[0].isValid() ) {
    return;
  }

  mHolidayMask.resize( mSelectedDates.count() + 1 );

  for( uint i = 0; i < mSelectedDates.count(); ++i ) {
    mHolidayMask[i] = !KOGlobals::self()->isWorkDay( mSelectedDates[ i ] );
  }

  // Store the information about the day before the visible area (needed for
  // overnight working hours) in the last bit of the mask:
  bool showDay = !KOGlobals::self()->isWorkDay( mSelectedDates[ 0 ].addDays( -1 ) );
  mHolidayMask[ mSelectedDates.count() ] = showDay;

  mAgenda->setHolidayMask( &mHolidayMask );
  mAllDayAgenda->setHolidayMask( &mHolidayMask );
}

QMemArray<bool> KOAgendaView::busyDayMask()
{
  if ( mSelectedDates.isEmpty() || !mSelectedDates[0].isValid() ) {
    return QMemArray<bool>();
  }

  QMemArray<bool> busyDayMask;
  busyDayMask.resize( mSelectedDates.count() );

  for( uint i = 0; i < mSelectedDates.count(); ++i ) {
    busyDayMask[i] = !mBusyDays[mSelectedDates[i]].isEmpty();
  }

  return busyDayMask;
}

void KOAgendaView::setContentsPos( int y )
{
  mAgenda->setContentsPos( 0, y );
}

void KOAgendaView::setExpandedButton( bool expanded )
{
  if ( !mExpandButton ) return;

  if ( expanded ) {
    mExpandButton->setPixmap( mExpandedPixmap );
  } else {
    mExpandButton->setPixmap( mNotExpandedPixmap );
  }
}

void KOAgendaView::clearSelection()
{
  mAgenda->deselectItem();
  mAllDayAgenda->deselectItem();
}

void KOAgendaView::newTimeSpanSelectedAllDay( const QPoint &start, const QPoint &end )
{
  newTimeSpanSelected( start, end );
  mTimeSpanInAllDay = true;
}

void KOAgendaView::newTimeSpanSelected( const QPoint &start, const QPoint &end )
{
  if (!mSelectedDates.count()) return;

  mTimeSpanInAllDay = false;

  QDate dayStart = mSelectedDates[ kClamp( start.x(), 0, (int)mSelectedDates.size() - 1 ) ];
  QDate dayEnd = mSelectedDates[ kClamp( end.x(), 0, (int)mSelectedDates.size() - 1 ) ];

  QTime timeStart = mAgenda->gyToTime(start.y());
  QTime timeEnd = mAgenda->gyToTime( end.y() + 1 );

  QDateTime dtStart(dayStart,timeStart);
  QDateTime dtEnd(dayEnd,timeEnd);

  mTimeSpanBegin = dtStart;
  mTimeSpanEnd = dtEnd;
}

void KOAgendaView::deleteSelectedDateTime()
{
  mTimeSpanBegin.setDate(QDate());
  mTimeSpanEnd.setDate(QDate());
  mTimeSpanInAllDay = false;
}

void KOAgendaView::setTypeAheadReceiver( QObject *o )
{
  mAgenda->setTypeAheadReceiver( o );
  mAllDayAgenda->setTypeAheadReceiver( o );
}

void KOAgendaView::finishTypeAhead()
{
  mAgenda->finishTypeAhead();
  mAllDayAgenda->finishTypeAhead();
}

void KOAgendaView::removeIncidence( Incidence *incidence, bool relayoutNeighbours )
{
  mAgenda->removeIncidence( incidence, relayoutNeighbours );
  mAllDayAgenda->removeIncidence( incidence, relayoutNeighbours );
}

void KOAgendaView::updateEventIndicators()
{
  mMinY = mAgenda->minContentsY();
  mMaxY = mAgenda->maxContentsY();

  mAgenda->checkScrollBoundaries();
  updateEventIndicatorTop( mAgenda->visibleContentsYMin() );
  updateEventIndicatorBottom( mAgenda->visibleContentsYMax() );
}

void KOAgendaView::setIncidenceChanger( IncidenceChangerBase *changer )
{
  mChanger = changer;
  mAgenda->setIncidenceChanger( changer );
  mAllDayAgenda->setIncidenceChanger( changer );
}

void KOAgendaView::clearTimeSpanSelection()
{
  mAgenda->clearSelection();
  mAllDayAgenda->clearSelection();
  deleteSelectedDateTime();
}

bool KOAgendaView::filterByResource( Incidence *incidence )
{
  // Special handling for groupware to-dos that are in Task folders.
  // Put them in the top-level "Calendar" folder for lack of a better
  // place since we never show Task type folders even in the
  // multiagenda view.
  if ( resourceCalendar() && incidence->type() == "Todo" ) {
    QString subRes = resourceCalendar()->subresourceIdentifier( incidence );
    if ( resourceCalendar()->subresourceType( subRes ) == "todo" ) {
      QString calmatch = "/.INBOX.directory/Calendar";
      QString i18nmatch = "/.INBOX.directory/" + i18n( "Calendar" );
      if ( subResourceCalendar().contains( calmatch ) ||
           subResourceCalendar().contains( i18nmatch ) ) {
        return true;
      }
    }
  }

  // Normal handling
  if ( !resourceCalendar() )
    return true;
  CalendarResources *calRes = dynamic_cast<CalendarResources*>( calendar() );
  if ( !calRes )
    return true;
  if ( calRes->resource( incidence ) != resourceCalendar() )
    return false;
  if ( !subResourceCalendar().isEmpty() ) {
    if ( resourceCalendar()->subresourceIdentifier( incidence ) != subResourceCalendar() )
      return false;
  }
  return true;
}

void KOAgendaView::resourcesChanged()
{
  mPendingChanges = true;
}

void KOAgendaView::calendarIncidenceAdded(Incidence * incidence)
{
  Q_UNUSED( incidence );
  mPendingChanges = true;
}

void KOAgendaView::calendarIncidenceChanged(Incidence * incidence)
{
  Q_UNUSED( incidence );
  mPendingChanges = true;
  
  // We shouldn't delete the agenda item while we're still processing
  if ( !mProcessingDrop )
    fillAgenda();
}

void KOAgendaView::calendarIncidenceDeleted(Incidence * incidence)
{
  Q_UNUSED( incidence );
  mPendingChanges = true;
}
