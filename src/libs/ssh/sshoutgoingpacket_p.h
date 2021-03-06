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

#ifndef SSHOUTGOINGPACKET_P_H
#define SSHOUTGOINGPACKET_P_H

#include "sshpacket_p.h"

#include "sshpseudoterminal.h"

#include <QStringList>

namespace QSsh {
namespace Internal {

class SshEncryptionFacility;

class SshOutgoingPacket : public AbstractSshPacket
{
public:
    SshOutgoingPacket(const SshEncryptionFacility &encrypter,
        const quint32 &seqNr);

    QByteArray generateKeyExchangeInitPacket(); // Returns payload.
    void generateKeyDhInitPacket(const Botan::BigInt &e);
    void generateNewKeysPacket();
    void generateDisconnectPacket(SshErrorCode reason,
        const QByteArray &reasonString);
    void generateMsgUnimplementedPacket(quint32 serverSeqNr);
    void generateUserAuthServiceRequestPacket();
    void generateUserAuthByPasswordRequestPacket(const QByteArray &user,
        const QByteArray &service, const QByteArray &pwd);
    void generateUserAuthByPublicKeyRequestPacket(const QByteArray &user,
        const QByteArray &service);
    void generateUserAuthByKeyboardInteractiveRequestPacket(const QByteArray &user,
        const QByteArray &service);
    void generateUserAuthInfoResponsePacket(const QStringList &responses);
    void generateRequestFailurePacket();
    void generateIgnorePacket();
    void generateInvalidMessagePacket();
    void generateSessionPacket(quint32 channelId, quint32 windowSize,
        quint32 maxPacketSize);
    void generateDirectTcpIpPacket(quint32 channelId, quint32 windowSize,
        quint32 maxPacketSize, const QByteArray &remoteHost, quint32 remotePort,
        const QByteArray &localIpAddress, quint32 localPort);
    void generateEnvPacket(quint32 remoteChannel, const QByteArray &var,
        const QByteArray &value);
    void generatePtyRequestPacket(quint32 remoteChannel,
        const SshPseudoTerminal &terminal);
    void generateExecPacket(quint32 remoteChannel, const QByteArray &command);
    void generateShellPacket(quint32 remoteChannel);
    void generateSftpPacket(quint32 remoteChannel);
    void generateWindowAdjustPacket(quint32 remoteChannel, quint32 bytesToAdd);
    void generateChannelDataPacket(quint32 remoteChannel,
        const QByteArray &data);
    void generateChannelSignalPacket(quint32 remoteChannel,
        const QByteArray &signalName);
    void generateChannelEofPacket(quint32 remoteChannel);
    void generateChannelClosePacket(quint32 remoteChannel);

private:
    virtual quint32 cipherBlockSize() const;
    virtual quint32 macLength() const;

    static QByteArray encodeNameList(const QList<QByteArray> &list);

    void generateServiceRequest(const QByteArray &service);

    SshOutgoingPacket &init(SshPacketType type);
    SshOutgoingPacket &setPadding();
    SshOutgoingPacket &encrypt();
    void finalize();

    SshOutgoingPacket &appendInt(quint32 val);
    SshOutgoingPacket &appendString(const QByteArray &string);
    SshOutgoingPacket &appendMpInt(const Botan::BigInt &number);
    SshOutgoingPacket &appendBool(bool b);
    int sizeDivisor() const;

    const SshEncryptionFacility &m_encrypter;
    const quint32 &m_seqNr;
};

} // namespace Internal
} // namespace QSsh

#endif // SSHOUTGOINGPACKET_P_H
