#include "incidencerecurrence.h"

#include <QtCore/QDebug>

#include <KCalendarSystem>

#include "incidencedatetime.h"
#include "ui_eventortododesktop.h"

using namespace IncidenceEditorsNG;

IncidenceRecurrence::IncidenceRecurrence( IncidenceDateTime *dateTime, Ui::EventOrTodoDesktop *ui )
  : mUi( ui )
  , mDateTime( dateTime )
{
  toggleRecurrenceWidgets( false );

  const int weekStart = KGlobal::locale()->weekStartDay();
  for ( int i = 0; i < 7; ++i ) {
    // i is the nr of the combobox, not the day of week!
    // label=(i+weekStart+6)%7 + 1;
    // index in CheckBox array(=day): label-1
    const int index = ( i + weekStart + 6 ) % 7;

    const KCalendarSystem *calSys = KGlobal::locale()->calendar();
    QString weekDayName = calSys->weekDayName( index + 1, KCalendarSystem::ShortDayName );
    mUi->mWeekDayCombo->addItem( weekDayName );
  }

  connect( mUi->mExceptionDateEdit, SIGNAL(dateChanged(QDate)),
           SLOT(handleExceptionDateChange(QDate)) );
  connect( mUi->mTypeCombo, SIGNAL(currentIndexChanged(int)),
           SLOT(handleRecurrenceTypeChange(int)));
}

void IncidenceRecurrence::load( KCal::Incidence::ConstPtr incidence )
{

}

void IncidenceRecurrence::save( KCal::Incidence::Ptr incidence )
{

}

bool IncidenceRecurrence::isDirty() const
{
  return false;
}

bool IncidenceRecurrence::isValid()
{
  return true;
}

void IncidenceRecurrence::handleExceptionDateChange( const QDate &currentDate )
{
  mUi->mExceptionAddButton->setEnabled( currentDate >= mDateTime->startDate() );
}

void IncidenceRecurrence::handleRecurrenceTypeChange( int currentIndex )
{
  toggleRecurrenceWidgets( currentIndex > 0 );
  switch ( currentIndex ) {
  case 1:
    mUi->mRecurrenceRuleLabel->setText( i18n("day(s)") );
    break;
  case 2:
    mUi->mRecurrenceRuleLabel->setText( i18n("week(s)") );
    break;
  case 3:
    mUi->mRecurrenceRuleLabel->setText( i18n("month(s)") );
    break;
  case 4:
    mUi->mRecurrenceRuleLabel->setText( i18n("year(s)") );
    break;
  default:
    mUi->mRecurrenceRuleLabel->setText( i18n("day(s)") );
  }
}

void IncidenceRecurrence::toggleRecurrenceWidgets( bool enable )
{
  mUi->mFrequencyLabel->setVisible( enable );
  mUi->mFrequencyEdit->setVisible( enable );
  mUi->mRecurrenceRuleLabel->setVisible( enable );
  mUi->mRepeatStack->setVisible( enable && mUi->mTypeCombo->currentIndex() > 1 );
  mUi->mRecurrenceEndLabel->setVisible( enable );
  mUi->mRecurrenceEndCombo->setVisible( enable );
  mUi->mEndDurationEdit->setVisible( enable );
  mUi->mRecurrenceEndStack->setVisible( enable );

  // Exceptions widgets
  mUi->mExceptionsLabel->setVisible( enable );
  mUi->mExceptionDateEdit->setVisible( enable );
  mUi->mExceptionAddButton->setVisible( enable );
  mUi->mExceptionAddButton->setEnabled( mUi->mExceptionDateEdit->date() >= mDateTime->startDate() );
  mUi->mExceptionList->setVisible( enable );
  mUi->mExceptionSeperator->setVisible( enable );
}
