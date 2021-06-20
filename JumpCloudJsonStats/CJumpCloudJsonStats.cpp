#include "CJumpCloudJsonStats.h"
#include <string>
#include <mutex>
#include <iterator>

std::mutex ActionMutex;

/// <summary>
/// Constructor requires a simple, single line json entry with the following format
/// {"action":"jump", "time":100} --> there is no native JSON support so my class
/// doesn't support spaces, etc.
/// </summary>
/// <param name="InputJson">example of expected format 
/// {\"action\":\"jump\",\"time\":100}
/// </param>
CJumpCloudJsonStats::CJumpCloudJsonStats(std::string InputJson)
{
	addAction(InputJson);
}

/// <summary>
/// This function does some very simple validation of the JSON input.  There is 
/// no native support for JSON in C++ so in the spirit of not taking a lot of dependencies
/// I opted for simple validation.
/// </summary>
/// <param name="InputJson">example of expected format 
/// {\"action\":\"jump\",\"time\":100}
/// </param>
/// <returns>return Ok if Action is successfully added to hashmap
/// else return </returns>
JumpCloudJsonState CJumpCloudJsonStats::addAction(std::string inputJson)
{
	JumpCloudJsonState returnState = INVALIDJSON;
	std::string colon = ":";
	std::size_t firstFoundColon = inputJson.find(colon);
	if (firstFoundColon != std::string::npos)
	{
		std::size_t lastFoundColon = inputJson.rfind(colon);
		if (lastFoundColon != std::string::npos)
		{
			//make sure there are 2 colons
			if (firstFoundColon != lastFoundColon)
			{
				//get action string
				std::string TmpStr = inputJson.substr(firstFoundColon + 2);
				std::string quote = "\"";
				std::size_t firstQuoteLocation = TmpStr.find(quote);
				//from example above expection actionName to be jump
				std::string actionName = inputJson.substr(firstFoundColon + 2, firstQuoteLocation);
				
				std::string closeBracket = "}";
				TmpStr = inputJson.substr(lastFoundColon + 1);
				std::size_t lastCloseBracketLocation = TmpStr.find(closeBracket);
				//from example expecting time to be 100
				uint32_t time = stoul(inputJson.substr(lastFoundColon + 1, lastCloseBracketLocation));
				
				//attempt only add to actionHash if valid
				if (actionName.empty() == false && (time != NULL))
				{
					Action newAction(actionName, 1, time);
					returnState = updateActionHash(newAction);
				}
				
			}
		}
	}

	return returnState;
}

