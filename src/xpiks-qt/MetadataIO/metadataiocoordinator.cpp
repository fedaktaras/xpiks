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

#include "metadataiocoordinator.h"
#include <QThread>
#include <QDebug>
#include <QHash>
#include "metadatareadingworker.h"
#include "backupsaverservice.h"
#include "../Models/artworkmetadata.h"
#include "../Models/settingsmodel.h"
#include "../Commands/commandmanager.h"
#include "../Suggestion/locallibrary.h"

namespace MetadataIO {
    MetadataIOCoordinator::MetadataIOCoordinator():
        Common::BaseEntity()
    {
    }

    void MetadataIOCoordinator::workerFinished(bool success) {
        qDebug() << "Metadata reading finished with status" << success;

        const QHash<QString, ImportDataResult> &importResult = m_ReadingWorker->getImportResult();
        const QVector<Models::ArtworkMetadata*> &itemsToRead = m_ReadingWorker->getArtworksToImport();

        qDebug() << "Setting imported metadata";
        int size = itemsToRead.size();
        for (int i = 0; i < size; ++i) {
            Models::ArtworkMetadata *metadata = itemsToRead.at(i);
            const QString &filepath = metadata->getFilepath();

            if (importResult.contains(filepath)) {
                const ImportDataResult &importResultItem = importResult.value(filepath);
                metadata->initialize(importResultItem.Title,
                                     importResultItem.Description,
                                     importResultItem.Keywords);
            }
        }

        afterImportHandler(itemsToRead, m_ReadingWorker->getIgnoreBackups());

        qDebug() << "Metadata import finished";
        m_ReadingWorker->shutdown();

        emit metadataReadingFinished();
    }

    void MetadataIOCoordinator::readMetadata(const QVector<Models::ArtworkMetadata *> &artworksToRead, bool ignoreBackups) {
        m_ReadingWorker = new MetadataReadingWorker(artworksToRead,
                                                    m_CommandManager->getSettingsModel(),
                                                    ignoreBackups);
        QThread *thread = new QThread();
        m_ReadingWorker->moveToThread(thread);

        QObject::connect(thread, SIGNAL(started()), m_ReadingWorker, SLOT(process()));
        QObject::connect(m_ReadingWorker, SIGNAL(stopped()), thread, SLOT(quit()));

        QObject::connect(m_ReadingWorker, SIGNAL(stopped()), m_ReadingWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        QObject::connect(m_ReadingWorker, SIGNAL(finished(bool)), this, SLOT(workerFinished(bool)));

        thread->start();
    }

    void MetadataIOCoordinator::afterImportHandler(const QVector<Models::ArtworkMetadata*> &itemsToRead, bool ignoreBackups) {
        Models::SettingsModel *settingsModel = m_CommandManager->getSettingsModel();

        if (!ignoreBackups && settingsModel->getSaveBackups()) {
            BackupSaverService *saverService = m_CommandManager->getBackupSaverService();
            //saverService->readArtworks(itemsToRead);
        }

        m_CommandManager->addToLibrary(itemsToRead);
        m_CommandManager->saveLocalLibraryAsync();
        m_CommandManager->submitForSpellCheck(itemsToRead);
    }
}

