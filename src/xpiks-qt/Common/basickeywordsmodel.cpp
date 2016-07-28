/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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

#include "basickeywordsmodel.h"
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include "../SpellCheck/spellcheckitem.h"
#include "../SpellCheck/spellsuggestionsitem.h"
#include "../SpellCheck/spellcheckiteminfo.h"
#include "../Helpers/keywordvalidator.h"
#include "../Helpers/stringhelper.h"
#include "flags.h"
#include "../Common/defines.h"
#include "../Helpers/indiceshelper.h"
#include "../Common/flags.h"

namespace Common {
    BasicKeywordsModel::BasicKeywordsModel(Hold &hold, QObject *parent):
        AbstractListModel(parent),
        m_Hold(hold),
        m_SpellCheckInfo(NULL)
    {}

#ifdef CORE_TESTS
    void BasicKeywordsModel::initialize(const QString &title, const QString &description, const QString &rawKeywords) {
        setTitle(title);
        setDescription(description);
        setKeywords(rawKeywords.split(',', QString::SkipEmptyParts));
    }
#endif

    void BasicKeywordsModel::removeItemsAtIndices(const QVector<QPair<int, int> > &ranges) {
        LOG_INFO << "#";

        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        int rangesLength = Helpers::getRangesLength(ranges);
        AbstractListModel::doRemoveItemsAtIndices(ranges, rangesLength);
    }

    void BasicKeywordsModel::removeInnerItem(int row) {
        QString removedKeyword;
        bool wasCorrect = false;

        this->takeKeywordAtUnsafe(row, removedKeyword, wasCorrect);
#ifdef INTEGRATION_TESTS
        LOG_DEBUG << "keyword:" << removedKeyword << "was correct:" << wasCorrect;
#else
        Q_UNUSED(removedKeyword);
        Q_UNUSED(wasCorrect);
#endif
    }

    int BasicKeywordsModel::rowCount(const QModelIndex &parent) const {
        Q_UNUSED(parent);

        // QReadLocker readLocker(&m_KeywordsLock);
        // Q_UNUSED(readLocker);
        // due to the qt limitations, pray the keyword will be there

        return m_KeywordsList.length();
    }

    QVariant BasicKeywordsModel::data(const QModelIndex &index, int role) const {
        // QReadLocker readLocker(&m_KeywordsLock);
        // Q_UNUSED(readLocker);
        // due to the qt limitations, pray the keyword will be there

        int row = index.row();

        if (row < 0 || row >= m_KeywordsList.length()) {
            return QVariant();
        }

        switch (role) {
            case KeywordRole:
                return m_KeywordsList.at(index.row());
            case IsCorrectRole:
                return m_SpellCheckResults.at(index.row());
            default:
                return QVariant();
        }
    }

    QString BasicKeywordsModel::getDescription() {
        QReadLocker readLocker(&m_DescriptionLock);

        Q_UNUSED(readLocker);

        return m_Description;
    }

    QString BasicKeywordsModel::getTitle() {
        QReadLocker readLocker(&m_TitleLock);

        Q_UNUSED(readLocker);

        return m_Title;
    }

    int BasicKeywordsModel::getKeywordsCount() {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return m_KeywordsSet.count();
    }

    QSet<QString> BasicKeywordsModel::getKeywordsSet() {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return m_KeywordsSet;
    }

    QString BasicKeywordsModel::getKeywordsString() {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return m_KeywordsList.join(", ");
    }

    bool BasicKeywordsModel::appendKeyword(const QString &keyword) {
        QWriteLocker writeLocker(&m_KeywordsLock);

        Q_UNUSED(writeLocker);

        return appendKeywordUnsafe(keyword);
    }

    bool BasicKeywordsModel::takeKeywordAt(int index, QString &removedKeyword) {
        bool wasCorrect = false, result = false;

        m_KeywordsLock.lockForWrite();
        {
            if (0 <= index && index < m_KeywordsList.length()) {
                beginRemoveRows(QModelIndex(), index, index);
                takeKeywordAtUnsafe(index, removedKeyword, wasCorrect);
                endRemoveRows();
                result = true;
            }
        }
        m_KeywordsLock.unlock();

        if (!wasCorrect) {
            emit spellCheckErrorsChanged();
        }

        return result;
    }

