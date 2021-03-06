/**************************************************************************
**
** Copyright (c) 2014 BogDan Vatra <bog_dan_ro@yahoo.com>
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

#ifndef ANDROIDTOOLCHAIN_H
#define ANDROIDTOOLCHAIN_H

#include <projectexplorer/gcctoolchain.h>
#include <projectexplorer/toolchainconfigwidget.h>

namespace Android {
namespace Internal {

class AndroidToolChain : public ProjectExplorer::GccToolChain
{
public:
    ~AndroidToolChain();

    QString type() const;
    QString typeDisplayName() const;

    bool isValid() const;

    void addToEnvironment(Utils::Environment &env) const;

    bool operator ==(const ProjectExplorer::ToolChain &) const;

    ProjectExplorer::ToolChainConfigWidget *configurationWidget();
    virtual Utils::FileName suggestedDebugger() const;
    Utils::FileName suggestedGdbServer() const;

    QVariantMap toMap() const;
    bool fromMap(const QVariantMap &data);
    QList<Utils::FileName> suggestedMkspecList() const;
    QString makeCommand(const Utils::Environment &environment) const;

    QString ndkToolChainVersion() const;

    bool isSecondaryToolChain() const;
    void setSecondaryToolChain(bool b);

protected:
    QList<ProjectExplorer::Abi> detectSupportedAbis() const;

private:
    explicit AndroidToolChain(ProjectExplorer::Abi::Architecture arch, const QString &ndkToolChainVersion, Detection d);
    AndroidToolChain();
    AndroidToolChain(const AndroidToolChain &);

    QString m_ndkToolChainVersion;
    bool m_secondaryToolChain;

    friend class AndroidToolChainFactory;
};


class AndroidToolChainConfigWidget : public ProjectExplorer::ToolChainConfigWidget
{
    Q_OBJECT

public:
    AndroidToolChainConfigWidget(AndroidToolChain *);

private:
    void applyImpl() {}
    void discardImpl() {}
    bool isDirtyImpl() const { return false; }
    void makeReadOnlyImpl() {}
};


class AndroidToolChainFactory : public ProjectExplorer::ToolChainFactory
{
    Q_OBJECT

public:
    AndroidToolChainFactory();

    QList<ProjectExplorer::ToolChain *> autoDetect();
    bool canRestore(const QVariantMap &data);
    ProjectExplorer::ToolChain *restore(const QVariantMap &data);

    class AndroidToolChainInformation
    {
    public:
        Utils::FileName compilerCommand;
        ProjectExplorer::Abi::Architecture architecture;
        QString version;
    };

    static QList<ProjectExplorer::ToolChain *> createToolChainsForNdk(const Utils::FileName &ndkPath);
    static QList<AndroidToolChainInformation> toolchainPathsForNdk(const Utils::FileName &ndkPath);

    static QList<int> versionNumberFromString(const QString &version);
    static bool versionCompareLess(const QList<int> &a, const QList<int> &b);
    static bool versionCompareLess(AndroidToolChain *atc, AndroidToolChain *btc);
    static QList<int> newestToolChainVersionForArch(ProjectExplorer::Abi::Architecture arch);
private:
    static QMap<ProjectExplorer::Abi::Architecture, QList<int> > m_newestVersionForArch;
    static Utils::FileName m_ndkLocation;
};

} // namespace Internal
} // namespace Android

#endif // ANDROIDTOOLCHAIN_H
