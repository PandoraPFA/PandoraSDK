/**
 *  @file   PandoraPFANew/Framework/src/Api/PandoraContentApiImpl.cc
 * 
 *  @brief  Implementation of the pandora content api class.
 * 
 *  $Log: $
 */

#include "Pandora/Algorithm.h"

#include "Api/PandoraContentApiImpl.h"

#include "Helpers/ReclusterHelper.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/MCManager.h"
#include "Managers/TrackManager.h"
#include "Managers/ParticleFlowObjectManager.h"

#include "Objects/Cluster.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraSettings.h"

namespace pandora
{

template <typename CLUSTER_PARAMETERS>
StatusCode PandoraContentApiImpl::CreateCluster(CLUSTER_PARAMETERS *pClusterParameters, Cluster *&pCluster) const
{
    return STATUS_CODE_FAILURE;
}

template <>
StatusCode PandoraContentApiImpl::CreateCluster(CaloHit *pCaloHit, Cluster *&pCluster) const
{
    if (!m_pPandora->m_pCaloHitManager->IsCaloHitAvailable(pCaloHit))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateCluster(pCaloHit, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(pCaloHit, false));

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::CreateCluster(CaloHitList *pCaloHitList, Cluster *&pCluster) const
{
    if (!m_pPandora->m_pCaloHitManager->AreCaloHitsAvailable(*pCaloHitList))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateCluster(pCaloHitList, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(*pCaloHitList, false));

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::CreateCluster(Track *pTrack, Cluster *&pCluster) const
{
    return m_pPandora->m_pClusterManager->CreateCluster(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters) const
{
    const TrackList &trackList(particleFlowObjectParameters.m_trackList);
    const ClusterList &clusterList(particleFlowObjectParameters.m_clusterList);

    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        if (!(*iter)->IsAvailable())
            return STATUS_CODE_NOT_ALLOWED;
    }

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        if (!(*iter)->IsAvailable())
            return STATUS_CODE_NOT_ALLOWED;
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->CreateParticleFlowObject(particleFlowObjectParameters));

    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
        (*iter)->SetAvailability(false);

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
        (*iter)->SetAvailability(false);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RepeatEventPreparation() const
{
    return m_pPandora->PrepareEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::CreateDaughterAlgorithm(TiXmlElement *const pXmlElement, std::string &daughterAlgorithmName) const
{
    return m_pPandora->m_pAlgorithmManager->CreateAlgorithm(pXmlElement, daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RunAlgorithm(const std::string &algorithmName) const
{
    AlgorithmManager::AlgorithmMap::const_iterator iter = m_pPandora->m_pAlgorithmManager->m_algorithmMap.find(algorithmName);

    if (m_pPandora->m_pAlgorithmManager->m_algorithmMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->RegisterAlgorithm(iter->second));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RegisterAlgorithm(iter->second));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->RegisterAlgorithm(iter->second));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->RegisterAlgorithm(iter->second));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RegisterAlgorithm(iter->second));

    try
    {
        static const bool shouldDisplayAlgorithmInfo(PandoraSettings::ShouldDisplayAlgorithmInfo());

        if (shouldDisplayAlgorithmInfo)
        {
            for (unsigned int i = 1; i < m_pPandora->m_pCaloHitManager->m_algorithmInfoMap.size(); ++i)
                std::cout << "----";
            std::cout << "> Running Algorithm: " << iter->first << ", " << iter->second->GetAlgorithmType() << std::endl;
        }
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->Run());
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failure in algorithm " << iter->first << ", " << iter->second->GetAlgorithmType() << ", " << statusCodeException.ToString()
                  << statusCodeException.GetBackTrace() << std::endl;
    }
    catch (...)
    {
        std::cout << "Failure in algorithm " << iter->first << ", " << iter->second->GetAlgorithmType() << ", unrecognized exception" << std::endl;
    }

    ClusterList clustersToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetResetDeletionObjects(iter->second, clustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(clustersToBeDeleted));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetAlgorithmInfo(iter->second, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetAlgorithmInfo(iter->second, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->ResetAlgorithmInfo(iter->second, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->ResetAlgorithmInfo(iter->second, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetAlgorithmInfo(iter->second, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RunClusteringAlgorithm(const Algorithm &algorithm, const std::string &clusteringAlgorithmName,
    const ClusterList *&pNewClusterList, std::string &newClusterListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateTemporaryListAndSetCurrent(&algorithm, newClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->PrepareForClustering(&algorithm, newClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RunAlgorithm(clusteringAlgorithmName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentList(pNewClusterList, newClusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentCaloHitList(const CaloHitList *&pCaloHitList, std::string &caloHitListName) const
{
    return m_pPandora->m_pCaloHitManager->GetCurrentList(pCaloHitList, caloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentCaloHitListName(std::string &caloHitListName) const
{
    return m_pPandora->m_pCaloHitManager->GetCurrentListName(caloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCaloHitList(const std::string &caloHitListName, const CaloHitList *&pCaloHitList) const
{
    return m_pPandora->m_pCaloHitManager->GetList(caloHitListName, pCaloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveCaloHitList(const CaloHitList &caloHitList, const std::string &newListName) const
{
    return m_pPandora->m_pCaloHitManager->SaveList(newListName, caloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReplaceCurrentCaloHitList(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pCaloHitManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DropCurrentCaloHitList() const
{
    return m_pPandora->m_pCaloHitManager->DropCurrentList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PandoraContentApiImpl::IsCaloHitAvailable(pandora::CaloHit *pCaloHit) const
{
    return m_pPandora->m_pCaloHitManager->IsCaloHitAvailable(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PandoraContentApiImpl::AreCaloHitsAvailable(const pandora::CaloHitList &caloHitList) const
{
    return m_pPandora->m_pCaloHitManager->AreCaloHitsAvailable(caloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit) const
{
    if (!m_pPandora->m_pCaloHitManager->IsCaloHitAvailable(pCaloHit))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->AddCaloHitToCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(pCaloHit, false));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit) const
{
    if (pCluster->GetNCaloHits() <= 1)
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveCaloHitFromCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(pCaloHit, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddIsolatedCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit) const
{
    if (!m_pPandora->m_pCaloHitManager->IsCaloHitAvailable(pCaloHit))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->AddIsolatedCaloHitToCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(pCaloHit, false));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveIsolatedCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveIsolatedCaloHitFromCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(pCaloHit, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::FragmentCaloHit(CaloHit *pOriginalCaloHit, const float fraction1, CaloHit *&pDaughterCaloHit1,
    CaloHit *&pDaughterCaloHit2) const
{
    return m_pPandora->m_pCaloHitManager->FragmentCaloHit(pOriginalCaloHit, fraction1, pDaughterCaloHit1, pDaughterCaloHit2);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeCaloHitFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2, CaloHit *&pMergedCaloHit) const
{
    return m_pPandora->m_pCaloHitManager->MergeCaloHitFragments(pFragmentCaloHit1, pFragmentCaloHit2, pMergedCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentTrackList(const TrackList *&pTrackList, std::string &trackListName) const
{
    return m_pPandora->m_pTrackManager->GetCurrentList(pTrackList, trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentTrackListName(std::string &trackListName) const
{
    return m_pPandora->m_pTrackManager->GetCurrentListName(trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetTrackList(const std::string &trackListName, const TrackList *&pTrackList) const
{
    return m_pPandora->m_pTrackManager->GetList(trackListName, pTrackList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveTrackList(const TrackList &trackList, const std::string &newListName) const
{
    return m_pPandora->m_pTrackManager->SaveList(newListName, trackList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReplaceCurrentTrackList(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pTrackManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DropCurrentTrackList() const
{
    return m_pPandora->m_pTrackManager->DropCurrentList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddTrackClusterAssociation(Track *const pTrack, Cluster *const pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pTrack->SetAssociatedCluster(pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCluster->AddTrackAssociation(pTrack));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveTrackClusterAssociation(Track *const pTrack, Cluster *const pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pTrack->RemoveAssociatedCluster(pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCluster->RemoveTrackAssociation(pTrack));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveCurrentTrackClusterAssociations() const
{
    TrackList danglingTracks;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveCurrentTrackAssociations(danglingTracks));

    if (!danglingTracks.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(danglingTracks));

    TrackToClusterMap danglingClusters;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveCurrentClusterAssociations(danglingClusters));

    if (!danglingClusters.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveTrackAssociations(danglingClusters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveAllTrackClusterAssociations() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveAllClusterAssociations());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveAllTrackAssociations());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentClusterList(const ClusterList *&pClusterList, std::string &clusterListName) const
{
    return m_pPandora->m_pClusterManager->GetCurrentList(pClusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentClusterListName(std::string &clusterListName) const
{
    return m_pPandora->m_pClusterManager->GetCurrentListName(clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetClusterList(const std::string &clusterListName, const ClusterList *&pClusterList) const
{
    return m_pPandora->m_pClusterManager->GetList(clusterListName, pClusterList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::CreateTemporaryClusterListAndSetCurrent(const Algorithm &algorithm, const ClusterList *&pClusterList,
    std::string &temporaryListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateTemporaryListAndSetCurrent(&algorithm, temporaryListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentList(pClusterList, temporaryListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveClusterList(const std::string &newClusterListName) const
{
    std::string currentClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentListName(currentClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveObjects(newClusterListName, currentClusterListName));

    const ClusterList *pNewClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetList(newClusterListName, pNewClusterList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveClusterList(const std::string &newClusterListName, const ClusterList &clustersToSave) const
{
    std::string currentClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentListName(currentClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveObjects(newClusterListName, currentClusterListName, clustersToSave));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveClusterList(const std::string &oldClusterListName, const std::string &newClusterListName) const
{
    return m_pPandora->m_pClusterManager->SaveObjects(newClusterListName, oldClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveClusterList(const std::string &oldClusterListName, const std::string &newClusterListName,
    const ClusterList &clustersToSave) const
{
    return m_pPandora->m_pClusterManager->SaveObjects(newClusterListName, oldClusterListName, clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReplaceCurrentClusterList(const Algorithm &algorithm, const std::string &newClusterListName) const
{
    return m_pPandora->m_pClusterManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::TemporarilyReplaceCurrentClusterList(const std::string &newClusterListName) const
{
    return m_pPandora->m_pClusterManager->TemporarilyReplaceCurrentList(newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DropCurrentClusterList() const
{
    return m_pPandora->m_pClusterManager->DropCurrentList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeleteCluster(Cluster *pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteObject(pCluster));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeleteCluster(Cluster *pCluster, const std::string &clusterListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteObject(pCluster, clusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeleteClusters(const ClusterList &clusterList) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(clusterList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteObjects(clusterList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeleteClusters(const ClusterList &clusterList, const std::string &clusterListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(clusterList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteObjects(clusterList, clusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete) const
{
    if ((pClusterToEnlarge == pClusterToDelete) || !pClusterToEnlarge->IsAvailable() || !pClusterToDelete->IsAvailable())
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pClusterToDelete->GetAssociatedTrackList()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete, const std::string &enlargeListName,
    const std::string &deleteListName) const
{
    if ((pClusterToEnlarge == pClusterToDelete) || !pClusterToEnlarge->IsAvailable() || !pClusterToDelete->IsAvailable())
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pClusterToDelete->GetAssociatedTrackList()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete,
        enlargeListName, deleteListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentPfoList(const PfoList *&pPfoList, std::string &pfoListName) const
{
    return m_pPandora->m_pPfoManager->GetCurrentList(pPfoList, pfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentPfoListName(std::string &pfoListName) const
{
    return m_pPandora->m_pPfoManager->GetCurrentListName(pfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetPfoList(const std::string &pfoListName, const PfoList *&pPfoList) const
{
    return m_pPandora->m_pPfoManager->GetList(pfoListName, pPfoList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::CreateTemporaryPfoListAndSetCurrent(const Algorithm &algorithm, const PfoList *&pPfoList,
    std::string &temporaryListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->CreateTemporaryListAndSetCurrent(&algorithm, temporaryListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->GetCurrentList(pPfoList, temporaryListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SavePfoList(const std::string &newPfoListName) const
{
    std::string currentPfoListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->GetCurrentListName(currentPfoListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->SaveObjects(newPfoListName, currentPfoListName));

    const PfoList *pNewPfoList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->GetList(newPfoListName, pNewPfoList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SavePfoList(const std::string &newPfoListName, const PfoList &pfosToSave) const
{
    std::string currentPfoListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->GetCurrentListName(currentPfoListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->SaveObjects( newPfoListName, currentPfoListName, pfosToSave));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SavePfoList(const std::string &oldPfoListName, const std::string &newPfoListName) const
{
    return m_pPandora->m_pPfoManager->SaveObjects(newPfoListName, oldPfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SavePfoList(const std::string &oldPfoListName, const std::string &newPfoListName,
    const PfoList &pfosToSave) const
{
    return m_pPandora->m_pPfoManager->SaveObjects(newPfoListName, oldPfoListName, pfosToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReplaceCurrentPfoList(const Algorithm &algorithm, const std::string &newPfoListName) const
{
    return m_pPandora->m_pPfoManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newPfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::TemporarilyReplaceCurrentPfoList(const std::string &newPfoListName) const
{
    return m_pPandora->m_pPfoManager->TemporarilyReplaceCurrentList(newPfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DropCurrentPfoList() const
{
    return m_pPandora->m_pPfoManager->DropCurrentList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeletePfo(ParticleFlowObject *pPfo) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pPfo));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->DeleteObject(pPfo));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeletePfo(ParticleFlowObject *pPfo, const std::string &pfoListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pPfo));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->DeleteObject(pPfo, pfoListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeletePfos(const PfoList &pfoList) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pfoList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->DeleteObjects(pfoList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeletePfos(const PfoList &pfoList, const std::string &pfoListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pfoList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->DeleteObjects(pfoList, pfoListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddClusterToPfo(ParticleFlowObject *pPfo, Cluster *pCluster) const
{
    if (!pCluster->IsAvailable())
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->AddClusterToPfo(pPfo, pCluster));
    pCluster->SetAvailability(false);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddTrackToPfo(ParticleFlowObject *pPfo, Track *pTrack) const
{
    if (!pTrack->IsAvailable())
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->AddTrackToPfo(pPfo, pTrack));
    pTrack->SetAvailability(false);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveClusterFromPfo(ParticleFlowObject *pPfo, Cluster *pCluster) const
{
    if ((pPfo->GetNClusters() <= 1) && (pPfo->GetNTracks() == 0))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->RemoveClusterFromPfo(pPfo, pCluster));
    pCluster->SetAvailability(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveTrackFromPfo(ParticleFlowObject *pPfo, Track *pTrack) const
{
    if ((pPfo->GetNTracks() <= 1) && (pPfo->GetNClusters() == 0))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->RemoveTrackFromPfo(pPfo, pTrack));
    pTrack->SetAvailability(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentMCParticleList(const MCParticleList *&pMCParticleList, std::string &mcParticleListName) const
{
    return m_pPandora->m_pMCManager->GetCurrentList(pMCParticleList, mcParticleListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentMCParticleListName(std::string &mcParticleListName) const
{
    return m_pPandora->m_pMCManager->GetCurrentListName(mcParticleListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetMCParticleList(const std::string &mcParticleListName, const MCParticleList *&pMCParticleList) const
{
    return m_pPandora->m_pMCManager->GetList(mcParticleListName, pMCParticleList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveMCParticleList(const MCParticleList &mcParticleList, const std::string &newListName) const
{
    return m_pPandora->m_pMCManager->SaveList(newListName, mcParticleList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReplaceCurrentMCParticleList(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pMCManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DropCurrentMCParticleList() const
{
    return m_pPandora->m_pMCManager->DropCurrentList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RepeatMCParticlePreparation() const
{
    return m_pPandora->PrepareMCParticles();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveAllMCParticleRelationships() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->RemoveAllMCParticleRelationships());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->RemoveAllMCParticleRelationships());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveAllMCParticleRelationships());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::InitializeFragmentation(const Algorithm &algorithm, const ClusterList &inputClusterList,
    std::string &originalClustersListName, std::string &fragmentClustersListName) const
{
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MoveObjectsToTemporaryListAndSetCurrent(&algorithm, inputClusterListName, originalClustersListName, inputClusterList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->InitializeReclustering(&algorithm, inputClusterList, originalClustersListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateTemporaryListAndSetCurrent(&algorithm, fragmentClustersListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->PrepareForClustering(&algorithm, fragmentClustersListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::EndFragmentation(const Algorithm &algorithm, const std::string &clusterListToSaveName,
    const std::string &clusterListToDeleteName) const
{
    std::string inputClusterListName;
    const ClusterList *pClustersToBeDeleted = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveObjects(inputClusterListName, clusterListToSaveName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetList(clusterListToDeleteName, pClustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForReclusteringDeletion(*pClustersToBeDeleted));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteTemporaryObjects(&algorithm, clusterListToDeleteName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetCurrentListToAlgorithmInputList(&algorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->EndReclustering(&algorithm, clusterListToSaveName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::InitializeReclustering(const Algorithm &algorithm, const TrackList &inputTrackList,
    const ClusterList &inputClusterList, std::string &originalClustersListName) const
{
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MoveObjectsToTemporaryListAndSetCurrent(&algorithm, inputClusterListName, originalClustersListName, inputClusterList));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->InitializeReclustering(&algorithm, inputTrackList, originalClustersListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->InitializeReclustering(&algorithm, inputClusterList, originalClustersListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ReclusterHelper::InitializeReclusterMonitoring(inputTrackList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::EndReclustering(const Algorithm &algorithm, const std::string &selectedClusterListName) const
{
    std::string inputClusterListName;
    ClusterList clustersToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveObjects(inputClusterListName, selectedClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetResetDeletionObjects(&algorithm, clustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForReclusteringDeletion(clustersToBeDeleted));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->EndReclustering(&algorithm, selectedClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ReclusterHelper::EndReclusterMonitoring());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraContentApiImpl::PandoraContentApiImpl(Pandora *pPandora) :
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PrepareForDeletion(const Cluster *const pCluster) const
{
    if (!pCluster->IsAvailable())
        return STATUS_CODE_NOT_ALLOWED;

    CaloHitList caloHitList;
    pCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);

    const CaloHitList &isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());
    caloHitList.insert(isolatedCaloHitList.begin(), isolatedCaloHitList.end());

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(caloHitList, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pCluster->GetAssociatedTrackList()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PrepareForDeletion(const ClusterList &clusterList) const
{
    TrackList trackList;
    CaloHitList caloHitList;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (!pCluster->IsAvailable())
            return STATUS_CODE_NOT_ALLOWED;

        pCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);

        const CaloHitList &isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());
        caloHitList.insert(isolatedCaloHitList.begin(), isolatedCaloHitList.end());

        const TrackList &associatedTrackList(pCluster->GetAssociatedTrackList());
        trackList.insert(associatedTrackList.begin(), associatedTrackList.end());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(caloHitList, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(trackList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PrepareForDeletion(const PfoList &pfoList) const
{
    for (PfoList::const_iterator iter = pfoList.begin(), iterEnd = pfoList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(*iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PrepareForDeletion(const ParticleFlowObject *const pPfo) const
{
    const TrackList trackList(pPfo->GetTrackList());
    const ClusterList pfoList(pPfo->GetClusterList());

    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
        (*iter)->SetAvailability(true);

    for (ClusterList::const_iterator iter = pfoList.begin(), iterEnd = pfoList.end(); iter != iterEnd; ++iter)
        (*iter)->SetAvailability(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PrepareForReclusteringDeletion(const ClusterList &clusterList) const
{
    TrackList trackList;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (!pCluster->IsAvailable())
            return STATUS_CODE_NOT_ALLOWED;

        trackList.insert(pCluster->GetAssociatedTrackList().begin(), pCluster->GetAssociatedTrackList().end());
    }

    return m_pPandora->m_pTrackManager->RemoveClusterAssociations(trackList);
}

} // namespace pandora
