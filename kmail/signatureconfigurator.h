/*  -*- c++ -*-
    signatureconfigurator.cpp

    KMail, the KDE mail client.
    Copyright (c) 2002 the KMail authors.
    See file AUTHORS for details

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License,
    version 2.0, as published by the Free Software Foundation.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, US
*/

#ifndef __KMAIL_SIGNATURECONFIGURATOR_H__
#define __KMAIL_SIGNATURECONFIGURATOR_H__

#include <QWidget>

#include <kpimidentities/identity.h> // for Signature::Type
using KPIMIdentities::Signature;

class QComboBox;
class QCheckBox;
class KUrlRequester;
class KLineEdit;
class QString;
class QPushButton;
class QTextEdit;
class QTextCharFormat;

namespace KMail {

  class SignatureConfigurator : public QWidget {
    Q_OBJECT
  public:
    SignatureConfigurator( QWidget * parent=0 );
    virtual ~SignatureConfigurator();

    enum ViewMode { ShowCode, ShowHtml };

    bool isSignatureEnabled() const;
    void setSignatureEnabled( bool enable );

    Signature::Type signatureType() const;
    void setSignatureType( Signature::Type type );

    QString inlineText() const;
    void setInlineText( const QString & text );

    QString fileURL() const;
    void setFileURL( const QString & url );

    QString commandURL() const;
    void setCommandURL( const QString & url );

    /**
       Conveniece method.
       @return a Signature object representing the state of the widgets.
     **/
    Signature signature() const;
    /**
       Convenience method. Sets the widgets according to @p sig
    **/
    void setSignature( const Signature & sig );

  private:
    void toggleHtmlBtnState( ViewMode state );

    void initHtmlState();

    // Returns the current text of the textedit as HTML code, but strips
    // unnecessary tags Qt inserts
    QString asCleanedHTML() const;

  protected slots:
    void slotEnableEditButton( const QString & );
    void slotEdit();
    void slotShowCodeOrHtml();
    void slotSetHtml();

  protected:
    QCheckBox     * mEnableCheck;
    QCheckBox     * mHtmlCheck;
    QComboBox     * mSourceCombo;
    KUrlRequester * mFileRequester;
    QPushButton   * mEditButton;
    QPushButton   * mShowCodeOrHtmlBtn;
    KLineEdit     * mCommandEdit;
    QTextEdit     * mTextEdit;

  private:
    bool mInlinedHtml;
    ViewMode  mViewMode;
  };

} // namespace KMail

#endif // __KMAIL_SIGNATURECONFIGURATOR_H__