    bool BasicKeywordsModel::takeLastKeyword(QString &removedKeyword) {
        bool wasCorrect = false, result = false;

        m_KeywordsLock.lockForWrite();
        {
            if (m_KeywordsList.length() > 0) {
                int index = m_KeywordsList.length() - 1;
                beginRemoveRows(QModelIndex(), index, index);
                takeKeywordAtUnsafe(index, removedKeyword, wasCorrect);
                endRemoveRows();
                result = true;
            }
        }
        m_KeywordsLock.unlock();

        if (!wasCorrect) {
            emit spellCheckErrorsChanged();
        }

        return result;
    }

    void BasicKeywordsModel::setKeywords(const QStringList &keywordsList) {
        QWriteLocker writeLocker(&m_KeywordsLock);

        Q_UNUSED(writeLocker);

        setKeywordsUnsafe(keywordsList);
    }

    int BasicKeywordsModel::appendKeywords(const QStringList &keywordsList) {
        QWriteLocker writeLocker(&m_KeywordsLock);

        Q_UNUSED(writeLocker);

        return appendKeywordsUnsafe(keywordsList);
    }

    bool BasicKeywordsModel::editKeyword(int index, const QString &replacement) {
        bool result = false;

        m_KeywordsLock.lockForWrite();
        {
            if (0 <= index && index < m_KeywordsList.length()) {
                result = editKeywordUnsafe(index, replacement);
            } else {
                LOG_WARNING << "Failed to edit keyword with index" << index;
            }
        }
        m_KeywordsLock.unlock();

        if (result) {
            QModelIndex i = this->index(index);
            emit dataChanged(i, i, QVector<int>() << KeywordRole);
        }

        return result;
    }

    bool BasicKeywordsModel::clearKeywords() {
        bool result = false;

        m_KeywordsLock.lockForWrite();
        {
            result = clearKeywordsUnsafe();
        }
        m_KeywordsLock.unlock();

        if (result) {
            emit spellCheckErrorsChanged();
        }

        return result;
    }

    bool BasicKeywordsModel::areKeywordsEmpty() {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return m_KeywordsList.isEmpty();
    }

    bool BasicKeywordsModel::replace(const QString &replaceWhat, const QString &replaceTo, int flags) {
        LOG_INFO << replaceWhat << "->" << replaceTo << "with flags:" << flags;
        Q_ASSERT(!replaceWhat.isEmpty());
        Q_ASSERT(!replaceTo.isEmpty());
        Q_ASSERT((flags & Common::SearchFlagSearchMetadata) != 0);
        bool anyChanged = false;

        bool isCaseSensitive = Common::HasFlag(flags, Common::SearchFlagCaseSensitive);
        Qt::CaseSensitivity caseSensivity = isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        const bool needToCheckDescription = Common::HasFlag(flags, Common::SearchFlagSearchDescription);
        if (needToCheckDescription) {
            if (this->replaceInDescription(replaceWhat, replaceTo, caseSensivity)) {
                anyChanged = true;
            }
        }

        const bool needToCheckTitle = Common::HasFlag(flags, Common::SearchFlagSearchTitle);
        if (needToCheckTitle) {
            if (this->replaceInTitle(replaceWhat, replaceTo, caseSensivity)) {
                anyChanged = true;
            }
        }

        const bool needToCheckKeywords = Common::HasFlag(flags, Common::SearchFlagSearchKeywords);
        if (needToCheckKeywords) {
            QWriteLocker locker(&m_KeywordsLock);
            Q_UNUSED(locker);
            if (this->replaceInKeywordsUnsafe(replaceWhat, replaceTo, caseSensivity)) {
                anyChanged = true;
            }
        }

        return anyChanged;
    }

    bool BasicKeywordsModel::removeKeywords(const QSet<QString> &keywords) {
        QWriteLocker writeLocker(&m_KeywordsLock);

        Q_UNUSED(writeLocker);

        bool result = removeKeywordsUnsafe(keywords);
        return result;
    }

