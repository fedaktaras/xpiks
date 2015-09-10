/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "uploadinforepository.h"
#include <QDebug>
#include "uploadinfo.h"
#include "../Commands/commandmanager.h"
#include "../Encryption/secretsmanager.h"

namespace Models {
    UploadInfoRepository::~UploadInfoRepository() { qDeleteAll(m_UploadInfos); m_UploadInfos.clear();  }

    void UploadInfoRepository::initFromString(const QString &savedString)
    {
        QByteArray originalData;
        originalData.append(savedString.toLatin1());
        QByteArray result = QByteArray::fromBase64(originalData);

        QList<QHash<int, QString> > items;
        QDataStream stream(&result, QIODevice::ReadOnly);
        stream >> items;

        int length = items.length();
        for (int i = 0; i < length; ++i) {
            QHash<int, QString> hash = items[i];
            UploadInfo *info = new UploadInfo(hash);
            m_UploadInfos.append(info);
        }
    }

    void UploadInfoRepository::addItem() {
        int lastIndex = m_UploadInfos.length();
        beginInsertRows(QModelIndex(), lastIndex, lastIndex);
        m_UploadInfos.append(new UploadInfo());
        endInsertRows();
        emit infosCountChanged();
    }

    QString UploadInfoRepository::getInfoString() const {
        QList<QHash<int, QString> > items;
        foreach (UploadInfo *info, m_UploadInfos) {
            if (!info->isEmpty()) {
                items.append(info->toHash());
            }
        }

        // TODO: move to SFTP
        // while stocks use FTP, sophisticated passwords
        // saving on client side is useless
        QByteArray result;
        QDataStream stream(&result, QIODevice::WriteOnly);
        stream << items;
        return QString::fromUtf8(result.toBase64());
    }

    int UploadInfoRepository::getSelectedInfosCount() const {
        int count = 0;
        foreach (UploadInfo *info, m_UploadInfos) {
            if (info->getIsSelected()) count++;
        }
        return count;
    }

    QString UploadInfoRepository::getAgenciesWithMissingDetails()
    {
        QStringList items;
        foreach (UploadInfo *info, m_UploadInfos) {
            if (info->getIsSelected() && info->isSomethingMissing()) {
                items.append(info->getTitle());
            }
        }

        return items.join(", ");
    }

    void UploadInfoRepository::updateProperties(int itemIndex) {
        QModelIndex itemModelIndex = index(itemIndex);
        // could be any role - just to triger Advanced tab checkbox connections
        emit dataChanged(itemModelIndex, itemModelIndex, QVector<int>() << ZipBeforeUploadRole);
    }

    // mp == master password
    void UploadInfoRepository::initializeAccounts(bool mpIsCorrectOrEmpty)
    {
        this->setEmptyPasswordsMode(!mpIsCorrectOrEmpty);
        if (!mpIsCorrectOrEmpty) {
            this->backupAndDropRealPasswords();
        }
    }

    void UploadInfoRepository::finalizeAccounts()
    {
        if (m_EmptyPasswordsMode) {
            this->restoreRealPasswords();
        }
    }

    void UploadInfoRepository::backupAndDropRealPasswords() {
        foreach (UploadInfo *info, m_UploadInfos) {
            info->backupPassword();
            info->dropPassword();
        }
    }

    void UploadInfoRepository::restoreRealPasswords() {
        foreach (UploadInfo *info, m_UploadInfos) {
            info->restorePassword();
        }
    }

    void UploadInfoRepository::updatePercentages() {
        emit dataChanged(index(0), index(m_UploadInfos.count() - 1), QVector<int>() << PercentRole);
    }

    void UploadInfoRepository::resetPercents() { foreach (UploadInfo *info, m_UploadInfos) { info->resetPercent(); } }

