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

#ifndef ICOMMANDBASE_H
#define ICOMMANDBASE_H

#include <memory>
#include <QtGlobal>

namespace Commands {
    class ICommandManager;

    class ICommandResult {
    public:
        virtual ~ICommandResult() {}
        virtual void afterExecCallback(const ICommandManager *commandManager) const { Q_UNUSED(commandManager); }
        virtual int getStatus() const = 0;
    };

    class ICommandBase {
    public:
        virtual ~ICommandBase() {}

        virtual std::shared_ptr<ICommandResult> execute(const ICommandManager *commandManager) const = 0;
        virtual int getCommandType() const = 0;
    };
}

#endif // ICOMMANDBASE_H
