/*
    Empath - Mailer for KDE
    
    Copyright 1999, 2000
        Rik Hemsley <rik@kde.org>
        Wilco Greven <j.w.greven@student.utwente.nl>
    
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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef __GNUG__
# pragma interface "EmpathMainWidget.h"
#endif

#ifndef EMPATHMAINWIDGET_H
#define EMPATHMAINWIDGET_H

// Qt includes
#include <qwidget.h>

// Local includes
#include "EmpathURL.h"

class EmpathMessageListWidget;
class EmpathMessageViewWidget;

class EmpathMainWidget : public QWidget
{
    Q_OBJECT

    public:
        
        EmpathMainWidget(QWidget * parent);
        ~EmpathMainWidget();

        EmpathMessageListWidget * messageListWidget()
        { return messageListWidget_; }

    protected slots:
    
        void s_showFolder(const EmpathURL & url);
        void s_changeView(const QString & messageID);
        void s_reply(const QString &);
        void s_replyAll(const QString &);
        void s_forward(const QString &);
        void s_bounce(const QString &);
        void s_remove(const QStringList &);
        void s_save(const QString &);
        void s_copy(const QStringList &);
        void s_move(const QStringList &);
        void s_print(const QStringList &);
        void s_filter(const QStringList &);
        void s_view(const QString &);
        
    private:

        EmpathURL currentFolder_;

        EmpathMessageListWidget * messageListWidget_;
        EmpathMessageViewWidget * messageViewWidget_;
};

#endif
// vim:ts=4:sw=4:tw=78
