/*                                                                      
    This file is part of KAddressBook.                                  
    Copyright (c) 2002 Mike Pilone <mpilone@slac.com>                   
                                                                        
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
                                                                        
    As a special exception, permission is given to link this program    
    with any edition of Qt, and distribute the resulting executable,    
    without including the source code for Qt in the source distribution.
*/                                                                      


#include <qbuttongroup.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qsignal.h>
#include <qstring.h>

#include <kapplication.h>
#include <kbuttonbox.h>
#include <kconfig.h>
#include <klineedit.h>
#include <klistview.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdialog.h>

#include "addresseditwidget.h"


class AddressItem : public QListBoxText
{
public:
  AddressItem( QListBox *parent, const KABC::Address::List &list, const KABC::Address &addr )
    : QListBoxText( parent, QString::null ), mAddress( addr )
  {
    KABC::Address::List::ConstIterator it;
    int occurances = 0;
    for ( it = list.begin(); it != list.end(); ++it ) {
      if ( (*it).id() == mAddress.id() ) {
        QString text = mAddress.typeLabel();
        if ( occurances > 0 )
          text += " " + QString::number( occurances + 1 );
        setText( text );
        break;
      }

      if ( ( (*it).type() & ~KABC::Address::Pref ) == ( mAddress.type() & ~KABC::Address::Pref ) )
        occurances++;
    }
  }

  KABC::Address address() const { return mAddress; }


private:
  KABC::Address mAddress;
};

AddressEditWidget::AddressEditWidget( QWidget *parent, const char *name )
  : QWidget( parent, name )
{
  QGridLayout *layout = new QGridLayout( this, 4, 2 );
  layout->setSpacing( KDialog::spacingHint() );

  mTypeCombo = new KComboBox(this);
  connect( mTypeCombo, SIGNAL( highlighted(int) ), SLOT( typeHighlighted(int) ) );
  layout->addWidget(mTypeCombo, 0, 0);

  mAddressTextEdit = new QTextEdit( this );
  mAddressTextEdit->setReadOnly( true );
  layout->addMultiCellWidget( mAddressTextEdit, 1, 3, 0, 0 );
  
  QPushButton *addButton = new QPushButton( i18n( "&Add" ), this );
  layout->addWidget( addButton, 1, 1 );
  connect( addButton, SIGNAL( clicked() ), SLOT( addAddress() ) );

  mEditButton = new QPushButton( i18n( "&Edit..." ), this );
  mEditButton->setEnabled( false );
  connect( mEditButton, SIGNAL( clicked() ), SLOT( editAddress() ) );
  layout->addWidget( mEditButton, 2, 1 );

  mRemoveButton = new QPushButton( i18n( "&Remove" ), this );
  mRemoveButton->setEnabled( false );
  connect( mRemoveButton, SIGNAL( clicked() ), SLOT( removeAddress() ) );
  layout->addWidget( mRemoveButton, 3, 1 );

  mIndex = 0;
}

AddressEditWidget::~AddressEditWidget()
{
}
    
const KABC::Address::List &AddressEditWidget::addresses()
{
  return mAddressList;
}

void AddressEditWidget::setAddresses(const KABC::Address::List &list)
{
  mAddressList = list;
  
  mAddressTextEdit->setText("");

  updateTypeCombo( mAddressList, mTypeCombo );

  typeHighlighted( mIndex );
}

void AddressEditWidget::addressChanged()
{
  bool state = ( mTypeCombo->count() != 0 );

  mEditButton->setEnabled( state );
  mRemoveButton->setEnabled( state );
}

void AddressEditWidget::addAddress()
{
  KABC::Address a;
  AddressEditDialog dialog( mAddressList, a, this );
  if ( dialog.exec() ) {
    mAddressList.append( dialog.address() );
    updateTypeCombo( mAddressList, mTypeCombo );
    emit modified();
  }
  
  typeHighlighted( mIndex );
}

void AddressEditWidget::editAddress()
{
  AddressEditDialog dialog( mAddressList, currentAddress( mTypeCombo, mTypeCombo->currentItem() ) , this );
  if ( dialog.exec() ) {
    KABC::Address addr = dialog.address();

    KABC::Address::List::Iterator it;
    for ( it = mAddressList.begin(); it != mAddressList.end(); ++it )
      if ( (*it).id() == addr.id() )
        (*it) = addr;

    updateTypeCombo( mAddressList, mTypeCombo );
    emit modified();
  }

  typeHighlighted( mIndex );
}

