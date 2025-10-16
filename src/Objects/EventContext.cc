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

const EventContextObject *EventContext::GetEventContextObject(const std::string &key) const
{
    const auto iter{m_eventObjectMap.find(key)};
    if (iter == m_eventObjectMap.end())
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
    return iter->second;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void EventContext::AddEventContextObject(const std::string &key, const EventContextObject *const pObject)
{
    if (m_eventObjectMap.find(key) != m_eventObjectMap.end())
        throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
    m_eventObjectMap[std::move(key)] = pObject;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void EventContext::RemoveEventContextObject(const std::string &key)
{
    const auto iter{m_eventObjectMap.find(key)};
    if (iter == m_eventObjectMap.end())
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    m_eventObjectMap.erase(iter);
}

//------------------------------------------------------------------------------------------------------------------------------------------
  
bool EventContext::DoesKeyExist(const std::string &key) const
{
    return m_eventObjectMap.find(key) != m_eventObjectMap.end();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventContext::ResetForNextEvent()
{
    for (auto &[key, pObject] : m_eventObjectMap)
        delete pObject;
    m_eventObjectMap.clear();

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

