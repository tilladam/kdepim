/* ============================================================
*
* This file is a part of the rekonq project
* Copyright (c) 2013, 2014 Montel Laurent <montel@kde.org>
* based on code from rekonq
* Copyright (C) 2010-2011 by Benjamin Poulain <ikipou at gmail dot com>
* Copyright (C) 2010-2013 by Andrea Diamantini <adjam7 at gmail dot com>
*
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License or (at your option) version 3 or any later version
* accepted by the membership of KDE e.V. (or its successor approved
* by the membership of KDE e.V.), which shall act as a proxy
* defined in Section 14 of version 3 of the license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* ============================================================ */


#ifndef ADBLOCKRULETEXTMATCHIMPL_H
#define ADBLOCKRULETEXTMATCHIMPL_H

#include "adblockruleimpl.h"

// Qt Includes
#include <QString>

namespace MessageViewer {
// Simple rule to find a string in the URL
class AdBlockRuleTextMatchImpl : public AdBlockRuleImpl
{
public:
    explicit AdBlockRuleTextMatchImpl(const QString &filter);
    
    bool match(const QNetworkRequest &request, const QString &encodedUrl, const QString &encodedUrlLowerCase) const;

    static bool isTextMatchFilter(const QString &filter);

    QString ruleString() const;
    QString ruleType() const;

private:
    QString m_textToMatch;
};
}

#endif // ADBLOCKRULETEXTMATCHIMPL_H
