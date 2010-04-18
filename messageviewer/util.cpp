/*******************************************************************************
**
** Filename   : util
** Created on : 03 April, 2005
** Copyright  : (c) 2005 Till Adam
** Email      : <adam@kde.org>
**
*******************************************************************************/

/*******************************************************************************
**
**   This program is free software; you can redistribute it and/or modify
**   it under the terms of the GNU General Public License as published by
**   the Free Software Foundation; either version 2 of the License, or
**   (at your option) any later version.
**
**   It is distributed in the hope that it will be useful, but
**   WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**   General Public License for more details.
**
**   You should have received a copy of the GNU General Public License
**   along with this program; if not, write to the Free Software
**   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
**   In addition, as a special exception, the copyright holders give
**   permission to link the code of this program with any edition of
**   the Qt library by Trolltech AS, Norway (or with modified versions
**   of Qt that use the same license as Qt), and distribute linked
**   combinations including the two.  You must obey the GNU General
**   Public License in all respects for all of the code used other than
**   Qt.  If you modify this file, you may extend this exception to
**   your version of the file, but you are not obligated to do so.  If
**   you do not wish to do so, delete this exception statement from
**   your version.
**
*******************************************************************************/
#include "util.h"

#include "iconnamecache.h"

#include "messagecore/globalsettings.h"

#include <kcharsets.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <kdebug.h>
#include <KMimeType>

#include <QTextCodec>
#include <QWidget>

using namespace MessageViewer;

bool Util::checkOverwrite( const KUrl &url, QWidget *w )
{
  if ( KIO::NetAccess::exists( url, KIO::NetAccess::DestinationSide, w ) ) {
    if ( KMessageBox::Cancel == KMessageBox::warningContinueCancel(
         w,
         i18n( "A file named \"%1\" already exists. "
             "Are you sure you want to overwrite it?", url.prettyUrl() ),
             i18n( "Overwrite File?" ),
                   KStandardGuiItem::overwrite() ) )
      return false;
  }
  return true;
}

QString Util::fileNameForMimetype( const QString &mimeType, int iconSize,
                                   const QString &fallbackFileName1,
                                   const QString &fallbackFileName2 )
{
  QString fileName;
  KMimeType::Ptr mime = KMimeType::mimeType( mimeType, KMimeType::ResolveAliases );
  if ( mime ) {
    fileName = mime->iconName();
  } else {
    kWarning() << "unknown mimetype" << mimeType;
  }

  if ( fileName.isEmpty() )
  {
    fileName = fallbackFileName1;
    if ( fileName.isEmpty() )
      fileName = fallbackFileName2;
    if ( !fileName.isEmpty() ) {
      fileName = KMimeType::findByPath( "/tmp/" + fileName, 0, true )->iconName();
    }
  }

  return IconNameCache::instance()->iconPath( fileName, iconSize );
}

#ifdef Q_WS_MACX
#include <QDesktopServices>
#endif

bool Util::handleUrlOnMac( const KUrl& url )
{
#ifdef Q_WS_MACX
  QDesktopServices::openUrl( url );
  return true;
#else
  Q_UNUSED( url );
  return false;
#endif
}

int Util::getWritePermissions()
{
  // #79685, #232001 by default use the umask the user defined, but let it be configurable
  if ( MessageCore::GlobalSettings::self()->disregardUmask() ) {
    return S_IRUSR | S_IWUSR;
  } else {
    return -1;
  }
}


