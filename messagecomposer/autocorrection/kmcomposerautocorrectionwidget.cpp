/*
  Copyright (c) 2012 Montel Laurent <montel@kde.org>
  
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

#include "kmcomposerautocorrectionwidget.h"
#include "kmcomposerautocorrection.h"
#include "ui_kmcomposerautocorrectionwidget.h"

#include "messagecomposersettings.h"

#include <KCharSelect>


KMComposerAutoCorrectionWidget::KMComposerAutoCorrectionWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::KMComposerAutoCorrectionWidget),
  mAutoCorrection(0)
{
  ui->setupUi(this);

  ui->tableWidget->setSortingEnabled(true);
  ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);

  ui->add1->setEnabled(false);
  ui->add2->setEnabled(false);

  connect(ui->upperCase,SIGNAL(clicked()),SIGNAL(changed()));
  connect(ui->upperUpper,SIGNAL(clicked()),SIGNAL(changed()));
  connect(ui->ignoreDoubleSpace,SIGNAL(clicked()),SIGNAL(changed()));
  connect(ui->autoReplaceNumber,SIGNAL(clicked()),SIGNAL(changed()));
  connect(ui->capitalizeDaysName,SIGNAL(clicked()),SIGNAL(changed()));
  connect(ui->advancedAutocorrection,SIGNAL(clicked()),SIGNAL(changed()));
  connect(ui->enabledAutocorrection,SIGNAL(clicked()),SIGNAL(changed()));
  connect(ui->typographicSingleQuotes, SIGNAL(clicked (bool)), this, SLOT(enableSingleQuotes(bool)));
  connect(ui->typographicDoubleQuotes, SIGNAL(clicked (bool)), this, SLOT(enableDoubleQuotes(bool)));
  connect(ui->singleQuote1, SIGNAL(clicked()), this, SLOT(selectSingleQuoteCharOpen()));
  connect(ui->singleQuote2, SIGNAL(clicked()), this, SLOT(selectSingleQuoteCharClose()));
  connect(ui->singleDefault, SIGNAL(clicked()), this, SLOT(setDefaultSingleQuotes()));
  connect(ui->doubleQuote1, SIGNAL(clicked()), this, SLOT(selectDoubleQuoteCharOpen()));
  connect(ui->doubleQuote2, SIGNAL(clicked()), this, SLOT(selectDoubleQuoteCharClose()));
  connect(ui->doubleDefault, SIGNAL(clicked()), this, SLOT(setDefaultDoubleQuotes()));
  connect(ui->advancedAutocorrection, SIGNAL(clicked (bool)), this, SLOT(enableAdvAutocorrection(bool)));
  connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addAutocorrectEntry()));
  connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeAutocorrectEntry()));
  connect(ui->tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(setFindReplaceText(int,int)));
  connect(ui->find, SIGNAL(textChanged(QString)), this, SLOT(enableAddRemoveButton()));
  connect(ui->replace, SIGNAL(textChanged(QString)), this, SLOT(enableAddRemoveButton()));
  connect(ui->abbreviation, SIGNAL(textChanged(QString)), this, SLOT(abbreviationChanged(QString)));
  connect(ui->twoUpperLetter, SIGNAL(textChanged(QString)), this, SLOT(twoUpperLetterChanged(QString)));
  connect(ui->add1, SIGNAL(clicked()), this, SLOT(addAbbreviationEntry()));
  connect(ui->remove1, SIGNAL(clicked()), this, SLOT(removeAbbreviationEntry()));
  connect(ui->add2, SIGNAL(clicked()), this, SLOT(addTwoUpperLetterEntry()));
  connect(ui->remove2, SIGNAL(clicked()), this, SLOT(removeTwoUpperLetterEntry()));
  connect(ui->typographicDoubleQuotes,SIGNAL(clicked()),SIGNAL(changed()));
  connect(ui->typographicSingleQuotes,SIGNAL(clicked()),SIGNAL(changed()));

}

KMComposerAutoCorrectionWidget::~KMComposerAutoCorrectionWidget()
{
  delete ui;
}

void KMComposerAutoCorrectionWidget::setAutoCorrection(KMComposerAutoCorrection * autoCorrect)
{
  mAutoCorrection = autoCorrect;
}

void KMComposerAutoCorrectionWidget::loadConfig()
{
    if(!mAutoCorrection)
      return;

    ui->enabledAutocorrection->setChecked(mAutoCorrection->isEnabledAutoCorrection());
    ui->upperCase->setChecked(mAutoCorrection->isUppercaseFirstCharOfSentence());
    ui->upperUpper->setChecked(mAutoCorrection->isFixTwoUppercaseChars());
    ui->ignoreDoubleSpace->setChecked(mAutoCorrection->isSingleSpaces());
    ui->autoReplaceNumber->setChecked(mAutoCorrection->isAutoFractions());
    ui->capitalizeDaysName->setChecked(mAutoCorrection->isCapitalizeWeekDays());
    ui->advancedAutocorrection->setChecked(mAutoCorrection->isAdvancedAutocorrect());

    /* tab 2 - Custom Quotes */
    ui->typographicDoubleQuotes->setChecked(mAutoCorrection->isReplaceDoubleQuotes());
    ui->typographicSingleQuotes->setChecked(mAutoCorrection->isReplaceSingleQuotes());
    m_singleQuotes = mAutoCorrection->typographicSingleQuotes();
    ui->singleQuote1->setText(m_singleQuotes.begin);
    ui->singleQuote2->setText(m_singleQuotes.end);
    m_doubleQuotes = mAutoCorrection->typographicDoubleQuotes();
    ui->doubleQuote1->setText(m_doubleQuotes.begin);
    ui->doubleQuote2->setText(m_doubleQuotes.end);
    enableSingleQuotes(ui->typographicSingleQuotes->isChecked());
    enableDoubleQuotes(ui->typographicDoubleQuotes->isChecked());

    /* tab 3 - Advanced Autocorrection */
    m_autocorrectEntries = mAutoCorrection->autocorrectEntries();
    ui->tableWidget->setRowCount(m_autocorrectEntries.size());
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->verticalHeader()->hide();
    QHash<QString, QString>::const_iterator i = m_autocorrectEntries.constBegin();
    int j = 0;
    while (i != m_autocorrectEntries.constEnd()) {
        qDebug()<<" i.key() "<<i.key()<<" i.value()"<<i.value()<<" j "<<j;
        ui->tableWidget->setItem(j, 0, new QTableWidgetItem(i.key()));
        ui->tableWidget->setItem(j, 1, new QTableWidgetItem(i.value()));
        i++;
        j++;
    }
    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);

    enableAdvAutocorrection(ui->advancedAutocorrection->isChecked());
    /* tab 4 - Exceptions */
    m_upperCaseExceptions = mAutoCorrection->upperCaseExceptions();
    m_twoUpperLetterExceptions = mAutoCorrection->twoUpperLetterExceptions();
    ui->abbreviationList->addItems(m_upperCaseExceptions.toList());
}

