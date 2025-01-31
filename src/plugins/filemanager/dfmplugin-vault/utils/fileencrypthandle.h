// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VAULTHANDLE_H
#define VAULTHANDLE_H

#include "dfmplugin_vault_global.h"

#include <QObject>

DPVAULT_BEGIN_NAMESPACE

class FileEncryptHandlerPrivate;
class FileEncryptHandle : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FileEncryptHandle)

public:
    static FileEncryptHandle *instance();

    void createVault(QString lockBaseDir, QString unlockFileDir, QString DSecureString, EncryptType type = EncryptType::AES_256_GCM, int blockSize = 32768);
    int unlockVault(QString lockBaseDir, QString unlockFileDir, QString DSecureString);
    void lockVault(QString unlockFileDir, bool isForced);
    void createDirIfNotExist(QString path);
    VaultState state(const QString &encryptBaseDir, const QString &decryptFileDir) const;

    EncryptType encryptAlgoTypeOfGroupPolicy();
signals:
    void signalReadError(QString error);
    void signalReadOutput(QString msg);
    void signalCreateVault(int state);
    void signalUnlockVault(int state);
    void signalLockVault(int state);

public slots:
    void slotReadError();
    void slotReadOutput();

private:
    explicit FileEncryptHandle(QObject *parent = nullptr);
    virtual ~FileEncryptHandle() override;

private:
    FileEncryptHandlerPrivate *d = nullptr;
};

DPVAULT_END_NAMESPACE

#endif   // VAULTHANDLE_H
