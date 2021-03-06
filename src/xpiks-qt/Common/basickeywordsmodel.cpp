/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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
#include "../Helpers/keywordshelpers.h"
#include "../Helpers/stringhelper.h"
#include "flags.h"
#include "../Common/defines.h"
#include "../Helpers/indiceshelper.h"
#include "../Common/flags.h"

namespace Common {
    BasicKeywordsModel::BasicKeywordsModel(Hold &hold, QObject *parent):
        AbstractListModel(parent),
        m_Hold(hold)
    {}

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
        LOG_INTEGRATION_TESTS << "keyword:" << removedKeyword << "was correct:" << wasCorrect;
        Q_UNUSED(removedKeyword);
        Q_UNUSED(wasCorrect);
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

    bool BasicKeywordsModel::removeKeywordAt(int index, QString &removedKeyword) {
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

    bool BasicKeywordsModel::removeLastKeyword(QString &removedKeyword) {
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

    bool BasicKeywordsModel::expandPreset(int keywordIndex, const QStringList &presetList) {
        LOG_INFO << keywordIndex;
        bool result = false;
        QWriteLocker writeLocker(&m_KeywordsLock);

        Q_UNUSED(writeLocker);
        if ((0 <= keywordIndex) && (keywordIndex < m_KeywordsList.length())) {
            expandPresetUnsafe(keywordIndex, presetList);
            result = true;
        }

        return result;
    }

    bool BasicKeywordsModel::areKeywordsEmpty() {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return m_KeywordsList.isEmpty();
    }

    bool BasicKeywordsModel::replace(const QString &replaceWhat, const QString &replaceTo, Common::SearchFlags flags) {
        LOG_INTEGR_TESTS_OR_DEBUG << replaceWhat << "->" << replaceTo << "with flags:" << (int)flags;
        Q_ASSERT(!replaceWhat.isEmpty());
        Q_ASSERT(((int)flags & (int)Common::SearchFlags::Metadata) != 0);

        bool anyChanged = false;

        const bool needToCheckKeywords = Common::HasFlag(flags, Common::SearchFlags::Keywords);
        if (needToCheckKeywords) {
            QWriteLocker locker(&m_KeywordsLock);
            Q_UNUSED(locker);
            if (this->replaceInKeywordsUnsafe(replaceWhat, replaceTo, flags)) {
                anyChanged = true;
            }
        }

        return anyChanged;
    }

    bool BasicKeywordsModel::removeKeywords(const QSet<QString> &keywords, bool caseSensitive) {
        QWriteLocker writeLocker(&m_KeywordsLock);

        Q_UNUSED(writeLocker);

        bool result = removeKeywordsUnsafe(keywords, caseSensitive);
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

    bool BasicKeywordsModel::canEditKeywordUnsafe(int index, const QString &replacement) const {
        bool result = false;
        LOG_INFO << "index:" << index << "replacement:" << replacement;

        QString sanitized = Helpers::doSanitizeKeyword(replacement);
        QString existing = m_KeywordsList.at(index);
        // IMPORTANT: keep track of copy-paste in editKeywordUnsafe()
        if (existing != sanitized && Helpers::isValidKeyword(sanitized)) {
            QString lowerCasedNew = sanitized.toLower();
            QString lowerCasedExisting = existing.toLower();

            if (!m_KeywordsSet.contains(lowerCasedNew)) {
                result = true;
            } else if (lowerCasedNew == lowerCasedExisting) {
                result = true;
            }
        }

        return result;
    }

    bool BasicKeywordsModel::editKeywordUnsafe(int index, const QString &replacement) {
        bool result = false;

        LOG_INFO << "index:" << index << "replacement:" << replacement;
        QString sanitized = Helpers::doSanitizeKeyword(replacement);

        QString existing = m_KeywordsList.at(index);
        // IMPORTANT: keep track of copy-paste in canEditKeywordUnsafe()
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
                LOG_INFO << "changing case in same keyword";
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
            LOG_INFO << "Replacing composite keyword";
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

    bool BasicKeywordsModel::containsKeywordUnsafe(const QString &searchTerm, Common::SearchFlags searchFlags) {
        bool hasMatch = false;
        int length = m_KeywordsList.length();
        const bool caseSensitive = Common::HasFlag(searchFlags, Common::SearchFlags::CaseSensitive);
        Qt::CaseSensitivity caseSensivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
        const bool wholeWords = Common::HasFlag(searchFlags, Common::SearchFlags::WholeWords);

        if (wholeWords) {
            for (int i = 0; i < length; ++i) {
                if (QString::compare(m_KeywordsList.at(i), searchTerm, caseSensivity) == 0) {
                    hasMatch = true;
                    break;
                }
            }
        } else {
            for (int i = 0; i < length; ++i) {
                if (m_KeywordsList.at(i).contains(searchTerm, caseSensivity)) {
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

    bool BasicKeywordsModel::removeKeywordsUnsafe(const QSet<QString> &keywordsToRemove, bool caseSensitive) {
        int size = m_KeywordsList.size();

        QVector<int> indicesToRemove;
        indicesToRemove.reserve(size/2);

        for (int i = 0; i < size; ++i) {
            QString keyword = m_KeywordsList.at(i);

            if (!caseSensitive) {
                keyword = keyword.toLower();
            }

            if (keywordsToRemove.contains(keyword)) {
                indicesToRemove.append(i);
            }
        }

        bool anythingRemoved = !indicesToRemove.empty();
        removeKeywordsAtIndicesUnsafe(indicesToRemove);

        return anythingRemoved;
    }

    void BasicKeywordsModel::expandPresetUnsafe(int keywordsIndex, const QStringList &keywordsList) {
        Q_ASSERT((0 <= keywordsIndex) && (keywordsIndex < m_KeywordsList.size()));

        LOG_INFO << "index" << keywordsIndex << "list:" << keywordsList;
        QString removedKeyword;
        bool wasCorrect = false;

        beginRemoveRows(QModelIndex(), keywordsIndex, keywordsIndex);
        this->takeKeywordAtUnsafe(keywordsIndex, removedKeyword, wasCorrect);
        endRemoveRows();
        LOG_INFO << "replaced keyword" << removedKeyword;
        Q_UNUSED(wasCorrect);

        int addedCount = appendKeywordsUnsafe(keywordsList);
        LOG_INFO << addedCount << "new added";
    }

    void BasicKeywordsModel::removeKeywordsAtIndicesUnsafe(const QVector<int> &indices) {
        LOG_DEBUG << indices.size() << "item(s)";
        QVector<QPair<int, int> > rangesToRemove;
        Helpers::indicesToRanges(indices, rangesToRemove);
        AbstractListModel::removeItemsAtIndices(rangesToRemove);
    }

    bool BasicKeywordsModel::replaceInKeywordsUnsafe(const QString &replaceWhat, const QString &replaceTo,
                                                     Common::SearchFlags flags) {
        bool anyChanged = false;

        QVector<int> indicesToRemove;

        const bool caseSensitive = Common::HasFlag(flags, Common::SearchFlags::CaseSensitive);
        const bool wholeWords = Common::HasFlag(flags, Common::SearchFlags::WholeWords);
        const Qt::CaseSensitivity caseSensivity = caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

        const int size = m_KeywordsList.size();
        for (int i = 0; i < size; ++i) {
            QString internal = m_KeywordsList.at(i);
            const bool hasMatch = wholeWords ?
                                  Helpers::containsWholeWords(internal, replaceWhat, caseSensivity) :
                                  internal.contains(replaceWhat, caseSensivity);
            LOG_FOR_TESTS << "[" << internal << "] has match [" << replaceWhat << "] =" << hasMatch;

            if (hasMatch) {
                QString replaced = wholeWords ?
                                   Helpers::replaceWholeWords(internal, replaceWhat, replaceTo, caseSensivity) :
                                   internal.replace(replaceWhat, replaceTo, caseSensivity);

                QString replacement = Helpers::doSanitizeKeyword(replaced);

                if (!this->editKeywordUnsafe(i, replacement)) {
                    if (replacement.isEmpty()) {
                        LOG_INFO << "Replaced" << internal << "to empty";
                        indicesToRemove.append(i);
                    } else if (m_KeywordsSet.contains(replacement.toLower())) {
                        LOG_INFO << "Replacing" << internal << "to" << replacement << "creates a duplicate";
                        indicesToRemove.append(i);
                    }
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

    bool BasicKeywordsModel::containsKeyword(const QString &searchTerm, Common::SearchFlags searchFlags) {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return containsKeywordUnsafe(searchTerm, searchFlags);
    }

    bool BasicKeywordsModel::isEmpty() {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return m_KeywordsList.isEmpty();
    }

    bool BasicKeywordsModel::hasKeywordsSpellError() {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return hasKeywordsSpellErrorUnsafe();
    }

    bool BasicKeywordsModel::hasSpellErrors() {
        bool hasErrors = hasKeywordsSpellError();
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

    void BasicKeywordsModel::notifySpellCheckResults(Common::SpellCheckFlags flags) {
        if (Common::HasFlag(flags, Common::SpellCheckFlags::Description) ||
            Common::HasFlag(flags, Common::SpellCheckFlags::Title)) {
            emit spellCheckResultsReady();
        }

        emit spellCheckErrorsChanged();
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

    bool BasicKeywordsModel::canEditKeyword(int index, const QString &replacement) {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        return canEditKeywordUnsafe(index, replacement);
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

    void BasicKeywordsModel::setKeywordsSpellCheckResults(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items) {
        LOG_DEBUG << items.size() << "results";

        QWriteLocker writeLocker(&m_KeywordsLock);
        Q_UNUSED(writeLocker);

        setSpellCheckResultsUnsafe(items);
    }

    std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > BasicKeywordsModel::createKeywordsSuggestionsList() {
        QReadLocker readLocker(&m_KeywordsLock);

        Q_UNUSED(readLocker);

        std::vector<std::shared_ptr<SpellCheck::SpellSuggestionsItem> > spellCheckSuggestions;
        int length = m_KeywordsList.length();
        spellCheckSuggestions.reserve(length/2);

        for (int i = 0; i < length; ++i) {
            if (!m_SpellCheckResults[i]) {
                const QString &keyword = m_KeywordsList.at(i);
                LOG_DEBUG << keyword << "has wrong spelling";

                if (!keyword.contains(QChar::Space)) {
                    spellCheckSuggestions.emplace_back(new SpellCheck::KeywordSpellSuggestions(keyword, i));
                } else {
                    QStringList items = keyword.split(QChar::Space, QString::SkipEmptyParts);
                    foreach(const QString &item, items) {
                        spellCheckSuggestions.emplace_back(new SpellCheck::KeywordSpellSuggestions(item, i, keyword));
                    }
                }
            }
        }

        return spellCheckSuggestions;
    }

    Common::KeywordReplaceResult BasicKeywordsModel::fixKeywordSpelling(int index, const QString &existing, const QString &replacement) {
        Common::KeywordReplaceResult result;

        LOG_INFO << "Replacing" << existing << "to" << replacement << "with index" << index;

        m_KeywordsLock.lockForWrite();
        {
            if (0 <= index && index < m_KeywordsList.length()) {
                if (replaceKeywordUnsafe(index, existing, replacement)) {
                    m_SpellCheckResults[index] = true;
                    result = Common::KeywordReplaceResult::Succeeded;
                } else {
                    result = Common::KeywordReplaceResult::FailedDuplicate;
                }
            } else {
                LOG_INFO << "Failure. Index is negative or exceeds count" << m_KeywordsList.length();
                result = Common::KeywordReplaceResult::FailedIndex;
            }
        }
        m_KeywordsLock.unlock();

        if (result == Common::KeywordReplaceResult::Succeeded) {
            QModelIndex i = this->index(index);
            // combined roles from legacy editKeyword() and replace()
            emit dataChanged(i, i, QVector<int>() << KeywordRole << IsCorrectRole);
        }

        return result;
    }

    bool BasicKeywordsModel::processFailedKeywordReplacements(const
                                                              std::vector<std::shared_ptr<SpellCheck::KeywordSpellSuggestions> > &candidatesForRemoval)
    {
        LOG_INFO << candidatesForRemoval.size() << "candidates to remove";
        bool anyReplaced = false;

        if (candidatesForRemoval.empty()) {
            return anyReplaced;
        }

        QVector<int> indicesToRemove;
        size_t size = candidatesForRemoval.size();
        indicesToRemove.reserve((int)size);

        QWriteLocker writeLocker(&m_KeywordsLock);

        for (size_t i = 0; i < size; ++i) {
            auto &item = candidatesForRemoval.at(i);

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

    void BasicKeywordsModel::afterReplaceCallback() {
        LOG_DEBUG << "#";
        emit spellCheckErrorsChanged();
        emit afterSpellingErrorsFixed();
    }

    void BasicKeywordsModel::connectSignals(SpellCheck::SpellCheckItem *item) {
        QObject::connect(item, SIGNAL(resultsReady(Common::SpellCheckFlags, int)),
                         this, SLOT(spellCheckRequestReady(Common::SpellCheckFlags, int)));
    }

    void BasicKeywordsModel::spellCheckRequestReady(Common::SpellCheckFlags flags, int index) {
        if (Common::HasFlag(flags, Common::SpellCheckFlags::Keywords)) {
            emitSpellCheckChanged(index);
        }

        notifySpellCheckResults(flags);
    }

    void BasicKeywordsModel::setSpellCheckResultsUnsafe(const std::vector<std::shared_ptr<SpellCheck::SpellCheckQueryItem> > &items) {
        if (m_KeywordsList.length() != m_SpellCheckResults.length()) {
            LOG_INTEGRATION_TESTS << "Current keywords list length:" << m_KeywordsList.length();
            LOG_INTEGRATION_TESTS << "SpellCheck list length:" << m_SpellCheckResults.length();
        }

        // sync issue between adding/removing/undo/spellcheck
        Q_ASSERT(m_KeywordsList.length() == m_SpellCheckResults.length());

        const size_t size = items.size();
        const int keywordsLength = m_KeywordsList.length();

        // reset relative items
        for (size_t i = 0; i < size; ++i) {
            auto &item = items.at(i);
            int index = item->m_Index;
            if (0 <= index && index < keywordsLength) {
                m_SpellCheckResults[index] = true;
            }

            if (index >= keywordsLength) {
                break;
            }
        }

        for (size_t i = 0; i < size; ++i) {
            auto &item = items.at(i);
            int index = item->m_Index;
            Q_ASSERT(keywordsLength == m_KeywordsList.length());

            if (0 <= index && index < keywordsLength) {
                if (m_KeywordsList[index].contains(item->m_Word)) {
                    // if keyword contains several words, there would be
                    // several queryitems and there's error if any has error
                    m_SpellCheckResults[index] = m_SpellCheckResults[index] && item->m_IsCorrect;
                }
            }

            if (index >= keywordsLength) {
                break;
            }
        }
    }

    bool BasicKeywordsModel::isReplacedADuplicateUnsafe(int index, const QString &existingPrev,
                                                        const QString &replacement) const {
        bool isDuplicate = false;
        const QString &existingCurrent = m_KeywordsList.at(index);

        if (existingCurrent == existingPrev) {
            if (m_KeywordsSet.contains(replacement.toLower())) {
                isDuplicate = true;
                LOG_INFO << "safe to remove duplicate [" << existingCurrent << "] at index" << index;
            } else {
                LOG_INFO << replacement << "was not found";
            }
        } else if (existingCurrent.contains(existingPrev) && existingCurrent.contains(QChar::Space)) {
            QString existingFixed = existingCurrent;
            existingFixed.replace(existingPrev, replacement);

            if (m_KeywordsSet.contains(existingFixed.toLower())) {
                isDuplicate = true;
                LOG_INFO << "safe to remove composite duplicate [" << existingCurrent << "] at index" << index;
            } else {
                LOG_INFO << existingFixed << "was not found";
            }
        } else {
            LOG_INFO << existingCurrent << "is now instead of" << existingPrev << "at index" << index;
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
