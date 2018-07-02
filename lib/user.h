/******************************************************************************
 * Copyright (C) 2015 Felix Rohrbach <kde@fxrh.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include <QtCore/QString>
#include <QtCore/QObject>
#include "avatar.h"

namespace QMatrixClient
{
    class Connection;
    class Room;
    class RoomMemberEvent;

    class User: public QObject
    {
            Q_OBJECT
            Q_PROPERTY(QString id READ id CONSTANT)
            Q_PROPERTY(bool isGuest READ isGuest CONSTANT)
            Q_PROPERTY(QString name READ name NOTIFY nameChanged)
            Q_PROPERTY(QString displayName READ displayname NOTIFY nameChanged STORED false)
            Q_PROPERTY(QString fullName READ fullName NOTIFY nameChanged STORED false)
            Q_PROPERTY(QString bridgeName READ bridged NOTIFY nameChanged STORED false)
            Q_PROPERTY(QString avatarMediaId READ avatarMediaId NOTIFY avatarChanged STORED false)
            Q_PROPERTY(QUrl avatarUrl READ avatarUrl NOTIFY avatarChanged)
        public:
            User(QString userId, Connection* connection);
            ~User() override;

            Connection* connection() const;

            /** Get unique stable user id
             * User id is generated by the server and is not changed ever.
             */
            QString id() const;

            /** Get the name chosen by the user
             * This may be empty if the user didn't choose the name or cleared
             * it. If the user is bridged, the bridge postfix (such as '(IRC)')
             * is stripped out. No disambiguation for the room is done.
             * \sa displayName, rawName
             */
            QString name(const Room* room = nullptr) const;

            /** Get the user name along with the bridge postfix
             * This function is similar to name() but appends the bridge postfix
             * (such as '(IRC)') to the user name. No disambiguation is done.
             * \sa name, displayName
             */
            QString rawName(const Room* room = nullptr) const;

            /** Get the displayed user name
             * When \p room is null, this method returns result of name() if
             * the name is non-empty; otherwise it returns user id.
             * When \p room is non-null, this call is equivalent to
             * Room::roomMembername invocation for the user (i.e. the user's
             * disambiguated room-specific name is returned).
             * \sa name, id, fullName, Room::roomMembername
             */
            QString displayname(const Room* room = nullptr) const;

            /** Get user name and id in one string
             * The constructed string follows the format 'name (id)'
             * which the spec recommends for users disambiguation in
             * a room context and in other places.
             * \sa displayName, Room::roomMembername
             */
            QString fullName(const Room* room = nullptr) const;

            /**
             * Returns the name of bridge the user is connected from or empty.
            */
            QString bridged() const;

            /** Whether the user is a guest
             * As of now, the function relies on the convention used in Synapse
             * that guests and only guests have all-numeric IDs. This may or
             * may not work with non-Synapse servers.
             */
            bool isGuest() const;

            const Avatar& avatarObject(const Room* room = nullptr) const;
            Q_INVOKABLE QImage avatar(int dimension, const Room* room = nullptr);
            Q_INVOKABLE QImage avatar(int requestedWidth, int requestedHeight,
                                      const Room* room = nullptr);
            QImage avatar(int width, int height, const Room* room,
                          Avatar::get_callback_t callback);

            QString avatarMediaId(const Room* room = nullptr) const;
            QUrl avatarUrl(const Room* room = nullptr) const;

            void processEvent(const RoomMemberEvent& event, const Room* r);

        public slots:
            /** Set a new name in the global user profile */
            void rename(const QString& newName);
            /** Set a new name for the user in one room */
            void rename(const QString& newName, const Room* r);
            /** Upload the file and use it as an avatar */
            bool setAvatar(const QString& fileName);
            /** Upload contents of the QIODevice and set that as an avatar */
            bool setAvatar(QIODevice* source);
            /** Create or find a direct chat with this user
             * The resulting chat is returned asynchronously via
             * Connection::directChatAvailable()
             */
            void requestDirectChat();
            /** Add the user to the ignore list */
            void ignore();
            /** Remove the user from the ignore list */
            void unmarkIgnore();

        signals:
            void nameAboutToChange(QString newName, QString oldName,
                                   const Room* roomContext);
            void nameChanged(QString newName, QString oldName,
                             const Room* roomContext);
            void avatarChanged(User* user, const Room* roomContext);

        private slots:
            void updateName(const QString& newName, const Room* room = nullptr);
            void updateName(const QString& newName, const QString& oldName,
                            const Room* room = nullptr);
            void updateAvatarUrl(const QUrl& newUrl, const QUrl& oldUrl,
                                 const Room* room = nullptr);

        private:
            class Private;
            QScopedPointer<Private> d;
    };
}
Q_DECLARE_METATYPE(QMatrixClient::User*)
