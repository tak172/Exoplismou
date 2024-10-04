/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CDatabaseManager.h
    @Created:		29.09.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "DatabaseHelper.h"

bool DatabaseProcessing::SUserData::operator==( const SUserData & guest ) const
{
    return _id == guest._id && _login == guest._login &&
        _password == guest._password && _phone == guest._phone &&
        _fullName == guest._fullName && _status == guest._status &&
        _photo == guest._photo;
}
