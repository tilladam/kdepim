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

#include "textrulerwidgethandler.h"
#include <pimcommon/widgets/minimumcombobox.h>

#include "search/searchpattern.h"
#include "widgets/regexplineedit.h"
using MailCommon::RegExpLineEdit;

#include <KDebug>
#include <KLocale>

#include <QLineEdit>
#include <QStackedWidget>


using namespace MailCommon;

#include <QLabel>

// also see SearchRule::matches() and SearchRule::Function
// if you change the following strings!
static const struct {
    SearchRule::Function id;
    const char *displayName;
} TextFunctions[] = {
    { SearchRule::FuncContains,           I18N_NOOP( "contains" )          },
    { SearchRule::FuncContainsNot,        I18N_NOOP( "does not contain" )   },
    { SearchRule::FuncEquals,             I18N_NOOP( "equals" )            },
    { SearchRule::FuncNotEqual,           I18N_NOOP( "does not equal" )     },
    { SearchRule::FuncStartWith,          I18N_NOOP( "starts with" )         },
    { SearchRule::FuncNotStartWith,       I18N_NOOP( "does not start with" )},
    { SearchRule::FuncEndWith,            I18N_NOOP( "ends with" )           },
    { SearchRule::FuncNotEndWith,         I18N_NOOP( "does not end with" )  },

    { SearchRule::FuncRegExp,             I18N_NOOP( "matches regular expr." ) },
    { SearchRule::FuncNotRegExp,          I18N_NOOP( "does not match reg. expr." ) }
};
static const int TextFunctionCount =
        sizeof( TextFunctions ) / sizeof( *TextFunctions );

//---------------------------------------------------------------------------

QWidget *TextRuleWidgetHandler::createFunctionWidget(
        int number, QStackedWidget *functionStack, const QObject *receiver, bool /*isBalooSearch*/ ) const
{
    if ( number != 0 ) {
        return 0;
    }

    PimCommon::MinimumComboBox *funcCombo = new PimCommon::MinimumComboBox( functionStack );
    funcCombo->setObjectName( QLatin1String("textRuleFuncCombo") );
    for ( int i = 0; i < TextFunctionCount; ++i ) {
        funcCombo->addItem( i18n( TextFunctions[i].displayName ) );
    }
    funcCombo->adjustSize();
    QObject::connect( funcCombo, SIGNAL(activated(int)),
                      receiver, SLOT(slotFunctionChanged()) );
    return funcCombo;
}

//---------------------------------------------------------------------------

QWidget *TextRuleWidgetHandler::createValueWidget( int number,
                                                   QStackedWidget *valueStack,
                                                   const QObject *receiver ) const
{
    if ( number == 0 ) {
        RegExpLineEdit *lineEdit = new RegExpLineEdit( valueStack );
        lineEdit->setObjectName( QLatin1String("regExpLineEdit") );
        QObject::connect( lineEdit, SIGNAL(textChanged(QString)),
                          receiver, SLOT(slotValueChanged()) );
        QObject::connect( lineEdit, SIGNAL(returnPressed()),
                          receiver, SLOT(slotReturnPressed()) );
        return lineEdit;
    }

    // blank QLabel to hide value widget for in-address-book rule
    if ( number == 1 ) {
        QLabel *label = new QLabel( valueStack );
        label->setObjectName( QLatin1String("textRuleValueHider") );
        label->setBuddy( valueStack );
        return label;
    }
    return 0;
}

//---------------------------------------------------------------------------

SearchRule::Function TextRuleWidgetHandler::currentFunction(
        const QStackedWidget *functionStack ) const
{
    const PimCommon::MinimumComboBox *funcCombo =
            functionStack->findChild<PimCommon::MinimumComboBox*>( QLatin1String( "textRuleFuncCombo" ) );

    if ( funcCombo && funcCombo->currentIndex() >= 0 ) {
        return TextFunctions[funcCombo->currentIndex()].id;
    }

    return SearchRule::FuncNone;
}

//---------------------------------------------------------------------------

SearchRule::Function TextRuleWidgetHandler::function( const QByteArray &,
                                                      const QStackedWidget *functionStack ) const
{
    return currentFunction( functionStack );
}

//---------------------------------------------------------------------------

QString TextRuleWidgetHandler::currentValue( const QStackedWidget *valueStack,
                                             SearchRule::Function ) const
{
    //in other cases of func it is a lineedit
    const RegExpLineEdit *lineEdit = valueStack->findChild<RegExpLineEdit*>( QLatin1String("regExpLineEdit") );

    if ( lineEdit ) {
        return lineEdit->text();
    }

    // or anything else, like addressbook
    return QString();
}