void AddressEditWidget::removeAddress()
{
  mAddressList.remove( currentAddress( mTypeCombo, mTypeCombo->currentItem() ) );
  updateTypeCombo( mAddressList, mTypeCombo );
  emit modified();

  typeHighlighted( mIndex );
}

void AddressEditWidget::typeHighlighted(int index)
{
  // First try to find the type
  mIndex = index;

  KABC::Address a = currentAddress( mTypeCombo, mIndex );

  bool block = signalsBlocked();
  blockSignals(true);
          
  if ( !a.isEmpty() ) {
    QString text;
    if ( !a.street().isEmpty() )
      text += a.street() + "\n";

    if ( !a.postOfficeBox().isEmpty() )
      text += a.postOfficeBox() + "\n";

    text += a.locality() + QString(" ") + a.region();

    if ( !a.postalCode().isEmpty() )
      text += QString(", ") + a.postalCode();

    text += "\n";

    if ( !a.country().isEmpty() )
      text += a.country() + "\n";

    text += a.extended();

    mAddressTextEdit->setText(text);
  } else
    mAddressTextEdit->setText("");
  
  blockSignals(block);

  addressChanged();
}

void AddressEditWidget::updateTypeCombo( const KABC::Address::List &list, KComboBox *combo )
{
  int pos = combo->currentItem();
  combo->clear();

  QListBox *lb = combo->listBox();

  KABC::Address::List::ConstIterator it;
  for ( it = list.begin(); it != list.end(); ++it )
    new AddressItem( lb, list, *it );

  lb->sort();
  combo->setCurrentItem( pos );
}

KABC::Address AddressEditWidget::currentAddress( KComboBox *combo, int index )
{
  if ( index < 0 ) index = 0;
  if ( index >= combo->count() ) index = combo->count() - 1;

  QListBox *lb = combo->listBox();
  AddressItem *item = dynamic_cast<AddressItem*>( lb->item( index ) );
  if ( item )
    return item->address();
  else
    return KABC::Address();
}

AddressEditDialog::AddressEditDialog( const KABC::Address::List &list, const KABC::Address &addr, QWidget *parent, const char *name )
  : KDialogBase( KDialogBase::Plain, i18n( "Edit Address" ),
                 KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok,
                 parent, name )
{
  mAddress = addr;

  QWidget *page = plainPage();
  
  QGridLayout *topLayout = new QGridLayout(page, 8, 2);
  topLayout->setSpacing(spacingHint());

  mTypeCombo = new KComboBox( page );
  topLayout->addWidget( mTypeCombo, 0, 0 );
  QPushButton *editTypeButton = new QPushButton( i18n( "&Other Type..." ), page );
  connect( editTypeButton, SIGNAL( clicked() ), SLOT( editType() ) );
  topLayout->addWidget( editTypeButton, 0, 1 );

  QLabel *label = new QLabel(i18n("Street:"), page);
  label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  topLayout->addWidget(label, 1, 0);
  mStreetTextEdit = new QTextEdit(page, "mStreetTextEdit");
  topLayout->addWidget(mStreetTextEdit, 1, 1);

  label = new QLabel(i18n("Post office box:"), page);
  topLayout->addWidget(label, 2 , 0);
  mPOBoxEdit = new KLineEdit(page, "mPOBoxEdit");
  topLayout->addWidget(mPOBoxEdit, 2, 1);

  label = new QLabel(i18n("Locality:"), page);
  topLayout->addWidget(label, 3, 0);
  mLocalityEdit = new KLineEdit(page, "mLocalityEdit");
  topLayout->addWidget(mLocalityEdit, 3, 1);

  label = new QLabel(i18n("Region:"), page);
  topLayout->addWidget(label, 4, 0);
  mRegionEdit = new KLineEdit(page, "mRegionEdit");
  topLayout->addWidget(mRegionEdit, 4, 1);

  label = new QLabel(i18n("Postal code:"), page);
  topLayout->addWidget(label, 5, 0);
  mPostalCodeEdit = new KLineEdit(page, "mPostalCodeEdit");
  topLayout->addWidget(mPostalCodeEdit, 5, 1);

  label = new QLabel(i18n("Country:"), page);
  topLayout->addWidget(label, 6, 0);
  mCountryCombo = new KComboBox(page, "mCountryCombo");
  mCountryCombo->setEditable(true);
  mCountryCombo->setDuplicatesEnabled(false);
  mCountryCombo->setAutoCompletion(true);
  topLayout->addWidget(mCountryCombo, 6, 1);
  
  mPreferredCheckBox = new QCheckBox( i18n( "This is the preferred address" ), page );
  topLayout->addMultiCellWidget( mPreferredCheckBox, 7, 7, 0, 1 );
  
  fillCombo(mCountryCombo);

  // fill the type list
  KABC::Address::List::ConstIterator it;
  for ( it = list.begin(); it != list.end(); ++it ) {
    int type = ( (*it).type() & ~KABC::Address::Pref );
    if ( !mTypeList.contains( type ) )
      mTypeList.append( type );
  }
  if ( !mTypeList.contains( KABC::Address::Home ) )
    mTypeList.append( KABC::Address::Home );
  if ( !mTypeList.contains( KABC::Address::Work ) )
    mTypeList.append( KABC::Address::Work );

  updateTypeCombo();
  mTypeCombo->setCurrentItem( mTypeList.findIndex( mAddress.type() & ~KABC::Address::Pref ) );

  // Fill in the values
  mStreetTextEdit->setText(mAddress.street());
  mStreetTextEdit->setFocus();
  mRegionEdit->setText(mAddress.region());
  mLocalityEdit->setText(mAddress.locality());
  mPostalCodeEdit->setText(mAddress.postalCode());
  mPOBoxEdit->setText(mAddress.postOfficeBox());
  mCountryCombo->setCurrentText(mAddress.country());
  mPreferredCheckBox->setChecked( mAddress.type() & KABC::Address::Pref );

  // initialize the layout
  topLayout->activate();
}