void KMComposerAutoCorrectionWidget::writeConfig()
{
  if(!mAutoCorrection)
    return;
  mAutoCorrection->setEnabledAutoCorrection(ui->enabledAutocorrection->isChecked());
  mAutoCorrection->setUppercaseFirstCharOfSentence(ui->upperCase->isChecked());
  mAutoCorrection->setFixTwoUppercaseChars(ui->upperUpper->isChecked());
  mAutoCorrection->setSingleSpaces(ui->ignoreDoubleSpace->isChecked());
  mAutoCorrection->setCapitalizeWeekDays(ui->capitalizeDaysName->isChecked());
  mAutoCorrection->setAdvancedAutocorrect(ui->advancedAutocorrection->isChecked());

  mAutoCorrection->setAutoFractions(ui->autoReplaceNumber->isChecked());

  mAutoCorrection->setAutocorrectEntries(m_autocorrectEntries);
  mAutoCorrection->setUpperCaseExceptions(m_upperCaseExceptions);
  mAutoCorrection->setTwoUpperLetterExceptions(m_twoUpperLetterExceptions);

  mAutoCorrection->setReplaceDoubleQuotes(ui->typographicDoubleQuotes->isChecked());
  mAutoCorrection->setReplaceSingleQuotes(ui->typographicSingleQuotes->isChecked());
  mAutoCorrection->setTypographicSingleQuotes(m_singleQuotes);
  mAutoCorrection->setTypographicDoubleQuotes(m_doubleQuotes);
  mAutoCorrection->writeConfig();

}

void KMComposerAutoCorrectionWidget::resetToDefault()
{
  ui->upperCase->setChecked(false);
  ui->upperUpper->setChecked(false);
  ui->ignoreDoubleSpace->setChecked(false);
  ui->capitalizeDaysName->setChecked(false);
  ui->advancedAutocorrection->setChecked(false);
  ui->typographicDoubleQuotes->setChecked(false);
  ui->typographicSingleQuotes->setChecked(false);
}

