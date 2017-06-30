/**
 *  @file   PandoraSDK/src/Objects/OrderedCaloHitList.cc
 * 
 *  @brief  Implementation of the ordered calo hit list class.
 * 
 *  $Log: $
 */

#include "Objects/CaloHit.h"
#include "Objects/OrderedCaloHitList.h"

#include <algorithm>

namespace pandora
{

OrderedCaloHitList::OrderedCaloHitList()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

OrderedCaloHitList::OrderedCaloHitList(const OrderedCaloHitList &rhs)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Add(rhs));
}

//------------------------------------------------------------------------------------------------------------------------------------------

OrderedCaloHitList::~OrderedCaloHitList()
{
    for (const value_type &entry : m_theList)
        delete entry.second;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Add(const OrderedCaloHitList &rhs)
{
    for (const value_type &rhsEntry : rhs)
    {
        for (const CaloHit *const pCaloHit : *rhsEntry.second)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Add(pCaloHit, rhsEntry.first));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Remove(const OrderedCaloHitList &rhs)
{
    for (const value_type &rhsEntry : rhs)
    {
        for (const CaloHit *const pCaloHit : *rhsEntry.second)
        {
            PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, this->Remove(pCaloHit, rhsEntry.first));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Add(const CaloHitList &caloHitList)
{
    for (const CaloHit *const pCaloHit : caloHitList)
    {
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, this->Add(pCaloHit, pCaloHit->GetPseudoLayer()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Remove(const CaloHitList &caloHitList)
{
    for (const CaloHit *const pCaloHit : caloHitList)
    {
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, this->Remove(pCaloHit, pCaloHit->GetPseudoLayer()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::GetCaloHitsInPseudoLayer(const unsigned int pseudoLayer, CaloHitList *&pCaloHitList) const
{
    OrderedCaloHitList::const_iterator iter = this->find(pseudoLayer);

    if (this->end() == iter)
        return STATUS_CODE_NOT_FOUND;

    pCaloHitList = iter->second;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int OrderedCaloHitList::GetNCaloHitsInPseudoLayer(const unsigned int pseudoLayer) const
{
    OrderedCaloHitList::const_iterator iter = this->find(pseudoLayer);

    if (this->end() == iter)
        return 0;

    return iter->second->size();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void OrderedCaloHitList::Reset()
{
    for (const value_type &entry : m_theList)
        delete entry.second;

    this->clear();

    if (!this->empty())
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void OrderedCaloHitList::FillCaloHitList(CaloHitList &caloHitList) const
{
    for (const value_type &entry : m_theList)
    {
        caloHitList.insert(caloHitList.end(), entry.second->begin(), entry.second->end());
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool OrderedCaloHitList::operator= (const OrderedCaloHitList &rhs)
{
    if (this == &rhs)
        return true;

    if (!this->empty())
        this->Reset();

    return (STATUS_CODE_SUCCESS == this->Add(rhs));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Add(const CaloHit *const pCaloHit, const unsigned int pseudoLayer)
{
    TheList::iterator iter = m_theList.find(pseudoLayer);

    if (m_theList.end() == iter)
    {
        CaloHitList *const pCaloHitList = new CaloHitList;
        pCaloHitList->push_back(pCaloHit);

        if (!(m_theList.insert(TheList::value_type(pseudoLayer, pCaloHitList)).second))
        {
            delete pCaloHitList;
            return STATUS_CODE_FAILURE;
        }
    }
    else
    {
        if (iter->second->end() != std::find(iter->second->begin(), iter->second->end(), pCaloHit))
            return STATUS_CODE_ALREADY_PRESENT;

        iter->second->push_back(pCaloHit);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Remove(const CaloHit *const pCaloHit, const unsigned int pseudoLayer)
{
    TheList::iterator listIter = m_theList.find(pseudoLayer);

    if (m_theList.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    CaloHitList::iterator caloHitIter = std::find(listIter->second->begin(), listIter->second->end(), pCaloHit);

    if (listIter->second->end() == caloHitIter)
        return STATUS_CODE_NOT_FOUND;

    caloHitIter = listIter->second->erase(caloHitIter);

    if (listIter->second->empty())
    {
        delete listIter->second;
        listIter = m_theList.erase(listIter);
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
