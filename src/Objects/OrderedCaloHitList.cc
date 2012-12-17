/**
 *  @file   PandoraPFANew/Framework/src/Objects/OrderedCaloHitList.cc
 * 
 *  @brief  Implementation of the ordered calo hit list class.
 * 
 *  $Log: $
 */

#include "Objects/CaloHit.h"
#include "Objects/OrderedCaloHitList.h"

namespace pandora
{

OrderedCaloHitList::OrderedCaloHitList() :
    std::map<PseudoLayer, CaloHitList *>()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

OrderedCaloHitList::OrderedCaloHitList(const OrderedCaloHitList &rhs) :
    std::map<PseudoLayer, CaloHitList *>()
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Add(rhs));
}

//------------------------------------------------------------------------------------------------------------------------------------------

OrderedCaloHitList::~OrderedCaloHitList()
{
    for (OrderedCaloHitList::iterator iter = this->begin(), iterEnd = this->end(); iter != iterEnd; ++iter)
    {
        delete iter->second;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Add(const OrderedCaloHitList &rhs)
{
    for (OrderedCaloHitList::const_iterator iter = rhs.begin(), iterEnd = rhs.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator caloHitIter = iter->second->begin(), caloHitIterEnd = iter->second->end();
            caloHitIter != caloHitIterEnd; ++caloHitIter)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Add(*caloHitIter, iter->first));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Remove(const OrderedCaloHitList &rhs)
{
    for (OrderedCaloHitList::const_iterator iter = rhs.begin(), iterEnd = rhs.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator caloHitIter = iter->second->begin(), caloHitIterEnd = iter->second->end();
            caloHitIter != caloHitIterEnd; ++caloHitIter)
        {
            PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, this->Remove(*caloHitIter, iter->first));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Add(const CaloHitList &caloHitList)
{
    for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, this->Add(*iter, (*iter)->GetPseudoLayer()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Remove(const CaloHitList &caloHitList)
{
    for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, this->Remove(*iter, (*iter)->GetPseudoLayer()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::GetCaloHitsInPseudoLayer(const PseudoLayer pseudoLayer, CaloHitList *&pCaloHitList) const
{
    OrderedCaloHitList::const_iterator iter = this->find(pseudoLayer);

    if (this->end() == iter)
        return STATUS_CODE_NOT_FOUND;

    pCaloHitList = iter->second;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int OrderedCaloHitList::GetNCaloHitsInPseudoLayer(const PseudoLayer pseudoLayer) const
{
    OrderedCaloHitList::const_iterator iter = this->find(pseudoLayer);

    if (this->end() == iter)
        return 0;

    return iter->second->size();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void OrderedCaloHitList::Reset()
{
    for (OrderedCaloHitList::iterator iter = this->begin(), iterEnd = this->end(); iter != iterEnd; ++iter)
        delete iter->second;

    this->clear();

    if (!this->empty())
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void OrderedCaloHitList::GetCaloHitList(CaloHitList &caloHitList) const
{
    for (OrderedCaloHitList::const_iterator iter = this->begin(), iterEnd = this->end(); iter != iterEnd; ++iter)
    {
        caloHitList.insert(iter->second->begin(), iter->second->end());
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

StatusCode OrderedCaloHitList::Add(CaloHit *const pCaloHit, const PseudoLayer pseudoLayer)
{
    OrderedCaloHitList::iterator iter = this->find(pseudoLayer);

    if (this->end() == iter)
    {
        CaloHitList *pCaloHitList = new CaloHitList;

        if (!pCaloHitList->insert(pCaloHit).second)
        {
            delete pCaloHitList;
            return STATUS_CODE_FAILURE;
        }

        if (!(this->insert(OrderedCaloHitList::value_type(pseudoLayer, pCaloHitList)).second))
        {
            delete pCaloHitList;
            return STATUS_CODE_FAILURE;
        }
    }
    else
    {
        if (!iter->second->insert(pCaloHit).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Remove(CaloHit *const pCaloHit, const PseudoLayer pseudoLayer)
{
    OrderedCaloHitList::iterator listIter = this->find(pseudoLayer);

    if (this->end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    CaloHitList::iterator caloHitIter = listIter->second->find(pCaloHit);

    if (listIter->second->end() == caloHitIter)
        return STATUS_CODE_NOT_FOUND;

    listIter->second->erase(caloHitIter);

    if (listIter->second->empty())
    {
        delete listIter->second;
        this->erase(listIter);
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
