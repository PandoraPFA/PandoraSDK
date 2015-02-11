/**
 *  @file   LCContent/src/LCMonitoring/ClusterComparisonAlgorithm.cc
 * 
 *  @brief  Implementation of the cluster comparison algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCMonitoring/ClusterComparisonAlgorithm.h"

using namespace pandora;

namespace lc_content
{

ClusterComparisonAlgorithm::ClusterComparisonAlgorithm() :
    m_restoreOriginalCaloHitList(false),
    m_restoreOriginalTrackList(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterComparisonAlgorithm::Run()
{
    std::string originalCaloHitListName, originalTrackListName;
    this->TweakCurrentLists(originalCaloHitListName, originalTrackListName);
    this->CreateAndSaveInitialClusters();
    this->RunReclustering();
    this->RestoreInputLists(originalCaloHitListName, originalTrackListName);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ClusterComparisonAlgorithm::TweakCurrentLists(std::string &originalCaloHitListName, std::string &originalTrackListName) const
{
    if (!m_inputCaloHitListName.empty())
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentListName<CaloHit>(*this, originalCaloHitListName));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<CaloHit>(*this, m_inputCaloHitListName));
    }

    if (!m_inputTrackListName.empty())
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentListName<Track>(*this, originalTrackListName));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Track>(*this, m_inputTrackListName));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ClusterComparisonAlgorithm::CreateAndSaveInitialClusters() const
{
    std::string tempClusterListName;
    const ClusterList *pTempClusterList(NULL);
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pTempClusterList, tempClusterListName));
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList<Cluster>(*this, m_clusterListName));
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Cluster>(*this, m_clusterListName));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ClusterComparisonAlgorithm::RunReclustering() const
{
    const TrackList *pTrackList(NULL);
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

    const ClusterList *pOriginalClusterList(NULL);
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pOriginalClusterList));

    const TrackList trackList(*pTrackList);
    const ClusterList originalClusterList(*pOriginalClusterList);
    std::string newListNameForOriginalClusters;
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeReclustering(*this, trackList, originalClusterList, newListNameForOriginalClusters));

    for (StringVector::const_iterator iter = m_comparisonAlgorithms.begin(), iterEnd = m_comparisonAlgorithms.end(); iter != iterEnd; ++iter)
    {
        std::string reclusterListName;
        const ClusterList *pReclusterList(NULL);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, *iter, pReclusterList, reclusterListName));
        this->CompareClusters(originalClusterList, *pReclusterList);
    }

    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndReclustering(*this, newListNameForOriginalClusters));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ClusterComparisonAlgorithm::CompareClusters(const ClusterList &clusterList1, const ClusterList &clusterList2) const
{
    ClusterToHitListMap clusterToHitListMap1, clusterToHitListMap2;
    HitToClusterMap hitToClusterMap1, hitToClusterMap2;
    this->PopulateMaps(clusterList1, clusterToHitListMap1, hitToClusterMap1);
    this->PopulateMaps(clusterList2, clusterToHitListMap2, hitToClusterMap2);

    ClusterList outputList1, outputList2;

    for (ClusterToHitListMap::const_iterator iter = clusterToHitListMap1.begin(), iterEnd = clusterToHitListMap1.end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster1 = iter->first;
        const CaloHitList &caloHitList1 = iter->second;

        // Collect all clusters in list 2 associated (via hits) with the current cluster in list 1
        ClusterList linkedClusterList2;
        for (CaloHitList::const_iterator hIter = caloHitList1.begin(), hIterEnd = caloHitList1.end(); hIter != hIterEnd; ++hIter)
        {
            HitToClusterMap::const_iterator htcIter2 = hitToClusterMap2.find(*hIter);
            (hitToClusterMap2.end() != htcIter2) ? linkedClusterList2.insert(htcIter2->second) : outputList1.insert(pCluster1);
        }

        // Check how many clusters in list 2 share hits with the single cluster in list 1; if just one, compare total numbers of hits
        bool isDifference(linkedClusterList2.size() != 1);

        if (!isDifference)
        {
            ClusterToHitListMap::const_iterator cthIter2 = clusterToHitListMap2.find(*(linkedClusterList2.begin()));

            if (clusterToHitListMap2.end() == cthIter2)
                throw StatusCodeException(STATUS_CODE_FAILURE);

            if (caloHitList1.size() != cthIter2->second.size())
                isDifference = true;
        }

        if (isDifference)
        {
            outputList1.insert(pCluster1);
            outputList2.insert(linkedClusterList2.begin(), linkedClusterList2.end());
        }
    }

    // Debug event display showing relevant clusters
    if (!outputList1.empty() || !outputList2.empty())
    {
        std::cout << "nRelevant clusters in list 1: " << outputList1.size() << ", nRelevant clusters in list 2: " << outputList2.size() << std::endl;
        PANDORA_MONITORING_API(SetEveDisplayParameters(this->GetPandora()));
        PANDORA_MONITORING_API(VisualizeClusters(this->GetPandora(), &outputList1, "Original Clusters", BLUE));
        PANDORA_MONITORING_API(VisualizeClusters(this->GetPandora(), &outputList2, "Alternative Clusters", RED));
        PANDORA_MONITORING_API(ViewEvent(this->GetPandora()));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ClusterComparisonAlgorithm::PopulateMaps(const pandora::ClusterList &clusterList, ClusterToHitListMap &clusterToHitListMap, HitToClusterMap &hitToClusterMap) const
{
    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster = *iter;

        CaloHitList caloHitList;
        pCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);
        caloHitList.insert(pCluster->GetIsolatedCaloHitList().begin(), pCluster->GetIsolatedCaloHitList().end());

        if (!clusterToHitListMap.insert(ClusterToHitListMap::value_type(pCluster, caloHitList)).second)
            throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);

        for (CaloHitList::const_iterator hIter = caloHitList.begin(), hIterEnd = caloHitList.end(); hIter != hIterEnd; ++hIter)
        {
            if (!hitToClusterMap.insert(HitToClusterMap::value_type(*hIter, pCluster)).second)
                throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ClusterComparisonAlgorithm::RestoreInputLists(const std::string &originalCaloHitListName, const std::string &originalTrackListName) const
{
    if (m_restoreOriginalCaloHitList && !m_inputCaloHitListName.empty())
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<CaloHit>(*this, originalCaloHitListName));

    if (m_restoreOriginalTrackList && !m_inputTrackListName.empty())
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<Track>(*this, originalTrackListName));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterComparisonAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InputCaloHitListName", m_inputCaloHitListName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InputTrackListName", m_inputTrackListName));

    m_restoreOriginalCaloHitList = !m_inputCaloHitListName.empty();
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "RestoreOriginalCaloHitList", m_restoreOriginalCaloHitList));

    m_restoreOriginalTrackList = !m_inputTrackListName.empty();
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "RestoreOriginalTrackList", m_restoreOriginalTrackList));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "OriginalClusters", m_clusteringAlgorithmName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle,
        "ComparisonAlgorithms", m_comparisonAlgorithms));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterListName", m_clusterListName));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
