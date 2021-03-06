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

#include "cppeditortestcase.h"

#include <QByteArray>
#include <QList>
#include <QSharedPointer>
#include <QStringList>

typedef QByteArray _;

namespace CppTools { class CppCodeStylePreferences; }
namespace TextEditor { class QuickFixOperation; }

namespace CppEditor {
namespace Internal {
namespace Tests {

///
/// Represents a test document before and after applying the quick fix.
///
/// A TestDocument's source may contain an '@' character to denote
/// the cursor position. This marker is removed before the Editor reads
/// the document.
///

class QuickFixTestDocument : public TestDocument
{
public:
    typedef QSharedPointer<QuickFixTestDocument> Ptr;

    QuickFixTestDocument(const QByteArray &fileName, const QByteArray &source,
                         const QByteArray &expectedSource);

    static Ptr create(const QByteArray &fileName, const QByteArray &source,
                      const QByteArray &expectedSource);

public:
    QString m_expectedSource;
};

class BaseQuickFixTestCase : public TestCase
{
public:
    /// Exactly one QuickFixTestDocument must contain the cursor position marker '@'.
    BaseQuickFixTestCase(const QList<QuickFixTestDocument::Ptr> &testDocuments,
                         const CppTools::ProjectPart::HeaderPaths &headerPaths
                            = CppTools::ProjectPart::HeaderPaths());

    ~BaseQuickFixTestCase();

protected:
    QuickFixTestDocument::Ptr m_documentWithMarker;
    QList<QuickFixTestDocument::Ptr> m_testDocuments;

private:
    QScopedPointer<CppTools::Tests::TemporaryDir> m_temporaryDirectory;

    CppTools::CppCodeStylePreferences *m_cppCodeStylePreferences;
    QByteArray m_cppCodeStylePreferencesOriginalDelegateId;

    CppTools::ProjectPart::HeaderPaths m_headerPathsToRestore;
    bool m_restoreHeaderPaths;
};

/// Tests a concrete QuickFixOperation of a given CppQuickFixFactory
class QuickFixOperationTest : public BaseQuickFixTestCase
{
public:
    QuickFixOperationTest(const QList<QuickFixTestDocument::Ptr> &testDocuments,
                          CppQuickFixFactory *factory,
                          const CppTools::ProjectPart::HeaderPaths &headerPaths
                            = CppTools::ProjectPart::HeaderPaths(),
                          int operationIndex = 0,
                          const QByteArray &expectedFailMessage = QByteArray());

    static void run(const QList<QuickFixTestDocument::Ptr> &testDocuments,
                    CppQuickFixFactory *factory,
                    const QString &headerPath,
                    int operationIndex = 0);
};

/// Tests the offered operations provided by a given CppQuickFixFactory
class QuickFixOfferedOperationsTest : public BaseQuickFixTestCase
{
public:
    QuickFixOfferedOperationsTest(const QList<QuickFixTestDocument::Ptr> &testDocuments,
                                  CppQuickFixFactory *factory,
                                  const CppTools::ProjectPart::HeaderPaths &headerPaths
                                    = CppTools::ProjectPart::HeaderPaths(),
                                  const QStringList &expectedOperations = QStringList());
};

QList<QuickFixTestDocument::Ptr> singleDocument(const QByteArray &original,
                                                const QByteArray &expected);

} // namespace Tests
} // namespace Internal
} // namespace CppEditor
