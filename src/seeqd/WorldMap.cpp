#include "seeqd/WorldMap.h"
#include "seeqd/World.h"

std::list<SOCKET> WorldMap::GetNearbyPlayersSockets(){
	return m_index;
}

void WorldMap::AddPlayer(SOCKET sessionId){
	m_index.push_back(sessionId);
}

void WorldMap::RemovePlayer(SOCKET sessionId){
	m_index.remove(sessionId);
}