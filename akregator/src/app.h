/***************************************************************************
 *   Copyright (C) 2004 by Sashmit Bhaduri                                 *
 *   smt@vfemail.net                                                       *
 *                                                                         *
 *   Licensed under GPL.                                                   *
 ***************************************************************************/

#ifndef APP_H
#define APP_H

#include <kuniqueapplication.h>

#define akreapp (static_cast<aKregatorApp*>(kapp))

class aKregatorApp : public KUniqueApplication {
    Q_OBJECT
    public:
        aKregatorApp();
        ~aKregatorApp();
        void setHaveWindowLoaded(bool b);
        bool haveWindowLoaded();
    private:
        bool m_haveWindowLoaded;
};


#endif

