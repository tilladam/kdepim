/*
    knscoring.cpp

    KNode, the KDE newsreader
    Copyright (c) 1999-2001 the KNode authors.
    See file AUTHORS for details

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US
*/

#include <qobject.h>
#include <qstring.h>

#include <kdebug.h>

#include "knmime.h"
#include "knscoring.h"
#include "knaccountmanager.h"
#include "kngroupmanager.h"
#include "knnntpaccount.h"
#include "knglobals.h"

//----------------------------------------------------------------------------
KNScorableArticle::KNScorableArticle(KNRemoteArticle* a)
  : ScorableArticle(), _a(a)
{
}

KNScorableArticle::~KNScorableArticle()
{
}

void KNScorableArticle::addScore(short s)
{
  _a->addScore(s);
}

QString KNScorableArticle::from() const 
{
  return _a->from()->asUnicodeString();
}

QString KNScorableArticle::subject() const 
{
  return _a->subject()->asUnicodeString();
}

QString KNScorableArticle::getHeaderByType(const QString& s) const 
{
  KNHeaders::Base *h = _a->getHeaderByType(s.latin1());
  if (!h) return "";
  QString t = _a->getHeaderByType(s.latin1())->asUnicodeString();
  ASSERT( t );
  return t;
}

//----------------------------------------------------------------------------
KNScorableGroup::KNScorableGroup()
{
}

KNScorableGroup::~KNScorableGroup()
{
}

//----------------------------------------------------------------------------
KNScoringManager::KNScoringManager()
{
}

KNScoringManager::~KNScoringManager()
{
}

QStringList KNScoringManager::getGroups() const 
{
  KNAccountManager *am = knGlobals.accManager;
  KNNntpAccount *ac = am->first();
  QStringList res;
  while (ac) {
    QStringList groups;
    knGlobals.grpManager->getSubscribed(ac,groups);
    res += groups;
    ac = am->next();
  }
  res.sort();
  return res;
}