AddressEditDialog::~AddressEditDialog()
{
}
    
const KABC::Address &AddressEditDialog::address()
{
  mAddress.setType( mTypeList[ mTypeCombo->currentItem() ] );
  mAddress.setLocality(mLocalityEdit->text());
  mAddress.setRegion(mRegionEdit->text());
  mAddress.setPostalCode(mPostalCodeEdit->text());
  mAddress.setCountry(mCountryCombo->currentText());
  mAddress.setPostOfficeBox(mPOBoxEdit->text());
  mAddress.setStreet(mStreetTextEdit->text());

  if ( mPreferredCheckBox->isChecked() )
    mAddress.setType( mAddress.type() | KABC::Address::Pref );
  else
    mAddress.setType( mAddress.type() & ~KABC::Address::Pref );

  return mAddress;
}

void AddressEditDialog::editType()
{
  AddressTypeDialog dlg( mAddress.type(), this );
  if ( dlg.exec() ) {
    if ( !mTypeList.contains( dlg.type() ) )
      mTypeList.append( dlg.type() );

    updateTypeCombo();
    mTypeCombo->setCurrentItem( mTypeList.findIndex( dlg.type() ) );
  }
}

void AddressEditDialog::updateTypeCombo()
{
  int pos = mTypeCombo->currentItem();
  mTypeCombo->clear();
  
  QValueList<int>::Iterator it;
  for ( it = mTypeList.begin(); it != mTypeList.end(); ++it ) {
    KABC::Address addr( *it );
    mTypeCombo->insertItem( addr.typeLabel() );
  }

  mTypeCombo->setCurrentItem( pos );
}

