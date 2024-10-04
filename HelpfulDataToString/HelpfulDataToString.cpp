#include "pch.h"
#include "HelpfulDataToString.h"

std::wstring DatabaseProcessing::StringFromUserStatus( EUserStatus status )
{
    switch ( status )
    {
        case DatabaseProcessing::EUserStatus::MANAGER:
        return L"Менеджер";
        case DatabaseProcessing::EUserStatus::MASTER:
        return  L"Мастер";
        case DatabaseProcessing::EUserStatus::OPERATOR:
        return L"Оператор";
        case DatabaseProcessing::EUserStatus::CUSTOMER:
        return L"Заказчик";
    }

    return L"";
}

std::wstring DatabaseProcessing::StringFromRequestStatus( ERequestStatus status )
{
    switch ( status )
    {
        case DatabaseProcessing::ERequestStatus::NEW:
        return  L"Новый";
        case DatabaseProcessing::ERequestStatus::ATWORK:
        return L"В работе";
        case DatabaseProcessing::ERequestStatus::COMPLETED:
        return L"Завершен";
        case DatabaseProcessing::ERequestStatus::CANCELED:
        return  L"Отменен";
    }

    return L"";
}