/*
  Copyright (c) 2013 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "folderarchiveconfiguredialog.h"

#include "kdepim-version.h"

#include <KLocale>
#include <KConfigGroup>
#include <KGlobal>
#include <KHelpMenu>
#include <KMenu>
#include <KIcon>
#include <KAboutData>

FolderArchiveConfigureDialog::FolderArchiveConfigureDialog(QWidget *parent)
    : KDialog(parent)
{
    setCaption( i18n("Configure") );
    setWindowIcon( KIcon( QLatin1String("kmail") ) );
    setButtons( Help|Ok|Cancel );

    readConfig();

    mAboutData = new KAboutData(
                QByteArray( "folderarchiveagent" ),
                QByteArray(),
                ki18n( "Folder Archive Agent" ),
                QByteArray( KDEPIM_VERSION ),
                ki18n( "Move mails in specific archive folder." ),
                KAboutData::License_GPL_V2,
                ki18n( "Copyright (C) 2013 Laurent Montel" ) );

    mAboutData->addAuthor( ki18n( "Laurent Montel" ),
                         ki18n( "Maintainer" ), "montel@kde.org" );

    mAboutData->setProgramIconName( QLatin1String("kmail") );
    mAboutData->setTranslator( ki18nc( "NAME OF TRANSLATORS", "Your names" ),
                             ki18nc( "EMAIL OF TRANSLATORS", "Your emails" ) );


    KHelpMenu *helpMenu = new KHelpMenu(this, mAboutData, true);
    //Initialize menu
    KMenu *menu = helpMenu->menu();
    helpMenu->action(KHelpMenu::menuAboutApp)->setIcon(KIcon(QLatin1String("kmail")));
    setButtonMenu( Help, menu );
}

FolderArchiveConfigureDialog::~FolderArchiveConfigureDialog()
{
    writeConfig();
    delete mAboutData;
}

static const char *myConfigGroupName = "FolderArchiveConfigureDialog";

void FolderArchiveConfigureDialog::readConfig()
{
    KConfigGroup group( KGlobal::config(), myConfigGroupName );

    const QSize size = group.readEntry( "Size", QSize() );
    if ( size.isValid() ) {
        resize( size );
    } else {
        resize( 500, 300 );
    }
}

void FolderArchiveConfigureDialog::writeConfig()
{
    KConfigGroup group( KGlobal::config(), myConfigGroupName );
    group.writeEntry( "Size", size() );
    group.sync();
}


#include "folderarchiveconfiguredialog.moc"