void KMComposerAutoCorrectionWidget::enableSingleQuotes(bool state)
{
  ui->singleQuote1->setEnabled(state);
  ui->singleQuote2->setEnabled(state);
  ui->singleDefault->setEnabled(state);
}

void KMComposerAutoCorrectionWidget::enableDoubleQuotes(bool state)
{
  ui->doubleQuote1->setEnabled(state);
  ui->doubleQuote2->setEnabled(state);
  ui->doubleDefault->setEnabled(state);
}

void KMComposerAutoCorrectionWidget::selectSingleQuoteCharOpen()
{
  CharSelectDialog *dlg = new CharSelectDialog(this);
  dlg->setCurrentChar(m_singleQuotes.begin);
  if (dlg->exec()) {
    m_singleQuotes.begin = dlg->currentChar();
    ui->singleQuote1->setText(m_singleQuotes.begin);
    Q_EMIT changed();
  }
  delete dlg;
}

void KMComposerAutoCorrectionWidget::selectSingleQuoteCharClose()
{
  CharSelectDialog *dlg = new CharSelectDialog(this);
  dlg->setCurrentChar(m_singleQuotes.end);
  if (dlg->exec()) {
    m_singleQuotes.end = dlg->currentChar();
    ui->singleQuote2->setText(m_singleQuotes.end);
    Q_EMIT changed();
  }
  delete dlg;
}

void KMComposerAutoCorrectionWidget::setDefaultSingleQuotes()
{
  m_singleQuotes = mAutoCorrection->typographicDefaultSingleQuotes();
  ui->singleQuote1->setText(m_singleQuotes.begin);
  ui->singleQuote2->setText(m_singleQuotes.end);
}

void KMComposerAutoCorrectionWidget::selectDoubleQuoteCharOpen()
{
  CharSelectDialog *dlg = new CharSelectDialog(this);
  dlg->setCurrentChar(m_doubleQuotes.begin);
  if (dlg->exec()) {
    m_doubleQuotes.begin = dlg->currentChar();
    ui->doubleQuote1->setText(m_doubleQuotes.begin);
    Q_EMIT changed();
  }
  delete dlg;
}

void KMComposerAutoCorrectionWidget::selectDoubleQuoteCharClose()
{
  CharSelectDialog *dlg = new CharSelectDialog(this);
  dlg->setCurrentChar(m_doubleQuotes.end);
  if (dlg->exec()) {
    m_doubleQuotes.end = dlg->currentChar();
    ui->doubleQuote2->setText(m_doubleQuotes.end);
    Q_EMIT changed();
  }
  delete dlg;
}

void KMComposerAutoCorrectionWidget::setDefaultDoubleQuotes()
{
  m_doubleQuotes = mAutoCorrection->typographicDefaultDoubleQuotes();
  ui->doubleQuote1->setText(m_doubleQuotes.begin);
  ui->doubleQuote2->setText(m_doubleQuotes.end);
}

void KMComposerAutoCorrectionWidget::enableAdvAutocorrection(bool state)
{
  ui->findLabel->setEnabled(state);
  ui->find->setEnabled(state);
  ui->replaceLabel->setEnabled(state);
  ui->replace->setEnabled(state);
  ui->addButton->setEnabled(state);
  ui->removeButton->setEnabled(state);
  ui->tableWidget->setEnabled(state);
}


void KMComposerAutoCorrectionWidget::addAutocorrectEntry()
{
    int currentRow = ui->tableWidget->currentRow();
    QString find = ui->find->text();
    bool modify = false;

    // Modify actually, not add, so we want to remove item from hash
    if (currentRow != -1 && find == ui->tableWidget->item(currentRow, 0)->text()) {
        m_autocorrectEntries.remove(find);
        modify = true;
    }

    m_autocorrectEntries.insert(find, ui->replace->text());
    ui->tableWidget->setSortingEnabled(false);
    int size = ui->tableWidget->rowCount();

    if (modify) {
        ui->tableWidget->removeRow(currentRow);
        size--;
    }
    else
        ui->tableWidget->setRowCount(++size);

    QTableWidgetItem *item = new QTableWidgetItem(find);
    ui->tableWidget->setItem(size - 1, 0, item);
    ui->tableWidget->setItem(size - 1, 1, new QTableWidgetItem(ui->replace->text()));

    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->setCurrentCell(item->row(), 0);
    Q_EMIT changed();
}

