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

#include "qdatastream.h"

#include "Empath.h"
#include "EmpathInterface.h"
#include "RMM_Message.h"
#include "RMM_Enum.h"

enum EmpathFn {
    EmpathFnInboxURL,
    EmpathFnOutboxURL,
    EmpathFnSentURL,
    EmpathFnDraftsURL,
    EmpathFnTrashURL,
    EmpathFnMessage,
    EmpathFnQueue,
    EmpathFnSend,
    EmpathFnSendQueued,
    EmpathFnCheckMail,
    EmpathFnCompose,
    EmpathFnReply,
    EmpathFnReplyAll,
    EmpathFnForward,
    EmpathFnBounce,
    EmpathFnCreateFolder,
    EmpathFnRemoveFolder,
    EmpathFnCopy,
    EmpathFnMove,
    EmpathFnRetrieve,
    EmpathFnWrite,
    EmpathFnRemove,
    EmpathFnRemoveMany,
    EmpathFnMark,
    EmpathFnMarkMany
};

EmpathInterface::EmpathInterface()
    :   DCOPObject()
{
    fnDict_.setAutoDelete(true);

    fnDict_.insert("QString inboxURL()", new int(EmpathFnInboxURL));
    fnDict_.insert("QString outboxURL()", new int(EmpathFnOutboxURL));
    fnDict_.insert("QString sentURL()", new int(EmpathFnSentURL));
    fnDict_.insert("QString draftsURL()", new int(EmpathFnDraftsURL));
    fnDict_.insert("QString trashURL()", new int(EmpathFnTrashURL));
    fnDict_.insert("QByteArray message(QString)", new int(EmpathFnMessage));
    fnDict_.insert("void queue(QByteArray)", new int(EmpathFnQueue));
    fnDict_.insert("void send(QByteArray)", new int(EmpathFnSend));
    fnDict_.insert("void sendQueued()", new int(EmpathFnSendQueued));
    fnDict_.insert("void checkMail()", new int(EmpathFnCheckMail));
    fnDict_.insert("void compose(QString)", new int(EmpathFnCompose));
    fnDict_.insert("void reply(QString)", new int(EmpathFnReply));
    fnDict_.insert("void replyAll(QString)", new int(EmpathFnReplyAll));
    fnDict_.insert("void forward(QString)", new int(EmpathFnForward));
    fnDict_.insert("void bounce(QString)", new int(EmpathFnBounce));
    fnDict_.insert("void createFolder(QString)", new int(EmpathFnCreateFolder));
    fnDict_.insert("void removeFolder(QString)", new int(EmpathFnRemoveFolder));
    fnDict_.insert("void copy(QString, QString)", new int(EmpathFnCopy));
    fnDict_.insert("void move(QString, QString)", new int(EmpathFnMove));
    fnDict_.insert("void retrieve(QString)", new int(EmpathFnRetrieve));
    fnDict_.insert("void write(QString, QByteArray)", new int(EmpathFnWrite));
    fnDict_.insert("void remove(QString)", new int(EmpathFnRemove));
    fnDict_.insert("void remove(QString, QStringList)", new int(EmpathFnRemoveMany));
    fnDict_.insert("void mark(QString, int)", new int(EmpathFnMark));
    fnDict_.insert("void mark(QString, QStringList, int)", new int(EmpathFnMarkMany));
}

    bool
