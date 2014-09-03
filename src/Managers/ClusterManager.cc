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

StatusCode ClusterManager::CreateCluster(const PandoraContentApi::Cluster::Parameters &parameters, Cluster *&pCluster)
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

StatusCode ClusterManager::MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete)
{
    if (pClusterToEnlarge == pClusterToDelete)
        return STATUS_CODE_INVALID_PARAMETER;

    NameToListMap::iterator listIter = m_nameToListMap.find(m_currentListName);

    if (m_nameToListMap.end() == listIter)
        return STATUS_CODE_NOT_INITIALIZED;

    ClusterList::iterator clusterToEnlargeIter = listIter->second->find(pClusterToEnlarge);
    ClusterList::iterator clusterToDeleteIter = listIter->second->find(pClusterToDelete);

    if ((listIter->second->end() == clusterToEnlargeIter) || (listIter->second->end() == clusterToDeleteIter))
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pClusterToEnlarge->AddHitsFromSecondCluster(pClusterToDelete));

    delete pClusterToDelete;
    listIter->second->erase(clusterToDeleteIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete, const std::string &enlargeListName,
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

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pClusterToEnlarge->AddHitsFromSecondCluster(pClusterToDelete));

    delete pClusterToDelete;
    deleteListIter->second->erase(clusterToDeleteIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveAllTrackAssociations() const
{
    for (NameToListMap::const_iterator iter = m_nameToListMap.begin(); iter != m_nameToListMap.end(); ++iter)
    {
        for (ClusterList::iterator clusterIter = iter->second->begin(), clusterIterEnd = iter->second->end(); 
            clusterIter != clusterIterEnd; ++clusterIter)
        {
            (*clusterIter)->m_associatedTrackList.clear();
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

    for (ClusterList::iterator clusterIter = iter->second->begin(), clusterIterEnd = iter->second->end(); clusterIter != clusterIterEnd;
        ++clusterIter)
    {
        TrackList &associatedTrackList((*clusterIter)->m_associatedTrackList);

        if (associatedTrackList.empty())
            continue;

        danglingTracks.insert(associatedTrackList.begin(), associatedTrackList.end());
        associatedTrackList.clear();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveTrackAssociations(const TrackToClusterMap &trackToClusterList) const
{
    for (TrackToClusterMap::const_iterator iter = trackToClusterList.begin(), iterEnd = trackToClusterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->RemoveTrackAssociation(iter->first));
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
