/*
	Empath - Mailer for KDE
	
	Copyright (C) 1998 Rik Hemsley rik@kde.org
	
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
# pragma interface "EmpathAttachmentEditDialog.h"
#endif

#ifndef EMPATHATTACHMENTEDITDIALOG_H
#define EMPATHATTACHMENTEDITDIALOG_H

// Qt includes
#include <qdialog.h>
#include <qwidget.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qcombobox.h>

// KDE includes
#include <kbuttonbox.h>

#include "EmpathAttachmentSpec.h"

class RikGroupBox;

class EmpathAttachmentEditDialog : public QDialog
{
	Q_OBJECT

	public:
	
		EmpathAttachmentEditDialog(QWidget * parent = 0, const char * name = 0);
		virtual ~EmpathAttachmentEditDialog();
		
		void setSpec(const EmpathAttachmentSpec & s);
		EmpathAttachmentSpec spec();
		
	protected slots:
		
		void s_OK();
		void s_cancel();
		void s_help();
		
		void s_browse();
		void s_typeChanged(int);

	private:

		RikGroupBox		* rgb_main_;
		RikGroupBox		* rgb_encoding_;

		QWidget			* w_main_;
		QWidget			* w_encoding_;
		
		QButtonGroup	* bg_encoding_;

		QGridLayout		* mainLayout_;
		QGridLayout		* layout_;
		QGridLayout		* encodingLayout_;

		QLabel			* l_filename_;
		QLabel			* l_description_;
		
		QLineEdit		* le_filename_;
		QLineEdit		* le_description_;
		
		QPushButton		* pb_browse_;

		QPushButton		* pb_OK_;
		QPushButton		* pb_cancel_;
		QPushButton		* pb_help_;
		
		QRadioButton	* rb_base64_;
		QRadioButton	* rb_8bit_;
		QRadioButton	* rb_7bit_;
		QRadioButton	* rb_qp_;
		
		QLabel			* l_type_;
		QComboBox		* cb_type_;
		QLabel			* l_subType_;
		QComboBox		* cb_subType_;
		
		QLabel			* l_charset_;
		QComboBox		* cb_charset_;

		KButtonBox		* buttonBox_;
		
		QStringList	textSubTypes_,	messageSubTypes_,	applicationSubTypes_,
					imageSubTypes_,	videoSubTypes_,		audioSubTypes_;
					
		EmpathAttachmentSpec spec_;
};

#endif

