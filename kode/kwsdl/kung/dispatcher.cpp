/*
    This file is part of Kung.

    Copyright (c) 2005 Tobias Koenig <tokoe@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include <qwidget.h>

#include "inputfieldfactory.h"
#include "pageinputfield.h"

#include "dispatcher.h"

Dispatcher::Dispatcher()
  : QObject( 0, "Dispatcher" )
{
}

void Dispatcher::setWSDL( const KWSDL::WSDL &wsdl )
{
  mWSDL = wsdl;

  InputFieldFactory::self()->setTypes( mWSDL.types() );
}

void Dispatcher::run()
{
  const KWSDL::Service::Port::List servicePorts = mWSDL.service().ports();
  KWSDL::Service::Port::List::ConstIterator it;
  for ( it = servicePorts.begin(); it != servicePorts.end(); ++it ) {
    KWSDL::Binding binding = mWSDL.findBinding( (*it).mBinding );

    KWSDL::Port port = mWSDL.findPort( binding.type() );
    const KWSDL::Port::Operation::List operations = port.operations();
    KWSDL::Port::Operation::List::ConstIterator opIt;
    for ( opIt = operations.begin(); opIt != operations.end(); ++opIt ) {
      mInputMessages.append( mWSDL.findMessage( (*opIt).input() ) );
      mOutputMessages.append( mWSDL.findMessage( (*opIt).output() ) );
    }
  }

  KWSDL::Message message = mInputMessages.first();
  InputField *field = new PageInputField( message.name(), message );
  QWidget *wdg = field->createWidget( 0 );
  wdg->show();
}

#include "dispatcher.moc"
