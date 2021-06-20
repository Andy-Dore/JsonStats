#pragma once
#include <string>
#include <unordered_map>

//Return values for CJumpCloudJsonStats
enum JumpCloudJsonState
{
	OK = 0,
	INVALIDJSON,
	NOACTIONS,
	NOTFOUND,
	UNKNOWN
};

//create Action struct with operator == so it can be added to unordered_map
struct Action
{
	std::string actionName;
	std::uint32_t count;
	std::uint32_t time;
	Action() {};
	Action(std::string inName, std::uint32_t inCount, std::uint32_t inSum)
	{
		actionName = inName;
		count = inCount;
		time = inSum;
	}

	bool operator==(const Action& A) const
	{
		return actionName == A.actionName;
	}
};

//hash is required for look ups in unordered_map
class ActionHash
{
public:

	size_t operator()(const Action& a) const
	{
		return (std::hash<std::string>()(a.actionName)); 
	}

};

/// <summary>
/// 
/// Class implements taking a simple JSON entry converts to Action struct
/// Then the Action struct is added to unordered_map with key pair <Action, uint32_t>
/// The uint32_t is the avg of time field from the Action struct.  Leveraging
/// unordered_map will hopefully speed getStats() calls.
/// 
/// Calling addAction and getStats are multithread with critical section
/// protection on updating unorder_map or printing off avgs
/// 
/// </summary>
class CJumpCloudJsonStats
{
private:
	std::unordered_map<Action, std::uint32_t, ActionHash> actionHash;
	JumpCloudJsonState updateActionHash(Action NewAction);
	JumpCloudJsonState getActionByName(std::string actionName, Action &outAction);

public:
	CJumpCloudJsonStats(std::string InputJson);
	JumpCloudJsonState addAction(std::string InputJson);
	std::string getStats();

	/// <summary>
	/// Get number of Actions in unordered_map
	/// </summary>
	/// <returns>Number of Action in unorder_map</returns>
	size_t getAllActionCount() { return actionHash.size(); }

	//added these public members in so callers can valide actions are as expected
	//this was done to mitigate that lack of Json support nativaly in C++
	JumpCloudJsonState getActionAvgByActionName(std::string actionName, uint32_t &returnedAvg);
	JumpCloudJsonState getActionTimeByActionName(std::string actionName, uint32_t &returnedTime);
	JumpCloudJsonState getActionCountByActionName(std::string actionName, uint32_t &returnedSum);

};

