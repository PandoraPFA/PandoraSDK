/**
 *  @file PandoraSDK/src/Managers/ClusterManager.cc
 * 
 *  @brief Implementation of the cluster manager class.
 * 
 *  $Log: $
 */

#include "Managers/ClusterManager.h"

#include "Objects/Cluster.h"

#include "Pandora/ObjectFactory.h"

#include <algorithm>

namespace pandora
{

ClusterManager::ClusterManager(const Pandora *const pPandora) :
    AlgorithmObjectManager<Cluster>(pPandora)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());
}

//------------------------------------------------------------------------------------------------------------------------------------------

ClusterManager::~ClusterManager()
{
    (void) this->EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::Create(const object_creation::Cluster::Parameters &parameters, const Cluster *&pCluster,
    const ObjectFactory<object_creation::Cluster::Parameters, object_creation::Cluster::Object> &factory)
{
    pCluster = nullptr;

    try
    {
        if (!m_canMakeNewObjects)
            throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);

        NameToListMap::iterator iter = m_nameToListMap.find(m_currentListName);

        if (m_nameToListMap.end() == iter)
             throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters, pCluster));

        if (!pCluster)
             throw StatusCodeException(STATUS_CODE_FAILURE);

        iter->second->push_back(pCluster);
        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create cluster: " << statusCodeException.ToString() << std::endl;
        delete pCluster;
        pCluster = nullptr;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::AlterMetadata(const Cluster *const pCluster, const object_creation::Cluster::Metadata &metadata) const
{
    return this->Modifiable(pCluster)->AlterMetadata(metadata);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
bool ClusterManager::IsAvailable(const Cluster *const pCluster) const
{
    return pCluster->IsAvailable();
}

template <>
bool ClusterManager::IsAvailable(const ClusterList *const pClusterList) const
{
    bool isAvailable(true);

    for (const Cluster *const pCluster : *pClusterList)
        isAvailable &= this->IsAvailable(pCluster);

    return isAvailable;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
void ClusterManager::SetAvailability(const Cluster *const pCluster, bool isAvailable) const
{
    this->Modifiable(pCluster)->SetAvailability(isAvailable);
}

template <>
void ClusterManager::SetAvailability(const ClusterList *const pClusterList, bool isAvailable) const
{
    for (const Cluster *const pCluster : *pClusterList)
        this->SetAvailability(pCluster, isAvailable);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::AddToCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit)
{
    return this->Modifiable(pCluster)->AddCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveFromCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit)
{
    return this->Modifiable(pCluster)->RemoveCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::AddIsolatedToCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit)
{
    return this->Modifiable(pCluster)->AddIsolatedCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveIsolatedFromCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit)
{
    return this->Modifiable(pCluster)->RemoveIsolatedCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::MergeAndDeleteClusters(const Cluster *const pClusterToEnlarge, const Cluster *const pClusterToDelete, const std::string &enlargeListName,
    const std::string &deleteListName)
{
    if (pClusterToEnlarge == pClusterToDelete)
        return STATUS_CODE_INVALID_PARAMETER;

    NameToListMap::iterator enlargeListIter = m_nameToListMap.find(enlargeListName);
    NameToListMap::iterator deleteListIter = m_nameToListMap.find(deleteListName);

    if ((m_nameToListMap.end() == enlargeListIter) || (m_nameToListMap.end() == deleteListIter))
        return STATUS_CODE_NOT_INITIALIZED;

    ClusterList::iterator clusterToEnlargeIter = std::find(enlargeListIter->second->begin(), enlargeListIter->second->end(), pClusterToEnlarge);
    ClusterList::iterator clusterToDeleteIter = std::find(deleteListIter->second->begin(), deleteListIter->second->end(), pClusterToDelete);

    if ((enlargeListIter->second->end() == clusterToEnlargeIter) || (deleteListIter->second->end() == clusterToDeleteIter))
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pClusterToEnlarge)->AddHitsFromSecondCluster(pClusterToDelete));

    clusterToDeleteIter = deleteListIter->second->erase(clusterToDeleteIter);
    delete pClusterToDelete;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::AddTrackAssociation(const Cluster *const pCluster, const Track *const pTrack) const
{
    return this->Modifiable(pCluster)->AddTrackAssociation(pTrack);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveTrackAssociation(const Cluster *const pCluster, const Track *const pTrack) const
{
    return this->Modifiable(pCluster)->RemoveTrackAssociation(pTrack);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveAllTrackAssociations() const
{
    for (const NameToListMap::value_type &mapEntry : m_nameToListMap)
    {
        for (const Cluster *const pCluster : *mapEntry.second)
        {
            const TrackList trackList(pCluster->GetAssociatedTrackList());

            for (const Track *const pTrack : trackList)
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveTrackAssociation(pCluster, pTrack));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveCurrentTrackAssociations(TrackList &danglingTracks) const
{
    NameToListMap::const_iterator iter = m_nameToListMap.find(m_currentListName);

    if (m_nameToListMap.end() == iter)
        return STATUS_CODE_NOT_INITIALIZED;

    for (const Cluster *const pCluster : *iter->second)
    {
        const TrackList trackList(pCluster->GetAssociatedTrackList());

        danglingTracks.insert(danglingTracks.end(), trackList.begin(), trackList.end());

        for (const Track *const pTrack : trackList)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveTrackAssociation(pCluster, pTrack));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveTrackAssociations(const TrackToClusterMap &trackToClusterList) const
{
    for (TrackToClusterMap::const_iterator iter = trackToClusterList.begin(), iterEnd = trackToClusterList.end(); iter != iterEnd; ++iter)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveTrackAssociation(iter->second, iter->first));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
