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

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

  As a special exception, permission is given to link this program
  with any edition of Qt, and distribute the resulting executable,
  without including the source code for Qt in the source distribution.
*/

// Journal Entry

#include "journalview.h"
#include "kodialogmanager.h"
#include "incidencechanger.h"
#include "koglobals.h"
#ifndef KORG_NOPRINTER
#include "kocorehelper.h"
#include "calprinter.h"
#endif

#include <kcal/journal.h>
#include <kcal/calendar.h>

#include <kdebug.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klocale.h>
#include <ktextedit.h>
#include <ktimeedit.h>
#include <klineedit.h>
#include <kstandardguiitem.h>
#include <kmessagebox.h>
#include <kvbox.h>

#include <QLabel>
#include <QLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QPixmap>
#include <QGridLayout>
#include <QEvent>

#include "journalview.moc"

JournalDateView::JournalDateView( Calendar *calendar, QWidget *parent ) :
  KVBox( parent ), mCalendar( calendar )
{
//kDebug(5850)<<"JournalView::JournalView, parent="<<parent;
  mChanger = 0;

#ifdef __GNUC__
#warning "kde4: porting"
#endif
}

JournalDateView::~JournalDateView()
{
}

void JournalDateView::setDate(const QDate &date)
{
  mDate = date;
  emit setDateSignal( date );
}

void JournalDateView::clear()
{
  QList<JournalView*> values( mEntries.values() );

  QList<JournalView*>::Iterator it = values.begin();
  for ( ; it != values.end(); ++it ) {
    delete (*it);
  }
  mEntries.clear();
}

// should only be called by the KOJournalView now.
void JournalDateView::addJournal( Journal *j )
{
  QMap<Journal*,JournalView*>::Iterator pos = mEntries.find( j );
  if ( pos != mEntries.end() ) return;

  JournalView *entry = new JournalView( j, this );
  entry->show();
  entry->setDate( mDate );
  entry->setIncidenceChanger( mChanger );

  mEntries.insert( j, entry );
  connect( this, SIGNAL( setIncidenceChangerSignal( IncidenceChangerBase * ) ),
           entry, SLOT( setIncidenceChanger( IncidenceChangerBase * ) ) );
  connect( this, SIGNAL( setDateSignal( const QDate & ) ),
           entry, SLOT( setDate( const QDate & ) ) );
  connect( entry, SIGNAL( deleteIncidence( Incidence* ) ),
           this, SIGNAL( deleteIncidence( Incidence* ) ) );
  connect( entry, SIGNAL( editIncidence( Incidence* ) ),
           this, SIGNAL( editIncidence( Incidence* ) ) );
}

Journal::List JournalDateView::journals() const
{
  QList<Journal*> jList( mEntries.keys() );
  Journal::List l;
  QList<Journal*>::Iterator it = jList.begin();
  for ( ; it != jList.end(); ++it ) {
    l.append( *it );
  }
  return l;
}

void JournalDateView::setIncidenceChanger( IncidenceChangerBase *changer )
{
  mChanger = changer;
  emit setIncidenceChangerSignal( changer );
}

void JournalDateView::emitNewJournal()
{
  emit newJournal( mDate );
}

void JournalDateView::journalEdited( Journal *journal )
{
  QMap<Journal*,JournalView*>::Iterator pos = mEntries.find( journal );
  if ( pos == mEntries.end() ) return;

  pos.value()->setJournal( journal );

}

void JournalDateView::journalDeleted( Journal *journal )
{
  QMap<Journal*,JournalView*>::Iterator pos = mEntries.find( journal );
  if ( pos == mEntries.end() ) return;

  delete pos.value();
}





