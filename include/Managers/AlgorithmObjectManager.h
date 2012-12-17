/**
 *  @file   PandoraPFANew/Framework/include/Managers/AlgorithmObjectManager.h
 * 
 *  @brief  Header file for the algorithm object manager class.
 * 
 *  $Log: $
 */
#ifndef ALGORITHM_OBJECT_MANAGER
#define ALGORITHM_OBJECT_MANAGER 1

#include "Managers/Manager.h"

namespace pandora
{

/**
 *  @brief  AlgorithmObjectManager class
 */
template<typename T>
class AlgorithmObjectManager : public Manager<T>
{
public:
    /**
     *  @brief  Default constructor
     */
     AlgorithmObjectManager();

    /**
     *  @brief  Destructor
     */
    virtual ~AlgorithmObjectManager();

protected:
    typedef typename Manager<T>::ObjectList ObjectList;

    /**
     *  @brief  Make a temporary list and set it to be the current list
     * 
     *  @param  pAlgorithm address of the algorithm requesting a temporary list
     *  @param  temporaryListName to receive the name of the temporary list
     */
    virtual StatusCode CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName);

    /**
     *  @brief  Move objects to a new temporary object list and set it to be the current object list
     * 
     *  @param  pAlgorithm address of the algorithm requesting a temporary list
     *  @param  originalListName the list in which the object currently exist
     *  @param  temporaryListName to receive the name of the temporary list
     *  @param  objectsToMove only objects in both this and the current list will be moved
     *          - other object in the current list will remain in original list
     *          - an empty object list will be rejected
     */
    virtual StatusCode MoveObjectsToTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const std::string &originalListName,
        std::string &temporaryListName, const ObjectList &objectsToMove);

    /**
     *  @brief  Save a list of objects
     * 
     *  @param  targetListName the name of the target object list, which will be created if it doesn't currently exist
     *  @param  sourceListName the name of the (typically temporary) object list to save
     */
    virtual StatusCode SaveObjects(const std::string &targetListName, const std::string &sourceListName);

    /**
     *  @brief  Save a list of objects
     * 
     *  @param  targetListName the name of the target object list, which will be created if it doesn't currently exist
     *  @param  sourceListName the name of the (typically temporary) object list containing objects to save
     *  @param  objectToSave only objects in both this and the temporary list will be stored
     *          - other object will remain in the temporary list and will be deleted when the parent algorithm exits
     *          - an empty object list will be rejected
     */
    virtual StatusCode SaveObjects(const std::string &targetListName, const std::string &sourceListName, const ObjectList &objectsToSave);

    /**
     *  @brief  Move (a subset of) objects between two lists
     * 
     *  @param  targetListName the name of the target object list, which will be created if it doesn't currently exist
     *  @param  sourceListName the name of the object list containing objects to save
     *  @param  pObjectSubset if specified, only objects in both this and the source list will be moved
     */
    virtual StatusCode MoveObjectsBetweenLists(const std::string &targetListName, const std::string &sourceListName,
        const ObjectList *pObjectSubset = NULL);

    /**
     *  @brief  Temporarily replace the current list with another list, which may only be a temporary list.
     *          This switch will persist only for the duration of the algorithm and its daughters; unless otherwise
     *          specified, the list will revert to the algorithm input list upon algorithm completion.
     * 
     *  @param  listName the name of the new current (and algorithm input) list
     */
    virtual StatusCode TemporarilyReplaceCurrentList(const std::string &listName);

    /**
     *  @brief  Delete an object from the current list
     * 
     *  @param  pCluster address of the object to delete
     */
    virtual StatusCode DeleteObject(T *pT);

    /**
     *  @brief  Delete an object from a specified list
     * 
     *  @param  pCluster address of the object to delete
     *  @param  listName the name of the list containing the object
     */
    virtual StatusCode DeleteObject(T *pT, const std::string &listName);

    /**
     *  @brief  Delete a list of objects from the current list
     * 
     *  @param  objectList the list of objects to delete
     */
    virtual StatusCode DeleteObjects(const ObjectList &objectList);

    /**
     *  @brief  Delete a list of objects from a specified list
     * 
     *  @param  objectList the list of objects to delete
     *  @param  listName the name of the list containing the objects
     */
    virtual StatusCode DeleteObjects(const ObjectList &objectList, const std::string &listName);

    /**
     *  @brief  Delete the contents of a temporary list
     * 
     *  @param  pAlgorithm address of the algorithm calling this function
     *  @param  temporaryListName the name of the temporary list
     */
    virtual StatusCode DeleteTemporaryObjects(const Algorithm *const pAlgorithm, const std::string &temporaryListName);

    /**
     *  @brief  Get the list of objects that will be deleted when the algorithm info is reset
     * 
     *  @param  pAlgorithm address of the algorithm
     *  @param  objectList to receive the list of objects that will be deleted when the algorithm info is reset
     */
    virtual StatusCode GetResetDeletionObjects(const Algorithm *const pAlgorithm, ObjectList &objectList) const;

    /**
     *  @brief  Reset the current list to the algorithm input list
     *
     *  @param  pAlgorithm address of the algorithm changing the current track list
     */
    virtual StatusCode ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm);

    /**
     *  @brief  Replace the current and algorithm input lists with a pre-existing list
     *
     *  @param  pAlgorithm address of the algorithm changing the current list
     *  @param  listName the name of the new current (and algorithm input) list
     */
    virtual StatusCode ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &listName);

    /**
     *  @brief  Remove temporary lists and reset the current cluster list to that when algorithm was initialized
     * 
     *  @param  pAlgorithm the algorithm associated with the temporary objects
     *  @param  isAlgorithmFinished whether the algorithm has completely finished and the algorithm info should be entirely removed
     */
    virtual StatusCode ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished);

    /**
     *  @brief  Erase all manager content
     */
    virtual StatusCode EraseAllContent();

    /**
     *  @brief  Drop the current list, returning the current list to its default empty/null state
     */
    virtual StatusCode DropCurrentList();

    bool        m_canMakeNewObjects;            ///< Whether the manager is allowed to make new objects when requested by algorithms
};

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode AlgorithmObjectManager<T>::DeleteObject(T *pT)
{
    return this->DeleteObject(pT, Manager<T>::m_currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode AlgorithmObjectManager<T>::DeleteObjects(const ObjectList &objectList)
{
    return this->DeleteObjects(objectList, Manager<T>::m_currentListName);
}

} // namespace pandora

#endif // #ifndef ALGORITHM_OBJECT_MANAGER
