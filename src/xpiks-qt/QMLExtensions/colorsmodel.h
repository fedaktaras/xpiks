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

#ifndef COLORSMODEL_H
#define COLORSMODEL_H

#include <QObject>
#include <QColor>
#include <QJsonObject>
#include <QVector>
#include <QStringList>

namespace QMLExtensions {
    class ColorsModel : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QColor defaultDarkColor READ defaultDarkColor WRITE setDefaultDarkColor NOTIFY defaultDarkColorChanged)
        Q_PROPERTY(QColor defaultDarkerColor READ defaultDarkerColor WRITE setDefaultDarkerColor NOTIFY defaultDarkerColorChanged)
        Q_PROPERTY(QColor defaultControlColor READ defaultControlColor WRITE setDefaultControlColor NOTIFY defaultControlColorChanged)
        Q_PROPERTY(QColor whiteColor READ whiteColor WRITE setWhiteColor NOTIFY whiteColorChanged)

        Q_PROPERTY(QColor inputBackgroundColor READ inputBackgroundColor WRITE setInputBackgroundColor NOTIFY inputBackgroundColorChanged)
        Q_PROPERTY(QColor inputInactiveBackground READ inputInactiveBackground WRITE setInputInactiveBackground NOTIFY inputInactiveBackgroundChanged)
        Q_PROPERTY(QColor inputForegroundColor READ inputForegroundColor WRITE setInputForegroundColor NOTIFY inputForegroundColorChanged)
        Q_PROPERTY(QColor inputInactiveForeground READ inputInactiveForeground WRITE setInputInactiveForeground NOTIFY inputInactiveForegroundChanged)
        Q_PROPERTY(QColor labelActiveForeground READ labelActiveForeground WRITE setLabelActiveForeground NOTIFY labelActiveForegroundChanged)
        Q_PROPERTY(QColor labelInactiveForeground READ labelInactiveForeground WRITE setLabelInactiveForeground NOTIFY labelInactiveForegroundChanged)

        Q_PROPERTY(QColor artworkBackground READ artworkBackground WRITE setArtworkBackground NOTIFY artworkBackgroundChanged)
        Q_PROPERTY(QColor artworkImageBackground READ artworkImageBackground WRITE setArtworkImageBackground NOTIFY artworkImageBackgroundChanged)
        Q_PROPERTY(QColor artworkModifiedColor READ artworkModifiedColor WRITE setArtworkModifiedColor NOTIFY artworkModifiedColorChanged)
        Q_PROPERTY(QColor artworkSavedColor READ artworkSavedColor WRITE setArtworkSavedColor NOTIFY artworkSavedColorChanged)
        Q_PROPERTY(QColor artworkActiveColor READ artworkActiveColor WRITE setArtworkActiveColor NOTIFY artworkActiveColorChanged)

        Q_PROPERTY(QColor defaultLightColor READ defaultLightColor WRITE setDefaultLightColor NOTIFY defaultLightColorChanged)
        Q_PROPERTY(QColor defaultLightGrayColor READ defaultLightGrayColor WRITE setDefaultLightGrayColor NOTIFY defaultLightGrayColorChanged)

        Q_PROPERTY(QColor buttonHoverBackground READ buttonHoverBackground WRITE setButtonHoverBackground NOTIFY buttonHoverBackgroundChanged)
        Q_PROPERTY(QColor buttonPressedBackground READ buttonPressedBackground WRITE setButtonPressedBackground NOTIFY buttonPressedBackgroundChanged)
        Q_PROPERTY(QColor buttonPressedForeground READ buttonPressedForeground WRITE setButtonPressedForeground NOTIFY buttonPressedForegroundChanged)
        Q_PROPERTY(QColor buttonDisabledForeground READ buttonDisabledForeground WRITE setButtonDisabledForeground NOTIFY buttonDisabledForegroundChanged)

        Q_PROPERTY(QColor linkClickedColor READ linkClickedColor WRITE setLinkClickedColor NOTIFY linkClickedColorChanged)

        Q_PROPERTY(QColor selectedImageBackground READ selectedImageBackground WRITE setSelectedImageBackground NOTIFY selectedImageBackgroundChanged)
        Q_PROPERTY(QColor selectedArtworkBackground READ selectedArtworkBackground WRITE setSelectedArtworkBackground NOTIFY selectedArtworkBackgroundChanged)
        Q_PROPERTY(QColor checkboxCheckedColor READ checkboxCheckedColor WRITE setCheckboxCheckedColor NOTIFY checkboxCheckedColorChanged)

        Q_PROPERTY(QColor itemsSourceBackground READ itemsSourceBackground WRITE setItemsSourceBackground NOTIFY itemsSourceBackgroundChanged)
        Q_PROPERTY(QColor itemsSourceSelected READ itemsSourceSelected WRITE setItemsSourceSelected NOTIFY itemsSourceSelectedChanged)
        Q_PROPERTY(QColor itemsSourceForeground READ itemsSourceForeground WRITE setItemsSourceForeground NOTIFY itemsSourceForegroundChanged)

        Q_PROPERTY(QColor destructiveColor READ destructiveColor WRITE setDestructiveColor NOTIFY destructiveColorChanged)
        Q_PROPERTY(QColor greenColor READ greenColor WRITE setGreenColor NOTIFY greenColorChanged)


        // AUTOGENERATED
        QColor m_defaultDarkColor;
        QColor m_defaultDarkerColor;
        QColor m_defaultControlColor;
        QColor m_whiteColor;
        QColor m_inputBackgroundColor;
        QColor m_inputInactiveBackground;
        QColor m_inputForegroundColor;
        QColor m_inputInactiveForeground;
        QColor m_labelActiveForeground;
        QColor m_labelInactiveForeground;
        QColor m_artworkBackground;
        QColor m_artworkImageBackground;
        QColor m_artworkModifiedColor;
        QColor m_artworkSavedColor;
        QColor m_artworkActiveColor;
        QColor m_defaultLightColor;
        QColor m_defaultLightGrayColor;
        QColor m_buttonHoverBackground;
        QColor m_buttonPressedBackground;
        QColor m_buttonPressedForeground;
        QColor m_buttonDisabledForeground;
        QColor m_linkClickedColor;
        QColor m_selectedImageBackground;
        QColor m_selectedArtworkBackground;
        QColor m_checkboxCheckedColor;
        QColor m_itemsSourceBackground;
        QColor m_itemsSourceSelected;
        QColor m_itemsSourceForeground;
        QColor m_destructiveColor;
        QColor m_greenColor;

        QVector<QJsonObject> m_RegisteredThemes;
        QStringList m_ThemeNames;

    public:
        explicit ColorsModel(QObject *parent = 0);

        Q_INVOKABLE void registerTheme(const QJsonObject &theme);
        Q_INVOKABLE bool applyTheme(int index);
        Q_INVOKABLE QStringList getThemeNames() const { return m_ThemeNames; }

        QColor defaultDarkColor() const
        {
            return m_defaultDarkColor;
        }

        QColor defaultControlColor() const
        {
            return m_defaultControlColor;
        }

        QColor whiteColor() const
        {
            return m_whiteColor;
        }

        QColor inputBackgroundColor() const
        {
            return m_inputBackgroundColor;
        }

        QColor inputInactiveBackground() const
        {
            return m_inputInactiveBackground;
        }

        QColor inputForegroundColor() const
        {
            return m_inputForegroundColor;
        }

        QColor inputInactiveForeground() const
        {
            return m_inputInactiveForeground;
        }

        QColor labelActiveForeground() const
        {
            return m_labelActiveForeground;
        }

        QColor labelInactiveForeground() const
        {
            return m_labelInactiveForeground;
        }

        QColor artworkBackground() const
        {
            return m_artworkBackground;
        }

        QColor artworkImageBackground() const
        {
            return m_artworkImageBackground;
        }

        QColor artworkModifiedColor() const
        {
            return m_artworkModifiedColor;
        }

        QColor artworkSavedColor() const
        {
            return m_artworkSavedColor;
        }

        QColor artworkActiveColor() const
        {
            return m_artworkActiveColor;
        }

        QColor defaultLightColor() const
        {
            return m_defaultLightColor;
        }

        QColor defaultLightGrayColor() const
        {
            return m_defaultLightGrayColor;
        }

        QColor buttonHoverBackground() const
        {
            return m_buttonHoverBackground;
        }

        QColor buttonPressedBackground() const
        {
            return m_buttonPressedBackground;
        }

        QColor buttonPressedForeground() const
        {
            return m_buttonPressedForeground;
        }

        QColor buttonDisabledForeground() const
        {
            return m_buttonDisabledForeground;
        }

        QColor linkClickedColor() const
        {
            return m_linkClickedColor;
        }

        QColor selectedImageBackground() const
        {
            return m_selectedImageBackground;
        }

        QColor selectedArtworkBackground() const
        {
            return m_selectedArtworkBackground;
        }

        QColor checkboxCheckedColor() const
        {
            return m_checkboxCheckedColor;
        }

        QColor itemsSourceBackground() const
        {
            return m_itemsSourceBackground;
        }

        QColor itemsSourceSelected() const
        {
            return m_itemsSourceSelected;
        }

        QColor itemsSourceForeground() const
        {
            return m_itemsSourceForeground;
        }

        QColor destructiveColor() const
        {
            return m_destructiveColor;
        }

        QColor greenColor() const
        {
            return m_greenColor;
        }

        QColor defaultDarkerColor() const
        {
            return m_defaultDarkerColor;
        }

    signals:
        void themeChanged();
        void defaultDarkColorChanged(QColor defaultDarkColor);
        void defaultControlColorChanged(QColor defaultControlColor);
        void whiteColorChanged(QColor whiteColor);
        void inputBackgroundColorChanged(QColor inputBackgroundColor);
        void inputInactiveBackgroundChanged(QColor inputInactiveBackground);
        void inputForegroundColorChanged(QColor inputForegroundColor);
        void inputInactiveForegroundChanged(QColor inputInactiveForeground);
        void labelActiveForegroundChanged(QColor labelActiveForeground);
        void labelInactiveForegroundChanged(QColor labelInactiveForeground);
        void artworkBackgroundChanged(QColor artworkBackground);
        void artworkImageBackgroundChanged(QColor artworkImageBackground);
        void artworkModifiedColorChanged(QColor artworkModifiedColor);
        void artworkSavedColorChanged(QColor artworkSavedColor);
        void artworkActiveColorChanged(QColor artworkActiveColor);
        void defaultLightColorChanged(QColor defaultLightColor);
        void defaultLightGrayColorChanged(QColor defaultLightGrayColor);
        void buttonHoverBackgroundChanged(QColor buttonHoverBackground);
        void buttonPressedBackgroundChanged(QColor buttonPressedBackground);
        void buttonPressedForegroundChanged(QColor buttonPressedForeground);
        void buttonDisabledForegroundChanged(QColor buttonDisabledForeground);
        void linkClickedColorChanged(QColor linkClickedColor);
        void selectedImageBackgroundChanged(QColor selectedImageBackground);
        void selectedArtworkBackgroundChanged(QColor selectedArtworkBackground);
        void checkboxCheckedColorChanged(QColor checkboxCheckedColor);
        void itemsSourceBackgroundChanged(QColor itemsSourceBackground);
        void itemsSourceSelectedChanged(QColor itemsSourceSelected);
        void itemsSourceForegroundChanged(QColor itemsSourceForeground);
        void destructiveColorChanged(QColor destructiveColor);
        void greenColorChanged(QColor greenColor);        
        void defaultDarkerColorChanged(QColor defaultDarkerColor);

    public slots:
        void setDefaultDarkColor(QColor defaultDarkColor)
        {
            if (m_defaultDarkColor == defaultDarkColor)
                return;

            m_defaultDarkColor = defaultDarkColor;
            emit defaultDarkColorChanged(defaultDarkColor);
        }
        void setDefaultControlColor(QColor defaultControlColor)
        {
            if (m_defaultControlColor == defaultControlColor)
                return;

            m_defaultControlColor = defaultControlColor;
            emit defaultControlColorChanged(defaultControlColor);
        }
        void setWhiteColor(QColor whiteColor)
        {
            if (m_whiteColor == whiteColor)
                return;

            m_whiteColor = whiteColor;
            emit whiteColorChanged(whiteColor);
        }
        void setInputBackgroundColor(QColor inputBackgroundColor)
        {
            if (m_inputBackgroundColor == inputBackgroundColor)
                return;

            m_inputBackgroundColor = inputBackgroundColor;
            emit inputBackgroundColorChanged(inputBackgroundColor);
        }
        void setInputInactiveBackground(QColor inputInactiveBackground)
        {
            if (m_inputInactiveBackground == inputInactiveBackground)
                return;

            m_inputInactiveBackground = inputInactiveBackground;
            emit inputInactiveBackgroundChanged(inputInactiveBackground);
        }
        void setInputForegroundColor(QColor inputForegroundColor)
        {
            if (m_inputForegroundColor == inputForegroundColor)
                return;

            m_inputForegroundColor = inputForegroundColor;
            emit inputForegroundColorChanged(inputForegroundColor);
        }
        void setInputInactiveForeground(QColor inputInactiveForeground)
        {
            if (m_inputInactiveForeground == inputInactiveForeground)
                return;

            m_inputInactiveForeground = inputInactiveForeground;
            emit inputInactiveForegroundChanged(inputInactiveForeground);
        }
        void setLabelActiveForeground(QColor labelActiveForeground)
        {
            if (m_labelActiveForeground == labelActiveForeground)
                return;

            m_labelActiveForeground = labelActiveForeground;
            emit labelActiveForegroundChanged(labelActiveForeground);
        }
        void setLabelInactiveForeground(QColor labelInactiveForeground)
        {
            if (m_labelInactiveForeground == labelInactiveForeground)
                return;

            m_labelInactiveForeground = labelInactiveForeground;
            emit labelInactiveForegroundChanged(labelInactiveForeground);
        }
        void setArtworkBackground(QColor artworkBackground)
        {
            if (m_artworkBackground == artworkBackground)
                return;

            m_artworkBackground = artworkBackground;
            emit artworkBackgroundChanged(artworkBackground);
        }
        void setArtworkImageBackground(QColor artworkImageBackground)
        {
            if (m_artworkImageBackground == artworkImageBackground)
                return;

            m_artworkImageBackground = artworkImageBackground;
            emit artworkImageBackgroundChanged(artworkImageBackground);
        }
        void setArtworkModifiedColor(QColor artworkModifiedColor)
        {
            if (m_artworkModifiedColor == artworkModifiedColor)
                return;

            m_artworkModifiedColor = artworkModifiedColor;
            emit artworkModifiedColorChanged(artworkModifiedColor);
        }
        void setArtworkSavedColor(QColor artworkSavedColor)
        {
            if (m_artworkSavedColor == artworkSavedColor)
                return;

            m_artworkSavedColor = artworkSavedColor;
            emit artworkSavedColorChanged(artworkSavedColor);
        }
        void setArtworkActiveColor(QColor artworkActiveColor)
        {
            if (m_artworkActiveColor == artworkActiveColor)
                return;

            m_artworkActiveColor = artworkActiveColor;
            emit artworkActiveColorChanged(artworkActiveColor);
        }
        void setDefaultLightColor(QColor defaultLightColor)
        {
            if (m_defaultLightColor == defaultLightColor)
                return;

            m_defaultLightColor = defaultLightColor;
            emit defaultLightColorChanged(defaultLightColor);
        }
        void setDefaultLightGrayColor(QColor defaultLightGrayColor)
        {
            if (m_defaultLightGrayColor == defaultLightGrayColor)
                return;

            m_defaultLightGrayColor = defaultLightGrayColor;
            emit defaultLightGrayColorChanged(defaultLightGrayColor);
        }
        void setButtonHoverBackground(QColor buttonHoverBackground)
        {
            if (m_buttonHoverBackground == buttonHoverBackground)
                return;

            m_buttonHoverBackground = buttonHoverBackground;
            emit buttonHoverBackgroundChanged(buttonHoverBackground);
        }
        void setButtonPressedBackground(QColor buttonPressedBackground)
        {
            if (m_buttonPressedBackground == buttonPressedBackground)
                return;

            m_buttonPressedBackground = buttonPressedBackground;
            emit buttonPressedBackgroundChanged(buttonPressedBackground);
        }
        void setButtonPressedForeground(QColor buttonPressedForeground)
        {
            if (m_buttonPressedForeground == buttonPressedForeground)
                return;

            m_buttonPressedForeground = buttonPressedForeground;
            emit buttonPressedForegroundChanged(buttonPressedForeground);
        }
        void setButtonDisabledForeground(QColor buttonDisabledForeground)
        {
            if (m_buttonDisabledForeground == buttonDisabledForeground)
                return;

            m_buttonDisabledForeground = buttonDisabledForeground;
            emit buttonDisabledForegroundChanged(buttonDisabledForeground);
        }
        void setLinkClickedColor(QColor linkClickedColor)
        {
            if (m_linkClickedColor == linkClickedColor)
                return;

            m_linkClickedColor = linkClickedColor;
            emit linkClickedColorChanged(linkClickedColor);
        }
        void setSelectedImageBackground(QColor selectedImageBackground)
        {
            if (m_selectedImageBackground == selectedImageBackground)
                return;

            m_selectedImageBackground = selectedImageBackground;
            emit selectedImageBackgroundChanged(selectedImageBackground);
        }
        void setSelectedArtworkBackground(QColor selectedArtworkBackground)
        {
            if (m_selectedArtworkBackground == selectedArtworkBackground)
                return;

            m_selectedArtworkBackground = selectedArtworkBackground;
            emit selectedArtworkBackgroundChanged(selectedArtworkBackground);
        }
        void setCheckboxCheckedColor(QColor checkboxCheckedColor)
        {
            if (m_checkboxCheckedColor == checkboxCheckedColor)
                return;

            m_checkboxCheckedColor = checkboxCheckedColor;
            emit checkboxCheckedColorChanged(checkboxCheckedColor);
        }
        void setItemsSourceBackground(QColor itemsSourceBackground)
        {
            if (m_itemsSourceBackground == itemsSourceBackground)
                return;

            m_itemsSourceBackground = itemsSourceBackground;
            emit itemsSourceBackgroundChanged(itemsSourceBackground);
        }
        void setItemsSourceSelected(QColor itemsSourceSelected)
        {
            if (m_itemsSourceSelected == itemsSourceSelected)
                return;

            m_itemsSourceSelected = itemsSourceSelected;
            emit itemsSourceSelectedChanged(itemsSourceSelected);
        }
        void setItemsSourceForeground(QColor itemsSourceForeground)
        {
            if (m_itemsSourceForeground == itemsSourceForeground)
                return;

            m_itemsSourceForeground = itemsSourceForeground;
            emit itemsSourceForegroundChanged(itemsSourceForeground);
        }
        void setDestructiveColor(QColor destructiveColor)
        {
            if (m_destructiveColor == destructiveColor)
                return;

            m_destructiveColor = destructiveColor;
            emit destructiveColorChanged(destructiveColor);
        }
        void setGreenColor(QColor greenColor)
        {
            if (m_greenColor == greenColor)
                return;

            m_greenColor = greenColor;
            emit greenColorChanged(greenColor);
        }
        void setDefaultDarkerColor(QColor defaultDarkerColor)
        {
            if (m_defaultDarkerColor == defaultDarkerColor)
                return;

            m_defaultDarkerColor = defaultDarkerColor;
            emit defaultDarkerColorChanged(defaultDarkerColor);
        }
    };
}

#endif // COLORSMODEL_H