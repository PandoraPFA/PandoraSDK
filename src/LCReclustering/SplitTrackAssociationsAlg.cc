/**
 *  @file   LCContent/src/LCReclustering/SplitTrackAssociationsAlg.cc
 * 
 *  @brief  Implementation of the split track associations algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ReclusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCReclustering/SplitTrackAssociationsAlg.h"

using namespace pandora;

namespace lc_content
{

SplitTrackAssociationsAlg::SplitTrackAssociationsAlg() :
    m_minTrackAssociations(2),
    m_maxTrackAssociations(std::numeric_limits<unsigned int>::max()),
    m_chiToAttemptReclustering(-3.f),
    m_minChi2Improvement(1.f),
    m_minClusterEnergyForTrackAssociation(0.1f),
    m_chi2ForAutomaticClusterSelection(1.f),
    m_usingOrderedAlgorithms(false),
    m_bestChi2ForReclusterHalt(4.f),
    m_currentChi2ForReclusterHalt(16.f),
    m_shouldUseForcedClustering(false),
    m_minChiForForcedClustering(4.f),
    m_minForcedChi2Improvement(9.f),
    m_maxForcedChi2(36.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SplitTrackAssociationsAlg::Run()
{
    // Begin by recalculating track-cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    // Store copy of input cluster list in a vector
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    // Examine each cluster in the input list
    for (ClusterVector::iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster = *iter;

        // Check compatibility of cluster with its associated tracks
        const TrackList &trackList(pCluster->GetAssociatedTrackList());
        const unsigned int nTrackAssociations(trackList.size());

        if ((nTrackAssociations < m_minTrackAssociations) || (nTrackAssociations > m_maxTrackAssociations))
            continue;

        const float chi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), pCluster, trackList));

        if (chi < m_chiToAttemptReclustering)
            continue;

        // Specify clusters and tracks to be used in reclustering
        ClusterList reclusterClusterList;
        reclusterClusterList.insert(pCluster);

        TrackList reclusterTrackList(trackList.begin(), trackList.end());

        // Initialize reclustering with these local lists
        std::string originalClustersListName;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeReclustering(*this, reclusterTrackList, 
            reclusterClusterList, originalClustersListName));

        // Run multiple clustering algorithms and identify the best cluster candidates produced
        float bestReclusterChi(std::numeric_limits<float>::max());
        float bestReclusterChi2(std::numeric_limits<float>::max());
        std::string bestReclusterListName(originalClustersListName);

        for (StringVector::const_iterator clusteringIter = m_clusteringAlgorithms.begin(), clusteringIterEnd = m_clusteringAlgorithms.end();
            clusteringIter != clusteringIterEnd; ++clusteringIter)
        {
            // Produce new cluster candidates
            std::string reclusterListName;
            const ClusterList *pReclusterList = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, *clusteringIter, 
                pReclusterList, reclusterListName));

            if (pReclusterList->empty())
                continue;

            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_associationAlgorithmName));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

            // Calculate figure of merit for recluster candidates. Label as best recluster candidates if applicable
            ReclusterResult reclusterResult;

            if (STATUS_CODE_SUCCESS != ReclusterHelper::ExtractReclusterResults(this->GetPandora(), pReclusterList, reclusterResult))
                continue;

            if (reclusterResult.GetMinTrackAssociationEnergy() < m_minClusterEnergyForTrackAssociation)
                continue;

            if (reclusterResult.GetNExcessTrackAssociations() != 0)
                continue;

            // Are recluster candidates good enough to justify replacing original clusters?
            const float reclusterChi2(reclusterResult.GetChi2PerDof());
            const float minChi2(m_chiToAttemptReclustering * m_chiToAttemptReclustering);

            if ((bestReclusterChi2 - reclusterChi2 > m_minChi2Improvement) && (reclusterChi2 < minChi2))
            {
                bestReclusterChi = reclusterResult.GetChiPerDof();
                bestReclusterChi2 = reclusterChi2;
                bestReclusterListName = reclusterListName;
            }

            // If chi2 is very good, stop the reclustering attempts
            if(bestReclusterChi2 < m_chi2ForAutomaticClusterSelection)
                break;

            // If using ordered algorithms, chi2 is good enough and things are getting worse, stop
            if (m_usingOrderedAlgorithms && (bestReclusterChi2 < m_bestChi2ForReclusterHalt) && (reclusterChi2 > m_currentChi2ForReclusterHalt))
                break;
        }

        // If cannot produce satisfactory split of cluster using main clustering algorithms, use forced clustering algorithm
        if (m_shouldUseForcedClustering)
        {
            if ((bestReclusterListName == originalClustersListName) || (bestReclusterChi > m_minChiForForcedClustering))
            {
                std::string forcedListName;
                const ClusterList *pForcedClusterList = NULL;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_forcedClusteringAlgorithmName,
                    pForcedClusterList, forcedListName));

                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

                ReclusterResult forcedClusterResult;

                if (STATUS_CODE_SUCCESS == ReclusterHelper::ExtractReclusterResults(this->GetPandora(), pForcedClusterList, forcedClusterResult))
                {
                    const float forcedChi2(forcedClusterResult.GetChi2PerDof());
                    const float originalChi2(chi * chi);

                    if ((originalChi2 - forcedChi2 > m_minForcedChi2Improvement) && (forcedChi2 < m_maxForcedChi2))
                        bestReclusterListName = forcedListName;
                }
            }
        }

        // Recreate track-cluster associations for chosen recluster candidates
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::TemporarilyReplaceCurrentList<Cluster>(*this, bestReclusterListName));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

        // Choose the best recluster candidates, which may still be the originals
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndReclustering(*this, bestReclusterListName));

        // Original cluster may have been deleted: for safety, remove its address from ClusterVector
        (*iter) = NULL;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SplitTrackAssociationsAlg::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "clusteringAlgorithms",
        m_clusteringAlgorithms));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "ClusterAssociation",
        m_associationAlgorithmName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "TrackClusterAssociation",
        m_trackClusterAssociationAlgName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinTrackAssociations", m_minTrackAssociations));

    if (m_minTrackAssociations < 1)
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackAssociations", m_maxTrackAssociations));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ChiToAttemptReclustering", m_chiToAttemptReclustering));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinChi2Improvement", m_minChi2Improvement));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterEnergyForTrackAssociation", m_minClusterEnergyForTrackAssociation));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Chi2ForAutomaticClusterSelection", m_chi2ForAutomaticClusterSelection));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "UsingOrderedAlgorithms", m_usingOrderedAlgorithms));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "BestChi2ForReclusterHalt", m_bestChi2ForReclusterHalt));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CurrentChi2ForReclusterHalt", m_currentChi2ForReclusterHalt));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseForcedClustering", m_shouldUseForcedClustering));

    if (m_shouldUseForcedClustering)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "ForcedClustering",
            m_forcedClusteringAlgorithmName));
    }

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinChiForForcedClustering", m_minChiForForcedClustering));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinForcedChi2Improvement", m_minForcedChi2Improvement));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxForcedChi2", m_maxForcedChi2));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
