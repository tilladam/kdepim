/* options.cc			KPilot
**
** Copyright (C) 2000-2001 by Adriaan de Groot
**
** This is a file of odds and ends, with debugging functions and stuff.
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
** MA 02111-1307, USA.
*/

/*
** Bug reports and questions can be sent to kde-pim@kde.org
*/


static const char *options_id =
	"$Id$";

#include "options.h"


#include <iostream>

#include <qsize.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kcmdlineargs.h>

// The daemon also has a debug level; debug_spaces is 60 spaces,
// to align FUNCTIONSETUP output.
//
//
#ifdef DEBUG
int debug_level = 1;
#else
int debug_level = 0;
#endif
const char *debug_spaces =
	"                                                    ";

QString qstringExpansion(const QString & s)
{
	QString t;

	for (unsigned i = 0; i < s.length(); i++)
	{
		t += s[i];
		t += ' ';
		t += QString::number((int) s[i].unicode());
		t += ' ';
	}

	return t;
}

QString charExpansion(const char *s)
{
	QString t;

	while (*s)
	{
		t += QChar(*s);
		t += ' ';
		t += QString::number(*s);
		t += ' ';
		s++;
	}

	return t;
}

QDateTime readTm(const struct tm &t)
{
  QDateTime dt;
  dt.setDate(QDate(1900 + t.tm_year, t.tm_mon + 1, t.tm_mday));
  dt.setTime(QTime(t.tm_hour, t.tm_min, t.tm_sec));
  return dt;
}



struct tm writeTm(const QDateTime &dt)
{
  struct tm t;

  t.tm_wday = 0; // unimplemented
  t.tm_yday = 0; // unimplemented
  t.tm_isdst = 0; // unimplemented

  t.tm_year = dt.date().year() - 1900;
  t.tm_mon = dt.date().month() - 1;
  t.tm_mday = dt.date().day();
  t.tm_hour = dt.time().hour();
  t.tm_min = dt.time().minute();
  t.tm_sec = dt.time().second();

  return t;
}



struct tm writeTm(const QDate &dt)
{
  struct tm t;

  t.tm_wday = 0; // unimplemented
  t.tm_yday = 0; // unimplemented
  t.tm_isdst = 0; // unimplemented

  t.tm_year = dt.year() - 1900;
  t.tm_mon = dt.month() - 1;
  t.tm_mday = dt.day();
  t.tm_hour = 0;
  t.tm_min = 0;
  t.tm_sec = 0;

  return t;
}

static KCmdLineOptions debug_options_[] = {
	{"debug <level>", I18N_NOOP("Set debugging level"), "0"},
	{0, 0, 0}
};

KCmdLineOptions *debug_options = debug_options_;


int const fname = ((int) options_id);


