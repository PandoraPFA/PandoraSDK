/**
 *  @file PandoraSDK/src/Managers/ClusterManager.cc
 * 
 *  @brief Implementation of the cluster manager class.
 * 
 *  $Log: $
 */

#include "Managers/ClusterManager.h"

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

StatusCode ClusterManager::Create(const PandoraContentApi::Cluster::Parameters &parameters, const Cluster *&pCluster)
{
    pCluster = NULL;

    try
    {
        if (!m_canMakeNewObjects)
            throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);

        NameToListMap::iterator iter = m_nameToListMap.find(m_currentListName);

        if (m_nameToListMap.end() == iter)
             throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

        pCluster = new Cluster(parameters);

        if (NULL == pCluster)
             throw StatusCodeException(STATUS_CODE_FAILURE);

        if (!iter->second->insert(pCluster).second)
             throw StatusCodeException(STATUS_CODE_FAILURE);

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create cluster: " << statusCodeException.ToString() << std::endl;
        delete pCluster;
        pCluster = NULL;
        return statusCodeException.GetStatusCode();
    }
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

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
        isAvailable &= this->IsAvailable(*iter);

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
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
        this->SetAvailability(*iter, isAvailable);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::AlterMetadata(const Cluster *const pCluster, const PandoraContentApi::Cluster::Metadata &metadata) const
{
    return this->Modifiable(pCluster)->AlterMetadata(metadata);
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

    ClusterList::iterator clusterToEnlargeIter = enlargeListIter->second->find(pClusterToEnlarge);
    ClusterList::iterator clusterToDeleteIter = deleteListIter->second->find(pClusterToDelete);

    if ((enlargeListIter->second->end() == clusterToEnlargeIter) || (deleteListIter->second->end() == clusterToDeleteIter))
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pClusterToEnlarge)->AddHitsFromSecondCluster(pClusterToDelete));

    delete pClusterToDelete;
    deleteListIter->second->erase(clusterToDeleteIter);

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
    for (NameToListMap::const_iterator iter = m_nameToListMap.begin(); iter != m_nameToListMap.end(); ++iter)
    {
        for (ClusterList::const_iterator cIter = iter->second->begin(), cIterEnd = iter->second->end(); cIter != cIterEnd; ++cIter)
        {
            const TrackList trackList((*cIter)->GetAssociatedTrackList());

            for (TrackList::const_iterator tIter = trackList.begin(), tIterEnd = trackList.end(); tIter != tIterEnd; ++tIter)
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveTrackAssociation(*cIter, *tIter));
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

    for (ClusterList::iterator cIter = iter->second->begin(), cIterEnd = iter->second->end(); cIter != cIterEnd; ++cIter)
    {
        const TrackList trackList((*cIter)->GetAssociatedTrackList());

        danglingTracks.insert(trackList.begin(), trackList.end());

        for (TrackList::const_iterator tIter = trackList.begin(), tIterEnd = trackList.end(); tIter != tIterEnd; ++tIter)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveTrackAssociation(*cIter, *tIter));
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
