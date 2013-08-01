/* Copyright (C) 2013 Laurent Montel <montel@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "sieveeditorgraphicalmodewidget.h"
#include "sievescriptlistbox.h"
#include "scriptsparsing/parsingutil.h"

#include <KLocale>
#include <KGlobal>
#include <KConfigGroup>
#include <KMessageBox>

#include <QVBoxLayout>
#include <QSplitter>
#include <QStackedWidget>
#include <QDomDocument>
#include <QDebug>


using namespace KSieveUi;

QStringList SieveEditorGraphicalModeWidget::sCapabilities = QStringList();

SieveEditorGraphicalModeWidget::SieveEditorGraphicalModeWidget(QWidget *parent)
    : SieveEditorAbstractWidget(parent)
{
    QVBoxLayout *vlay = new QVBoxLayout;
    vlay->setMargin(0);

    mSplitter = new QSplitter;
    mSplitter->setChildrenCollapsible(false);
    mSieveScript = new SieveScriptListBox( i18n("Sieve Script"));
    connect(mSieveScript, SIGNAL(addNewPage(QWidget*)), SLOT(slotAddScriptPage(QWidget*)));
    connect(mSieveScript, SIGNAL(removePage(QWidget*)), SLOT(slotRemoveScriptPage(QWidget*)));
    connect(mSieveScript, SIGNAL(activatePage(QWidget*)), SLOT(slotActivateScriptPage(QWidget*)));
    connect(mSieveScript, SIGNAL(enableButtonOk(bool)), SIGNAL(enableButtonOk(bool)));
    mSplitter->addWidget(mSieveScript);
    vlay->addWidget(mSplitter);

    mStackWidget = new QStackedWidget;
    mSplitter->addWidget(mStackWidget);
    setLayout(vlay);
    readConfig();
}

SieveEditorGraphicalModeWidget::~SieveEditorGraphicalModeWidget()
{
    writeConfig();
}

void SieveEditorGraphicalModeWidget::loadScript(const QDomDocument &doc)
{
    for (int i = mStackWidget->count(); i>=0; --i) {
        mStackWidget->removeWidget(mStackWidget->widget(i));
    }
    mSieveScript->loadScript(doc);
}

void SieveEditorGraphicalModeWidget::readConfig()
{
    KConfigGroup group( KGlobal::config(), "AutoCreateScriptDialog" );
    QList<int> size;
    size << 100 << 400;
    mSplitter->setSizes(group.readEntry( "mainSplitter", size));
}

void SieveEditorGraphicalModeWidget::writeConfig()
{
    KConfigGroup group( KGlobal::config(), "AutoCreateScriptDialog" );
    group.writeEntry( "mainSplitter", mSplitter->sizes());
}

void SieveEditorGraphicalModeWidget::setSieveCapabilities( const QStringList &capabilities )
{
    sCapabilities = capabilities;
}

QStringList SieveEditorGraphicalModeWidget::sieveCapabilities()
{
    return sCapabilities;
}

QString SieveEditorGraphicalModeWidget::script(QString &requires) const
{
    return mSieveScript->generatedScript(requires);
}

void SieveEditorGraphicalModeWidget::slotAddScriptPage(QWidget *page)
{
    mStackWidget->addWidget(page);
    mStackWidget->setCurrentWidget(page);
}

void SieveEditorGraphicalModeWidget::slotRemoveScriptPage(QWidget *page)
{
    mStackWidget->removeWidget(page);
}

void SieveEditorGraphicalModeWidget::slotActivateScriptPage(QWidget *page)
{
    mStackWidget->setCurrentWidget(page);
}

QString SieveEditorGraphicalModeWidget::currentscript()
{
    QString requires;
    QString script = mSieveScript->generatedScript(requires);
    if (!requires.isEmpty()) {
        script.prepend(requires);
    }
    return script;
}

void SieveEditorGraphicalModeWidget::setImportScript( const QString &script )
{
    bool result = false;
    const QDomDocument doc = ParsingUtil::parseScript(script, result);
    if (result) {
        loadScript(doc);
    } else {
        if (KMessageBox::Yes == KMessageBox::questionYesNo(this, i18n("Error during importing script. Do you want to switch in text mode ?"))) {
            Q_EMIT switchTextMode(script);
        }
        qDebug()<<" can not import script";
    }
}

#include "sieveeditorgraphicalmodewidget.moc"