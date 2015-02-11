/**
 *  @file   PandoraSDK/include/Objects/OrderedCaloHitList.h
 * 
 *  @brief  Header file for the ordered calo hit list class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_ORDERED_CALO_HIT_LIST_H
#define PANDORA_ORDERED_CALO_HIT_LIST_H 1

#include "Objects/CaloHit.h"

#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

#include <map>

namespace pandora
{

/**
 *  @brief  Calo hit lists arranged by pseudo layer
 */
class OrderedCaloHitList
{
public:
    typedef std::map<unsigned int, CaloHitList *> TheList;
    typedef TheList::const_iterator const_iterator;
    typedef TheList::const_reverse_iterator const_reverse_iterator;

    /**
     *  @brief  Default constructor
     */
    OrderedCaloHitList();

    /**
     *  @brief  Copy constructor
     * 
     *  @param  rhs the ordered calo hit list to copy
     */
    OrderedCaloHitList(const OrderedCaloHitList &rhs);

    /**
     *  @brief  Destructor
     */
    ~OrderedCaloHitList();

    /**
     *  @brief  Add the hits from a second ordered calo hit list to this list
     * 
     *  @param  rhs the source ordered calo hit list
     */
    StatusCode Add(const OrderedCaloHitList &rhs);

    /**
     *  @brief  Remove the hits in a second ordered calo hit list from this list
     * 
     *  @param  rhs the source ordered calo hit list
     */
    StatusCode Remove(const OrderedCaloHitList &rhs);

    /**
     *  @brief  Add a list of calo hits to the ordered calo hit list
     * 
     *  @param  caloHitList the calo hit list
     */
    StatusCode Add(const CaloHitList &caloHitList);

    /**
     *  @brief  Remove a list of calo hits from the ordered calo hit list
     * 
     *  @param  caloHitList the calo hit list
     */
    StatusCode Remove(const CaloHitList &caloHitList);

    /**
     *  @brief  Add a calo hit to the ordered calo hit list
     * 
     *  @param  pCaloHit the address of the calo hit
     */
    StatusCode Add(const CaloHit *const pCaloHit);

    /**
     *  @brief  Remove a calo hit from the ordered calo hit list
     * 
     *  @param  pCaloHit the address of the calo hit
     */
    StatusCode Remove(const CaloHit *const pCaloHit);

    /**
     *  @brief  Get calo hits in specified pseudo layer
     * 
     *  @param  pseudoLayer the pseudo layer
     *  @param  pCaloHitList to receive the address of the relevant calo hit list
     */
    StatusCode GetCaloHitsInPseudoLayer(const unsigned int pseudoLayer, CaloHitList *&pCaloHitList) const;

    /**
     *  @brief  Get the number of calo hits in a specified pseudo layer
     * 
     *  @param  pseudoLayer the pseudo layer
     * 
     *  @return The number of calo hits in the specified pseudo layer
     */
    unsigned int GetNCaloHitsInPseudoLayer(const unsigned int pseudoLayer) const;

    /**
     *  @brief  Reset the ordered calo hit list, emptying its contents
     */
    void Reset();

    /**
     *  @brief  Get a simple list of all the calo hits in the ordered calo hit list (no ordering by pseudolayer)
     * 
     *  @param  caloHitList to receive the simple list of calo hits
     */
    void GetCaloHitList(CaloHitList &caloHitList) const;

    /**
     *  @brief  Returns a const iterator referring to the first element in the ordered calo hit list
     */
    const_iterator begin() const;

    /**
     *  @brief  Returns a const iterator referring to the past-the-end element in the ordered calo hit list
     */
    const_iterator end() const;

    /**
     *  @brief  Returns a const reverse iterator referring to the first element in the ordered calo hit list
     */
    const_reverse_iterator rbegin() const;

    /**
     *  @brief  Returns a const reverse iterator referring to the past-the-end element in the ordered calo hit list
     */
    const_reverse_iterator rend() const;

    /**
     *  @brief  Searches the container for an element with specified layer and returns an iterator to it if found,
     *          otherwise it returns an iterator to the past-the-end element.
     */
    const_iterator find(const unsigned int index) const;

    /**
     *  @brief  Returns the number of elements in the container.
     */
    unsigned int size() const;

    /**
     *  @brief  Returns whether the map container is empty (i.e. whether its size is 0)
     */
    bool empty() const;

    /**
     *  @brief  Assignment operator
     * 
     *  @param  rhs the ordered calo hit list to assign
     */
    bool operator= (const OrderedCaloHitList &rhs);

private:
    /**
     *  @brief  Clear the ordered calo hit list
     */
    void clear();

    /**
     *  @brief  Add a calo hit to a specified pseudo layer
     * 
     *  @param  pCaloHit the address of the calo hit
     *  @param  pseudoLayer the pesudo layer
     */
    StatusCode Add(const CaloHit *const pCaloHit, const unsigned int pseudoLayer);

    /**
     *  @brief  Remove a calo hit from a specified pseudo layer
     * 
     *  @param  pCaloHit the address of the calo hit
     *  @param  pseudoLayer the pesudo layer
     */
    StatusCode Remove(const CaloHit *const pCaloHit, const unsigned int pseudoLayer);

    TheList     m_theList;      ///< The ordered calo hit list
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline OrderedCaloHitList::const_iterator OrderedCaloHitList::begin() const
{
    return m_theList.begin();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline OrderedCaloHitList::const_iterator OrderedCaloHitList::end() const
{
    return m_theList.end();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline OrderedCaloHitList::const_iterator OrderedCaloHitList::find(const unsigned int index) const
{
    return m_theList.find(index);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline OrderedCaloHitList::const_reverse_iterator OrderedCaloHitList::rbegin() const
{
    return m_theList.rbegin();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline OrderedCaloHitList::const_reverse_iterator OrderedCaloHitList::rend() const
{
    return m_theList.rend();
}
//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int OrderedCaloHitList::size() const
{
    return m_theList.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool OrderedCaloHitList::empty() const
{
    return m_theList.empty();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode OrderedCaloHitList::Add(const CaloHit *const pCaloHit)
{
    return this->Add(pCaloHit, pCaloHit->GetPseudoLayer());
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode OrderedCaloHitList::Remove(const CaloHit *const pCaloHit)
{
    return this->Remove(pCaloHit, pCaloHit->GetPseudoLayer());
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void OrderedCaloHitList::clear()
{
    m_theList.clear();
}

} // namespace pandora

#endif // #ifndef PANDORA_ORDERED_CALO_HIT_LIST_H
