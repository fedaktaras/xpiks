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

#ifndef KEYWORDSSUGGESTOR_H
#define KEYWORDSSUGGESTOR_H

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QObject>
#include <QList>
#include <QHash>
#include <QSet>
#include "suggestionartwork.h"
#include "../Common/baseentity.h"
#include "../Common/basickeywordsmodel.h"
#include "suggestionqueryengine.h"

namespace Suggestion {
    class KeywordsSuggestor : public QAbstractListModel, public Common::BaseEntity
    {
        Q_OBJECT
        Q_PROPERTY(int suggestedKeywordsCount READ getSuggestedKeywordsCount NOTIFY suggestedKeywordsCountChanged)
        Q_PROPERTY(int otherKeywordsCount READ getOtherKeywordsCount NOTIFY otherKeywordsCountChanged)
        Q_PROPERTY(bool isInProgress READ getIsInProgress NOTIFY isInProgressChanged)
    public:
        KeywordsSuggestor(QObject *parent=NULL) :
            QAbstractListModel(parent),
            m_QueryEngine(this),
            m_SuggestedKeywords(this),
            m_AllOtherKeywords(this),
            m_SelectedArtworksCount(0),
            m_IsInProgress(false)
        {}

        ~KeywordsSuggestor() { qDeleteAll(m_Suggestions); }

    public:
        void setSuggestedArtworks(const QList<SuggestionArtwork*> &suggestedArtworks);
        void clear();

    private:
        int getSuggestedKeywordsCount() const { return m_SuggestedKeywords.rowCount(); }
        int getOtherKeywordsCount() const { return m_AllOtherKeywords.rowCount(); }
        bool getIsInProgress() const { return m_IsInProgress; }

    signals:
        void suggestedKeywordsCountChanged();
        void otherKeywordsCountChanged();
        void isInProgressChanged();

    private:
        void setInProgress() { m_IsInProgress = true; emit isInProgressChanged(); }
    public:
        void unsetInProgress() { m_IsInProgress = false; emit isInProgressChanged(); }

    public:
        Q_INVOKABLE void appendKeywordToSuggested(const QString &keyword) { m_SuggestedKeywords.appendKeyword(keyword); emit suggestedKeywordsCountChanged(); }
        Q_INVOKABLE void appendKeywordToOther(const QString &keyword) { m_AllOtherKeywords.appendKeyword(keyword); emit otherKeywordsCountChanged(); }
        Q_INVOKABLE QString removeSuggestedKeywordAt(int keywordIndex);
        Q_INVOKABLE QString removeOtherKeywordAt(int keywordIndex);
        Q_INVOKABLE void setArtworkSelected(int index, bool newState);
        Q_INVOKABLE void searchArtworks(const QString &searchTerm);
        Q_INVOKABLE void close() { clear(); }
        Q_INVOKABLE QStringList getSuggestedKeywords() const { return m_SuggestedKeywords.getKeywords(); }

        Q_INVOKABLE QObject *getSuggestedKeywordsModel() {
            QObject *item = &m_SuggestedKeywords;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        }

        Q_INVOKABLE QObject *getAllOtherKeywordsModel() {
            QObject *item = &m_AllOtherKeywords;
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
            return item;
        }

    public:
        enum KeywordsSuggestorRoles {
            UrlRole = Qt::UserRole + 1,
            IsSelectedRole
        };

        int rowCount(const QModelIndex & parent = QModelIndex()) const;
        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    protected:
        QHash<int, QByteArray> roleNames() const;

    private:
        void accountKeywords(const QStringList &keywords, int sign);
        QSet<QString> getSelectedArtworksKeywords() const;
        void updateSuggestedKeywords();

    private:
        QHash<QString, int> m_KeywordsHash;
        QList<SuggestionArtwork *> m_Suggestions;
        SuggestionQueryEngine m_QueryEngine;
        Common::BasicKeywordsModel m_SuggestedKeywords;
        Common::BasicKeywordsModel m_AllOtherKeywords;
        int m_SelectedArtworksCount;
        bool m_IsInProgress;
    };
}

#endif // KEYWORDSSUGGESTOR_H
