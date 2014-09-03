/**
 *  @file   PandoraSDK/src/Managers/ParticleFlowObjectManager.cc
 * 
 *  @brief  Implementation of the particle flow object manager class.
 * 
 *  $Log: $
 */

#include "Managers/ParticleFlowObjectManager.h"

#include "Objects/ParticleFlowObject.h"

namespace pandora
{

ParticleFlowObjectManager::ParticleFlowObjectManager(const Pandora *const pPandora) :
    AlgorithmObjectManager<ParticleFlowObject>(pPandora)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());
}

//------------------------------------------------------------------------------------------------------------------------------------------

ParticleFlowObjectManager::~ParticleFlowObjectManager()
{
    (void) this->EraseAllContent();
}
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObject::Parameters &parameters,
    ParticleFlowObject *&pPfo)
{
    pPfo = NULL;

    try
    {
        if (!m_canMakeNewObjects)
            throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);

        NameToListMap::iterator iter = m_nameToListMap.find(m_currentListName);

        if (m_nameToListMap.end() == iter)
             throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

        pPfo = new ParticleFlowObject(parameters);

        if (NULL == pPfo)
             throw StatusCodeException(STATUS_CODE_FAILURE);

        if (!iter->second->insert(pPfo).second)
             throw StatusCodeException(STATUS_CODE_FAILURE);

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create particle flow object: " << statusCodeException.ToString() << std::endl;
        delete pPfo;
        pPfo = NULL;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode ParticleFlowObjectManager::AddToPfo(ParticleFlowObject *pPfo, T *pT) const
{
    return pPfo->AddToPfo(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode ParticleFlowObjectManager::RemoveFromPfo(ParticleFlowObject *pPfo, T *pT) const
{
    return pPfo->RemoveFromPfo(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::SetParentDaughterAssociation(ParticleFlowObject *pParentPfo, ParticleFlowObject *pDaughterPfo) const
{
    if (pParentPfo == pDaughterPfo)
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pParentPfo->AddDaughter(pDaughterPfo));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pDaughterPfo->AddParent(pParentPfo));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::RemoveParentDaughterAssociation(ParticleFlowObject *pParentPfo, ParticleFlowObject *pDaughterPfo) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pParentPfo->RemoveDaughter(pDaughterPfo));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pDaughterPfo->RemoveParent(pParentPfo));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode ParticleFlowObjectManager::AddToPfo<Cluster>(ParticleFlowObject *, Cluster *) const;
template StatusCode ParticleFlowObjectManager::AddToPfo<Track>(ParticleFlowObject *, Track *) const;
template StatusCode ParticleFlowObjectManager::AddToPfo<Vertex>(ParticleFlowObject *, Vertex *) const;

template StatusCode ParticleFlowObjectManager::RemoveFromPfo<Cluster>(ParticleFlowObject *, Cluster *) const;
template StatusCode ParticleFlowObjectManager::RemoveFromPfo<Track>(ParticleFlowObject *, Track *) const;
template StatusCode ParticleFlowObjectManager::RemoveFromPfo<Vertex>(ParticleFlowObject *, Vertex *) const;

} //  namespace pandora
