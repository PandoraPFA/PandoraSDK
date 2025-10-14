/**
 *  @file   PandoraSDK/include/Objects/EventContext.h
 * 
 *  @brief  Header file of the EventContext class.
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

class EventContextObject;

/**
 *  @brief  The EventContext class provides a container for event-level quantities that should be accessible by algorithms.
 */
class EventContext
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the associated pandora object
     */
    EventContext(const Pandora *const pPandora);

    /**
     *  @brief  Copy constructor
     * 
     *  @param  event the event object to be copied
     */
    EventContext(const EventContext &event);

    /**
     *  @brief  Destructor
     */
    virtual ~EventContext();

    /**
     *  @brief  Adds an EventContextObject object to this event.
     *
     *  @param  key the key to associated with the event object
     *  @param  eventObj the object to be stored
     */
    void AddEventContextObject(const std::string &key, const EventContextObject *const eventObject);

    /**
     *  @brief  Remove an EventContextObject object from this event context.
     *
     *  @param  key the key of the event context object to be removed
     */
    void RemoveEventContextObject(const std::string &key);

    /**
    *  @brief  Retrieves the EventContextObject object associated with a key.
    *
    *  @param  key the key associated with the desired event object
    *  @return the event object associated with the specified key
    */
   const EventContextObject *GetEventContextObject(const std::string &key) const;
   
    /**
    *  @brief  Checks if an EventContextObject object is associated with a key.
    *
    *  @param  key the key associated with the desired event object
    *  @return true if there is an event object associated with the specified key
    */
   bool DoesKeyExist(const std::string &key) const;

   // Needs persistency functions

private:
    /**
     *  @brief  Resets all event level properties
     *
     *  @return Whether or not the reset was successful
     */
    StatusCode ResetForNextEvent();

    typedef std::map<std::string, const EventContextObject *> EventContextObjectMap;

    EventContextObjectMap m_eventObjectMap; ///< A map to all event objects
    const Pandora *const m_pPandora; ///< The associated pandora object

    friend class PandoraImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The EventContextObject class provides a pure virtual class for custom information to be added to an EventContext object.
 */
class EventContextObject
{
public:
    /**
     *  @brief Default constructor
     */
    EventContextObject() = default;

    /**
     *  @brief  Destructor
     */
    virtual ~EventContextObject() = default;
};

} // namespace pandora

#endif // #ifndef EVENT_H
