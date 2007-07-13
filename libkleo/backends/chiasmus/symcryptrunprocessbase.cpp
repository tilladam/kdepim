/*
    symcryptrunbackend.cpp

    This file is part of libkleopatra, the KDE keymanagement library
    Copyright (c) 2005 Klar�vdalens Datakonsult AB

    Libkleopatra is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    Libkleopatra is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#include "symcryptrunprocessbase.h"

#include <ktemporaryfile.h>
#include <kdebug.h>
#include <kshell.h>

#include <QTimer>
#include <QFile>
#include <QStringList>

#include <cstring>

Kleo::SymCryptRunProcessBase::SymCryptRunProcessBase( const QString & class_, const QString & program,
                                                      const QString & keyFile, const QString & options,
                                                      Operation mode,
                                                      QObject * parent )
  : KProcess( parent ),
    mOperation( mode ), mOptions( options )
{
  *this << "symcryptrun"
        << "--class" << class_
        << "--program" << program
        << "--keyfile" << keyFile
        << ( mode == Encrypt ? "--encrypt" : "--decrypt" );
}

Kleo::SymCryptRunProcessBase::~SymCryptRunProcessBase() {}


bool Kleo::SymCryptRunProcessBase::startBlock(const QByteArray & input)
{
  connect( this, SIGNAL(readyReadStandardOutput() ), this, SLOT( slotReceivedStdout() ) );
  connect( this, SIGNAL( readyReadStandardError () ), this, SLOT(slotReceivedStderr() ) );
  KTemporaryFile tempfile;
  if ( tempfile.open() )
    tempfile.write( input );
  else
    return false;

  tempfile.flush();
  *this << "--input" << tempfile.fileName();
  addOptions();
  setOutputChannelMode( KProcess::SeparateChannels );
  return ( execute () == 0 );
}

bool Kleo::SymCryptRunProcessBase::startNotify(const QByteArray & input)
{
  connect( this, SIGNAL(readyReadStandardOutput() ), this, SLOT( slotReceivedStdout() ) );
  connect( this, SIGNAL( readyReadStandardError () ), this, SLOT(slotReceivedStderr() ) );
  addOptions();
  setOutputChannelMode( KProcess::SeparateChannels );
  start();
  const bool ok = waitForStarted();
  if ( !ok )
    return ok;
  mInput = input;
  write(  mInput.begin(), mInput.size() );
  closeWriteChannel();
  waitForFinished();
  return true;
}

void Kleo::SymCryptRunProcessBase::addOptions() {
  if ( !mOptions.isEmpty() )
  {
      const QStringList args = KShell::splitArgs( mOptions );
      *this << "--" << args;
  }
}

void Kleo::SymCryptRunProcessBase::slotReceivedStdout() {
  QByteArray str = readAllStandardOutput();
  const int oldsize = mOutput.size();
  mOutput.resize( oldsize + str.length() );
  memcpy( mOutput.data() + oldsize, /*QString::fromLocal8Bit(*/ str /*)*/,  str.length() );
}

void Kleo::SymCryptRunProcessBase::slotReceivedStderr() {
  QByteArray str = readAllStandardError();
  if ( !str.isEmpty() )
    mStderr += QString::fromLocal8Bit( readAllStandardError ());
}

#include "symcryptrunprocessbase.moc"
