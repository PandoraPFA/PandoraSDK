/**
 *  @file   PandoraPFANew/Framework/src/Api/PandoraContentApi.cc
 * 
 *  @brief  Implementation of the pandora content api class.
 * 
 *  $Log: $
 */

#include "Pandora/Algorithm.h"

#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

template <typename CLUSTER_PARAMETERS>
pandora::StatusCode PandoraContentApi::Cluster::Create(const pandora::Algorithm &algorithm, CLUSTER_PARAMETERS *pClusterParameters)
{
    pandora::Cluster *pCluster = NULL;
    return algorithm.GetPandoraContentApiImpl()->CreateCluster(pClusterParameters, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename CLUSTER_PARAMETERS>
pandora::StatusCode PandoraContentApi::Cluster::Create(const pandora::Algorithm &algorithm, CLUSTER_PARAMETERS *pClusterParameters,
    pandora::Cluster *&pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->CreateCluster(pClusterParameters, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::ParticleFlowObject::Create(const pandora::Algorithm &algorithm, const Parameters &parameters)
{
    return algorithm.GetPandoraContentApiImpl()->CreateParticleFlowObject(parameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RepeatEventPreparation(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RepeatEventPreparation();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::CreateDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, pandora::TiXmlElement *const pXmlElement,
    std::string &daughterAlgorithmName)
{
    return parentAlgorithm.GetPandoraContentApiImpl()->CreateDaughterAlgorithm(pXmlElement, daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RunDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, const std::string &daughterAlgorithmName)
{
    return parentAlgorithm.GetPandoraContentApiImpl()->RunAlgorithm(daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
    const pandora::ClusterList *&pNewClusterList)
{
    std::string newClusterListName;
    return algorithm.GetPandoraContentApiImpl()->RunClusteringAlgorithm(algorithm, clusteringAlgorithmName, pNewClusterList, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
    const pandora::ClusterList *&pNewClusterList, std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->RunClusteringAlgorithm(algorithm, clusteringAlgorithmName, pNewClusterList, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentCaloHitList(const pandora::Algorithm &algorithm, const pandora::CaloHitList *&pCaloHitList)
{
    std::string caloHitListName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentCaloHitList(pCaloHitList, caloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentCaloHitList(const pandora::Algorithm &algorithm, const pandora::CaloHitList *&pCaloHitList,
    std::string &caloHitListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentCaloHitList(pCaloHitList, caloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentCaloHitListName(const pandora::Algorithm &algorithm, std::string &caloHitListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentCaloHitListName(caloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCaloHitList(const pandora::Algorithm &algorithm, const std::string &caloHitListName,
    const pandora::CaloHitList *&pCaloHitList)
{
    return algorithm.GetPandoraContentApiImpl()->GetCaloHitList(caloHitListName, pCaloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveCaloHitList(const pandora::Algorithm &algorithm, const pandora::CaloHitList &caloHitList,
    const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveCaloHitList(caloHitList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DropCurrentCaloHitList(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->DropCurrentCaloHitList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::ReplaceCurrentCaloHitList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentCaloHitList(algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PandoraContentApi::IsCaloHitAvailable(const pandora::Algorithm &algorithm, pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->IsCaloHitAvailable(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PandoraContentApi::AreCaloHitsAvailable(const pandora::Algorithm &algorithm, const pandora::CaloHitList &caloHitList)
{
    return algorithm.GetPandoraContentApiImpl()->AreCaloHitsAvailable(caloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddCaloHitToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
    pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->AddCaloHitToCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveCaloHitFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
    pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveCaloHitFromCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddIsolatedCaloHitToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
    pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->AddIsolatedCaloHitToCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveIsolatedCaloHitFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
    pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveIsolatedCaloHitFromCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::FragmentCaloHit(const pandora::Algorithm &algorithm, pandora::CaloHit *pOriginalCaloHit,
    const float fraction1, pandora::CaloHit *&pDaughterCaloHit1, pandora::CaloHit *&pDaughterCaloHit2)
{
    return algorithm.GetPandoraContentApiImpl()->FragmentCaloHit(pOriginalCaloHit, fraction1, pDaughterCaloHit1, pDaughterCaloHit2);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::MergeCaloHitFragments(const pandora::Algorithm &algorithm, pandora::CaloHit *pFragmentCaloHit1,
    pandora::CaloHit *pFragmentCaloHit2, pandora::CaloHit *&pMergedCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->MergeCaloHitFragments(pFragmentCaloHit1, pFragmentCaloHit2, pMergedCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList *&pTrackList)
{
    std::string trackListName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentTrackList(pTrackList, trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList *&pTrackList,
    std::string &trackListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentTrackList(pTrackList, trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentTrackListName(const pandora::Algorithm &algorithm, std::string &trackListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentTrackListName(trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetTrackList(const pandora::Algorithm &algorithm, const std::string &trackListName,
    const pandora::TrackList *&pTrackList)
{
    return algorithm.GetPandoraContentApiImpl()->GetTrackList(trackListName, pTrackList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList &trackList,
    const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveTrackList(trackList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::ReplaceCurrentTrackList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentTrackList(algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DropCurrentTrackList(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->DropCurrentTrackList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
    pandora::Cluster *const pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->AddTrackClusterAssociation(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
    pandora::Cluster *const pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveTrackClusterAssociation(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveCurrentTrackClusterAssociations(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveCurrentTrackClusterAssociations();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveAllTrackClusterAssociations(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveAllTrackClusterAssociations();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentClusterList(const pandora::Algorithm &algorithm, const pandora::ClusterList *&pClusterList)
{
    std::string clusterListName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentClusterList(pClusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentClusterList(const pandora::Algorithm &algorithm, const pandora::ClusterList *&pClusterList,
    std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentClusterList(pClusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentClusterListName(const pandora::Algorithm &algorithm, std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentClusterListName(clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetClusterList(const pandora::Algorithm &algorithm, const std::string &clusterListName,
    const pandora::ClusterList *&pClusterList)
{
    return algorithm.GetPandoraContentApiImpl()->GetClusterList(clusterListName, pClusterList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::CreateTemporaryClusterListAndSetCurrent(const pandora::Algorithm &algorithm,
    const pandora::ClusterList *&pClusterList, std::string &temporaryListName)
{
    return algorithm.GetPandoraContentApiImpl()->CreateTemporaryClusterListAndSetCurrent(algorithm, pClusterList, temporaryListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
    const pandora::ClusterList &clustersToSave)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(newClusterListName, clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &oldClusterListName,
    const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(oldClusterListName, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &oldClusterListName,
    const std::string &newClusterListName, const pandora::ClusterList &clustersToSave)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(oldClusterListName, newClusterListName, clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::ReplaceCurrentClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentClusterList(algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::TemporarilyReplaceCurrentClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->TemporarilyReplaceCurrentClusterList(newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DropCurrentClusterList(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->DropCurrentClusterList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeleteCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteCluster(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeleteCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, const std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteCluster(pCluster, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeleteClusters(const pandora::Algorithm &algorithm, const pandora::ClusterList &clusterList)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteClusters(clusterList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeleteClusters(const pandora::Algorithm &algorithm, const pandora::ClusterList &clusterList,
    const std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteClusters(clusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
    pandora::Cluster *pClusterToDelete)
{
    return algorithm.GetPandoraContentApiImpl()->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
    pandora::Cluster *pClusterToDelete, const std::string &enlargeListName, const std::string &deleteListName)
{
    return algorithm.GetPandoraContentApiImpl()->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete, enlargeListName, deleteListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentPfoList(const pandora::Algorithm &algorithm, const pandora::PfoList *&pPfoList)
{
    std::string pfoListName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentPfoList(pPfoList, pfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentPfoList(const pandora::Algorithm &algorithm, const pandora::PfoList *&pPfoList,
    std::string &pfoListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentPfoList(pPfoList, pfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentPfoListName(const pandora::Algorithm &algorithm, std::string &pfoListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentPfoListName(pfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetPfoList(const pandora::Algorithm &algorithm, const std::string &pfoListName,
    const pandora::PfoList *&pPfoList)
{
    return algorithm.GetPandoraContentApiImpl()->GetPfoList(pfoListName, pPfoList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::CreateTemporaryPfoListAndSetCurrent(const pandora::Algorithm &algorithm,
    const pandora::PfoList *&pPfoList, std::string &temporaryListName)
{
    return algorithm.GetPandoraContentApiImpl()->CreateTemporaryPfoListAndSetCurrent(algorithm, pPfoList, temporaryListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SavePfoList(const pandora::Algorithm &algorithm, const std::string &newPfoListName)
{
    return algorithm.GetPandoraContentApiImpl()->SavePfoList(newPfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SavePfoList(const pandora::Algorithm &algorithm, const std::string &newPfoListName,
    const pandora::PfoList &pfosToSave)
{
    return algorithm.GetPandoraContentApiImpl()->SavePfoList(newPfoListName, pfosToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SavePfoList(const pandora::Algorithm &algorithm, const std::string &oldPfoListName,
    const std::string &newPfoListName)
{
    return algorithm.GetPandoraContentApiImpl()->SavePfoList(oldPfoListName, newPfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SavePfoList(const pandora::Algorithm &algorithm, const std::string &oldPfoListName,
    const std::string &newPfoListName, const pandora::PfoList &pfosToSave)
{
    return algorithm.GetPandoraContentApiImpl()->SavePfoList(oldPfoListName, newPfoListName, pfosToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::ReplaceCurrentPfoList(const pandora::Algorithm &algorithm, const std::string &newPfoListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentPfoList(algorithm, newPfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::TemporarilyReplaceCurrentPfoList(const pandora::Algorithm &algorithm, const std::string &newPfoListName)
{
    return algorithm.GetPandoraContentApiImpl()->TemporarilyReplaceCurrentPfoList(newPfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DropCurrentPfoList(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->DropCurrentPfoList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeletePfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo)
{
    return algorithm.GetPandoraContentApiImpl()->DeletePfo(pPfo);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeletePfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo,
    const std::string &pfoListName)
{
    return algorithm.GetPandoraContentApiImpl()->DeletePfo(pPfo, pfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeletePfos(const pandora::Algorithm &algorithm, const pandora::PfoList &pfoList)
{
    return algorithm.GetPandoraContentApiImpl()->DeletePfos(pfoList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeletePfos(const pandora::Algorithm &algorithm, const pandora::PfoList &pfoList,
    const std::string &pfoListName)
{
    return algorithm.GetPandoraContentApiImpl()->DeletePfos(pfoList, pfoListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddClusterToPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo,
    pandora::Cluster *pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->AddClusterToPfo(pPfo, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddTrackToPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo,
    pandora::Track *pTrack)
{
    return algorithm.GetPandoraContentApiImpl()->AddTrackToPfo(pPfo, pTrack);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveClusterFromPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo,
    pandora::Cluster *pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveClusterFromPfo(pPfo, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveTrackFromPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pPfo,
    pandora::Track *pTrack)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveTrackFromPfo(pPfo, pTrack);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentMCParticleList(const pandora::Algorithm &algorithm, const pandora::MCParticleList *&pMCParticleList)
{
    std::string mcParticleListName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentMCParticleList(pMCParticleList, mcParticleListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentMCParticleList(const pandora::Algorithm &algorithm, const pandora::MCParticleList *&pMCParticleList,
    std::string &mcParticleListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentMCParticleList(pMCParticleList, mcParticleListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentMCParticleListName(const pandora::Algorithm &algorithm, std::string &mcParticleListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentMCParticleListName(mcParticleListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetMCParticleList(const pandora::Algorithm &algorithm, const std::string &mcParticleListName,
    const pandora::MCParticleList *&pMCParticleList)
{
    return algorithm.GetPandoraContentApiImpl()->GetMCParticleList(mcParticleListName, pMCParticleList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveMCParticleList(const pandora::Algorithm &algorithm, const pandora::MCParticleList &mcParticleList,
    const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveMCParticleList(mcParticleList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::ReplaceCurrentMCParticleList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentMCParticleList(algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DropCurrentMCParticleList(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->DropCurrentMCParticleList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RepeatMCParticlePreparation(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RepeatMCParticlePreparation();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveAllMCParticleRelationships(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveAllMCParticleRelationships();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::InitializeFragmentation(const pandora::Algorithm &algorithm, const pandora::ClusterList &inputClusterList,
    std::string &originalClustersListName, std::string &fragmentClustersListName)
{
    return algorithm.GetPandoraContentApiImpl()->InitializeFragmentation(algorithm, inputClusterList, originalClustersListName,
        fragmentClustersListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::EndFragmentation(const pandora::Algorithm &algorithm, const std::string &clusterListToSaveName,
    const std::string &clusterListToDeleteName)
{
    return algorithm.GetPandoraContentApiImpl()->EndFragmentation(algorithm, clusterListToSaveName, clusterListToDeleteName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::InitializeReclustering(const pandora::Algorithm &algorithm, const pandora::TrackList &inputTrackList,
    const pandora::ClusterList &inputClusterList, std::string &originalClustersListName)
{
    return algorithm.GetPandoraContentApiImpl()->InitializeReclustering(algorithm, inputTrackList, inputClusterList, originalClustersListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::EndReclustering(const pandora::Algorithm &algorithm, const std::string &selectedClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->EndReclustering(algorithm, selectedClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHit>(const pandora::Algorithm &algorithm, pandora::CaloHit *pCaloHit);
template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHitList>(const pandora::Algorithm &algorithm, pandora::CaloHitList *pCaloHitList);
template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::Track>(const pandora::Algorithm &algorithm, pandora::Track *pTrack);

template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHit>(const pandora::Algorithm &algorithm, pandora::CaloHit *pCaloHit, pandora::Cluster *&pCluster);
template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHitList>(const pandora::Algorithm &algorithm, pandora::CaloHitList *pCaloHitList, pandora::Cluster *&pCluster);
template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::Track>(const pandora::Algorithm &algorithm, pandora::Track *pTrack, pandora::Cluster *&pCluster);