void AddressEditDialog::fillCombo(KComboBox *combo)
{
  QString sCountry[] = {
    i18n( "Afghanistan" ), i18n( "Albania" ), i18n( "Algeria" ),
    i18n( "American Samoa" ), i18n( "Andorra" ), i18n( "Angola" ),
    i18n( "Anguilla" ), i18n( "Antarctica" ), i18n( "Antigua and Barbuda" ),
    i18n( "Argentina" ), i18n( "Armenia" ), i18n( "Aruba" ),
    i18n( "Ashmore and Cartier Islands" ), i18n( "Australia" ),
    i18n( "Austria" ), i18n( "Azerbaijan" ), i18n( "Bahamas" ),
    i18n( "Bahrain" ), i18n( "Bangladesh" ), i18n( "Barbados" ),
    i18n( "Belarus" ), i18n( "Belgium" ), i18n( "Belize" ),
    i18n( "Benin" ), i18n( "Bermuda" ), i18n( "Bhutan" ),
    i18n( "Bolivia" ), i18n( "Bosnia and Herzegovina" ), i18n( "Botswana" ),
    i18n( "Brazil" ), i18n( "Brunei" ), i18n( "Bulgaria" ),
    i18n( "Burkina Faso" ), i18n( "Burundi" ), i18n( "Cambodia" ),
    i18n( "Cameroon" ), i18n( "Canada" ), i18n( "Cape Verde" ),
    i18n( "Cayman Islands" ), i18n( "Central African Republic" ),
    i18n( "Chad" ), i18n( "Chile" ), i18n( "China" ), i18n( "Colombia" ),
    i18n( "Comoros" ), i18n( "Congo" ), i18n( "Congo, Dem. Rep." ),
    i18n( "Costa Rica" ), i18n( "Croatia" ),
    i18n( "Cuba" ), i18n( "Cyprus" ), i18n( "Czech Republic" ),
    i18n( "Denmark" ), i18n( "Djibouti" ),
    i18n( "Dominica" ), i18n( "Dominican Republic" ), i18n( "Ecuador" ),
    i18n( "Egypt" ), i18n( "El Salvador" ), i18n( "Equatorial Guinea" ),
    i18n( "Eritrea" ), i18n( "Estonia" ), i18n( "England" ),
    i18n( "Ethiopia" ), i18n( "European Union" ), i18n( "Faroe Islands" ),
    i18n( "Fiji" ), i18n( "Finland" ), i18n( "France" ),
    i18n( "French Polynesia" ), i18n( "Gabon" ), i18n( "Gambia" ),
    i18n( "Georgia" ), i18n( "Germany" ), i18n( "Ghana" ),
    i18n( "Greece" ), i18n( "Greenland" ), i18n( "Grenada" ),
    i18n( "Guam" ), i18n( "Guatemala" ), i18n( "Guinea" ),
    i18n( "Guinea-Bissau" ), i18n( "Guyana" ), i18n( "Haiti" ),
    i18n( "Honduras" ), i18n( "Hong Kong" ), i18n( "Hungary" ), 
    i18n( "Iceland" ), i18n( "India" ), i18n( "Indonesia" ),
    i18n( "Iran" ), i18n( "Iraq" ), i18n( "Ireland" ),
    i18n( "Israel" ), i18n( "Italy" ), i18n( "Ivory Coast" ),
    i18n( "Jamaica" ), i18n( "Japan" ), i18n( "Jordan" ),
    i18n( "Kazakhstan" ), i18n( "Kenya" ), i18n( "Kiribati" ),
    i18n( "Korea, North" ), i18n( "Korea, South" ),
    i18n( "Kuwait" ), i18n( "Kyrgyzstan" ), i18n( "Laos" ),
    i18n( "Latvia" ), i18n( "Lebanon" ), i18n( "Lesotho" ),
    i18n( "Liberia" ), i18n( "Libya" ), i18n( "Liechtenstein" ),
    i18n( "Lithuania" ), i18n( "Luxembourg" ), i18n( "Macau" ),
    i18n( "Madagascar" ), i18n( "Malawi" ), i18n( "Malaysia" ),
    i18n( "Maldives" ), i18n( "Mali" ), i18n( "Malta" ),
    i18n( "Marshall Islands" ), i18n( "Martinique" ), i18n( "Mauritania" ),
    i18n( "Mauritius" ), i18n( "Mexico" ),
    i18n( "Micronesia, Federated States Of" ), i18n( "Moldova" ),
    i18n( "Monaco" ), i18n( "Mongolia" ), i18n( "Montserrat" ),
    i18n( "Morocco" ), i18n( "Mozambique" ), i18n( "Myanmar" ),
    i18n( "Namibia" ),
    i18n( "Nauru" ), i18n( "Nepal" ), i18n( "Netherlands" ),
    i18n( "Netherlands Antilles" ), i18n( "New Caledonia" ),
    i18n( "New Zealand" ), i18n( "Nicaragua" ), i18n( "Niger" ),
    i18n( "Nigeria" ), i18n( "Niue" ), i18n( "North Korea" ),
    i18n( "Northern Ireland" ), i18n( "Northern Mariana Islands" ),
    i18n( "Norway" ), i18n( "Oman" ), i18n( "Pakistan" ), i18n( "Palau" ),
    i18n( "Palestinian" ), i18n( "Panama" ), i18n( "Papua New Guinea" ),
    i18n( "Paraguay" ), i18n( "Peru" ), i18n( "Philippines" ),
    i18n( "Poland" ), i18n( "Portugal" ), i18n( "Puerto Rico" ),
    i18n( "Qatar" ), i18n( "Romania" ), i18n( "Russia" ), i18n( "Rwanda" ),
    i18n( "St. Kitts and Nevis" ), i18n( "St. Lucia" ),
    i18n( "St. Vincent and the Grenadines" ), i18n( "San Marino" ),
    i18n( "Sao Tome and Principe" ), i18n( "Saudi Arabia" ),
    i18n( "Senegal" ), i18n( "Serbia & Montenegro" ), i18n( "Seychelles" ),
    i18n( "Sierra Leone" ), i18n( "Singapore" ), i18n( "Slovakia" ),
    i18n( "Slovenia" ), i18n( "Solomon Islands" ), i18n( "Somalia" ),
    i18n( "South Africa" ), i18n( "South Korea" ), i18n( "Spain" ),
    i18n( "Sri Lanka" ), i18n( "St. Kitts and Nevis" ), i18n( "Sudan" ),
    i18n( "Suriname" ), i18n( "Swaziland" ), i18n( "Sweden" ),
    i18n( "Switzerland" ), i18n( "Syria" ), i18n( "Taiwan" ),
    i18n( "Tajikistan" ), i18n( "Tanzania" ), i18n( "Thailand" ),
    i18n( "Tibet" ), i18n( "Togo" ), i18n( "Tonga" ),
    i18n( "Trinidad and Tobago" ), i18n( "Tunisia" ), i18n( "Turkey" ),
    i18n( "Turkmenistan" ), i18n( "Turks and Caicos Islands" ),
    i18n( "Tuvalu" ), i18n( "Uganda " ), i18n( "Ukraine" ),
    i18n( "United Arab Emirates" ), i18n( "United Kingdom" ),
    i18n( "United States" ), i18n( "Uruguay" ), i18n( "Uzbekistan" ),
    i18n( "Vanuatu" ), i18n( "Vatican City" ), i18n( "Venezuela" ),
    i18n( "Vietnam" ), i18n( "Western Samoa" ), i18n( "Yemen" ),
    i18n( "Yugoslavia" ), i18n( "Zaire" ), i18n( "Zambia" ),
    i18n( "Zimbabwe" ),
    ""
  };
  
  QStringList countries;
  for (int i =0; sCountry[i] != ""; ++i )
    countries.append( sCountry[i] );

  countries.sort();
  
  combo->insertStringList( countries );
}

