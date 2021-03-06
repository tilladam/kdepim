/*
 * Copyright (c) 1996-1998 Stefan Taferner <taferner@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef MAILCOMMON_FILTERACTIONWITHSTRINGLIST_H
#define MAILCOMMON_FILTERACTIONWITHSTRINGLIST_H

#include "filteractionwithstring.h"

#include <QtCore/QStringList>

namespace MailCommon {

/**
 * @short Abstract base class for filter actions with a fixed set of string parameters.
 *
 * Abstract base class for mail filter actions that need a
 * parameter which can be chosen from a fixed set, e.g. 'set
 * identity'.  Can create a KComboBox as parameter widget. A
 * subclass of this must provide at least implementations for the
 * following methods:
 *
 * @li virtual FilterAction::ReturnCodes FilterAction::process
 * @li static  FilterAction::newAction
 *
 * Additionally, it's constructor should populate the
 * QStringList @p mParameterList with the valid parameter
 * strings. The combobox will then contain be populated automatically
 * with those strings. The default string will be the first one.
 *
 * @author Marc Mutz <mutz@kde.org>, based upon work by Stefan Taferner <taferner@kde.org>
 * @see FilterActionWithString FilterActionWithFolder FilterAction Filter
 */
class FilterActionWithStringList : public FilterActionWithString
{
    Q_OBJECT

public:
    /**
     * @copydoc FilterAction::FilterAction
     */
    FilterActionWithStringList(const QString &name, const QString &label, QObject *parent = 0 );

    /**
     * @copydoc FilterAction::createParamWidget
     */
    virtual QWidget *createParamWidget( QWidget *parent ) const;

    /**
     * @copydoc FilterAction::applyParamWidgetValue
     */
    virtual void applyParamWidgetValue( QWidget *paramWidget );

    /**
     * @copydoc FilterAction::setParamWidgetValue
     */
    virtual void setParamWidgetValue( QWidget *paramWidget ) const;

    /**
     * @copydoc FilterAction::clearParamWidget
     */
    virtual void clearParamWidget( QWidget *paramWidget ) const;

    /**
     * @copydoc FilterAction::argsFromString
     */
    virtual void argsFromString( const QString &argsStr );

protected:
    QStringList mParameterList;
};

}

#endif
