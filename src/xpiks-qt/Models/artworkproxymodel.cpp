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

#include "artworkproxymodel.h"
#include <QImageReader>
#include "../Commands/commandmanager.h"
#include "../Warnings/warningsservice.h"
#include "imageartwork.h"
#include "../Models/artitemsmodel.h"

namespace Models {
    ArtworkProxyModel::ArtworkProxyModel(QObject *parent) :
        QObject(parent),
        ArtworkProxyBase(),
        m_ArtworkMetadata(nullptr),
        m_ArtworkOriginalIndex(-1)
    {
    }

    ArtworkProxyModel::~ArtworkProxyModel() {
        doResetModel();
    }

    void ArtworkProxyModel::setDescription(const QString &description)  {
        LOG_INFO << description;

        if (doSetDescription(description)) {
            signalDescriptionChanged();

            if (m_ArtworkMetadata->isInitialized()) {
                m_ArtworkMetadata->requestBackup();
            }
        }
    }

    void ArtworkProxyModel::setTitle(const QString &title) {
        LOG_INFO << title;

        if (doSetTitle(title)) {
            signalTitleChanged();

            if (m_ArtworkMetadata->isInitialized()) {
                m_ArtworkMetadata->requestBackup();
            }
        }
    }

    void ArtworkProxyModel::setKeywords(const QStringList &keywords) {
        ArtworkProxyBase::setKeywords(keywords);

        if (m_ArtworkMetadata->isInitialized()) {
            m_ArtworkMetadata->requestBackup();
        }
    }

    void ArtworkProxyModel::userDictUpdateHandler(const QStringList &keywords, bool overwritten) {
        LOG_DEBUG << "#";
        if (m_ArtworkMetadata != NULL) {
            doHandleUserDictChanged(keywords, overwritten);
        } else {
            LOG_DEBUG << "Nothing to update";
        }
    }

    void ArtworkProxyModel::userDictClearedHandler() {
        LOG_DEBUG << "#";
        if (m_ArtworkMetadata != NULL) {
            doHandleUserDictCleared();
        } else {
            LOG_DEBUG << "Nothing to update";
        }
    }

    void ArtworkProxyModel::afterSpellingErrorsFixedHandler() {
        LOG_DEBUG << "#";
#ifdef QT_DEBUG
        auto *basicModel = qobject_cast<Common::BasicMetadataModel*>(sender());
        Q_ASSERT(basicModel == getBasicMetadataModel());
#endif
        // if squeezing took place after replace
        signalKeywordsCountChanged();
    }

    void ArtworkProxyModel::spellCheckErrorsChangedHandler() {
        LOG_DEBUG << "#";
#ifdef QT_DEBUG
        auto *basicModel = qobject_cast<Common::BasicMetadataModel*>(sender());
        Q_ASSERT(basicModel == getBasicMetadataModel());
#endif

        emit descriptionChanged();
        emit titleChanged();
    }

    void ArtworkProxyModel::itemUnavailableHandler(int index) {
        LOG_DEBUG << "#";

        if ((index == m_ArtworkOriginalIndex) && (m_ArtworkOriginalIndex != -1)) {
            LOG_INFO << "Item is not available anymore" << index;
            emit itemBecomeUnavailable();
        }
    }

    void ArtworkProxyModel::editKeyword(int index, const QString &replacement) {
        doEditKeyword(index, replacement);
    }

    void ArtworkProxyModel::removeKeywordAt(int keywordIndex) {
        QString keyword;
        doRemoveKeywordAt(keywordIndex, keyword);
    }

    void ArtworkProxyModel::removeLastKeyword() {
        QString keyword;
        doRemoveLastKeyword(keyword);
    }

    void ArtworkProxyModel::appendKeyword(const QString &keyword) {
        doAppendKeyword(keyword);
    }

    void ArtworkProxyModel::pasteKeywords(const QStringList &keywords) {
        doAppendKeywords(keywords);
    }

    void ArtworkProxyModel::clearKeywords() {
        doClearKeywords();
    }

    QString ArtworkProxyModel::getKeywordsString() {
        return doGetKeywordsString();
    }

    void ArtworkProxyModel::suggestCorrections() {
        doSuggestCorrections();
    }

    void ArtworkProxyModel::initDescriptionHighlighting(QQuickTextDocument *document) {
        doInitDescriptionHighlighting(document);
    }

    void ArtworkProxyModel::initTitleHighlighting(QQuickTextDocument *document) {
        doInitTitleHighlighting(document);
    }

    void ArtworkProxyModel::spellCheckDescription() {
        doSpellCheckDescription();
    }

    void ArtworkProxyModel::spellCheckTitle() {
        doSpellCheckTitle();
    }

    void ArtworkProxyModel::plainTextEdit(const QString &rawKeywords, bool spaceIsSeparator) {
        doPlainTextEdit(rawKeywords, spaceIsSeparator);
    }

    bool ArtworkProxyModel::hasTitleWordSpellError(const QString &word) {
        return getHasTitleWordSpellError(word);
    }

    bool ArtworkProxyModel::hasDescriptionWordSpellError(const QString &word) {
        return getHasDescriptionWordSpellError(word);
    }

