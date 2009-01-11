#pragma once
#include "seeqd/WorldHandlers.h"
#include "squtils/Logger.h"

void TalkHandler::run(){
	unsigned char *dataPtr = &(m_msg->data[0]);
	m_msg->data[m_msg->data.size()-1] = '\0';
	LOG(LVL_NOTICE,"Player %ld says '%s'", m_msg->sender.id, dataPtr);
	//delete m_msg;
}