JournalView::JournalView( Journal* j, QWidget *parent ) :
  QWidget( parent ), mJournal( j )
{
//kDebug(5850)<<"JournalView::JournalView, parent="<<parent;
  mDirty = false;
  mWriteInProgress = false;
  mChanger = 0;
  mReadOnly = false;

  mLayout = new QGridLayout( this );
  mLayout->setSpacing( KDialog::spacingHint() );
  mLayout->setMargin( KDialog::marginHint() );

  mEditor = new KTextEdit(this);
  mEditor->installEventFilter(this);
  mEditor->setTextInteractionFlags( Qt::TextSelectableByMouse |
                                    Qt::TextSelectableByKeyboard );
  mEditor->setFrameStyle( QFrame::StyledPanel );
  //FIXME: Get this padded
  mEditor->setStyleSheet( "padding: 50px;" );
  mLayout->addWidget( mEditor, 0, 0, 1, 3 );

  mEditButton = new QPushButton( this );
  mEditButton->setObjectName( "editButton" );
  mEditButton->setText( i18n("&Edit") );
  mEditButton->setIcon( KOGlobals::self()->smallIcon( "edit" ) );
  mEditButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  mEditButton->setToolTip( i18n("Edit this journal entry") );
  mEditButton->setWhatsThis( i18n("Opens an editor dialog for this journal entry") );
  mLayout->addWidget( mEditButton, 1, 0 );
  connect( mEditButton, SIGNAL(clicked()), this, SLOT( editItem() ) );

  mDeleteButton = new QPushButton( this );
  mDeleteButton->setObjectName( "deleteButton" );
  mDeleteButton->setText( i18n("&Delete") );
  QPixmap pix = KOGlobals::self()->smallIcon( "edit-delete" );
  mDeleteButton->setIcon( pix );
  mDeleteButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  mDeleteButton->setToolTip( i18n("Delete this journal entry") );
  mDeleteButton->setWhatsThis( i18n("Delete this journal entry") );
  mLayout->addWidget( mDeleteButton, 1, 1 );
  connect( mDeleteButton, SIGNAL(pressed()), this, SLOT(deleteItem()) );

#ifndef KORG_NOPRINTER
  mPrintButton = new QPushButton( this );
  mPrintButton->setText( i18n("&Print") );
  mPrintButton->setObjectName( "printButton" );
  mPrintButton->setIcon( KOGlobals::self()->smallIcon( "printer1" ) );
  mPrintButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  mPrintButton->setToolTip( i18n("Print this journal entry") );
  mPrintButton->setWhatsThis( i18n("Opens a print dialog for this journal entry") );
  mLayout->addWidget( mPrintButton, 1, 2 );
  connect( mPrintButton, SIGNAL(clicked()), this, SLOT( printItem() ) );
#endif

  readJournal( mJournal );
  mDirty = false;
}

JournalView::~JournalView()
{
}

void JournalView::deleteItem()
{
/*  KMessageBox::ButtonCode *code = KMessageBox::warningContinueCancel(this,
      i18n("The journal \"%1\" on %2 will be permanently deleted.")
               .arg( mJournal->summary() )
               .arg( mJournal->dtStartStr() ),
  i18n("KOrganizer Confirmation"), KStandardGuiItem::del() );
  if ( code == KMessageBox::Yes ) {*/
    if ( mJournal )
      emit deleteIncidence( mJournal );
//   }
}

void JournalView::editItem()
{
  if ( mJournal )
    emit editIncidence( mJournal );
}

void JournalView::printItem()
{
#ifndef KORG_NOPRINTER
  if ( mJournal ) {
    Calendar *cal = 0;
    KOCoreHelper helper;
    CalPrinter printer( this, cal, &helper );
    connect( this, SIGNAL(configChanged()), &printer, SLOT(updateConfig()) );

    Incidence::List selectedIncidences;
    selectedIncidences.append( mJournal );

    printer.print( KOrg::CalPrinterBase::Incidence,
                 QDate(), QDate(), selectedIncidences );
  }
#endif
}

void JournalView::setReadOnly( bool readonly )
{
  mReadOnly = readonly;
  mEditButton->setEnabled( !mReadOnly );
  mDeleteButton->setEnabled( !mReadOnly );
}


void JournalView::setDate(const QDate &date)
{
  mDate = date;
}

void JournalView::setJournal(Journal *journal)
{
  if ( !journal ) return;

  mJournal = journal;
  readJournal( journal );

  mDirty = false;
}

void JournalView::setDirty()
{
  mDirty = true;
  kDebug(5850) <<"JournalView::setDirty()";
}

bool JournalView::eventFilter( QObject *o, QEvent *e )
{
  return QWidget::eventFilter( o, e );    // standard event processing
}


void JournalView::readJournal( Journal *j )
{
  mJournal = j;
  mEditor->clear();
  QTextCursor cursor = QTextCursor( mEditor->textCursor() );
  cursor.movePosition( QTextCursor::Start );
  QTextCharFormat bodyFormat = QTextCharFormat( cursor.charFormat() );
  if ( !mJournal->summary().isEmpty() ) {
    QTextCharFormat titleFormat = bodyFormat;
    titleFormat.setFontWeight( QFont::Bold );
    // FIXME: Is there a way to just make this +4?
    titleFormat.setFontPointSize( 14 );
    cursor.insertText( mJournal->summary(), titleFormat );
    cursor.insertBlock();
  }
  if ( !mJournal->floats() ) {
    QTextCharFormat dateFormat = bodyFormat;
    dateFormat.setFontWeight( QFont::Bold );
    // FIXME: Is there a way to just make this +2?
    dateFormat.setFontPointSize( 11 );
    cursor.insertText( mJournal->dtStartStr(), dateFormat );
    cursor.insertBlock();
  }
  cursor.setBlockCharFormat( bodyFormat );
  if ( mJournal->descriptionIsRich() ) {
    QString description = mJournal->description();
    description.replace ( '\n', "<br/>" );
    mEditor->insertHtml( description );
  }
  else {
    mEditor->insertPlainText( mJournal->description() );
  }
  cursor.insertBlock();
  setReadOnly( mJournal->isReadOnly() );
}