    bool BasicKeywordsModel::appendKeywordUnsafe(const QString &keyword) {
        bool added = false;
        const QString &sanitizedKeyword = keyword.simplified();

        if (canBeAddedUnsafe(sanitizedKeyword)) {
            int keywordsCount = m_KeywordsList.length();

            m_KeywordsSet.insert(sanitizedKeyword.toLower());
            m_SpellCheckResults.append(true);

            beginInsertRows(QModelIndex(), keywordsCount, keywordsCount);
            m_KeywordsList.append(sanitizedKeyword);
            endInsertRows();
            added = true;
        }

        return added;
    }

    void BasicKeywordsModel::takeKeywordAtUnsafe(int index, QString &removedKeyword, bool &wasCorrect) {
        const QString &keyword = m_KeywordsList.at(index);
        QString invariant = keyword.toLower();

        m_KeywordsSet.remove(invariant);

        removedKeyword = m_KeywordsList.takeAt(index);
        wasCorrect = m_SpellCheckResults.takeAt(index);
    }

    void BasicKeywordsModel::setKeywordsUnsafe(const QStringList &keywordsList) {
        clearKeywordsUnsafe();
        appendKeywordsUnsafe(keywordsList);
    }

    int BasicKeywordsModel::appendKeywordsUnsafe(const QStringList &keywordsList) {
        QStringList keywordsToAdd;
        int appendedCount = 0, size = keywordsList.length();

        keywordsToAdd.reserve(size);
        QSet<QString> accountedKeywords;

        for (int i = 0; i < size; ++i) {
            const QString &keyword = keywordsList.at(i);
            const QString &sanitizedKeyword = keyword.simplified();
            const QString &lowerCased = sanitizedKeyword.toLower();

            if (canBeAddedUnsafe(sanitizedKeyword) && !accountedKeywords.contains(lowerCased)) {
                keywordsToAdd.append(sanitizedKeyword);
                accountedKeywords.insert(lowerCased);
                appendedCount++;
            }
        }

        size = keywordsToAdd.size();
        Q_ASSERT(size == appendedCount);

        if (size > 0) {
            int rowsCount = m_KeywordsList.length();
            beginInsertRows(QModelIndex(), rowsCount, rowsCount + size - 1);

            for (int i = 0; i < size; ++i) {
                const QString &keywordToAdd = keywordsToAdd.at(i);
                m_KeywordsSet.insert(keywordToAdd.toLower());
                m_SpellCheckResults.append(true);
                m_KeywordsList.append(keywordToAdd);
            }

            endInsertRows();
        }

        return appendedCount;
    }

    bool BasicKeywordsModel::editKeywordUnsafe(int index, const QString &replacement) {
        bool result = false;
        QString sanitized = Helpers::doSanitizeKeyword(replacement);

        QString existing = m_KeywordsList.at(index);

        if (existing != sanitized && Helpers::isValidKeyword(sanitized)) {
            QString lowerCasedNew = sanitized.toLower();
            QString lowerCasedExisting = existing.toLower();

            if (!m_KeywordsSet.contains(lowerCasedNew)) {
                m_KeywordsSet.insert(lowerCasedNew);
                m_KeywordsList[index] = sanitized;
                m_KeywordsSet.remove(lowerCasedExisting);
                LOG_INFO << "common case edit:" << existing << "->" << sanitized;

                result = true;
            } else if (lowerCasedNew == lowerCasedExisting) {
                LOG_DEBUG << "changing case in same keyword";
                m_KeywordsList[index] = sanitized;

                result = true;
            } else {
                LOG_WARNING << "Attempt to rename keyword to existing one. Use remove instead!";
            }
        }

        return result;
    }

    bool BasicKeywordsModel::replaceKeywordUnsafe(int index, const QString &existing, const QString &replacement) {
        bool result = false;

        const QString &internal = m_KeywordsList.at(index);

        if (internal == existing) {
            if (this->editKeywordUnsafe(index, replacement)) {
                result = true;
            }
        } else if (internal.contains(existing) && internal.contains(QChar::Space)) {
            LOG_DEBUG << "Replacing composite keyword";
            QString existingFixed = internal;
            existingFixed.replace(existing, replacement);
            if (this->editKeywordUnsafe(index, existingFixed)) {
                // TODO: reimplement this someday
                // no need to mark keyword as correct
                // if we replace only part of it
                result = true;
            }
        }

        return result;
    }

