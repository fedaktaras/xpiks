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

#include "updateservice.h"

#define UPDATE_JSON_URL "http://ribtoks.github.io/xpiks/update.json"
#define DEFAULT_UPDATE_URL "http://ribtoks.github.io/xpiks/downloads/"
#define UPDATE_JSON_MAJOR_VERSION "major_version"
#define UPDATE_JSON_MINOR_VERSION "minor_version"
#define UPDATE_JSON_FIX_VERSION "fix_version"

#include <QString>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGlobal>

#ifdef Q_OS_DARWIN
#define UPDATE_JSON_UPDATE_URL "osx_link"
#elif Q_OS_WIN
#define UPDATE_JSON_UPDATE_URL "windows_link"
#elif Q_OS_LINUX
#define UPDATE_JSON_UPDATE_URL "linux_link"
#else
#define UPDATE_JSON_UPDATE_URL "unknown_link"
#endif

#include "../Common/version.h"

namespace Helpers {
    UpdateService::UpdateService() :
        QObject(),
        m_NetworkManager(this)
    {
        QObject::connect(&m_NetworkManager, SIGNAL(finished(QNetworkReply*)),
                         this, SLOT(replyReceived(QNetworkReply*)));
    }

    void UpdateService::checkForUpdates() {
        QString queryString = QString(UPDATE_JSON_URL);
        QUrl url;
        url.setUrl(queryString);

        QNetworkRequest request(url);
        QNetworkReply *reply = m_NetworkManager.get(request);
        Q_UNUSED(reply);
    }

    void UpdateService::replyReceived(QNetworkReply *networkReply) {
        if (networkReply->error() == QNetworkReply::NoError) {
            QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());
            QJsonObject jsonObject = document.object();

            if (jsonObject.contains(UPDATE_JSON_MAJOR_VERSION) &&
                    jsonObject.contains(UPDATE_JSON_MINOR_VERSION) &&
                    jsonObject.contains(UPDATE_JSON_FIX_VERSION)) {

                int majorVersion = jsonObject.value(UPDATE_JSON_MAJOR_VERSION).toInt();
                int minorVersion = jsonObject.value(UPDATE_JSON_MINOR_VERSION).toInt();
                int fixVersion = jsonObject.value(UPDATE_JSON_FIX_VERSION).toInt();

                int availableVersion = majorVersion*100 + minorVersion*10 + fixVersion;
                int currVersion = XPIKS_MAJOR_VERSION*100 + XPIKS_MINOR_VERSION*10 + XPIKS_FIX_VERSION;

                if (availableVersion > currVersion) {
                    QString updateUrl = DEFAULT_UPDATE_URL;
                    if (jsonObject.contains(UPDATE_JSON_UPDATE_URL)) {
                        updateUrl = jsonObject.value(UPDATE_JSON_UPDATE_URL).toString();
                    }

                    emit updateAvailable(updateUrl);
                }
            }
        } else {
            qDebug() << networkReply->errorString();
        }

        networkReply->deleteLater();
    }
}