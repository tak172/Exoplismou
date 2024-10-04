#include "pch.h"
#include "HelpfulDataToString.h"

std::wstring DatabaseProcessing::StringFromUserStatus( EUserStatus status )
{
    switch ( status )
    {
        case DatabaseProcessing::EUserStatus::MANAGER:
        return L"��������";
        case DatabaseProcessing::EUserStatus::MASTER:
        return  L"������";
        case DatabaseProcessing::EUserStatus::OPERATOR:
        return L"��������";
        case DatabaseProcessing::EUserStatus::CUSTOMER:
        return L"��������";
    }

    return L"";
}

std::wstring DatabaseProcessing::StringFromRequestStatus( ERequestStatus status )
{
    switch ( status )
    {
        case DatabaseProcessing::ERequestStatus::NEW:
        return  L"�����";
        case DatabaseProcessing::ERequestStatus::ATWORK:
        return L"� ������";
        case DatabaseProcessing::ERequestStatus::COMPLETED:
        return L"��������";
        case DatabaseProcessing::ERequestStatus::CANCELED:
        return  L"�������";
    }

    return L"";
}