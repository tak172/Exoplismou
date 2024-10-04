/*
    Exoplismou source code. Tak172. 2024.

    @Name:			Global.cpp
    @Created:		30.09.2024
    @Programmer:	Timofey Kromachev

    Global variables. */

#include "stdafx.h"

namespace DatabaseProcessing
{
    struct SUserData;
}

/*[
    Global variables
]*/

std::shared_ptr<DatabaseProcessing::SUserData> _currentUser; // Currently logged in user