void KMComposerAutoCorrectionWidget::removeAutocorrectEntry()
{
  ui->tableWidget->setSortingEnabled(false);
  m_autocorrectEntries.remove(ui->find->text());
  ui->tableWidget->removeRow(ui->tableWidget->currentRow());
  ui->tableWidget->setSortingEnabled(true);
  Q_EMIT changed();
}

void KMComposerAutoCorrectionWidget::enableAddRemoveButton()
{
    QString find = ui->find->text();
    QString replace = ui->replace->text();
    int currentRow = -1;
    if (m_autocorrectEntries.contains(find)) {
        currentRow = ui->tableWidget->findItems(find, Qt::MatchCaseSensitive).first()->row();
        ui->tableWidget->setCurrentCell(currentRow, 0);
    }
    else
        currentRow = ui->tableWidget->currentRow();

    bool enable = false;
    if (currentRow == -1 || find.isEmpty() || replace.isEmpty()) // disable if no text in find/replace
        enable = !(find.isEmpty() || replace.isEmpty());
    else if (find == ui->tableWidget->item(currentRow, 0)->text()) {
        // We disable add / remove button if no text for the replacement
        enable = !ui->tableWidget->item(currentRow, 1)->text().isEmpty();
        ui->addButton->setText(i18n("&Modify"));
    }
    else if (!ui->tableWidget->item(currentRow, 1)->text().isEmpty()) {
        enable = true;
        ui->addButton->setText(i18n("&Add"));
    }

    if (currentRow != -1) {
    if (replace == ui->tableWidget->item(currentRow, 1)->text())
        ui->addButton->setEnabled(false);
    else
        ui->addButton->setEnabled(enable);
    }
    ui->removeButton->setEnabled(enable);
}

void KMComposerAutoCorrectionWidget::setFindReplaceText(int row, int column)
{
  Q_UNUSED(column);
  ui->find->setText(ui->tableWidget->item(row, 0)->text());
  ui->replace->setText(ui->tableWidget->item(row, 1)->text());
  Q_EMIT changed();
}


void KMComposerAutoCorrectionWidget::abbreviationChanged(const QString &text)
{
  ui->add1->setEnabled(!text.isEmpty());
  Q_EMIT changed();
}

void KMComposerAutoCorrectionWidget::twoUpperLetterChanged(const QString &text)
{
  ui->add2->setEnabled(!text.isEmpty());
  Q_EMIT changed();
}

void KMComposerAutoCorrectionWidget::addAbbreviationEntry()
{
  QString text = ui->abbreviation->text();
  if (!m_upperCaseExceptions.contains(text)) {
    m_upperCaseExceptions.insert(text);
    ui->abbreviationList->addItem(text);
  }
  ui->abbreviation->clear();
  Q_EMIT changed();
}

void KMComposerAutoCorrectionWidget::removeAbbreviationEntry()
{
  const int currentRow = ui->abbreviationList->currentRow();
  QListWidgetItem *item = ui->abbreviationList->takeItem(currentRow);
  if(!item)
    return;
  m_upperCaseExceptions.remove(item->text());
  delete item;
  Q_EMIT changed();
}

void KMComposerAutoCorrectionWidget::addTwoUpperLetterEntry()
{
  QString text = ui->twoUpperLetter->text();
  if (!m_twoUpperLetterExceptions.contains(text)) {
    m_twoUpperLetterExceptions.insert(text);
    ui->twoUpperLetterList->addItem(text);
    Q_EMIT changed();
  }
  ui->twoUpperLetter->clear();

}

void KMComposerAutoCorrectionWidget::removeTwoUpperLetterEntry()
{
  int currentRow = ui->twoUpperLetterList->currentRow();
  QListWidgetItem *item = ui->twoUpperLetterList->takeItem(currentRow);
  if(!item)
    return;
  m_twoUpperLetterExceptions.remove(item->text());
  delete item;
  Q_EMIT changed();
}

CharSelectDialog::CharSelectDialog(QWidget *parent)
  : KDialog(parent)
{
  m_charSelect = new KCharSelect(this, 0,
          KCharSelect::FontCombo | KCharSelect::BlockCombos | KCharSelect::CharacterTable);
  setMainWidget(m_charSelect);
  setCaption(i18n("Select Character"));
}

QChar CharSelectDialog::currentChar() const
{
  return m_charSelect->currentChar();
}

void CharSelectDialog::setCurrentChar(const QChar &c)
{
  m_charSelect->setCurrentChar(c);
}

#include "kmcomposerautocorrectionwidget.moc"