/// <summary>
/// Updates unordered_map with Action passed
/// </summary>
/// <param name="ActionToAddOrEdit">Action struct with entry to add/update hashmap entry</param>
/// <returns>Return OK if hashmap is updated with Action info</returns>
JumpCloudJsonState CJumpCloudJsonStats::updateActionHash(Action ActionToAddOrEdit)
{
	JumpCloudJsonState returnState = OK;
	//check if item is already in Hash
	try
	{
		std::unordered_map<Action, uint32_t>::iterator it = actionHash.find(ActionToAddOrEdit);
		
		//See if Action is already in the hashmap if so add time and increase count. 
		//Delete old hashmap entry and create new hashmap entry with updated count and time info
		if (it != actionHash.end())
		{
			Action updatedAction = it->first;
			updatedAction.count += 1;
			updatedAction.time += ActionToAddOrEdit.time;

			ActionMutex.lock();
			actionHash.erase(ActionToAddOrEdit);
			actionHash.insert(std::make_pair(updatedAction, updatedAction.time / updatedAction.count));
			ActionMutex.unlock();
		}
		else
		{
			//Action is not in hashmap so only need to insert
			ActionMutex.lock();
			actionHash.insert(std::make_pair(ActionToAddOrEdit, ActionToAddOrEdit.time / ActionToAddOrEdit.count));
			ActionMutex.unlock();
		}
	}
	catch (...)
	{
		returnState = UNKNOWN;
	}

	return returnState;
}

 /// <summary>
 /// return Json formatted string with the avg of actions stored in ActionHash
 /// </summary>
 /// <returns>valid JSON string
 /// [
 ///	{"action":"jump", "avg":123},
 ///	{"action":"walk", "avg":250}
 /// ]
 /// </returns>
 std::string CJumpCloudJsonStats::getStats()
{
	 ActionMutex.lock();
	 
	 //there has to be a smarter way to use iterator to know when
	 //we have reached last ActionHash item so "," is not added
	 size_t ActionCount = actionHash.size();
	 size_t CurrentActionPos = 0;
	 std::string JsonOutput = "[\n";
	 for (auto i : actionHash)
	 {
		 JsonOutput += "\t";
		 JsonOutput += "{\"Action\":\"";
		 JsonOutput += ((Action)i.first).actionName;
		 JsonOutput += "\", \"avg\":";
		 JsonOutput += std::to_string(i.second);
		 JsonOutput += "}";
		 //add comma to everyline with the exception of the last line
		 if (CurrentActionPos+1 < ActionCount)
		 {
			 JsonOutput += ",";
		 }
		 CurrentActionPos++;
		 JsonOutput += "\n";
	 }

	 JsonOutput += "]";
	 ActionMutex.unlock();
	 
	 return JsonOutput;
}

 /// <summary>
 /// Callers should check returned JumpCloudJsonState and only use returnedAction if 
 /// JumpClouseJsonState == OK
 /// </summary>
 /// <param name="actionName">name of action to look for in unordered_map</param>
 /// <param name="returnedAction">out parm set to Action struct with actionName</param>
 /// <returns>OK if Action with actionName is found else return NOTFOUND</returns>
 JumpCloudJsonState CJumpCloudJsonStats::getActionByName(std::string actionName, Action &returnedAction)
 {
	 for (auto i : actionHash)
	 {
		 if (((Action)i.first).actionName == actionName)
		 {
			 returnedAction = (Action)i.first;
			 return OK;
		 }
	 }

	 return NOTFOUND;
 }

/// <summary>
/// Callers should check returned JumpCloudJsonState and only use returnedAverage if 
/// JumpClouseJsonState == OK
/// </summary>
/// <param name="actionName">name of action to look for in unordered_map</param>
/// <param name="returnedAverage">out parm set to uint32_t in unordered_map that has time avg for Action</param>
/// <returns>OK if Action with actionName is found else return NOTFOUND</returns>
 JumpCloudJsonState CJumpCloudJsonStats::getActionAvgByActionName(std::string actionName, uint32_t &returnedAverage)
 {
	 for (auto i : actionHash)
	 {
		 if (((Action)i.first).actionName == actionName)
		 {
			 returnedAverage = i.second;
			 return OK;
		 }
	 }

	 return NOTFOUND;
 }

/// <summary>
/// Callers should check returned JumpCloudJsonState and only use returnedTime if 
/// JumpClouseJsonState == OK
/// </summary>
/// <param name="actionName">name of action to look for in unordered_map</param>
/// <param name="returnedTime">out parm set to time in Action </param>
/// <returns>OK if Action with actionName is found else return NOTFOUND</returns>
 JumpCloudJsonState CJumpCloudJsonStats::getActionTimeByActionName(std::string actionName, uint32_t &returnedTime)
 {
	 Action tmpAction;
	 if (getActionByName(actionName, tmpAction) == OK)
	 {
		 returnedTime = tmpAction.time;
		 return OK;
	 }
	 return NOTFOUND;
 }

/// <summary>
/// Callers should check returned JumpCloudJsonState and only use returnedCount if 
/// JumpClouseJsonState == OK
/// </summary>
/// <param name="actionName">name of action to look for in unordered_map</param>
/// <param name="returnedCount">out parm set to returnedCount in Action </param>
/// <returns>OK if Action with actionName is found else return NOTFOUND</returns>
 JumpCloudJsonState CJumpCloudJsonStats::getActionCountByActionName(std::string actionName, uint32_t& returnedCount)
 {
	 Action tmpAction;
	 if (getActionByName(actionName, tmpAction) == OK)
	 {
		 returnedCount = tmpAction.count;
		 return OK;
	 }
	 return NOTFOUND;
 }