/**************************************************************************
**
** Copyright (C) 2014 BlackBerry Limited. All rights reserved.
**
** Contact: BlackBerry (qt@blackberry.com)
** Contact: KDAB (info@kdab.com)
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

#ifndef QNX_INTERNAL_BLACKBERRYNDKPROCESS_H
#define QNX_INTERNAL_BLACKBERRYNDKPROCESS_H

#include <QObject>
#include <QProcess>
#include <QMap>

namespace Qnx {
namespace Internal {

class BlackBerryNdkProcess : public QObject
{
    Q_OBJECT

public:
    enum ProcessStatus
    {
        Success,
        FailedToStartInferiorProcess,
        InferiorProcessTimedOut,
        InferiorProcessCrashed,
        InferiorProcessWriteError,
        InferiorProcessReadError,
        UnknownError,
        UserStatus
    };

    /**
     * @brief Resolves full path to an NDK cmd-line tool.
     * @return a full-path to the NDK cmd-line tool;
     *         or empty QString when no default QNX configuration is found.
     */
    static const QString resolveNdkToolPath(const QString &tool);

signals:
    void finished(int status);
    void started();

protected:
    explicit BlackBerryNdkProcess(const QString &command, QObject *parent = 0);

    void start(const QStringList &arguments);
    void addErrorStringMapping(const QString &message, int errorCode);

    QString command() const;

private slots:
    void processFinished();
    void processError(QProcess::ProcessError error);

private:
    int errorLineToReturnStatus(const QString &line) const;
    virtual void processData(const QString &line);
    virtual void resetResults();

    QProcess *m_process;

    QString m_command;

    QMap<QString, int> m_errorStringMap;
};

}
}

#endif // QNX_INTERNAL_BLACKBERRYNDKPROCESS_H
