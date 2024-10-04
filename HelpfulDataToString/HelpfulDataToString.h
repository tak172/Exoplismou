#pragma once

#ifdef HELPFULDATATOSTRING
#define HELPFULDATA_API __declspec(dllexport)
#else
#define HELPFULDATA_API __declspec(dllimport)
#endif

#include <string>

namespace DatabaseProcessing
{
    enum class EUserStatus
    {
        MANAGER,
        MASTER,
        OPERATOR,
        CUSTOMER
    };

    HELPFULDATA_API std::wstring StringFromUserStatus(EUserStatus status);

    enum class ERequestStatus
    {
        ALL,
        NEW,
        ATWORK,
        COMPLETED,
        CANCELED
    };

    HELPFULDATA_API std::wstring StringFromRequestStatus( ERequestStatus status );
}