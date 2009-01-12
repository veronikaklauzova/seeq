#pragma once
#include <winsock.h>
#include <list>


class WorldMap{
private:
	///@todo develop an ACTUAL index of ACTUAL objects
	std::list<SOCKET> m_index;///< (: index for all game objects.
	
public:
	/**
	 function for determining to whom we ned to send local packets.
	 */
	std::list<SOCKET> GetNearbyPlayersSockets();
	void AddPlayer(SOCKET sessionId);
	void RemovePlayer(SOCKET sessionId);
};