/*
   This file is part of Kontact

   Copyright (c) 2001 Matthias Hoelzer-Kluepfel <mhk@kde.org>
   Copyright (c) 2002 Daniel Molkentin <molkentin@kde.org>

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

// $Id$

#ifndef KP_PLUGIN_H
#define KP_PLUGIN_H

#include <qobject.h>

#include <kxmlguiclient.h>

class QStringList;
class DCOPClient;
class DCOPObject;
class KAboutData;

namespace Kaplan
{
    class Core;

    /**
     * Base class for all Plugins in Kaplan. Inherit from it
     * to get a plugin. It can insert an icon into the sidepane,
     * add widgets to the widgetstack and add menu items via XMLGUI.
     */
    class Plugin : public QObject, virtual public KXMLGUIClient
    {
	Q_OBJECT

    public:
        /**
         * Creates a new Plugin, note that @param name is required if
         * you want your plugin to do dcop via it's own instance of
         * @ref DCOPClient by calling @ref dcopClient.
         */
        Plugin(Core *core, QObject *parent, const char *name);
        ~Plugin();

        /**
         * Offers access to Kaplans core.
         **/

        Core *core() const;

        /**
         * Create the DCOP interface for the given @p serviceType, if this
         * plugin provides it. Return false otherwise.
         */
        virtual bool createDCOPInterface( const QString& /*serviceType*/ ) { return 0L; }

        /**
         * Reimplement this method and return a @ref QStringList of all config
         * modules your application part should offer via Kaplan. Note that the
         * part and the module will have to take care for config syncing themselves.
         * Usually @p DCOP used for that purpose.
         *
         * @note Make sure you offer the modules in the form: 
         * <code>"pathrelativetosettings/mysettings.desktop"</code>
         *
         **/
        virtual QStringList configModules() const { return QStringList(); };

        /**
         * Reimplement this method if you want to add your credits to the Kaplan
         * about dialog.
         **/
        virtual KAboutData* aboutData() { return 0L; };

        /**
         * Retrieve the current DCOP Client for the plugin.
         *
         * The clients name is taken from the name argument in the constructor.
         * @note: The DCOPClient object will only be created when this method is
         * called for the first time. Make sure that the part has been loaded
         * before calling this method, if it's the one that contains the DCOP
         * interface that other parts might use.
         */
        DCOPClient *dcopClient() const;

    private:
        class Private;
        Private *d;

    };

};


#endif

// vim: ts=4 sw=4 et