    bool BasicKeywordsModel::clearKeywordsUnsafe() {
        bool anyKeywords = !m_KeywordsList.isEmpty();

        if (anyKeywords) {
            beginResetModel();
            m_KeywordsList.clear();
            endResetModel();

            m_SpellCheckResults.clear();
            m_KeywordsSet.clear();
        } else {
            Q_ASSERT(m_KeywordsSet.isEmpty());
            Q_ASSERT(m_SpellCheckResults.isEmpty());
        }

        return anyKeywords;
    }

    bool BasicKeywordsModel::containsKeywordUnsafe(const QString &searchTerm, int searchFlags) {
        bool hasMatch = false;
        int length = m_KeywordsList.length();
        const bool exactMatch = Common::HasFlag(searchFlags, Common::SearchFlagExactMatch);

        if (exactMatch) {
            for (int i = 0; i < length; ++i) {
                if (m_KeywordsList.at(i) == searchTerm) {
                    hasMatch = true;
                    break;
                }
            }
        } else {
            const bool caseSensitive = Common::HasFlag(searchFlags, Common::SearchFlagCaseSensitive);
            Qt::CaseSensitivity caseSensitity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

            for (int i = 0; i < length; ++i) {
                if (m_KeywordsList.at(i).contains(searchTerm, caseSensitity)) {
                    hasMatch = true;
                    break;
                }
            }
        }

        return hasMatch;
    }

    bool BasicKeywordsModel::hasKeywordsSpellErrorUnsafe() const {
        bool anyError = false;
        int length = m_SpellCheckResults.length();

        for (int i = 0; i < length; ++i) {
            if (!m_SpellCheckResults[i]) {
                anyError = true;
                break;
            }
        }
    
        return anyError;
    }

    bool BasicKeywordsModel::removeKeywordsUnsafe(const QSet<QString> &keywordsToRemove) {
        int size = m_KeywordsList.size();

        QVector<int> indicesToRemove;
        indicesToRemove.reserve(size/2);

        for (int i = 0; i < size; ++i) {
            const QString &keyword = m_KeywordsList.at(i);

            if (keywordsToRemove.contains(keyword.toLower())) {
                indicesToRemove.append(i);
            }
        }

        bool anythingRemoved = !indicesToRemove.empty();
        removeKeywordsAtIndicesUnsafe(indicesToRemove);

        return anythingRemoved;
    }

