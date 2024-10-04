/*
	Exoplismou source code. Tak172. 2024.

	@Name:			Global.h
	@Created:		30.09.2024
	@Programmer:	Timofey Kromachev

	Variables for external access to system elements. */

#pragma once

namespace DatabaseProcessing
{
	struct SUserData;
}

/*[
	Global variables
]*/

extern std::shared_ptr<DatabaseProcessing::SUserData> _currentUser; // Currently logged in user