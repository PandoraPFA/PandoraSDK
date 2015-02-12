/**
 *  @file   PandoraSDK/src/Api/PandoraContentApiImpl.cc
 * 
 *  @brief  Implementation of the pandora content api class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApiImpl.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/GeometryManager.h"
#include "Managers/MCManager.h"
#include "Managers/ParticleFlowObjectManager.h"
#include "Managers/PluginManager.h"
#include "Managers/TrackManager.h"
#include "Managers/VertexManager.h"

#include "Objects/Cluster.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"
#include "Objects/Vertex.h"

#include "Pandora/Algorithm.h"
#include "Pandora/AlgorithmTool.h"
#include "Pandora/Pandora.h"
#include "Pandora/PandoraSettings.h"

namespace pandora
{

template <>
StatusCode PandoraContentApiImpl::AlterMetadata(const CaloHit *const pCaloHit, const PandoraContentApi::CaloHit::Metadata &metadata) const
{
    return m_pPandora->m_pCaloHitManager->AlterMetadata(pCaloHit, metadata);
}

template <>
StatusCode PandoraContentApiImpl::AlterMetadata(const Cluster *const pCluster, const PandoraContentApi::Cluster::Metadata &metadata) const
{
    return m_pPandora->m_pClusterManager->AlterMetadata(pCluster, metadata);
}

template <>
StatusCode PandoraContentApiImpl::AlterMetadata(const ParticleFlowObject *const pPfo, const PandoraContentApi::ParticleFlowObject::Metadata &metadata) const
{
    return m_pPandora->m_pPfoManager->AlterMetadata(pPfo, metadata);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::CreateObject(const PandoraContentApi::Cluster::Parameters &parameters, const Cluster *&pCluster) const
{
    if (!m_pPandora->m_pCaloHitManager->IsAvailable(&parameters.m_caloHitList))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateCluster(parameters, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetAvailability(&parameters.m_caloHitList, false));

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::CreateObject(const PandoraContentApi::ParticleFlowObject::Parameters &pfoParameters,
    const ParticleFlowObject *&pPfo) const
{
    if (!m_pPandora->m_pClusterManager->IsAvailable(&pfoParameters.m_clusterList) ||
        !m_pPandora->m_pTrackManager->IsAvailable(&pfoParameters.m_trackList) ||
        !m_pPandora->m_pVertexManager->IsAvailable(&pfoParameters.m_vertexList))
    {
        return STATUS_CODE_NOT_ALLOWED;
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->CreateParticleFlowObject(pfoParameters, pPfo));
    m_pPandora->m_pClusterManager->SetAvailability(&pfoParameters.m_clusterList, false);
    m_pPandora->m_pTrackManager->SetAvailability(&pfoParameters.m_trackList, false);
    m_pPandora->m_pVertexManager->SetAvailability(&pfoParameters.m_vertexList, false);

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::CreateObject(const PandoraContentApi::Vertex::Parameters &parameters, const Vertex *&pVertex) const
{
    return m_pPandora->m_pVertexManager->CreateVertex(parameters, pVertex);
}

template <>
StatusCode PandoraContentApiImpl::CreateObject(const PandoraApi::MCParticle::Parameters &parameters, const MCParticle *&pMCParticle) const
{
    return m_pPandora->m_pMCManager->CreateMCParticle(parameters, pMCParticle);
}

template <>
StatusCode PandoraContentApiImpl::CreateObject(const PandoraApi::Track::Parameters &parameters, const Track *&pTrack) const
{
    return m_pPandora->m_pTrackManager->CreateTrack(parameters, pTrack);
}

template <>
StatusCode PandoraContentApiImpl::CreateObject(const PandoraApi::RectangularCaloHit::Parameters &parameters, const CaloHit *&pCaloHit) const
{
    return m_pPandora->m_pCaloHitManager->CreateCaloHit(parameters, pCaloHit);
}

template <>
StatusCode PandoraContentApiImpl::CreateObject(const PandoraApi::PointingCaloHit::Parameters &parameters, const CaloHit *&pCaloHit) const
{
    return m_pPandora->m_pCaloHitManager->CreateCaloHit(parameters, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PandoraSettings *PandoraContentApiImpl::GetSettings() const
{
    return m_pPandora->GetSettings();
}

//------------------------------------------------------------------------------------------------------------------------------------------

const GeometryManager *PandoraContentApiImpl::GetGeometry() const
{
    return m_pPandora->GetGeometry();
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PluginManager *PandoraContentApiImpl::GetPlugins() const
{
    return m_pPandora->GetPlugins();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RepeatEventPreparation() const
{
    return m_pPandora->PrepareEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::CreateAlgorithmTool(TiXmlElement *const pXmlElement, AlgorithmTool *&pAlgorithmTool) const
{
    return m_pPandora->m_pAlgorithmManager->CreateAlgorithmTool(pXmlElement, pAlgorithmTool);
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

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PreRunAlgorithm(iter->second));

    try
    {
        const bool shouldDisplayAlgorithmInfo(m_pPandora->GetSettings()->ShouldDisplayAlgorithmInfo());

        if (shouldDisplayAlgorithmInfo)
        {
            for (unsigned int i = 1, iMax = m_pPandora->m_pCaloHitManager->m_algorithmInfoMap.size(); i < iMax; ++i) std::cout << "----";
            std::cout << "> Running Algorithm: " << iter->first << ", " << iter->second->GetType() << std::endl;
        }

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->Run());
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failure in algorithm " << iter->first << ", " << iter->second->GetType() << ", " << statusCodeException.ToString()
                  << statusCodeException.GetBackTrace() << std::endl;
    }
    catch (...)
    {
        std::cout << "Failure in algorithm " << iter->first << ", " << iter->second->GetType() << ", unrecognized exception" << std::endl;
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PostRunAlgorithm(iter->second));

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

template <>
StatusCode PandoraContentApiImpl::GetCurrentList(const CaloHitList *&pCaloHitList, std::string &listName) const
{
    return m_pPandora->m_pCaloHitManager->GetCurrentList(pCaloHitList, listName);
}

template <>
StatusCode PandoraContentApiImpl::GetCurrentList(const TrackList *&pTrackList, std::string &listName) const
{
    return m_pPandora->m_pTrackManager->GetCurrentList(pTrackList, listName);
}

template <>
StatusCode PandoraContentApiImpl::GetCurrentList(const ClusterList *&pClusterList, std::string &listName) const
{
    return m_pPandora->m_pClusterManager->GetCurrentList(pClusterList, listName);
}

template <>
StatusCode PandoraContentApiImpl::GetCurrentList(const PfoList *&pPfoList, std::string &listName) const
{
    return m_pPandora->m_pPfoManager->GetCurrentList(pPfoList, listName);
}

template <>
StatusCode PandoraContentApiImpl::GetCurrentList(const MCParticleList *&pMCParticleList, std::string &listName) const
{
    return m_pPandora->m_pMCManager->GetCurrentList(pMCParticleList, listName);
}

template <>
StatusCode PandoraContentApiImpl::GetCurrentList(const VertexList *&pVertexList, std::string &listName) const
{
    return m_pPandora->m_pVertexManager->GetCurrentList(pVertexList, listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::GetCurrentListName<CaloHit>(std::string &listName) const
{
    return m_pPandora->m_pCaloHitManager->GetCurrentListName(listName);
}

template <>
StatusCode PandoraContentApiImpl::GetCurrentListName<Track>(std::string &listName) const
{
    return m_pPandora->m_pTrackManager->GetCurrentListName(listName);
}

template <>
StatusCode PandoraContentApiImpl::GetCurrentListName<MCParticle>(std::string &listName) const
{
    return m_pPandora->m_pMCManager->GetCurrentListName(listName);
}

template <>
StatusCode PandoraContentApiImpl::GetCurrentListName<Cluster>(std::string &listName) const
{
    return m_pPandora->m_pClusterManager->GetCurrentListName(listName);
}

template <>
StatusCode PandoraContentApiImpl::GetCurrentListName<Pfo>(std::string &listName) const
{
    return m_pPandora->m_pPfoManager->GetCurrentListName(listName);
}

template <>
StatusCode PandoraContentApiImpl::GetCurrentListName<Vertex>(std::string &listName) const
{
    return m_pPandora->m_pVertexManager->GetCurrentListName(listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::ReplaceCurrentList<CaloHit>(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pCaloHitManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

template <>
StatusCode PandoraContentApiImpl::ReplaceCurrentList<Track>(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pTrackManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

template <>
StatusCode PandoraContentApiImpl::ReplaceCurrentList<MCParticle>(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pMCManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

template <>
StatusCode PandoraContentApiImpl::ReplaceCurrentList<Cluster>(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pClusterManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

template <>
StatusCode PandoraContentApiImpl::ReplaceCurrentList<Pfo>(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pPfoManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

template <>
StatusCode PandoraContentApiImpl::ReplaceCurrentList<Vertex>(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pVertexManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::DropCurrentList<CaloHit>(const Algorithm &algorithm) const
{
    return m_pPandora->m_pCaloHitManager->DropCurrentList(&algorithm);
}

template <>
StatusCode PandoraContentApiImpl::DropCurrentList<Track>(const Algorithm &algorithm) const
{
    return m_pPandora->m_pTrackManager->DropCurrentList(&algorithm);
}

template <>
StatusCode PandoraContentApiImpl::DropCurrentList<MCParticle>(const Algorithm &algorithm) const
{
    return m_pPandora->m_pMCManager->DropCurrentList(&algorithm);
}

template <>
StatusCode PandoraContentApiImpl::DropCurrentList<Cluster>(const Algorithm &algorithm) const
{
    return m_pPandora->m_pClusterManager->DropCurrentList(&algorithm);
}

template <>
StatusCode PandoraContentApiImpl::DropCurrentList<Pfo>(const Algorithm &algorithm) const
{
    return m_pPandora->m_pPfoManager->DropCurrentList(&algorithm);
}

template <>
StatusCode PandoraContentApiImpl::DropCurrentList<Vertex>(const Algorithm &algorithm) const
{
    return m_pPandora->m_pVertexManager->DropCurrentList(&algorithm);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::GetList(const std::string &listName, const CaloHitList *&pList) const
{
    return m_pPandora->m_pCaloHitManager->GetList(listName, pList);
}

template <>
StatusCode PandoraContentApiImpl::GetList(const std::string &listName, const TrackList *&pList) const
{
    return m_pPandora->m_pTrackManager->GetList(listName, pList);
}

template <>
StatusCode PandoraContentApiImpl::GetList(const std::string &listName, const MCParticleList *&pList) const
{
    return m_pPandora->m_pMCManager->GetList(listName, pList);
}

template <>
StatusCode PandoraContentApiImpl::GetList(const std::string &listName, const ClusterList *&pList) const
{
    return m_pPandora->m_pClusterManager->GetList(listName, pList);
}

template <>
StatusCode PandoraContentApiImpl::GetList(const std::string &listName, const PfoList *&pList) const
{
    return m_pPandora->m_pPfoManager->GetList(listName, pList);
}

template <>
StatusCode PandoraContentApiImpl::GetList(const std::string &listName, const VertexList *&pList) const
{
    return m_pPandora->m_pVertexManager->GetList(listName, pList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::SaveList(const CaloHitList &list, const std::string &newListName) const
{
    return m_pPandora->m_pCaloHitManager->SaveList(newListName, list);
}

template <>
StatusCode PandoraContentApiImpl::SaveList(const TrackList &trackList, const std::string &newListName) const
{
    return m_pPandora->m_pTrackManager->SaveList(newListName, trackList);
}

template <>
StatusCode PandoraContentApiImpl::SaveList(const MCParticleList &mcParticleList, const std::string &newListName) const
{
    return m_pPandora->m_pMCManager->SaveList(newListName, mcParticleList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::SaveList<Cluster>(const std::string &newListName) const
{
    std::string currentListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentListName(currentListName));
    return m_pPandora->m_pClusterManager->SaveObjects(newListName, currentListName);
}

template <>
StatusCode PandoraContentApiImpl::SaveList<Pfo>(const std::string &newListName) const
{
    std::string currentListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->GetCurrentListName(currentListName));
    return m_pPandora->m_pPfoManager->SaveObjects(newListName, currentListName);
}

template <>
StatusCode PandoraContentApiImpl::SaveList<Vertex>(const std::string &newListName) const
{
    std::string currentListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pVertexManager->GetCurrentListName(currentListName));
    return m_pPandora->m_pVertexManager->SaveObjects(newListName, currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::SaveList<Cluster>(const std::string &oldListName, const std::string &newListName) const
{
    return m_pPandora->m_pClusterManager->SaveObjects(newListName, oldListName);
}

template <>
StatusCode PandoraContentApiImpl::SaveList<Pfo>(const std::string &oldListName, const std::string &newListName) const
{
    return m_pPandora->m_pPfoManager->SaveObjects(newListName, oldListName);
}

template <>
StatusCode PandoraContentApiImpl::SaveList<Vertex>(const std::string &oldListName, const std::string &newListName) const
{
    return m_pPandora->m_pVertexManager->SaveObjects(newListName, oldListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::SaveList(const std::string &newListName, const ClusterList &clustersToSave) const
{
    std::string currentListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentListName(currentListName));
    return m_pPandora->m_pClusterManager->SaveObjects(newListName, currentListName, clustersToSave);
}

template <>
StatusCode PandoraContentApiImpl::SaveList(const std::string &newListName, const PfoList &pfosToSave) const
{
    std::string currentListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->GetCurrentListName(currentListName));
    return m_pPandora->m_pPfoManager->SaveObjects(newListName, currentListName, pfosToSave);
}

template <>
StatusCode PandoraContentApiImpl::SaveList(const std::string &newListName, const VertexList &verticesToSave) const
{
    std::string currentListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pVertexManager->GetCurrentListName(currentListName));
    return m_pPandora->m_pVertexManager->SaveObjects(newListName, currentListName, verticesToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::SaveList(const std::string &oldListName, const std::string &newListName, const ClusterList &clustersToSave) const
{
    return m_pPandora->m_pClusterManager->SaveObjects(newListName, oldListName, clustersToSave);
}

template <>
StatusCode PandoraContentApiImpl::SaveList(const std::string &oldListName, const std::string &newListName, const PfoList &pfosToSave) const
{
    return m_pPandora->m_pPfoManager->SaveObjects(newListName, oldListName, pfosToSave);
}

template <>
StatusCode PandoraContentApiImpl::SaveList(const std::string &oldListName, const std::string &newListName, const VertexList &verticesToSave) const
{
    return m_pPandora->m_pVertexManager->SaveObjects(newListName, oldListName, verticesToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::TemporarilyReplaceCurrentList<Cluster>(const std::string &newListName) const
{
    return m_pPandora->m_pClusterManager->TemporarilyReplaceCurrentList(newListName);
}

template <>
StatusCode PandoraContentApiImpl::TemporarilyReplaceCurrentList<Pfo>(const std::string &newListName) const
{
    return m_pPandora->m_pPfoManager->TemporarilyReplaceCurrentList(newListName);
}

template <>
StatusCode PandoraContentApiImpl::TemporarilyReplaceCurrentList<Vertex>(const std::string &newListName) const
{
    return m_pPandora->m_pVertexManager->TemporarilyReplaceCurrentList(newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::CreateTemporaryListAndSetCurrent(const Algorithm &algorithm, const ClusterList *&pClusterList, std::string &temporaryListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateTemporaryListAndSetCurrent(&algorithm, temporaryListName));
    return m_pPandora->m_pClusterManager->GetCurrentList(pClusterList, temporaryListName);
}

template <>
StatusCode PandoraContentApiImpl::CreateTemporaryListAndSetCurrent(const Algorithm &algorithm, const PfoList *&pPfoList, std::string &temporaryListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->CreateTemporaryListAndSetCurrent(&algorithm, temporaryListName));
    return m_pPandora->m_pPfoManager->GetCurrentList(pPfoList, temporaryListName);
}

template <>
StatusCode PandoraContentApiImpl::CreateTemporaryListAndSetCurrent(const Algorithm &algorithm, const VertexList *&pVertexList, std::string &temporaryListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pVertexManager->CreateTemporaryListAndSetCurrent(&algorithm, temporaryListName));
    return m_pPandora->m_pVertexManager->GetCurrentList(pVertexList, temporaryListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
bool PandoraContentApiImpl::IsAvailable(const pandora::CaloHit *const pCaloHit) const
{
    return m_pPandora->m_pCaloHitManager->IsAvailable(pCaloHit);
}

template <>
bool PandoraContentApiImpl::IsAvailable(const pandora::CaloHitList *const pCaloHitList) const
{
    return m_pPandora->m_pCaloHitManager->IsAvailable(pCaloHitList);
}

template <>
bool PandoraContentApiImpl::IsAvailable(const pandora::Track *const pTrack) const
{
    return m_pPandora->m_pTrackManager->IsAvailable(pTrack);
}

template <>
bool PandoraContentApiImpl::IsAvailable(const pandora::TrackList *const pTrackList) const
{
    return m_pPandora->m_pTrackManager->IsAvailable(pTrackList);
}

template <>
bool PandoraContentApiImpl::IsAvailable(const pandora::Cluster *const pCluster) const
{
    return m_pPandora->m_pClusterManager->IsAvailable(pCluster);
}

template <>
bool PandoraContentApiImpl::IsAvailable(const pandora::ClusterList *const pClusterList) const
{
    return m_pPandora->m_pClusterManager->IsAvailable(pClusterList);
}

template <>
bool PandoraContentApiImpl::IsAvailable(const pandora::Vertex *const pVertex) const
{
    return m_pPandora->m_pVertexManager->IsAvailable(pVertex);
}

template <>
bool PandoraContentApiImpl::IsAvailable(const pandora::VertexList *const pVertexList) const
{
    return m_pPandora->m_pVertexManager->IsAvailable(pVertexList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::AddToCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    if (!this->IsAddToClusterAllowed(pCluster, pCaloHit))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->AddToCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetAvailability(pCaloHit, false));

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::AddToCluster(const Cluster *const pCluster, const CaloHitList *const pCaloHitList) const
{
    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (!this->IsAddToClusterAllowed(pCluster, *iter))
            return STATUS_CODE_NOT_ALLOWED;
    }

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->AddToCluster(pCluster, *iter));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetAvailability(*iter, false));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveFromCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    if ((pCluster->GetNCaloHits() <= 1) && (pCluster->GetNIsolatedCaloHits() == 0))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveFromCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetAvailability(pCaloHit, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::AddIsolatedToCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    if (!this->IsAddToClusterAllowed(pCluster, pCaloHit))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->AddIsolatedToCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetAvailability(pCaloHit, false));
    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::AddIsolatedToCluster(const Cluster *const pCluster, const CaloHitList *const pCaloHitList) const
{
    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (!this->IsAddToClusterAllowed(pCluster, *iter))
            return STATUS_CODE_NOT_ALLOWED;
    }

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->AddIsolatedToCluster(pCluster, *iter));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetAvailability(*iter, false));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveIsolatedFromCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    if ((pCluster->GetNCaloHits() == 0) && (pCluster->GetNIsolatedCaloHits() <= 1))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveIsolatedFromCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetAvailability(pCaloHit, true));
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::Fragment(const CaloHit *const pOriginalCaloHit, const float fraction1, const CaloHit *&pDaughterCaloHit1, const CaloHit *&pDaughterCaloHit2) const
{
    return m_pPandora->m_pCaloHitManager->FragmentCaloHit(pOriginalCaloHit, fraction1, pDaughterCaloHit1, pDaughterCaloHit2);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeFragments(const CaloHit *const pFragmentCaloHit1, const CaloHit *const pFragmentCaloHit2, const CaloHit *&pMergedCaloHit) const
{
    return m_pPandora->m_pCaloHitManager->MergeCaloHitFragments(pFragmentCaloHit1, pFragmentCaloHit2, pMergedCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddTrackClusterAssociation(const Track *const pTrack, const Cluster *const pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->SetAssociatedCluster(pTrack, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->AddTrackAssociation(pCluster, pTrack));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveTrackClusterAssociation(const Track *const pTrack, const Cluster *const pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveAssociatedCluster(pTrack, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveTrackAssociation(pCluster, pTrack));

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

StatusCode PandoraContentApiImpl::RemoveAllMCParticleRelationships() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->RemoveAllMCParticleRelationships());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->RemoveAllMCParticleRelationships());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveAllMCParticleRelationships());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeAndDeleteClusters(const Cluster *const pClusterToEnlarge, const Cluster *const pClusterToDelete) const
{
    if ((pClusterToEnlarge == pClusterToDelete) || !m_pPandora->m_pClusterManager->IsAvailable(pClusterToDelete))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pClusterToDelete->GetAssociatedTrackList()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeAndDeleteClusters(const Cluster *const pClusterToEnlarge, const Cluster *const pClusterToDelete, const std::string &enlargeListName,
    const std::string &deleteListName) const
{
    if ((pClusterToEnlarge == pClusterToDelete) || !m_pPandora->m_pClusterManager->IsAvailable(pClusterToDelete))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pClusterToDelete->GetAssociatedTrackList()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete,
        enlargeListName, deleteListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::AddToPfo(const ParticleFlowObject *const pPfo, const Cluster *const pCluster) const
{
    if (!m_pPandora->m_pClusterManager->IsAvailable(pCluster))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->AddToPfo(pPfo, pCluster));
    m_pPandora->m_pClusterManager->SetAvailability(pCluster, false);
    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::AddToPfo(const ParticleFlowObject *const pPfo, const Track *const pTrack) const
{
    if (!m_pPandora->m_pTrackManager->IsAvailable(pTrack))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->AddToPfo(pPfo, pTrack));
    m_pPandora->m_pTrackManager->SetAvailability(pTrack, false);
    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::AddToPfo(const ParticleFlowObject *const pPfo, const Vertex *const pVertex) const
{
    if (!m_pPandora->m_pVertexManager->IsAvailable(pVertex))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->AddToPfo(pPfo, pVertex));
    m_pPandora->m_pVertexManager->SetAvailability(pVertex, false);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::RemoveFromPfo(const ParticleFlowObject *const pPfo, const Cluster *const pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->RemoveFromPfo(pPfo, pCluster));
    m_pPandora->m_pClusterManager->SetAvailability(pCluster, true);

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::RemoveFromPfo(const ParticleFlowObject *const pPfo, const Track *const pTrack) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->RemoveFromPfo(pPfo, pTrack));
    m_pPandora->m_pTrackManager->SetAvailability(pTrack, true);

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::RemoveFromPfo(const ParticleFlowObject *const pPfo, const Vertex *const pVertex) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->RemoveFromPfo(pPfo, pVertex));
    m_pPandora->m_pVertexManager->SetAvailability(pVertex, true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SetPfoParentDaughterRelationship(const ParticleFlowObject *const pParentPfo, const ParticleFlowObject *const pDaughterPfo) const
{
    return m_pPandora->m_pPfoManager->SetParentDaughterAssociation(pParentPfo, pDaughterPfo);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemovePfoParentDaughterRelationship(const ParticleFlowObject *const pParentPfo, const ParticleFlowObject *const pDaughterPfo) const
{
    return m_pPandora->m_pPfoManager->RemoveParentDaughterAssociation(pParentPfo, pDaughterPfo);
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraContentApiImpl::PandoraContentApiImpl(Pandora *const pPandora) :
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PandoraContentApiImpl::IsAddToClusterAllowed(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    if (!m_pPandora->m_pCaloHitManager->IsAvailable(pCaloHit))
        return false;

    if (!m_pPandora->GetSettings()->SingleHitTypeClusteringMode())
        return true;

    const CaloHit *pFirstCaloHit(NULL);

    if (!pCluster->GetOrderedCaloHitList().empty())
    {
        if (pCluster->GetOrderedCaloHitList().begin()->second->empty())
            throw StatusCodeException(STATUS_CODE_FAILURE);

        pFirstCaloHit = *(pCluster->GetOrderedCaloHitList().begin()->second->begin());
    }
    else if (!pCluster->GetIsolatedCaloHitList().empty())
    {
        pFirstCaloHit = *(pCluster->GetIsolatedCaloHitList().begin());
    }

    if ((NULL == pFirstCaloHit) || (pFirstCaloHit->GetHitType() == pCaloHit->GetHitType()))
        return true;

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::PrepareForDeletion(const Cluster *const pCluster) const
{
    if (!m_pPandora->m_pClusterManager->IsAvailable(pCluster))
        return STATUS_CODE_NOT_ALLOWED;

    CaloHitList caloHitList;
    pCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);

    const CaloHitList &isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());
    caloHitList.insert(isolatedCaloHitList.begin(), isolatedCaloHitList.end());

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetAvailability(&caloHitList, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pCluster->GetAssociatedTrackList()));

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::PrepareForDeletion(const ClusterList *const pClusterList) const
{
    TrackList trackList;
    CaloHitList caloHitList;

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster = *iter;

        if (!m_pPandora->m_pClusterManager->IsAvailable(pCluster))
            return STATUS_CODE_NOT_ALLOWED;

        pCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);
        const CaloHitList &isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());
        caloHitList.insert(isolatedCaloHitList.begin(), isolatedCaloHitList.end());

        const TrackList &associatedTrackList(pCluster->GetAssociatedTrackList());
        trackList.insert(associatedTrackList.begin(), associatedTrackList.end());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetAvailability(&caloHitList, true));
    return m_pPandora->m_pTrackManager->RemoveClusterAssociations(trackList);
}

template <>
StatusCode PandoraContentApiImpl::PrepareForDeletion(const ParticleFlowObject *const pPfo) const
{
    m_pPandora->m_pTrackManager->SetAvailability(&pPfo->GetTrackList(), true);
    m_pPandora->m_pClusterManager->SetAvailability(&pPfo->GetClusterList(), true);

    const PfoList parentList(pPfo->GetParentPfoList());
    const PfoList daughterList(pPfo->GetDaughterPfoList());

    for (PfoList::const_iterator iter = parentList.begin(), iterEnd = parentList.end(); iter != iterEnd; ++iter)
        this->RemovePfoParentDaughterRelationship(*iter, pPfo);

    for (PfoList::const_iterator iter = daughterList.begin(), iterEnd = daughterList.end(); iter != iterEnd; ++iter)
        this->RemovePfoParentDaughterRelationship(pPfo, *iter);

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::PrepareForDeletion(const PfoList *const pPfoList) const
{
    for (PfoList::const_iterator iter = pPfoList->begin(), iterEnd = pPfoList->end(); iter != iterEnd; ++iter)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(*iter));

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::PrepareForDeletion(const Vertex *const pVertex) const
{
    if (!m_pPandora->m_pVertexManager->IsAvailable(pVertex))
        return STATUS_CODE_NOT_ALLOWED;

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::PrepareForDeletion(const VertexList *const pVertexList) const
{
    for (VertexList::const_iterator iter = pVertexList->begin(), iterEnd = pVertexList->end(); iter != iterEnd; ++iter)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(*iter));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::PrepareForReclusteringDeletion(const ClusterList *const pClusterList) const
{
    TrackList trackList;
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster = *iter;

        if (!m_pPandora->m_pClusterManager->IsAvailable(pCluster))
            return STATUS_CODE_NOT_ALLOWED;

        trackList.insert(pCluster->GetAssociatedTrackList().begin(), pCluster->GetAssociatedTrackList().end());
    }

    return m_pPandora->m_pTrackManager->RemoveClusterAssociations(trackList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::Delete(const Cluster *const pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pCluster));
    return m_pPandora->m_pClusterManager->DeleteObject(pCluster);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const ClusterList *const pClusterList) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pClusterList));
    return m_pPandora->m_pClusterManager->DeleteObjects(*pClusterList);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const ParticleFlowObject *const pPfo) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pPfo));
    return m_pPandora->m_pPfoManager->DeleteObject(pPfo);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const PfoList *const pPfoList) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pPfoList));
    return m_pPandora->m_pPfoManager->DeleteObjects(*pPfoList);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const Vertex *const pVertex) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pVertex));
    return m_pPandora->m_pVertexManager->DeleteObject(pVertex);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const VertexList *const pVertexList) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pVertexList));
    return m_pPandora->m_pVertexManager->DeleteObjects(*pVertexList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::Delete(const Cluster *const pCluster, const std::string &listName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pCluster));
    return m_pPandora->m_pClusterManager->DeleteObject(pCluster, listName);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const ClusterList *const pClusterList, const std::string &listName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pClusterList));
    return m_pPandora->m_pClusterManager->DeleteObjects(*pClusterList, listName);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const ParticleFlowObject *const pPfo, const std::string &listName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pPfo));
    return m_pPandora->m_pPfoManager->DeleteObject(pPfo, listName);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const PfoList *const pPfoList, const std::string &listName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pPfoList));
    return m_pPandora->m_pPfoManager->DeleteObjects(*pPfoList, listName);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const Vertex *const pVertex, const std::string &listName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pVertex));
    return m_pPandora->m_pVertexManager->DeleteObject(pVertex, listName);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const VertexList *const pVertexList, const std::string &listName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pVertexList));
    return m_pPandora->m_pVertexManager->DeleteObjects(*pVertexList, listName);
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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForReclusteringDeletion(pClustersToBeDeleted));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteTemporaryObjects(&algorithm, clusterListToDeleteName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetCurrentListToAlgorithmInputList(&algorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->EndReclustering(&algorithm, clusterListToSaveName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetCurrentListToAlgorithmInputList(&algorithm));

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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForReclusteringDeletion(&clustersToBeDeleted));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->EndReclustering(&algorithm, selectedClusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PreRunAlgorithm(Algorithm *const pAlgorithm) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->RegisterAlgorithm(pAlgorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RegisterAlgorithm(pAlgorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->RegisterAlgorithm(pAlgorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->RegisterAlgorithm(pAlgorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RegisterAlgorithm(pAlgorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pVertexManager->RegisterAlgorithm(pAlgorithm));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PostRunAlgorithm(Algorithm *const pAlgorithm) const
{
    PfoList pfosToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->GetResetDeletionObjects(pAlgorithm, pfosToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(&pfosToBeDeleted));

    ClusterList clustersToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetResetDeletionObjects(pAlgorithm, clustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(&clustersToBeDeleted));

    VertexList verticesToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pVertexManager->GetResetDeletionObjects(pAlgorithm, verticesToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(&verticesToBeDeleted));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetAlgorithmInfo(pAlgorithm, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetAlgorithmInfo(pAlgorithm, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->ResetAlgorithmInfo(pAlgorithm, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pPfoManager->ResetAlgorithmInfo(pAlgorithm, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetAlgorithmInfo(pAlgorithm, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pVertexManager->ResetAlgorithmInfo(pAlgorithm, true));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