    int UploadInfoRepository::rowCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent);
        return m_UploadInfos.count();
    }

    QVariant UploadInfoRepository::data(const QModelIndex &index, int role) const
    {
        if (index.row() < 0 || index.row() >= m_UploadInfos.count())
            return QVariant();

        UploadInfo *uploadInfo = m_UploadInfos[index.row()];

        switch (role) {
        case TitleRole:
            return uploadInfo->getTitle();
        case HostRole:
            return uploadInfo->getHost();
        case UsernameRole:
            return uploadInfo->getUsername();
        case PasswordRole: {
            const QString &encodedPassword = uploadInfo->getPassword();
            QString password = m_CommandManager->getSecretsManager()->decodePassword(encodedPassword);
            return password;
        }
        case IsSelectedRole:
            return uploadInfo->getIsSelected();
        case ZipBeforeUploadRole:
            return uploadInfo->getZipBeforeUpload();
        case PercentRole:
            return uploadInfo->getPercent();
        case FtpPassiveModeRole:
            return uploadInfo->getFtpPassiveMode();
        default:
            return QVariant();
        }
    }

    Qt::ItemFlags UploadInfoRepository::flags(const QModelIndex &index) const
    {
        if (!index.isValid()) {
            return Qt::ItemIsEnabled;
        }

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }

    bool UploadInfoRepository::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!index.isValid()) {
            return false;
        }

        UploadInfo *uploadInfo = m_UploadInfos[index.row()];
        QString title;
        int roleToUpdate = 0;
        bool needToUpdate = false;
        switch (role) {
        case EditTitleRole:
            roleToUpdate = TitleRole;
            title = value.toString();
            needToUpdate = uploadInfo->setTitle(title);
            // hack for updating checkbox binding
            if (!needToUpdate) {
                needToUpdate = true;
                roleToUpdate = ZipBeforeUploadRole;
            }
            break;
        case EditHostRole:
            roleToUpdate = HostRole;
            needToUpdate = uploadInfo->setHost(value.toString().trimmed());
            break;
        case EditUsernameRole:
            roleToUpdate = UsernameRole;
            needToUpdate = uploadInfo->setUsername(value.toString().trimmed());
            break;
        case EditPasswordRole: {
            roleToUpdate = PasswordRole;
            QString rawPassword = value.toString();
            QString encodedPassword = m_CommandManager->getSecretsManager()->encodePassword(rawPassword);
            // skip needUpdate
            uploadInfo->setPassword(encodedPassword);
            break;
        }
        case EditIsSelectedRole:
            roleToUpdate = IsSelectedRole;
            needToUpdate = uploadInfo->setIsSelected(value.toBool());
            break;
        case EditZipBeforeUploadRole:
            roleToUpdate = ZipBeforeUploadRole;
            needToUpdate = uploadInfo->setZipBeforeUpload(value.toBool());
            break;
        case EditFtpPassiveModeRole:
            roleToUpdate = FtpPassiveModeRole;
            needToUpdate = uploadInfo->setFtpPassiveMode(value.toBool());
        default:
            return false;
        }

        if (needToUpdate) {
            emit dataChanged(index, index, QVector<int>() << roleToUpdate);
        }

        return true;
    }

    void UploadInfoRepository::onBeforeMasterPasswordChanged(const QString &oldMasterPassword,
                                                             const QString &newMasterPassword) {
        m_CommandManager->recodePasswords(oldMasterPassword, newMasterPassword, m_UploadInfos);
    }

    void UploadInfoRepository::onAfterMasterPasswordReset() {
        foreach (UploadInfo *info, m_UploadInfos) {
            info->dropPassword();
        }
    }

    QHash<int, QByteArray> UploadInfoRepository::roleNames() const
    {
        QHash<int, QByteArray> roles;
        roles[TitleRole] = "title";
        roles[HostRole] = "host";
        roles[UsernameRole] = "username";
        roles[PasswordRole] = "password";
        roles[EditTitleRole] = "edittitle";
        roles[EditHostRole] = "edithost";
        roles[EditUsernameRole] = "editusername";
        roles[EditPasswordRole] = "editpassword";
        roles[IsSelectedRole] = "isselected";
        roles[EditIsSelectedRole] = "editisselected";
        roles[ZipBeforeUploadRole] = "zipbeforeupload";
        roles[EditZipBeforeUploadRole] = "editzipbeforeupload";
        roles[PercentRole] = "percent";
        roles[FtpPassiveModeRole] = "ftppassivemode";
        roles[EditFtpPassiveModeRole] = "editftppassivemode";
        return roles;
    }

    void UploadInfoRepository::removeInnerItem(int row) {
        UploadInfo *info = m_UploadInfos.takeAt(row);
        delete info;
    }
}