EmpathInterface::process(
    const QCString & cmd, const QByteArray & argData,
    QCString & replyType, QByteArray & replyData)
{
    bool retval = true;

    QDataStream inStream(argData, IO_ReadOnly);
    QDataStream outStream(replyData, IO_WriteOnly);

    int * cmdNamePtr = fnDict_[cmd];

    if (cmdNamePtr == 0)
        return false;

    switch (*(cmdNamePtr)) {

        case EmpathFnInboxURL:
            replyType = "QString";
            outStream << inboxURL();
            break;

        case EmpathFnOutboxURL:
            replyType = "QString";
            outStream << outboxURL();
            break;

        case EmpathFnSentURL:
            replyType = "QString";
            outStream << sentURL();
            break;

        case EmpathFnDraftsURL:
            replyType = "QString";
            outStream << draftsURL();
            break;

        case EmpathFnTrashURL:
            replyType = "QString";
            outStream << draftsURL();
            break;

        case EmpathFnMessage:
            {
                replyType = "QByteArray";
                QString arg;
                inStream >> arg;
                outStream << message(arg);
            }
            break;

        case EmpathFnSend:
            {
                replyType = "void";
                QByteArray arg;
                QDataStream intoArg(arg, IO_WriteOnly);
                inStream >> arg;
                RMM::RMessage msg(QCString(arg.data()));
                empath->send(msg);
            }
            break;

        case EmpathFnQueue:
            {
                replyType = "void";
                QByteArray arg;
                inStream >> arg;
                RMM::RMessage msg(QCString(arg.data()));
                empath->queue(msg);
            }
            break;

        case EmpathFnSendQueued:
            replyType = "void";
            empath->sendQueued();
            break;

        case EmpathFnCheckMail:
            replyType = "void";
            empath->s_checkMail();
            break;

        case EmpathFnCompose:
            {
                replyType = "void";
                QString arg;
                inStream >> arg;
                empath->s_compose(arg);
            }
            break;
            
        case EmpathFnReply:
            {
                replyType = "void";
                QString arg;
                inStream >> arg;
                empath->s_reply(EmpathURL(arg));
            }
            break;

        case EmpathFnReplyAll:
            {
                replyType = "void";
                QString arg;
                inStream >> arg;
                empath->s_replyAll(EmpathURL(arg));
            }
            break;

        case EmpathFnForward:
            {
                replyType = "void";
                QString arg;
                inStream >> arg;
                empath->s_forward(EmpathURL(arg));
            }
            break;

        case EmpathFnBounce:
            {
                replyType = "void";
                QString arg;
                inStream >> arg;
                empath->s_bounce(EmpathURL(arg));
            }
            break;

        case EmpathFnCreateFolder:
            {
                replyType = "void";
                QString arg;
                inStream >> arg;
                empath->createFolder(EmpathURL(arg));
            }
            break;

        case EmpathFnRemoveFolder:
            {
                replyType = "void";
                QString arg;
                inStream >> arg;
                empath->removeFolder(EmpathURL(arg));
            }
            break;

        case EmpathFnCopy:
            {
                replyType = "void";
                QString arg1, arg2;
                inStream >> arg1;
                inStream >> arg2;
                empath->copy(EmpathURL(arg1), EmpathURL(arg2));
            }
            break;

        case EmpathFnMove:
            {
                replyType = "void";
                QString arg1, arg2;
                inStream >> arg1;
                inStream >> arg2;
                empath->move(EmpathURL(arg1), EmpathURL(arg2));
            }
            break;

        case EmpathFnRetrieve:
            {
                replyType = "void";
                QString arg;
                inStream >> arg;
                empath->retrieve(EmpathURL(arg));
            }
            break;

        case EmpathFnWrite:
            {
                replyType = "void";
                QString arg1;
                QByteArray arg2;
                inStream >> arg1;
                inStream >> arg2;
                RMM::RMessage msg(QCString(arg2.data()));
                empath->write(EmpathURL(arg1), msg);
            }
            break;

        case EmpathFnRemove:
            {
                replyType = "void";
                QString arg;
                inStream >> arg;
                empath->remove(EmpathURL(arg));
            }
            break;

        case EmpathFnRemoveMany:
            {
                replyType = "void";
                QString arg1;
                QStringList arg2;
                inStream >> arg1;
                inStream >> arg2;
                empath->remove(EmpathURL(arg1), arg2);
            }
            break;

        case EmpathFnMark:
            {
                replyType = "void";
                QString arg1;
                int arg2;
                inStream >> arg1;
                inStream >> arg2;
                empath->mark(EmpathURL(arg1), RMM::MessageStatus(arg2));
            }
            break;

        case EmpathFnMarkMany:
            {
                replyType = "void";
                QString arg1;
                QStringList arg2;
                int arg3;
                inStream >> arg1;
                inStream >> arg2;
                inStream >> arg3;
                empath->mark(EmpathURL(arg1), arg2, RMM::MessageStatus(arg3));
            }
            break;

        default:
            retval = false;
            break;
    }

    return retval;
}

