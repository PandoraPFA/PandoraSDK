/**
 *  @file   PandoraPFANew/Framework/src/Managers/ParticleFlowObjectManager.cc
 * 
 *  @brief  Implementation of the particle flow object manager class.
 * 
 *  $Log: $
 */

#include "Managers/ParticleFlowObjectManager.h"

#include "Objects/ParticleFlowObject.h"

namespace pandora
{

ParticleFlowObjectManager::ParticleFlowObjectManager() :
    AlgorithmObjectManager<ParticleFlowObject>()
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());
}

//------------------------------------------------------------------------------------------------------------------------------------------

ParticleFlowObjectManager::~ParticleFlowObjectManager()
{
    (void) this->EraseAllContent();
}
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters)
{
    ParticleFlowObject *pPfo = NULL;

    try
    {
        if (!m_canMakeNewObjects)
            throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);

        NameToListMap::iterator iter = m_nameToListMap.find(m_currentListName);

        if (m_nameToListMap.end() == iter)
             throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

        pPfo = new ParticleFlowObject(particleFlowObjectParameters);

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
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::AddClusterToPfo(ParticleFlowObject *pPfo, Cluster *pCluster) const
{
    if (!pPfo->m_clusterList.insert(pCluster).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::AddTrackToPfo(ParticleFlowObject *pPfo, Track *pTrack) const
{
    if (!pPfo->m_trackList.insert(pTrack).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::RemoveClusterFromPfo(ParticleFlowObject *pPfo, Cluster *pCluster)
{
    ClusterList &clusterList = pPfo->m_clusterList;
    ClusterList::iterator clusterIter = clusterList.find(pCluster);

    if (clusterList.end() == clusterIter)
        return STATUS_CODE_NOT_FOUND;

    clusterList.erase(clusterIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::RemoveTrackFromPfo(ParticleFlowObject *pPfo, Track *pTrack)
{
    TrackList &trackList = pPfo->m_trackList;
    TrackList::iterator trackIter = trackList.find(pTrack);

    if (trackList.end() == trackIter)
        return STATUS_CODE_NOT_FOUND;

    trackList.erase(trackIter);

    return STATUS_CODE_SUCCESS;
}

} //  namespace pandora
