/**
 *  @file   LCContent/src/LCTopologicalAssociation/BackscatteredTracksAlgorithm.cc
 * 
 *  @brief  Implementation of the backscattered tracks algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCTopologicalAssociation/BackscatteredTracksAlgorithm.h"

using namespace pandora;

namespace lc_content
{

BackscatteredTracksAlgorithm::BackscatteredTracksAlgorithm() :
    m_canMergeMinMipFraction(0.7f),
    m_canMergeMaxRms(5.f),
    m_minCaloHitsPerCluster(6),
    m_fitToAllHitsRmsCut(10.f),
    m_nOuterFitExclusionLayers(2),
    m_nFitProjectionLayers(2),
    m_maxFitDistanceToClosestHit(30.f),
    m_maxCentroidDistance(100.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BackscatteredTracksAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    // Apply preselection and order clusters by inner layer
    ClusterVector clusterVector;
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        if (ClusterHelper::CanMergeCluster(this->GetPandora(), *iter, m_canMergeMinMipFraction, m_canMergeMaxRms))
            clusterVector.push_back(*iter);
    }

    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    // Loop over candidate daughter/parent cluster combinations
    for (ClusterVector::iterator iterI = clusterVector.begin(), iterIEnd = clusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = *iterI;

        // Check to see if cluster has already been changed
        if (NULL == pDaughterCluster)
            continue;

        if (pDaughterCluster->GetNCaloHits() < m_minCaloHitsPerCluster)
            continue;

        if (!pDaughterCluster->GetFitToAllHitsResult().IsFitSuccessful() || (pDaughterCluster->GetFitToAllHitsResult().GetRms() > m_fitToAllHitsRmsCut))
            continue;

        if (!ClusterHelper::CanMergeCluster(this->GetPandora(), pDaughterCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        // Fit all but the n outermost layers of the daughter cluster candidate
        const unsigned int daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());
        const unsigned int daughterOuterLayer(pDaughterCluster->GetOuterPseudoLayer());
        const unsigned int daughterOuterFitLayer((daughterOuterLayer > m_nOuterFitExclusionLayers) ? daughterOuterLayer - m_nOuterFitExclusionLayers : 0);

        ClusterFitResult daughterClusterFitResult;
        if (STATUS_CODE_SUCCESS != ClusterFitHelper::FitLayers(pDaughterCluster, daughterInnerLayer, daughterOuterFitLayer, daughterClusterFitResult))
            continue;

        Cluster *pBestParentCluster(NULL);
        float bestParentClusterEnergy(0.);
        float minFitDistanceToClosestHit(m_maxFitDistanceToClosestHit);

        // Find the most plausible parent cluster, with the smallest distance to the projection of the daughter cluster fit
        for (ClusterVector::const_iterator iterJ = clusterVector.begin(), iterJEnd = clusterVector.end(); iterJ != iterJEnd; ++iterJ)
        {
            Cluster *pParentCluster = *iterJ;

            // Check to see if cluster has already been changed
            if ((NULL == pParentCluster) || (pParentCluster == pDaughterCluster))
                continue;

            if (!ClusterHelper::CanMergeCluster(this->GetPandora(), pParentCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
                continue;

            if ((pParentCluster->GetOuterPseudoLayer() <= daughterOuterLayer) || (pParentCluster->GetInnerPseudoLayer() >= daughterOuterLayer))
                continue;

            // Cut on the closest approach within a layer between parent cluster candidate and the daughter cluster
            float centroidDistance(std::numeric_limits<float>::max());
            if (STATUS_CODE_SUCCESS != ClusterHelper::GetDistanceToClosestCentroid(pParentCluster, pDaughterCluster, centroidDistance))
                continue;

            if (centroidDistance > m_maxCentroidDistance)
                continue;

            // Calculate the smallest distance between the projected daughter cluster fit and the parent cluster candidate
            const unsigned int fitProjectionInnerLayer((daughterOuterLayer > m_nFitProjectionLayers) ? daughterOuterLayer - m_nFitProjectionLayers : 0);
            const unsigned int fitProjectionOuterLayer(daughterOuterLayer + m_nFitProjectionLayers);

            const float fitDistanceToClosestHit(ClusterHelper::GetDistanceToClosestHit(daughterClusterFitResult, pParentCluster, fitProjectionInnerLayer, fitProjectionOuterLayer));

            const float parentClusterEnergy(pParentCluster->GetHadronicEnergy());

            if ((fitDistanceToClosestHit < minFitDistanceToClosestHit) ||
                ((fitDistanceToClosestHit == minFitDistanceToClosestHit) && (parentClusterEnergy > bestParentClusterEnergy)))
            {
                minFitDistanceToClosestHit = fitDistanceToClosestHit;
                pBestParentCluster = pParentCluster;
                bestParentClusterEnergy = parentClusterEnergy;
            }
        }

        // If parent cluster found, merge the clusters
        if (pBestParentCluster != NULL)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
            *iterI = NULL;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BackscatteredTracksAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerCluster", m_minCaloHitsPerCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitToAllHitsRmsCut", m_fitToAllHitsRmsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NOuterFitExclusionLayers", m_nOuterFitExclusionLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NFitProjectionLayers", m_nFitProjectionLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitDistanceToClosestHit", m_maxFitDistanceToClosestHit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDistance", m_maxCentroidDistance));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