    void ArtworkProxyModel::setSourceArtwork(QObject *artworkMetadata, int originalIndex) {
        LOG_INFO << originalIndex;
        ArtworkMetadata *metadata = qobject_cast<ArtworkMetadata*>(artworkMetadata);
        Q_ASSERT(metadata != nullptr);

#ifdef QT_DEBUG
        if (originalIndex != -1) {
            auto *itemsModel = m_CommandManager->getArtItemsModel();
            Q_ASSERT(itemsModel->getArtwork(originalIndex) == metadata);
        }
#endif

        updateCurrentArtwork();
        disconnectCurrentArtwork();

        metadata->acquire();
        metadata->setIsLockedForEditing(true);
        m_ArtworkMetadata = metadata;
        m_ArtworkOriginalIndex = originalIndex;

        auto *keywordsModel = metadata->getBasicModel();
        QObject::connect(keywordsModel, SIGNAL(spellCheckErrorsChanged()),
                         this, SLOT(spellCheckErrorsChangedHandler()));

        QObject::connect(keywordsModel, SIGNAL(completionsAvailable()),
                         this, SIGNAL(completionsAvailable()));

        QObject::connect(keywordsModel, SIGNAL(afterSpellingErrorsFixed()),
                         this, SLOT(afterSpellingErrorsFixedHandler()));

        emit descriptionChanged();
        emit titleChanged();
        emit keywordsCountChanged();
        emit imagePathChanged();
    }

    void ArtworkProxyModel::resetModel() {
        LOG_DEBUG << "#";
        updateCurrentArtwork();
        doResetModel();
    }

    QSize ArtworkProxyModel::retrieveImageSize() const {
        ImageArtwork *image = dynamic_cast<ImageArtwork *>(m_ArtworkMetadata);

        if (image == NULL) {
            return QSize();
        }

        QSize size;

        if (image->isInitialized()) {
            size = image->getImageSize();
        } else {
            QImageReader reader(image->getFilepath());
            size = reader.size();
            // metadata->setSize(size);
        }

        return size;
    }

    QString ArtworkProxyModel::retrieveFileSize() const {
        double size = 0;

        if (m_ArtworkMetadata->isInitialized()) {
            size = m_ArtworkMetadata->getFileSize();
        } else {
            QFileInfo fi(m_ArtworkMetadata->getFilepath());
            size = fi.size(); // in bytes
        }

        size /= 1024.0*1024.0;

        QString sizeDescription;
        if (size >= 1) {
            sizeDescription = QString::number(size, 'f', 2) + QLatin1String(" MB");
        } else {
            size *= 1024;
            sizeDescription = QString::number(size, 'f', 2) + QLatin1String(" KB");
        }

        return sizeDescription;
    }

    QString ArtworkProxyModel::getDateTaken() const {
        ImageArtwork *image = dynamic_cast<ImageArtwork *>(m_ArtworkMetadata);
        if (image != NULL) {
            return image->getDateTaken();
        } else {
            return QLatin1String("");
        }
    }

    QString ArtworkProxyModel::getAttachedVectorPath() const {
        ImageArtwork *image = dynamic_cast<ImageArtwork *>(m_ArtworkMetadata);
        if (image != NULL) {
            return image->getAttachedVectorPath();
        } else {
            return QLatin1String("");
        }
    }

    void ArtworkProxyModel::expandPreset(int keywordIndex, int presetIndex) {
        doExpandPreset(keywordIndex, presetIndex);
    }

    void ArtworkProxyModel::expandLastKeywordAsPreset() {
        doExpandLastKeywordAsPreset();
    }

    void ArtworkProxyModel::addPreset(int presetIndex) {
        doAddPreset(presetIndex);
    }

    void ArtworkProxyModel::initSuggestion() {
        doInitSuggestion();
    }

    void ArtworkProxyModel::registerAsCurrentItem() {
        LOG_DEBUG << "#";
        doRegisterAsCurrentItem();
    }

    void ArtworkProxyModel::copyToQuickBuffer() {
        LOG_DEBUG << "#";
        doCopyToQuickBuffer();
    }

    void ArtworkProxyModel::updateCurrentArtwork() {
        LOG_DEBUG << "#";

        if (m_ArtworkOriginalIndex != -1) {
            m_CommandManager->updateArtworks(QVector<int>() << m_ArtworkOriginalIndex);
        }

        if (m_ArtworkMetadata != nullptr) {
            m_CommandManager->submitForWarningsCheck(m_ArtworkMetadata);
        }
    }

    void ArtworkProxyModel::doResetModel() {
        LOG_DEBUG << "#";
        disconnectCurrentArtwork();
        m_ArtworkOriginalIndex = -1;
    }

    void ArtworkProxyModel::disconnectCurrentArtwork() {
        LOG_DEBUG << "#";
        if (m_ArtworkMetadata != nullptr) {
            auto *basicModel = m_ArtworkMetadata->getBasicModel();
            basicModel->disconnect(this);
            this->disconnect(basicModel);
            m_ArtworkMetadata->setIsLockedForEditing(false);
            m_ArtworkMetadata->release();
            LOG_DEBUG << "Metadata released";
        }

        m_ArtworkMetadata = nullptr;
    }
}
