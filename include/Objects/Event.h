/**
 *  @file   PandoraSDK/include/Objects/Event.h
 * 
 *  @brief  Header file of the Event class.
 * 
 *  $Log: $
 */
#ifndef EVENT_H
#define EVENT_H 1

#include "Pandora/StatusCodes.h"

#include <map>

namespace pandora
{

class Pandora;
class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

class EventObject;

/**
 *  @brief  The Event class provides a container for event-level quantities that should be accessible by algorithms.
 */
class Event
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the associated pandora object
     */
    Event(const Pandora *const pPandora);

    /**
     *  @brief  Copy constructor
     * 
     *  @param  event the event object to be copied
     */
    Event(const Event &event);

    /**
     *  @brief  Destructor
     */
    virtual ~Event();

    /**
     *  @brief  Adds an EventObject object to this event.
     *
     *  @param  key the key to associated with the event object
     *  @param  eventObj the object to be stored
     */
    void AddEventObject(const std::string &key, EventObject &eventObject);

    /**
    *  @brief  Retrieves the EventObject object associated with a key.
    *
    *  @param  key the key associated with the desired event object
    *  @return the event object associated with the specified key
    */
   const EventObject *GetEventObject(const std::string &key);
   
    /**
    *  @brief  Checks if an EventObject object is associated with a key.
    *
    *  @param  key the key associated with the desired event object
    *  @return true if there is an event object associated with the specified key
    */
   bool Exists(const std::string &key);

   // Needs persistency functions

private:
    /**
     *  @brief  Initialize event
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode Initialize(const TiXmlHandle *const pXmlHandle);

    /**
     *  @brief  Resets all event level properties
     *
     *  @return Whether or not the reset was successful
     */
    StatusCode ResetForNextEvent();

    typedef std::map<std::string, EventObject *> EventObjectMap;

    int m_event; ///< The event number
    int m_run; ///< The run number
    int m_subrun; ///< The subrun number
    EventObjectMap m_eventObjectMap; ///< A map to all event objects
    const Pandora *const m_pPandora; ///< The associated pandora object

    friend class PandoraImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The EventObject class provides a pure virtual class for custom information to be added to an Event object.
 */
class EventObject
{
public:
    /**
     *  @brief  Clean up the event object
     */
    virtual void Clear() = 0;
};

} // namespace pandora

#endif // #ifndef EVENT_H
