/***************************************************************************
                          knusersettings.h  -  description
                             -------------------
    
    copyright            : (C) 2000 by Christian Thurner
    email                : cthurner@freepage.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/


#ifndef KNUSERSETTINGS_H
#define KNUSERSETTINGS_H

#include "knsettingswidget.h"
#include "knuserwidget.h"



class KNUserSettings : public KNSettingsWidget  {
	
	public:
		KNUserSettings(QWidget *p);
		virtual ~KNUserSettings();
		
		void init();
		void apply();
		
	protected:
		KNUserWidget *uw;
		KNUserEntry *user;
};

#endif
