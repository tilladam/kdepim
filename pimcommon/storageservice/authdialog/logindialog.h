/*
  Copyright (c) 2013, 2014 Montel Laurent <montel@kde.org>

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

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <KDialog>

class KLineEdit;
class QLabel;
namespace PimCommon {
class LoginDialog : public KDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent=0);
    ~LoginDialog();

    void setUsernameLabel(const QString &labelName);

    QString password() const;
    QString username() const;

    void setPassword(const QString &pass);
    void setUserName(const QString &name);
private slots:
    void slotUserNameChanged(const QString &passwd);

private:
    KLineEdit *mUsername;
    KLineEdit *mPassword;
    QLabel *mLabUsername;
};

}
#endif // LOGINDIALOG_H