    void BasicKeywordsModel::removeKeywordsAtIndicesUnsafe(const QVector<int> &indices) {
        LOG_DEBUG << indices.size() << "item(s)";
        QVector<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indices, rangesToRemove);
        AbstractListModel::removeItemsAtIndices(rangesToRemove);
    }

    bool BasicKeywordsModel::replaceInDescription(const QString &replaceWhat, const QString &replaceTo,
                                                  Qt::CaseSensitivity caseSensivity) {
        QString description = getDescription();
        description.replace(replaceWhat, replaceTo, caseSensivity);
        bool result = setDescription(description);
        return result;
    }

    bool BasicKeywordsModel::replaceInTitle(const QString &replaceWhat, const QString &replaceTo,
                                            Qt::CaseSensitivity caseSensivity) {
        QString title = getTitle();
        title.replace(replaceWhat, replaceTo, caseSensivity);
        bool result = setTitle(title);
        return result;
    }

    bool BasicKeywordsModel::replaceInKeywordsUnsafe(const QString &replaceWhat, const QString &replaceTo,
                                                     Qt::CaseSensitivity caseSensivity) {
        bool anyChanged = false;
        QVector<int> indicesToRemove;

        int size = m_KeywordsList.length();
        for (int i = 0; i < size; ++i) {
            QString internal = m_KeywordsList.at(i);
            if (internal.contains(replaceWhat, caseSensivity)) {
                QString replaced = internal.replace(replaceWhat, replaceTo, caseSensivity);
                QString replacement = Helpers::doSanitizeKeyword(replaced);

                if (!this->editKeywordUnsafe(i, replacement) &&
                        m_KeywordsSet.contains(replacement.toLower())) {
                    LOG_INFO << "Replacing" << internal << "to" << replacement << "creates a duplicate";
                    indicesToRemove.append(i);
                } else {
                    QModelIndex j = this->index(i);
                    emit dataChanged(j, j, QVector<int>() << KeywordRole);
                    anyChanged = true;
                }
            }
        }

        if (!indicesToRemove.isEmpty()) {
            this->removeKeywordsAtIndicesUnsafe(indicesToRemove);
            anyChanged = true;
        }

        return anyChanged;
    }

    bool BasicKeywordsModel::setDescription(const QString &value) {
        QWriteLocker writeLocker(&m_DescriptionLock);

        Q_UNUSED(writeLocker);

        bool result = value != m_Description;
        if (result) {
            m_Description = value;
        }

        return result;
    }

    bool BasicKeywordsModel::setTitle(const QString &value) {
        QWriteLocker writeLocker(&m_TitleLock);

        Q_UNUSED(writeLocker);

        bool result = value != m_Title;
        if (result) {
            m_Title = value;
        }

        return result;
    }

    bool BasicKeywordsModel::isEmpty() {
        QReadLocker readKeywordsLock(&m_KeywordsLock);

        Q_UNUSED(readKeywordsLock);

        QReadLocker readDescriptionLock(&m_DescriptionLock);
        Q_UNUSED(readDescriptionLock);

        return m_KeywordsList.isEmpty() || m_Description.trimmed().isEmpty();
    }

    bool BasicKeywordsModel::isTitleEmpty() {
        QReadLocker readLocker(&m_TitleLock);

        Q_UNUSED(readLocker);

        return m_Title.trimmed().isEmpty();
    }

    bool BasicKeywordsModel::isDescriptionEmpty() {
        QReadLocker readLocker(&m_DescriptionLock);

        Q_UNUSED(readLocker);

        return m_Description.trimmed().isEmpty();
    }

    bool BasicKeywordsModel::containsKeyword(const QString &searchTerm, int searchFlags) {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return containsKeywordUnsafe(searchTerm, searchFlags);
    }

    bool BasicKeywordsModel::hasKeywordsSpellError() {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return hasKeywordsSpellErrorUnsafe();
    }

    bool BasicKeywordsModel::hasDescriptionSpellError() {
        bool anyError = false;

        const QStringList &descriptionWords = getDescriptionWords();

        foreach(const QString &word, descriptionWords) {
            if (m_SpellCheckInfo->hasDescriptionError(word)) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    bool BasicKeywordsModel::hasTitleSpellError() {
        bool anyError = false;

        const QStringList &titleWords = getTitleWords();

        foreach(const QString &word, titleWords) {
            if (m_SpellCheckInfo->hasTitleError(word)) {
                anyError = true;
                break;
            }
        }

        return anyError;
    }

    bool BasicKeywordsModel::hasSpellErrors() {
        bool hasErrors = hasDescriptionSpellError() ||
                         hasTitleSpellError() ||
                         hasKeywordsSpellError();

        return hasErrors;
    }

    void BasicKeywordsModel::setSpellStatuses(BasicKeywordsModel *keywordsModel) {
        QWriteLocker writeLocker(&m_KeywordsLock);

        Q_UNUSED(writeLocker);

        keywordsModel->lockKeywordsRead();
        {
            const QVector<bool> &spellStatuses = keywordsModel->getSpellStatusesUnsafe();

#ifdef QT_DEBUG
            // sync issue between adding/removing/undo/spellcheck
            Q_ASSERT(spellStatuses.length() == m_SpellCheckResults.length());
#endif

            int size = qMin(spellStatuses.length(), m_SpellCheckResults.length());

            for (int i = 0; i < size; ++i) {
                m_SpellCheckResults[i] = spellStatuses[i];
            }
        }
        keywordsModel->unlockKeywords();
    }

    void BasicKeywordsModel::clearModel() {
        setDescription("");
        setTitle("");
        clearKeywords();
    }

    void BasicKeywordsModel::notifySpellCheckResults(int flags) {
        if (Common::HasFlag(flags, Common::SpellCheckDescription) ||
            Common::HasFlag(flags, Common::SpellCheckTitle)) {
            emit spellCheckResultsReady();
        }

        emit spellCheckErrorsChanged();
    }

    void BasicKeywordsModel::notifyDescriptionSpellCheck() {
        emit spellCheckResultsReady();
    }

    void BasicKeywordsModel::notifyTitleSpellCheck() {
        emit spellCheckResultsReady();
    }

    void BasicKeywordsModel::updateDescriptionSpellErrors(const QHash<QString, bool> &results) {
        QSet<QString> descriptionErrors;
        QStringList descriptionWords = getDescriptionWords();
        foreach(const QString &word, descriptionWords) {
            if (results.value(word, true) == false) {
                descriptionErrors.insert(word);
            }
        }

        m_SpellCheckInfo->setDescriptionErrors(descriptionErrors);
    }

    void BasicKeywordsModel::updateTitleSpellErrors(const QHash<QString, bool> &results) {
        QSet<QString> titleErrors;
        QStringList titleWords = getTitleWords();
        foreach(const QString &word, titleWords) {
            if (results.value(word, true) == false) {
                titleErrors.insert(word);
            }
        }

        m_SpellCheckInfo->setTitleErrors(titleErrors);
    }

    void BasicKeywordsModel::resetSpellCheckResultsUnsafe() {
        int size = m_SpellCheckResults.length();

        // TODO: use smth like memset
        for (int i = 0; i < size; ++i) {
            m_SpellCheckResults[i] = true;
        }
    }

    bool BasicKeywordsModel::canBeAddedUnsafe(const QString &keyword) const {
        bool isValid = Helpers::isValidKeyword(keyword);
        bool result = isValid && !m_KeywordsSet.contains(keyword.toLower());

        return result;
    }

    bool BasicKeywordsModel::hasKeyword(const QString &keyword) {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return !canBeAddedUnsafe(keyword.simplified());
    }

    QString BasicKeywordsModel::retrieveKeyword(int wordIndex) {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        QString keyword;
        if (0 <= wordIndex && wordIndex < m_KeywordsList.length()) {
            keyword = m_KeywordsList.at(wordIndex);
        }

        return keyword;
    }

    QStringList BasicKeywordsModel::getKeywords() {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return m_KeywordsList;
    }

    void BasicKeywordsModel::setSpellCheckResults(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items,
                                                  bool onlyOneKeyword) {
        QWriteLocker writeLocker(&m_KeywordsLock);

        Q_UNUSED(writeLocker);

#ifdef INTEGRATION_TESTS
        if (m_KeywordsList.length() != m_SpellCheckResults.length()) {
            LOG_DEBUG << "Current keywords list length:" << m_KeywordsList.length();
            LOG_DEBUG << "SpellCheck list length:" << m_SpellCheckResults.length();
        }

#endif
        // sync issue between adding/removing/undo/spellcheck
        Q_ASSERT(m_KeywordsList.length() == m_SpellCheckResults.length());

        if (!onlyOneKeyword) {
            resetSpellCheckResultsUnsafe();
        } else {
            Q_ASSERT(!items.empty());
            int index = items.front()->m_Index;
            m_SpellCheckResults[index] = true;
        }

        size_t size = items.size();
        for (size_t i = 0; i < size; ++i) {
            auto &item = items.at(i);
            int index = item->m_Index;
            if (0 <= index && index < m_KeywordsList.length()) {
                if (m_KeywordsList[index].contains(item->m_Word)) {
                    // if keyword contains several words, there would be
                    // several queryitems and there's error if any has error
                    m_SpellCheckResults[index] = m_SpellCheckResults[index] && item->m_IsCorrect;
                }
            }
        }
    }

    void BasicKeywordsModel::setSpellCheckResults(const QHash<QString, bool> &results, int flags) {
        if (Common::HasFlag(flags, Common::SpellCheckDescription)) {
            updateDescriptionSpellErrors(results);
        }

        if (Common::HasFlag(flags, Common::SpellCheckTitle)) {
            updateTitleSpellErrors(results);
        }
    }

    QVector<SpellCheck::SpellSuggestionsItem *> BasicKeywordsModel::createKeywordsSuggestionsList() {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        QVector<SpellCheck::SpellSuggestionsItem *> spellCheckSuggestions;
        int length = m_KeywordsList.length();
        spellCheckSuggestions.reserve(length/2);

        for (int i = 0; i < length; ++i) {
            if (!m_SpellCheckResults[i]) {
                const QString &keyword = m_KeywordsList[i];

                if (!keyword.contains(QChar::Space)) {
                    SpellCheck::KeywordSpellSuggestions *suggestionsItem = new SpellCheck::KeywordSpellSuggestions(keyword, i);
                    spellCheckSuggestions.append(suggestionsItem);
                } else {
                    QStringList items = keyword.split(QChar::Space, QString::SkipEmptyParts);
                    foreach(const QString &item, items) {
                        SpellCheck::KeywordSpellSuggestions *suggestionsItem =
                            new SpellCheck::KeywordSpellSuggestions(item, i, keyword);

                        spellCheckSuggestions.append(suggestionsItem);
                    }
                }
            }
        }

        return spellCheckSuggestions;
    }

    QVector<SpellCheck::SpellSuggestionsItem *> BasicKeywordsModel::createDescriptionSuggestionsList() {
        QStringList descriptionWords = getDescriptionWords();
        int length = descriptionWords.length();

        QVector<SpellCheck::SpellSuggestionsItem *> spellCheckSuggestions;
        spellCheckSuggestions.reserve(length / 3);

        for (int i = 0; i < length; ++i) {
            const QString &word = descriptionWords.at(i);
            if (m_SpellCheckInfo->hasDescriptionError(word)) {
                SpellCheck::DescriptionSpellSuggestions *suggestionsItem = new SpellCheck::DescriptionSpellSuggestions(word);
                spellCheckSuggestions.append(suggestionsItem);
            }
        }

        return spellCheckSuggestions;
    }

    QVector<SpellCheck::SpellSuggestionsItem *> BasicKeywordsModel::createTitleSuggestionsList() {
        QStringList titleWords = getTitleWords();
        int length = titleWords.length();

        QVector<SpellCheck::SpellSuggestionsItem *> spellCheckSuggestions;
        spellCheckSuggestions.reserve(length / 3);

        for (int i = 0; i < length; ++i) {
            const QString &word = titleWords.at(i);
            if (m_SpellCheckInfo->hasTitleError(word)) {
                SpellCheck::TitleSpellSuggestions *suggestionsItem = new SpellCheck::TitleSpellSuggestions(word);
                spellCheckSuggestions.append(suggestionsItem);
            }
        }

        return spellCheckSuggestions;
    }

    Common::KeywordReplaceResult BasicKeywordsModel::fixKeywordSpelling(int index, const QString &existing, const QString &replacement) {
        Common::KeywordReplaceResult result;

        m_KeywordsLock.lockForWrite();
        {
            LOG_DEBUG << "Replacing" << existing << "to" << replacement << "with index" << index;
            if (0 <= index && index < m_KeywordsList.length()) {
                if (replaceKeywordUnsafe(index, existing, replacement)) {
                    m_SpellCheckResults[index] = true;
                    result = Common::KeywordReplaceSucceeded;
                } else {
                    result = Common::KeywordReplaceFailedDuplicate;
                }
            } else {
                LOG_DEBUG << "Failure. Index is negative or exceeds count" << m_KeywordsList.length();
                result = Common::KeywordReplaceFailedIndex;
            }
        }
        m_KeywordsLock.unlock();

        if (result == Common::KeywordReplaceSucceeded) {
            QModelIndex i = this->index(index);
            // combined roles from legacy editKeyword() and replace()
            emit dataChanged(i, i, QVector<int>() << KeywordRole << IsCorrectRole);
        }

        return result;
    }

    bool BasicKeywordsModel::processFailedKeywordReplacements(const QVector<SpellCheck::KeywordSpellSuggestions *> &candidatesForRemoval) {
        LOG_DEBUG << candidatesForRemoval.size() << "candidates to remove";
        bool anyReplaced = false;

        if (candidatesForRemoval.isEmpty()) { return anyReplaced; }

        QVector<int> indicesToRemove;
        int size = candidatesForRemoval.size();
        indicesToRemove.reserve(size);

        QWriteLocker writeLocker(&m_KeywordsLock);

        for (int i = 0; i < size; ++i) {
            SpellCheck::KeywordSpellSuggestions *item = candidatesForRemoval.at(i);

            int index = item->getOriginalIndex();
            if (index < 0 || index >= m_KeywordsList.length()) {
                LOG_DEBUG << "index is out of range";
                continue;
            }

            const QString &existingPrev = item->getWord();
            QString sanitized = Helpers::doSanitizeKeyword(item->getReplacement());

            if (isReplacedADuplicateUnsafe(index, existingPrev, sanitized)) {
                indicesToRemove.append(index);
            }
        }

        LOG_INFO << "confirmed" << indicesToRemove.size() << "duplicates to remove";

        if (!indicesToRemove.isEmpty()) {
            this->removeKeywordsAtIndicesUnsafe(indicesToRemove);
            anyReplaced = true;
        }

        return anyReplaced;
    }

    void BasicKeywordsModel::fixDescriptionSpelling(const QString &word, const QString &replacement) {
        replaceInDescription(word, replacement);
    }

    void BasicKeywordsModel::fixTitleSpelling(const QString &word, const QString &replacement) {
        replaceInTitle(word, replacement);
    }

    void BasicKeywordsModel::afterReplaceCallback() {
        emit spellCheckErrorsChanged();
    }

    void BasicKeywordsModel::connectSignals(SpellCheck::SpellCheckItem *item) {
        QObject::connect(item, SIGNAL(resultsReady(int, int)), this, SLOT(spellCheckRequestReady(int, int)));
    }

    QStringList BasicKeywordsModel::getDescriptionWords() {
        QReadLocker readLocker(&m_DescriptionLock);

        Q_UNUSED(readLocker);

        QStringList words;
        Helpers::splitText(m_Description, words);
        return words;
    }

    QStringList BasicKeywordsModel::getTitleWords() {
        QReadLocker readLocker(&m_TitleLock);

        Q_UNUSED(readLocker);

        QStringList words;
        Helpers::splitText(m_Title, words);
        return words;
    }

    void BasicKeywordsModel::spellCheckRequestReady(int flags, int index) {
        if (Common::HasFlag(flags, Common::SpellCheckKeywords)) {
            emitSpellCheckChanged(index);
        }

        notifySpellCheckResults(flags);
    }

    bool BasicKeywordsModel::isReplacedADuplicateUnsafe(int index, const QString &existingPrev,
                                                        const QString &replacement) const {
        bool isDuplicate = false;
        const QString &existingCurrent = m_KeywordsList.at(index);

        if (existingCurrent == existingPrev) {
            if (m_KeywordsSet.contains(replacement.toLower())) {
                isDuplicate = true;
                LOG_DEBUG << "safe to remove duplicate [" << existingCurrent << "] at index" << index;
            } else {
                LOG_DEBUG << replacement << "was not found";
            }
        } else if (existingCurrent.contains(existingPrev) && existingCurrent.contains(QChar::Space)) {
            QString existingFixed = existingCurrent;
            existingFixed.replace(existingPrev, replacement);

            if (m_KeywordsSet.contains(existingFixed.toLower())) {
                isDuplicate = true;
                LOG_DEBUG << "safe to remove composite duplicate [" << existingCurrent << "] at index" << index;
            } else {
                LOG_DEBUG << existingFixed << "was not found";
            }
        } else {
            LOG_DEBUG << existingCurrent << "is now instead of" << existingPrev << "at index" << index;
        }

        return isDuplicate;
    }

    void BasicKeywordsModel::emitSpellCheckChanged(int index) {
        int count = m_KeywordsList.length();

        if (index == -1) {
            if (count > 0) {
                QModelIndex start = this->index(0);
                QModelIndex end = this->index(count - 1);
                emit dataChanged(start, end, QVector<int>() << IsCorrectRole);
            }
        } else {
            if (0 <= index && index < count) {
                QModelIndex i = this->index(index);
                emit dataChanged(i, i, QVector<int>() << IsCorrectRole);
            }
        }
    }

    QHash<int, QByteArray> BasicKeywordsModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[KeywordRole] = "keyword";
        roles[IsCorrectRole] = "iscorrect";
        return roles;
    }
}
