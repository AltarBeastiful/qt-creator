/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://www.qt.io/licensing.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "progressmanager_p.h"
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QStringList>
#include <QApplication>

namespace {
namespace Unity {

static int max = 0;
static int min = 0;
static bool isSupported = false;

static inline void sendMessage(const QVariantMap &map)
{
    QDBusMessage message = QDBusMessage::createSignal(QLatin1String("/qtcreator"), QLatin1String("com.canonical.Unity.LauncherEntry"), QLatin1String("Update"));
    QVariantList args;
    args << QLatin1String("application://qtcreator-qt-creator.desktop")
         << map;
    message.setArguments(args);
    if (!QDBusConnection::sessionBus().send(message))
        qWarning("Unable to send message");
}

template <typename T>
static void sendMessage(const char *name, const T &value)
{
    QVariantMap map;
    map.insert(QLatin1String(name), value);
    sendMessage(map);
}

} //namespace Unity
}

void Core::Internal::ProgressManagerPrivate::initInternal()
{
    QDBusConnection connection = QDBusConnection::sessionBus();
    QStringList services = connection.interface()->registeredServiceNames().value();
    Unity::isSupported = services.contains(QLatin1String("com.canonical.Unity"));
}

void Core::Internal::ProgressManagerPrivate::cleanup()
{
}

void Core::Internal::ProgressManagerPrivate::doSetApplicationLabel(const QString &text)
{
    if (Unity::isSupported) {
        qint64 count = text.toInt();
        QVariantMap map;
        map.insert(QLatin1String("count"), count);
        map.insert(QLatin1String("count-visible"), count > 0);
        Unity::sendMessage(map);
    }
}

void Core::Internal::ProgressManagerPrivate::setApplicationProgressRange(int min, int max)
{
    Unity::min = min;
    Unity::max = max;
}

void Core::Internal::ProgressManagerPrivate::setApplicationProgressValue(int value)
{
    if (Unity::isSupported) {
        int total = Unity::max-Unity::min;
        double percents = 0;
        if (total)
            percents = double(value-Unity::min)/double(total);
        Unity::sendMessage("progress", percents);
    }
}

void Core::Internal::ProgressManagerPrivate::setApplicationProgressVisible(bool visible)
{
    if (Unity::isSupported)
        Unity::sendMessage("progress-visible", visible);
}
