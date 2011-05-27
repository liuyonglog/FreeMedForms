/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef USERMANAGERPLUGIN_H
#define USERMANAGERPLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtCore/QObject>
#include <QPointer>
class QAction;

/**
 * \file usermanagerplugin.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 11 May 2011
*/

namespace UserPlugin {
class UserManagerDialog;
class FirstRun_UserConnection;
class FirstRun_UserCreation;
namespace Internal {
class UserManagerMode;
}

class UserManagerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    UserManagerPlugin();
    ~UserManagerPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();

private Q_SLOTS:
    void postCoreInitialization();
    void createUser();
    void changeCurrentUser();
    void updateActions();

private:
    QAction *aCreateUser;
    QAction *aChangeUser;

    FirstRun_UserConnection *m_First_Connection;
    FirstRun_UserCreation *m_FirstCreation;

    Internal::UserManagerMode *m_Mode;
};


}  // End UserPlugin

#endif  // End USERMANAGERPLUGIN_H