//---------------------------------------------------------------------------

QString TextRuleWidgetHandler::value( const QByteArray &,
                                      const QStackedWidget *functionStack,
                                      const QStackedWidget *valueStack ) const
{
    SearchRule::Function func = currentFunction( functionStack );
    return currentValue( valueStack, func );
}

//---------------------------------------------------------------------------

QString TextRuleWidgetHandler::prettyValue( const QByteArray &,
                                            const QStackedWidget *functionStack,
                                            const QStackedWidget *valueStack ) const
{
    SearchRule::Function func = currentFunction( functionStack );    
    return currentValue( valueStack, func );
}

//---------------------------------------------------------------------------

bool TextRuleWidgetHandler::handlesField( const QByteArray & ) const
{
    return true; // we handle all fields (as fallback)
}

//---------------------------------------------------------------------------

void TextRuleWidgetHandler::reset( QStackedWidget *functionStack,
                                   QStackedWidget *valueStack ) const
{
    // reset the function combo box
    PimCommon::MinimumComboBox *funcCombo = functionStack->findChild<PimCommon::MinimumComboBox*>( QLatin1String("textRuleFuncCombo") );

    if ( funcCombo ) {
        funcCombo->blockSignals( true );
        funcCombo->setCurrentIndex( 0 );
        funcCombo->blockSignals( false );
    }

    // reset the value widget
    RegExpLineEdit *lineEdit = valueStack->findChild<RegExpLineEdit*>( QLatin1String("regExpLineEdit") );
    if ( lineEdit ) {
        lineEdit->blockSignals( true );
        lineEdit->clear();
        lineEdit->blockSignals( false );
        lineEdit->showEditButton( false );
        valueStack->setCurrentWidget( lineEdit );
    }
}

//---------------------------------------------------------------------------

bool TextRuleWidgetHandler::setRule( QStackedWidget *functionStack,
                                     QStackedWidget *valueStack,
                                     const SearchRule::Ptr rule, bool /*isBalooSearch*/ ) const
{
    if ( !rule ) {
        reset( functionStack, valueStack );
        return false;
    }

    const SearchRule::Function func = rule->function();
    int i = 0;
    for ( ; i < TextFunctionCount; ++i ) {
        if ( func == TextFunctions[i].id ) {
            break;
        }
    }

    PimCommon::MinimumComboBox *funcCombo = functionStack->findChild<PimCommon::MinimumComboBox*>( QLatin1String("textRuleFuncCombo") );

    if ( funcCombo ) {
        funcCombo->blockSignals( true );
        if ( i < TextFunctionCount ) {
            funcCombo->setCurrentIndex( i );
        } else {
            funcCombo->setCurrentIndex( 0 );
        }
        funcCombo->blockSignals( false );
        functionStack->setCurrentWidget( funcCombo );
    }
    RegExpLineEdit *lineEdit =
            valueStack->findChild<RegExpLineEdit*>( QLatin1String("regExpLineEdit") );

    if ( lineEdit ) {
        lineEdit->blockSignals( true );
        lineEdit->setText( rule->contents() );
        lineEdit->blockSignals( false );
        lineEdit->showEditButton( func == SearchRule::FuncRegExp ||
                                  func == SearchRule::FuncNotRegExp );
        valueStack->setCurrentWidget( lineEdit );
    }
    return true;
}

//---------------------------------------------------------------------------

bool TextRuleWidgetHandler::update( const QByteArray &,
                                    QStackedWidget *functionStack,
                                    QStackedWidget *valueStack ) const
{
    // raise the correct function widget
    functionStack->setCurrentWidget( functionStack->findChild<QWidget*>( QLatin1String("textRuleFuncCombo") ) );

    // raise the correct value widget
    SearchRule::Function func = currentFunction( functionStack );
    if ( func == SearchRule::FuncIsInAddressbook ||
         func == SearchRule::FuncIsNotInAddressbook ) {
        valueStack->setCurrentWidget( valueStack->findChild<QWidget*>( QLatin1String("textRuleValueHider") ) );
    } else {
        RegExpLineEdit *lineEdit =
                valueStack->findChild<RegExpLineEdit*>( QLatin1String("regExpLineEdit") );

        if ( lineEdit ) {
            lineEdit->showEditButton( func == SearchRule::FuncRegExp ||
                                      func == SearchRule::FuncNotRegExp );
            valueStack->setCurrentWidget( lineEdit );
        }
    }
    return true;
}

