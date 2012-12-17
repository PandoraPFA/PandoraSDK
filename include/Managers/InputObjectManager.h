/**
 *  @file   PandoraPFANew/Framework/include/Managers/InputObjectManager.h
 * 
 *  @brief  Header file for the input object manager class.
 * 
 *  $Log: $
 */
#ifndef INPUT_OBJECT_MANAGER
#define INPUT_OBJECT_MANAGER 1

#include "Managers/Manager.h"

namespace pandora
{

/**
 *  @brief  InputObjectManager class
 */
template<typename T>
class InputObjectManager : public Manager<T>
{
public:
    /**
     *  @brief  Default constructor
     */
    InputObjectManager();

    /**
     *  @brief  Destructor
     */
    virtual ~InputObjectManager();

protected:
    typedef typename Manager<T>::ObjectList ObjectList;

    /**
     *  @brief  Create the input list (accessible to algorithms), using objects created by client application
     */
    virtual StatusCode CreateInputList();

    /**
     *  @brief  Change the current list to a specified temporary list
     *
     *  @param  pAlgorithm address of the algorithm changing the current list
     *  @param  objectList the specified temporary list
     *  @param  temporaryListName to receive the name of the temporary list
     */
    virtual StatusCode CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const ObjectList &objectList,
        std::string &temporaryListName);

    /**
     *  @brief  Save a list of objects in a list with a specified name; create new list if required
     * 
     *  @param  listName the list name
     *  @param  objectList the object list
     */
    virtual StatusCode SaveList(const std::string &listName, const ObjectList &objectList);

    /**
     *  @brief  Add objects to a saved list with a specified name
     *
     *  @param  listName the list to add the objects to
     *  @param  objectList the list of objects to be added
     */
    virtual StatusCode AddObjectsToList(const std::string &listName, const ObjectList &objectList);

    /**
     * @brief Remove objects from a saved list
     *
     * @param listName the list to remove the objects from
     * @param objectList the list of objects to be removed
     */
    virtual StatusCode RemoveObjectsFromList(const std::string &listName, const ObjectList &objectList);

    /**
     *  @brief  Erase all manager content
     */
    virtual StatusCode EraseAllContent();

    /**
     *  @brief  Create initial lists
     */
    virtual StatusCode CreateInitialLists();

    static const std::string        INPUT_LIST_NAME;                    ///< The name of the input list
};

} // namespace pandora

#endif // #ifndef INPUT_OBJECT_MANAGER
