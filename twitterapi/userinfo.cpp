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




#include "userinfo.h"



UserInfo::UserInfo() :
    id(0),
    name(QString()),
    screenName(QString()),
    location(QString()),
    description(QString()),
    homepage(QString()),
    profileProtected(false),
    followersCount(0),
    friendsCount(0),
    createdAt(QDateTime()),
    utcOffset(0),
    timeZone(QString()),
    statusesCount(0),
    notifications(false),
    following(false),
    currentStatus(QString())
{
}

