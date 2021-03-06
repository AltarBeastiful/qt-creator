/****************************************************************************
**
** Copyright (C) 2014 Petar Perisin <petar.perisin@gmail.com>
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

#include "ansiescapecodehandler.h"
#include <utils/qtcassert.h>

namespace Utils {

/*!
    \class Utils::AnsiEscapeCodeHandler

    \brief The AnsiEscapeCodeHandler class parses text and extracts ANSI escape codes from it.

    In order to preserve color information across text segments, an instance of this class
    must be stored for the lifetime of a stream.
    Also, one instance of this class should not handle multiple streams (at least not
    at the same time).

    Its main function is parseText(), which accepts text and default QTextCharFormat.
    This function is designed to parse text and split colored text to smaller strings,
    with their appropriate formatting information set inside QTextCharFormat.

    Usage:
    \list
    \li Create new instance of AnsiEscapeCodeHandler for a stream.
    \li To add new text, call parseText() with the text and a default QTextCharFormat.
        The result of this function is a list of strings with formats set in appropriate
        QTextCharFormat.
    \endlist
*/

AnsiEscapeCodeHandler::AnsiEscapeCodeHandler() :
    m_previousFormatClosed(true)
{
}

static QColor ansiColor(uint code)
{
    QTC_ASSERT(code < 8, return QColor());

    const int red   = code & 1 ? 170 : 0;
    const int green = code & 2 ? 170 : 0;
    const int blue  = code & 4 ? 170 : 0;
    return QColor(red, green, blue);
}

QList<FormattedText> AnsiEscapeCodeHandler::parseText(const FormattedText &input)
{
    enum AnsiEscapeCodes {
        ResetFormat            =  0,
        BoldText               =  1,
        TextColorStart         = 30,
        TextColorEnd           = 37,
        RgbTextColor           = 38,
        DefaultTextColor       = 39,
        BackgroundColorStart   = 40,
        BackgroundColorEnd     = 47,
        RgbBackgroundColor     = 48,
        DefaultBackgroundColor = 49
    };

    QList<FormattedText> outputData;

    QTextCharFormat charFormat = m_previousFormatClosed ? input.format : m_previousFormat;

    const QString escape = QLatin1String("\x1b[");
    const int escapePos = input.text.indexOf(escape);
    if (escapePos < 0) {
        outputData << FormattedText(input.text, charFormat);
        return outputData;
    } else if (escapePos != 0) {
        outputData << FormattedText(input.text.left(escapePos), charFormat);
    }

    const QChar semicolon       = QLatin1Char(';');
    const QChar colorTerminator = QLatin1Char('m');
    const QChar eraseToEol      = QLatin1Char('K');
    // strippedText always starts with "\e["
    QString strippedText = input.text.mid(escapePos);
    while (!strippedText.isEmpty()) {
        while (strippedText.startsWith(escape)) {
            strippedText.remove(0, 2);

            // \e[K is not supported. Just strip it.
            if (strippedText.startsWith(eraseToEol)) {
                strippedText.remove(0, 1);
                continue;
            }
            // get the number
            QString strNumber;
            QStringList numbers;
            while (strippedText.at(0).isDigit() || strippedText.at(0) == semicolon) {
                if (strippedText.at(0).isDigit()) {
                    strNumber += strippedText.at(0);
                } else {
                    numbers << strNumber;
                    strNumber.clear();
                }
                strippedText.remove(0, 1);
            }
            if (!strNumber.isEmpty())
                numbers << strNumber;

            // remove terminating char
            if (!strippedText.startsWith(colorTerminator)) {
                strippedText.remove(0, 1);
                continue;
            }
            strippedText.remove(0, 1);

            if (numbers.isEmpty()) {
                charFormat = input.format;
                endFormatScope();
            }

            for (int i = 0; i < numbers.size(); ++i) {
                const int code = numbers.at(i).toInt();

                if (code >= TextColorStart && code <= TextColorEnd) {
                    charFormat.setForeground(ansiColor(code - TextColorStart));
                    setFormatScope(charFormat);
                } else if (code >= BackgroundColorStart && code <= BackgroundColorEnd) {
                    charFormat.setBackground(ansiColor(code - BackgroundColorStart));
                    setFormatScope(charFormat);
                } else {
                    switch (code) {
                    case ResetFormat:
                        charFormat = input.format;
                        endFormatScope();
                        break;
                    case BoldText:
                        charFormat.setFontWeight(QFont::Bold);
                        setFormatScope(charFormat);
                        break;
                    case DefaultTextColor:
                        charFormat.setForeground(input.format.foreground());
                        setFormatScope(charFormat);
                        break;
                    case DefaultBackgroundColor:
                        charFormat.setBackground(input.format.background());
                        setFormatScope(charFormat);
                        break;
                    case RgbTextColor:
                    case RgbBackgroundColor:
                        // See http://en.wikipedia.org/wiki/ANSI_escape_code#Colors
                        if (++i >= numbers.size())
                            break;
                        switch (numbers.at(i).toInt()) {
                        case 2:
                            // RGB set with format: 38;2;<r>;<g>;<b>
                            if ((i + 3) < numbers.size()) {
                                (code == RgbTextColor) ?
                                      charFormat.setForeground(QColor(numbers.at(i + 1).toInt(),
                                                                      numbers.at(i + 2).toInt(),
                                                                      numbers.at(i + 3).toInt())) :
                                      charFormat.setBackground(QColor(numbers.at(i + 1).toInt(),
                                                                      numbers.at(i + 2).toInt(),
                                                                      numbers.at(i + 3).toInt()));
                                setFormatScope(charFormat);
                            }
                            i += 3;
                            break;
                        case 5:
                            // 256 color mode with format: 38;5;<i>
                            uint index = numbers.at(i + 1).toInt();

                            QColor color;
                            if (index < 8) {
                                // The first 8 colors are standard low-intensity ANSI colors.
                                color = ansiColor(index);
                            } else if (index < 16) {
                                // The next 8 colors are standard high-intensity ANSI colors.
                                color = ansiColor(index - 8).lighter(150);
                            } else if (index < 232) {
                                // The next 216 colors are a 6x6x6 RGB cube.
                                uint o = index - 16;
                                color = QColor((o / 36) * 51, ((o / 6) % 6) * 51, (o % 6) * 51);
                            } else {
                                // The last 24 colors are a greyscale gradient.
                                uint grey = (index - 232) * 11;
                                color = QColor(grey, grey, grey);
                            }

                            if (code == RgbTextColor)
                                charFormat.setForeground(color);
                            else
                                charFormat.setBackground(color);

                            setFormatScope(charFormat);
                            ++i;
                            break;
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        if (strippedText.isEmpty())
            break;
        int index = strippedText.indexOf(escape);
        if (index > 0) {
            outputData << FormattedText(strippedText.left(index), charFormat);
            strippedText.remove(0, index);
        } else if (index == -1) {
            outputData << FormattedText(strippedText, charFormat);
            break;
        }
    }
    return outputData;
}

void AnsiEscapeCodeHandler::endFormatScope()
{
    m_previousFormatClosed = true;
}

void AnsiEscapeCodeHandler::setFormatScope(const QTextCharFormat &charFormat)
{
    m_previousFormat = charFormat;
    m_previousFormatClosed = false;
}

} // namespace Utils
