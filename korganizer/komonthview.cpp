/*
    This file is part of KOrganizer.
    Copyright (c) 2000,2001 Cornelius Schumacher <schumacher@kde.org>

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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// $Id$

#include <qpopupmenu.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qkeycode.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qpainter.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kiconloader.h>

#ifndef KORG_NOPRINTER
#include "calprinter.h"
#endif
#include "koprefs.h"
#ifndef KORG_NOPLUGINS
#include "kocore.h"
#endif

#include "komonthview.h"
#include "komonthview.moc"

KNoScrollListBox::KNoScrollListBox(QWidget *parent,const char *name)
  : QListBox(parent, name)
{
}

void KNoScrollListBox::keyPressEvent(QKeyEvent *e)
{
  switch(e->key()) {
    case Key_Right:
      scrollBy(4,0);
      break;
    case Key_Left:
      scrollBy(-4,0);
      break;
    case Key_Up:
      if(!count()) break;
      setCurrentItem((currentItem()+count()-1)%count());
      if(!itemVisible(currentItem())) {
        if((unsigned int) currentItem() == (count()-1)) {
          setTopItem(currentItem()-numItemsVisible()+1);
        } else {
          setTopItem(topItem()-1);
        }
      }
      break;
    case Key_Down:
      if(!count()) break;
      setCurrentItem((currentItem()+1)%count());
      if(!itemVisible(currentItem())) {
        if(currentItem() == 0) {
          setTopItem(0);
        } else {
          setTopItem(topItem()+1);
        }
      }
    case Key_Shift:
      emit shiftDown();
      break;
    default:
      break;
  }
}

void KNoScrollListBox::keyReleaseEvent(QKeyEvent *e)
{
  switch(e->key()) {
    case Key_Shift:
      emit shiftUp();
      break;
    default:
      break;
  }
}

void KNoScrollListBox::mousePressEvent(QMouseEvent *e)
{
  QListBox::mousePressEvent(e);

  if(e->button() == RightButton) {
    emit rightClick();
  }
}


MonthViewItem::MonthViewItem( Incidence *incidence, const QString & s)
  : QListBoxItem()
{
  setText( s );

  mIncidence = incidence;

  mAlarmPixmap = SmallIcon("bell");
  mRecurPixmap = SmallIcon("recur");
  mReplyPixmap = SmallIcon("mail_reply");

  mRecur = false;
  mAlarm = false;
  mReply = false;
}

void MonthViewItem::paint(QPainter *p)
{
  int x = 3;
  if ( mRecur ) {
    p->drawPixmap( x, 0, mRecurPixmap );
    x += mRecurPixmap.width() + 2;
  }
  if ( mAlarm ) {
    p->drawPixmap( x, 0, mAlarmPixmap );
    x += mAlarmPixmap.width() + 2;
  }
  if ( mReply ) {
    p->drawPixmap(x, 0, mReplyPixmap );
    x += mReplyPixmap.width() + 2;
  }
  QFontMetrics fm = p->fontMetrics();
  int yPos;
  int pmheight = QMAX( mRecurPixmap.height(),
                       QMAX( mAlarmPixmap.height(), mReplyPixmap.height() ) );
  if( pmheight < fm.height() )
    yPos = fm.ascent() + fm.leading()/2;
  else
    yPos = pmheight/2 - fm.height()/2  + fm.ascent();

  p->setPen( palette().color( QPalette::Normal, QColorGroup::Foreground ) );
  p->drawText( x, yPos, text() );
}

int MonthViewItem::height(const QListBox *lb) const
{
  return QMAX( QMAX( mRecurPixmap.height(), mReplyPixmap.height() ),
	       QMAX( mAlarmPixmap.height(), lb->fontMetrics().lineSpacing()+1) );
}

int MonthViewItem::width(const QListBox *lb) const
{
  int x = 3;
  if( mRecur ) {
    x += mRecurPixmap.width()+2;
  }
  if( mAlarm ) {
    x += mAlarmPixmap.width()+2;
  }
  if( mReply ) {
    x += mReplyPixmap.width()+2;
  }

  return( x + lb->fontMetrics().width( text() ) + 1 );
}


MonthViewCell::MonthViewCell( KOMonthView *parent )
  : QWidget( parent ),
    mMonthView( parent )
{
  QVBoxLayout *topLayout = new QVBoxLayout( this );

  mLabel = new QLabel( this );
  mLabel->setFrameStyle( QFrame::Panel | QFrame::Plain );
  mLabel->setLineWidth( 1 );
  mLabel->setAlignment( AlignCenter );

  mItemList = new KNoScrollListBox( this );
  mItemList->setMinimumSize( 10, 10 );
  mItemList->setFrameStyle( QFrame::Panel | QFrame::Plain );
  mItemList->setLineWidth( 1 );
  topLayout->addWidget( mItemList );

  mLabel->raise();
  
  mStandardPalette = palette();

  updateConfig();

  connect( mItemList, SIGNAL( doubleClicked( QListBoxItem *) ),
           SLOT( defaultAction( QListBoxItem * ) ) );
  connect( mItemList, SIGNAL( rightButtonPressed( QListBoxItem *,
                                                  const QPoint &) ),
           SLOT( contextMenu( QListBoxItem * ) ) );
  connect( mItemList, SIGNAL( highlighted( QListBoxItem *) ),
           SLOT( selection( QListBoxItem * ) ) );
}

void MonthViewCell::setDate( const QDate &date )
{
//  kdDebug() << "MonthViewCell::setDate(): " << date.toString() << endl;

  mDate = date;

  QString text;
  if ( date.day() == 1 ) {
    text = KGlobal::locale()->monthName( date.month(), true ) + " ";
    QFontMetrics fm( mLabel->font() );
    mLabel->resize( mLabelSize + QSize( fm.width( text ), 0 ) );
  } else {
    mLabel->resize( mLabelSize );
  }
  text += QString::number( mDate.day() );
  mLabel->setText( text );
  
  resizeEvent( 0 );
}

QDate MonthViewCell::date() const
{
  return mDate;
}

void MonthViewCell::setPrimary( bool primary )
{
  mPrimary = primary;

  if ( mPrimary ) {
    mLabel->setBackgroundMode( PaletteBase );
  } else {
    mLabel->setBackgroundMode( PaletteBackground );
  }
}

bool MonthViewCell::isPrimary() const
{
  return mPrimary;
}

void MonthViewCell::setHoliday( bool holiday )
{
  mHoliday = holiday;

  if ( holiday ) {
    setPalette( mHolidayPalette );
  } else {
    setPalette( mStandardPalette );
  }
}

void MonthViewCell::setHoliday( const QString &holiday )
{
  mHolidayString = holiday;

  if ( !holiday.isEmpty() ) {
    setHoliday( true );
  }
}

void MonthViewCell::updateCell()
{
  if ( mDate == QDate::currentDate() ) {
    mItemList->setLineWidth( 3 );
  } else {
    mItemList->setLineWidth( 1 );
  }

  mItemList->clear();

  if ( !mHolidayString.isEmpty() ) {
    MonthViewItem *item = new MonthViewItem( 0, mHolidayString );
    item->setPalette( mHolidayPalette );
    mItemList->insertItem( item );
  }
  
  QPtrList<Event> events = mMonthView->calendar()->getEventsForDate( mDate, true );
  Event *event;
  for( event = events.first(); event; event = events.next() ) {
    QString text;
    if (event->isMultiDay()) {
      if (mDate == event->dtStart().date()) {
        text = "(-- " + event->summary();
      } else if (mDate == event->dtEnd().date()) {
        text = event->summary() + " --)";
      } else if (!(event->dtStart().date().daysTo(mDate) % 7)) {
        text = "-- " + event->summary() + " --";
      } else {
        text = "----------------";
      }
    } else {
      if (event->doesFloat())
        text = event->summary();
      else {
        text = KGlobal::locale()->formatTime(event->dtStart().time());
        text += " " + event->summary();
      }
    }

    MonthViewItem *item = new MonthViewItem( event, text );
    item->setPalette( mStandardPalette );
    item->setRecur( event->recurrence()->doesRecur() );
    item->setAlarm( event->isAlarmEnabled() );

    Attendee *me = event->attendeeByMails(KOPrefs::instance()->mAdditionalMails,
                                          KOPrefs::instance()->email());
    if ( me != 0 ) {
      if ( me->status() == Attendee::NeedsAction && me->RSVP())
        item->setReply(true);
      else
        item->setReply(false);
    } else
      item->setReply(false);

    mItemList->insertItem( item );
  }

  // insert due todos
  QPtrList<Todo> todos = mMonthView->calendar()->getTodosForDate( mDate );
  Todo *todo;
  for(todo = todos.first(); todo; todo = todos.next()) {
    QString text;
    if (todo->hasDueDate()) {
      if (!todo->doesFloat()) {
        text += KGlobal::locale()->formatTime(todo->dtDue().time());
        text += " ";
      }
    }
    text += i18n("To-Do: ") + todo->summary();

    MonthViewItem *item = new MonthViewItem( todo, text );
    item->setPalette( mStandardPalette );

    mItemList->insertItem( item );
  }
}

void MonthViewCell::updateConfig()
{
  enableScrollBars( KOPrefs::instance()->mEnableMonthScroll );

  setFont( KOPrefs::instance()->mMonthViewFont );

  QFontMetrics fm( font() );
  mLabelSize = fm.size( 0, "30" ) +
               QSize( mLabel->frameWidth() * 2, mLabel->frameWidth() * 2 ) +
               QSize( 2, 2 );

  mHolidayPalette = mStandardPalette;
  mHolidayPalette.setColor(QColorGroup::Foreground,
                           KOPrefs::instance()->mHolidayColor);
  mHolidayPalette.setColor(QColorGroup::Text,
                           KOPrefs::instance()->mHolidayColor);
}

void MonthViewCell::enableScrollBars( bool enabled )
{
  if ( enabled ) {
    mItemList->setVScrollBarMode(QScrollView::Auto);
    mItemList->setHScrollBarMode(QScrollView::Auto);
  } else {
    mItemList->setVScrollBarMode(QScrollView::AlwaysOff);
    mItemList->setHScrollBarMode(QScrollView::AlwaysOff);
  }
}

Incidence *MonthViewCell::selectedIncidence()
{
  int index = mItemList->currentItem();
  if ( index < 0 ) return 0;
  
  MonthViewItem *item =
      static_cast<MonthViewItem *>( mItemList->item( index ) );

  if ( !item ) return 0;
  
  return item->incidence();
}

void MonthViewCell::deselect()
{
  mItemList->clearSelection();
}

void MonthViewCell::resizeEvent ( QResizeEvent * )
{
  mLabel->move( width() - mLabel->width(), height() - mLabel->height() );
}

void MonthViewCell::defaultAction( QListBoxItem *item )
{
  if ( !item ) return;

  MonthViewItem *eventItem = static_cast<MonthViewItem *>( item );
  Incidence *incidence = eventItem->incidence();
  if ( incidence ) mMonthView->defaultAction( incidence );
}

void MonthViewCell::contextMenu( QListBoxItem *item )
{
  if ( !item ) return;
  
  MonthViewItem *eventItem = static_cast<MonthViewItem *>( item );
  Incidence *incidence = eventItem->incidence();
  if ( incidence ) mMonthView->showContextMenu( incidence );
}

void MonthViewCell::selection( QListBoxItem *item )
{
  if ( !item ) return;

  mMonthView->setSelectedCell( this );
}

KOMonthView::KOMonthView(Calendar *calendar, QWidget *parent, const char *name)
    : KOEventView( calendar, parent, name ),
      mDaysPerWeek( 7 ), mNumWeeks( 6 ), mNumCells( mDaysPerWeek * mNumWeeks ),
      mShortDayLabels( false ), mWidthLongDayLabel( 0 ), mSelectedCell( 0 )
{
  mCells.setAutoDelete( true );

  QGridLayout *dayLayout = new QGridLayout( this );

  // create the day of the week labels (Sun, Mon, etc) and add them to
  // the layout.
  mDayLabels.resize( mDaysPerWeek );
  QFont bfont = font();
  bfont.setBold( true );
  mShortDayLabels = true;
  int i;
  for( i = 0; i < mDaysPerWeek; i++ ) {
    QLabel *label = new QLabel( this );
    label->setFont(bfont);
    label->setFrameStyle(QFrame::Panel|QFrame::Raised);
    label->setLineWidth(1);
    label->setAlignment(AlignCenter);

    mDayLabels.insert( i, label );

    dayLayout->addWidget( label, 0, i );
  }

  int row, col;

  mCells.resize( mNumCells );
  for( row = 0; row < mNumWeeks; ++row ) {
    for( col = 0; col < mDaysPerWeek; ++col ) {
      MonthViewCell *cell = new MonthViewCell( this );
      mCells.insert( row * mDaysPerWeek + col, cell );
      dayLayout->addWidget( cell, row + 1, col );

      connect( cell, SIGNAL( defaultAction( Incidence * ) ),
               SLOT( defaultAction( Incidence * ) ) );
    }
    dayLayout->setRowStretch( row + 1, 1 );
  }

  mContextMenu = eventPopup();

  emit eventsSelected(false);
}

KOMonthView::~KOMonthView()
{
  delete mContextMenu;
}

int KOMonthView::maxDatesHint()
{
  return mNumCells;
}

int KOMonthView::currentDateCount()
{
  return mNumCells;
}

QPtrList<Incidence> KOMonthView::selectedIncidences()
{
  QPtrList<Incidence> selected;

  if ( mSelectedCell ) {
    Incidence *incidence = mSelectedCell->selectedIncidence();
    if ( incidence ) selected.append( incidence );
  }

  return selected;
}

void KOMonthView::printPreview(CalPrinter *calPrinter, const QDate &fd,
                               const QDate &td)
{
#ifndef KORG_NOPRINTER
  calPrinter->preview(CalPrinter::Month, fd, td);
#endif
}

void KOMonthView::updateConfig()
{
  mWeekStartsMonday = KGlobal::locale()->weekStartsMonday();

  QFontMetrics fontmetric(mDayLabels[0]->font());
  mWidthLongDayLabel = 0;

  for (int i = 0; i < 7; i++) {
    int width = fontmetric.width(KGlobal::locale()->weekDayName(i+1));
    if ( width > mWidthLongDayLabel ) mWidthLongDayLabel = width;
  }
  
  updateDayLabels();

  for (uint i = 0; i < mCells.count(); ++i) {
    mCells[i]->updateConfig();
  }
}

void KOMonthView::updateDayLabels()
{
  for (int i = 0; i < 7; i++) {
    if (mWeekStartsMonday) {
      mDayLabels[i]->setText(KGlobal::locale()->weekDayName(i+1,mShortDayLabels));
    } else {
      if (i==0) mDayLabels[i]->setText(KGlobal::locale()->weekDayName(7,mShortDayLabels));
      else mDayLabels[i]->setText(KGlobal::locale()->weekDayName(i,mShortDayLabels));
    }
  }
}

void KOMonthView::showDates(const QDate &start, const QDate &)
{
//  kdDebug() << "KOMonthView::showDates(): " << start.toString() << endl;

  mStartDate = start;

  int startWeekDay = mWeekStartsMonday ? 1 : 7;

  while( mStartDate.dayOfWeek() != startWeekDay ) {
    mStartDate = mStartDate.addDays( -1 );
  }

  bool primary = false;
  uint i;
  for( i = 0; i < mCells.size(); ++i ) {
    QDate date = mStartDate.addDays( i );
    if ( date.day() == 1 ) {
      primary = !primary;
    }
    mCells[i]->setPrimary( primary );

    if ( date.dayOfWeek() == 7 ) {
      mCells[i]->setHoliday( true );
    } else {
      mCells[i]->setHoliday( false );
    }

#ifndef KORG_NOPLUGINS
    // add holiday, if present
    QString hstring(KOCore::self()->holiday(date));
    mCells[i]->setHoliday( hstring );
#endif

    mCells[i]->setDate( date );
  }

  updateView();
}

void KOMonthView::showEvents(QPtrList<Event>)
{
  kdDebug() << "KOMonthView::selectEvents is not implemented yet." << endl;
}

void KOMonthView::changeEventDisplay(Event *, int)
{
  // this should be re-written to be much more efficient, but this
  // quick-and-dirty-hack gets the job done for right now.
  updateView();
}

void KOMonthView::updateView()
{
  uint i;
  for( i = 0; i < mCells.count(); ++i ) {
    mCells[i]->updateCell();
  }

  processSelectionChange();
}

void KOMonthView::resizeEvent(QResizeEvent *)
{
  // select the appropriate heading string size. E.g. "Wednesday" or "Wed".
  // note this only changes the text if the requested size crosses the
  // threshold between big enough to support the full name and not big
  // enough.
  if( mDayLabels[0]->width() < mWidthLongDayLabel ) {
    if ( !mShortDayLabels ) {
      mShortDayLabels = true;
      updateDayLabels();
    }
  } else {
    if ( mShortDayLabels ) {
      mShortDayLabels = false;
      updateDayLabels();
    }
  }
}

void KOMonthView::showContextMenu( Incidence *incidence )
{
  if( incidence && incidence->type() == "Event" ) {
    Event *event = static_cast<Event *>(incidence);
    mContextMenu->showEventPopup(event);
  } else {
    kdDebug() << "MonthView::showContextMenu(): cast failed." << endl;
  }
}

void KOMonthView::setSelectedCell( MonthViewCell *cell )
{
  if ( cell == mSelectedCell ) return;

  if ( mSelectedCell ) mSelectedCell->deselect();
  
  mSelectedCell = cell;
  
  emit eventsSelected( true );
}

void KOMonthView::processSelectionChange()
{
  QPtrList<Incidence> events = selectedIncidences();
  if (events.count() > 0) {
    emit eventsSelected(true);
//    kdDebug() << "KOMonthView::processSelectionChange() true" << endl;
  } else {
    emit eventsSelected(false);
//    kdDebug() << "KOMonthView::processSelectionChange() false" << endl;
  }
}
