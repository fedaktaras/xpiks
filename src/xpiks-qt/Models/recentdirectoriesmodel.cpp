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

#include "recentdirectoriesmodel.h"
#include <QDataStream>

namespace Models {
    RecentDirectoriesModel::RecentDirectoriesModel():
        QAbstractListModel(),
        m_MaxRecentDirectories(5)
    {
    }

    QString RecentDirectoriesModel::serializeForSettings() {
        QByteArray raw;
        QDataStream ds(&raw, QIODevice::WriteOnly);
        ds << m_RecentDirectories;
        return raw.toBase64();
    }

    void RecentDirectoriesModel::deserializeFromSettings(const QString &serialized) {
        QByteArray originalData;
        originalData.append(serialized.toLatin1());
        QByteArray serializedBA = QByteArray::fromBase64(originalData);

        QQueue<QString> items;
        QDataStream ds(&serializedBA, QIODevice::ReadOnly);
        ds >> items;

        foreach (const QString &item, items) {
            this->pushDirectory(item);
        }
    }

    void RecentDirectoriesModel::pushDirectory(const QString &directoryPath) {
        if (!m_DirectoriesSet.contains(directoryPath)) {
            m_DirectoriesSet.insert(directoryPath);

            int length = m_RecentDirectories.length();
            beginInsertRows(QModelIndex(), length, length);
            m_RecentDirectories.push_back(directoryPath);
            endInsertRows();

            if (m_RecentDirectories.length() > m_MaxRecentDirectories) {
                QString directoryToRemove = m_RecentDirectories.first();
                beginRemoveRows(QModelIndex(), 0, 0);
                m_RecentDirectories.pop_front();
                endRemoveRows();
                m_DirectoriesSet.remove(directoryToRemove);
            }
        }
    }

    QVariant RecentDirectoriesModel::data(const QModelIndex &index, int role) const {
        if (!index.isValid()) return QVariant();
        if (role == Qt::DisplayRole) { return m_RecentDirectories.at(index.row()); }
        return QVariant();
    }
}

