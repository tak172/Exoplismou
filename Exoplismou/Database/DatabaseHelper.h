/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CDatabaseManager.h
    @Created:		29.09.2024
    @Programmer:	Timofey Kromachev

    Data base helper classes. */

#ifndef __DATABASE_HELPER_H__
#define __DATABASE_HELPER_H__

#include "../../HelpfulDataToString/HelpfulDataToString.h"

namespace DatabaseProcessing
{
    struct SUserData
    {
        unsigned _id;
        QString _login;
        QString _password;
        QString _phone;
        QString _fullName;
        EUserStatus _status;
        QByteArray _photo;

        bool operator==( const SUserData & guest ) const;
    };

    struct SRequestData
    {
        unsigned _id;
        QDate _startDate;
        QDate _finishDate;
        QString _descr;
        unsigned _masterID;
        unsigned _clientID;
        unsigned _modelID;
        ERequestStatus _status;
    };

    struct SDeviceModel
    {
        unsigned _id;
        QString _name;
        unsigned _typeID;
    };

    struct SDeviceType
    {
        unsigned _id;
        QString _name;
    };
}

#endif // !__DATABASE_HELPER_H__