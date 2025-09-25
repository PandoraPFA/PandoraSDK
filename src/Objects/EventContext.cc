/**
 *  @file PandoraSDK/src/Objects/EventContext.cc
 * 
 *  @brief Implementation of the EventContext class.
 * 
 *  $Log: $
 */

#include "Objects/EventContext.h"

namespace pandora
{

EventContext::EventContext(const Pandora *const pPandora) :
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

EventContext::EventContext(const EventContext &event) :
    m_eventObjectMap(event.m_eventObjectMap),
    m_pPandora(event.m_pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

EventContext::~EventContext()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void EventContext::AddEventContextObject(const std::string &key, EventContextObject &eventObject)
{
    if (this->m_eventObjectMap.find(key) != this->m_eventObjectMap.end())
        throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
    this->m_eventObjectMap[key] = &eventObject;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const EventContextObject *EventContext::GetEventContextObject(const std::string &key)
{
    if (this->m_eventObjectMap.find(key) == this->m_eventObjectMap.end())
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
    return this->m_eventObjectMap.at(key);
}

//------------------------------------------------------------------------------------------------------------------------------------------
  
bool EventContext::Exists(const std::string &key)
{
    return this->m_eventObjectMap.find(key) != this->m_eventObjectMap.end();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventContext::Initialize(const TiXmlHandle *const /*pXmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventContext::ResetForNextEvent()
{
    for (auto &[key, object] : this->m_eventObjectMap)
        object->Clear();
    this->m_eventObjectMap.clear();

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