AddressTypeDialog::AddressTypeDialog( int type, QWidget *parent )
  : KDialogBase( KDialogBase::Plain, i18n( "Edit Address Type" ),
                 KDialogBase::Ok | KDialogBase::Cancel, KDialogBase::Ok,
                 parent, "AddressTypeDialog" )
{
  QWidget *page = plainPage();
  QVBoxLayout *layout = new QVBoxLayout( page );

  mGroup = new QButtonGroup( 2, Horizontal, i18n( "Address Types" ), page );
  layout->addWidget( mGroup );

  mTypeList = KABC::Address::typeList();
  mTypeList.remove( KABC::Address::Pref );

  KABC::Address::TypeList::Iterator it;
  for ( it = mTypeList.begin(); it != mTypeList.end(); ++it )
    new QCheckBox( KABC::Address::typeLabel( *it ), mGroup );

  for ( int i = 0; i < mGroup->count(); ++i ) {
    QCheckBox *box = (QCheckBox*)mGroup->find( i );
    box->setChecked( type & mTypeList[ i ] );
  }
}    

AddressTypeDialog::~AddressTypeDialog()
{
}

int AddressTypeDialog::type()
{
  int type = 0;
  for ( int i = 0; i < mGroup->count(); ++i ) {
    QCheckBox *box = (QCheckBox*)mGroup->find( i );
    if ( box->isChecked() )
      type += mTypeList[ i ];
  }

  return type;
}

#include "addresseditwidget.moc"
