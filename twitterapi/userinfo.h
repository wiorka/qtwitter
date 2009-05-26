/***************************************************************************
 *   Copyright (C) 2009 by Anna Nowak           <wiorka@gmail.com>         *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of      *
 *   the License, or (at your option) any later version.                   *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to                     *
 *   the Free Software Foundation, Inc.,                                   *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/




#ifndef USERINFO_H
#define USERINFO_H

#include <QMetaType>
#include <QDateTime>
#include "twitterapi_global.h"

struct TWITTERAPI_EXPORT UserInfo
{
    UserInfo();

    int id;                           //!< User ID
    QString name;                     //!< User's real name
    QString screenName;               //!< User's screen name/login
    QString location;                 //!< User's location
    QString description;              //!< User's profile description
    QString imageUrl;                //!< Profile image url/link to user's avatar
    QString homepage;                 //!< User's homepage url
    bool profileProtected;            //!< Indicates whether user's statuses are protected or publicly viewable
    int followersCount;               //!< Number of followers
    int friendsCount;                 //!< Number of friends
    //TODO: check if createdAt really means what is written here
    QDateTime createdAt;              //!< Profile/account creation date
    //favourites count
    int utcOffset;                    //!< User's local time's offset from UTC
    //TODO: check how/if Qt handles timezones
    QString timeZone;                 //!< User's timezone
    int statusesCount;                //!< Number of statuses
    //TODO: check below fields
    bool notifications;
    bool following;
    QString currentStatus;            //!< User's current status
};


Q_DECLARE_METATYPE(UserInfo)


#endif // USERINFO_H