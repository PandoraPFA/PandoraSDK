/**
 *  @file PandoraSDK/src/Objects/Event.cc
 * 
 *  @brief Implementation of the Event class.
 * 
 *  $Log: $
 */

#include "Objects/Event.h"

namespace pandora
{

Event::Event(const Pandora *const pPandora) :
    m_event{-1},
    m_run{-1},
    m_subrun{-1},
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

Event::Event(const Event &event) :
    m_event{event.m_event},
    m_run{event.m_run},
    m_subrun{event.m_subrun},
    m_eventObjectMap(event.m_eventObjectMap),
    m_pPandora(event.m_pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

Event::~Event()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Event::AddEventObject(const std::string &key, EventObject &eventObject)
{
    if (this->m_eventObjectMap.find(key) != this->m_eventObjectMap.end())
        throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
    this->m_eventObjectMap[key] = &eventObject;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const EventObject *Event::GetEventObject(const std::string &key)
{
    if (this->m_eventObjectMap.find(key) == this->m_eventObjectMap.end())
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);
    return this->m_eventObjectMap.at(key);
}

//------------------------------------------------------------------------------------------------------------------------------------------
  
bool Event::Exists(const std::string &key)
{
    return this->m_eventObjectMap.find(key) != this->m_eventObjectMap.end();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Event::Initialize(const TiXmlHandle *const /*pXmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Event::ResetForNextEvent()
{
    this->m_event = -1;
    this->m_run = -1;
    this->m_subrun = -1;

    for (auto &[key, object] : this->m_eventObjectMap)
        object->Clear();
    this->m_eventObjectMap.clear();

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

