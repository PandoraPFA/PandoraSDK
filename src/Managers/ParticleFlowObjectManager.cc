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

StatusCode ParticleFlowObjectManager::Create(const PandoraContentApi::ParticleFlowObject::Parameters &parameters, const ParticleFlowObject *&pPfo)
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

StatusCode ParticleFlowObjectManager::AlterMetadata(const ParticleFlowObject *const pPfo, const PandoraContentApi::ParticleFlowObject::Metadata &metadata) const
{
    return this->Modifiable(pPfo)->AlterMetadata(metadata);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode ParticleFlowObjectManager::AddToPfo(const ParticleFlowObject *const pPfo, const T *const pT) const
{
    return this->Modifiable(pPfo)->AddToPfo(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode ParticleFlowObjectManager::RemoveFromPfo(const ParticleFlowObject *const pPfo, const T *const pT) const
{
    return this->Modifiable(pPfo)->RemoveFromPfo(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::SetParentDaughterAssociation(const ParticleFlowObject *const pParentPfo, const ParticleFlowObject *const pDaughterPfo) const
{
    if (pParentPfo == pDaughterPfo)
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pParentPfo)->AddDaughter(pDaughterPfo));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pDaughterPfo)->AddParent(pParentPfo));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::RemoveParentDaughterAssociation(const ParticleFlowObject *const pParentPfo, const ParticleFlowObject *const pDaughterPfo) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pParentPfo)->RemoveDaughter(pDaughterPfo));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pDaughterPfo)->RemoveParent(pParentPfo));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode ParticleFlowObjectManager::AddToPfo<Cluster>(const ParticleFlowObject *, const Cluster *) const;
template StatusCode ParticleFlowObjectManager::AddToPfo<Track>(const ParticleFlowObject *, const Track *) const;
template StatusCode ParticleFlowObjectManager::AddToPfo<Vertex>(const ParticleFlowObject *, const Vertex *) const;

template StatusCode ParticleFlowObjectManager::RemoveFromPfo<Cluster>(const ParticleFlowObject *, const Cluster *) const;
template StatusCode ParticleFlowObjectManager::RemoveFromPfo<Track>(const ParticleFlowObject *, const Track *) const;
template StatusCode ParticleFlowObjectManager::RemoveFromPfo<Vertex>(const ParticleFlowObject *, const Vertex *) const;

} //  namespace pandora
