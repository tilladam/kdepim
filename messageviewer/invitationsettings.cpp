/*
    Boost Software License - Version 1.0 - August 17th, 2003

    Permission is hereby granted, free of charge, to any person or organization
    obtaining a copy of the software and accompanying documentation covered by
    this license (the "Software") to use, reproduce, display, distribute,
    execute, and transmit the Software, and to prepare derivative works of the
    Software, and to permit third-parties to whom the Software is furnished to
    do so, all subject to the following:

    The copyright notices in the Software and this entire statement, including
    the above license grant, this restriction and the following disclaimer,
    must be included in all copies of the Software, in whole or in part, and
    all derivative works of the Software, unless such copies or derivative
    works are solely in the form of machine-executable object code generated by
    a source language processor.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*/



#include "invitationsettings.h"
#include "invitationsettings.moc"
#include "ui_invitationsettings.h"
#include "globalsettings.h"

#include <KLocale>
#include <KMessageBox>

using namespace MessageViewer;

InvitationSettings::InvitationSettings( QWidget *parent )
  : QWidget( parent ), mInvitationUi( new Ui_InvitationSettings )
{
  mInvitationUi->setupUi( this );

  mInvitationUi->mDeleteInvitations->setText(
    i18n( GlobalSettings::self()->
          deleteInvitationEmailsAfterSendingReplyItem()->label().toUtf8() ) );
  mInvitationUi->mDeleteInvitations->setWhatsThis(
    i18n( GlobalSettings::self()->
          deleteInvitationEmailsAfterSendingReplyItem()->whatsThis().toUtf8() ) );
  connect( mInvitationUi->mDeleteInvitations, SIGNAL(toggled(bool)),
           SIGNAL(changed()) );

  mInvitationUi->mLegacyMangleFromTo->setWhatsThis(
    i18n( GlobalSettings::self()->legacyMangleFromToHeadersItem()->whatsThis().toUtf8() ) );
  connect( mInvitationUi->mLegacyMangleFromTo, SIGNAL(stateChanged(int)),
           this, SIGNAL(changed()) );

  mInvitationUi->mLegacyBodyInvites->setWhatsThis(
    i18n( GlobalSettings::self()->legacyBodyInvitesItem()->whatsThis().toUtf8() ) );
  connect( mInvitationUi->mLegacyBodyInvites, SIGNAL(toggled(bool)),
           this, SLOT(slotLegacyBodyInvitesToggled(bool)) );
  connect( mInvitationUi->mLegacyBodyInvites, SIGNAL(stateChanged(int)),
           this, SIGNAL(changed()) );

  mInvitationUi->mExchangeCompatibleInvitations->setWhatsThis(
    i18n( GlobalSettings::self()->exchangeCompatibleInvitationsItem()->whatsThis().toUtf8() ) );
  connect( mInvitationUi->mExchangeCompatibleInvitations, SIGNAL(stateChanged(int)),
           this, SIGNAL(changed()) );

  mInvitationUi->mOutlookCompatibleInvitationComments->setWhatsThis(
    i18n( GlobalSettings::self()->
          outlookCompatibleInvitationReplyCommentsItem()->whatsThis().toUtf8() ) );
  connect( mInvitationUi->mOutlookCompatibleInvitationComments, SIGNAL(stateChanged(int)),
           this, SIGNAL(changed()) );

  mInvitationUi->mOutlookCompatibleInvitationComparisons->setWhatsThis(
    i18n( GlobalSettings::self()->
          outlookCompatibleInvitationComparisonsItem()->whatsThis().toUtf8() ) );
  connect( mInvitationUi->mOutlookCompatibleInvitationComparisons, SIGNAL(stateChanged(int)),
           this, SIGNAL(changed()) );

  //Laurent BUG:257723: in kmail2 it's not possible to not send automaticly.
  mInvitationUi->mAutomaticSending->hide();
  mInvitationUi->mAutomaticSending->setWhatsThis(
    i18n( GlobalSettings::self()->automaticSendingItem()->whatsThis().toUtf8() ) );
  connect( mInvitationUi->mAutomaticSending, SIGNAL(stateChanged(int)),
           this, SIGNAL(changed()) );
}

InvitationSettings::~InvitationSettings()
{
  delete mInvitationUi;
  mInvitationUi = 0;
}

