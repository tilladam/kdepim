/*
    This file is part of KXForms.

    Copyright (c) 2005,2006 Cornelius Schumacher <schumacher@kde.org>

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
#ifndef KXFORMS_FORMGUI_H
#define KXFORMS_FORMGUI_H

#include "guielement.h"

#include <QWidget>
#include <QDomElement>
#include <QLabel>
#include <QList>

class QBoxLayout;
class QLabel;

namespace KXForms {

class Manager;

class FormGui : public QWidget
{
    Q_OBJECT
  public:
    typedef QList<FormGui *> List;

    FormGui( const QString &label, Manager *, QWidget *parent );

    void parseElement( const QDomElement & );

    void setRef( const Reference &ref );
    Reference ref() const;

    QString label() const;

    void loadData( const QDomDocument & );
    void saveData();

    void setLabelHidden( bool );

  signals:
    void editingFinished();

  protected:
    void setRefLabel( const Reference &ref );

    QDomElement findContextElement( const QDomDocument &doc );

  private:
    Manager *mManager;

    Reference mRef;

    GuiElement::List mGuiElements;

    QBoxLayout *mTopLayout;

    QLabel *mLabel;
    QLabel *mRefLabel;
    
    bool mLabelHidden;
};

}

#endif
