/*                                                                      
    This file is part of KitchenSync.
    Copyright (c) 2004 Tobias Koenig <tokoe@kde.org>                   
                                                                        
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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           
                                                                        
    As a special exception, permission is given to link this program    
    with any edition of Qt, and distribute the resulting executable,    
    without including the source code for Qt in the source distribution.
*/                                                                      

#include <kstaticdeleter.h>

#include "prefs.h"

using namespace KSync;

Prefs* Prefs::mInstance = 0;
static KStaticDeleter<Prefs> prefsStaticDeleter;

Prefs::Prefs()
  : PrefsBase()
{
}

Prefs::~Prefs()
{
}

Prefs* Prefs::instance()
{
  if ( !mInstance ) {
    prefsStaticDeleter.setObject( mInstance, new Prefs() );
    mInstance->readConfig();
  }

  return mInstance;
}