void InvitationSettings::slotLegacyBodyInvitesToggled( bool on )
{
  if ( on ) {
    const QString txt = i18n( "<qt>Invitations are normally sent as attachments to "
                        "a mail. This switch changes the invitation mails to "
                        "be sent in the text of the mail instead; this is "
                        "necessary to send invitations and replies to "
                        "Microsoft Outlook.<br />But, when you do this, you no "
                        "longer get descriptive text that mail programs "
                        "can read; so, to people who have email programs "
                        "that do not understand the invitations, the "
                        "resulting messages look very odd.<br />People that have email "
                        "programs that do understand invitations will still "
                        "be able to work with this.</qt>" );
    KMessageBox::information( this, txt, QString(), "LegacyBodyInvitesWarning" );
  }
  // Invitations in the body are autosent in any case (no point in editing raw ICAL)
  // So the autosend option is only available if invitations are sent as attachment.
  mInvitationUi->mAutomaticSending->setEnabled( !mInvitationUi->mLegacyBodyInvites->isChecked() );
}

void InvitationSettings::doLoadFromGlobalSettings()
{
  mInvitationUi->mLegacyMangleFromTo->setChecked(
    GlobalSettings::self()->legacyMangleFromToHeaders() );

  mInvitationUi->mExchangeCompatibleInvitations->setChecked(
    GlobalSettings::self()->exchangeCompatibleInvitations() );

  mInvitationUi->mLegacyBodyInvites->blockSignals( true );
  mInvitationUi->mLegacyBodyInvites->setChecked( GlobalSettings::self()->legacyBodyInvites() );
  mInvitationUi->mLegacyBodyInvites->blockSignals( false );

  mInvitationUi->mOutlookCompatibleInvitationComments->setChecked(
    GlobalSettings::self()->outlookCompatibleInvitationReplyComments() );

  mInvitationUi->mOutlookCompatibleInvitationComparisons->setChecked(
    GlobalSettings::self()->outlookCompatibleInvitationComparisons() );

  mInvitationUi->mAutomaticSending->setChecked( GlobalSettings::self()->automaticSending() );
  mInvitationUi->mAutomaticSending->setEnabled( !mInvitationUi->mLegacyBodyInvites->isChecked() );

  mInvitationUi->mDeleteInvitations->setChecked(
    GlobalSettings::self()->deleteInvitationEmailsAfterSendingReply() );
}

void InvitationSettings::save()
{
  KConfigGroup groupware( GlobalSettings::self()->config(), "Invitations" );

  GlobalSettings::self()->setLegacyMangleFromToHeaders(
    mInvitationUi->mLegacyMangleFromTo->isChecked() );

  GlobalSettings::self()->setLegacyBodyInvites( mInvitationUi->mLegacyBodyInvites->isChecked() );

  GlobalSettings::self()->setExchangeCompatibleInvitations(
    mInvitationUi->mExchangeCompatibleInvitations->isChecked() );

  GlobalSettings::self()->setOutlookCompatibleInvitationReplyComments(
    mInvitationUi->mOutlookCompatibleInvitationComments->isChecked() );

  GlobalSettings::self()->setOutlookCompatibleInvitationComparisons(
    mInvitationUi->mOutlookCompatibleInvitationComparisons->isChecked() );

  GlobalSettings::self()->setAutomaticSending( mInvitationUi->mAutomaticSending->isChecked() );

  GlobalSettings::self()->setDeleteInvitationEmailsAfterSendingReply(
    mInvitationUi->mDeleteInvitations->isChecked() );
}

QString InvitationSettings::helpAnchor() const
{
  return QString::fromLatin1( "configure-misc-invites" );
}

void InvitationSettings::doResetToDefaultsOther()
{
  const bool bUseDefaults = GlobalSettings::self()->useDefaults( true );

  mInvitationUi->mLegacyMangleFromTo->setChecked(
    GlobalSettings::self()->legacyMangleFromToHeaders() );

  mInvitationUi->mExchangeCompatibleInvitations->setChecked(
    GlobalSettings::self()->exchangeCompatibleInvitations() );

  mInvitationUi->mLegacyBodyInvites->blockSignals( true );
  mInvitationUi->mLegacyBodyInvites->setChecked( GlobalSettings::self()->legacyBodyInvites() );
  mInvitationUi->mLegacyBodyInvites->blockSignals( false );

  mInvitationUi->mOutlookCompatibleInvitationComments->setChecked(
    GlobalSettings::self()->outlookCompatibleInvitationReplyComments() );

  mInvitationUi->mOutlookCompatibleInvitationComparisons->setChecked(
    GlobalSettings::self()->outlookCompatibleInvitationComparisons() );

  mInvitationUi->mAutomaticSending->setChecked( GlobalSettings::self()->automaticSending() );
  mInvitationUi->mAutomaticSending->setEnabled( !mInvitationUi->mLegacyBodyInvites->isChecked() );

  mInvitationUi->mDeleteInvitations->setChecked(
    GlobalSettings::self()->deleteInvitationEmailsAfterSendingReply() );

  GlobalSettings::self()->useDefaults( bUseDefaults );
  
  
}
