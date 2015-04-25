/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014 Taras Kushnir <kushnirTV@gmail.com>
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

#ifndef ARTWORKUPLOADER_H
#define ARTWORKUPLOADER_H

#include <QAbstractListModel>
#include <QStringList>
#include <QFutureWatcher>
#include "artworksprocessor.h"
#include "uploadinfo.h"
#include "../Helpers/uploaditem.h"
#include "../Helpers/testconnectionresult.h"

namespace Models {
    class ArtworkUploader : public ArtworksProcessor
    {
        Q_OBJECT
    public:
         ArtworkUploader();
         ~ArtworkUploader() { delete m_ArtworksUploader; delete m_TestingCredentialWatcher; }

    public:
         Q_PROPERTY(bool includeEPS READ getIncludeEPS WRITE setIncludeEPS NOTIFY includeEPSChanged)

    signals:
         void includeEPSChanged(bool);
         void credentialsChecked(bool result, const QString &url);

    public:
         bool getIncludeEPS() const { return m_IncludeEPS; }
         void setIncludeEPS(bool value) {
             if (m_IncludeEPS != value) {
                 m_IncludeEPS = value;
                 emit includeEPSChanged(value);
             }
         }

     public slots:
         void artworkUploaded(int);
         void allFinished();
         void credentialsTestingFinished();

     private:
         void artworkUploadedHandler(Helpers::UploadItem *item);

     public:
         Q_INVOKABLE void uploadArtworks() { doUploadArtworks(getArtworkList()); }
         Q_INVOKABLE void checkCredentials(const QString &host, const QString &username, const QString &password) const;

     private:
         void doUploadArtworks(const QList<ArtworkMetadata*> &artworkList);
         QStringList *getAllFilepathes() const;

    protected:
        void cancelProcessing();

     private:
         QFutureWatcher<Helpers::UploadItem> *m_ArtworksUploader;
         QFutureWatcher<Helpers::TestConnectionResult> *m_TestingCredentialWatcher;
         QStringList *m_ActiveUploads;
         Commands::CommandManager *m_CommandManager;
         bool m_IncludeEPS;
    };
}

#endif // ARTWORKUPLOADER_H
