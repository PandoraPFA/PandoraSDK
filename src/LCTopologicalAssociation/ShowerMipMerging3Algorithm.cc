/**
 *  @file   LCContent/src/LCTopologicalAssociation/ShowerMipMerging3Algorithm.cc
 * 
 *  @brief  Implementation of the shower mip merging 3 algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCTopologicalAssociation/ShowerMipMerging3Algorithm.h"

using namespace pandora;

namespace lc_content
{

ShowerMipMerging3Algorithm::ShowerMipMerging3Algorithm() :
    m_canMergeMinMipFraction(0.7f),
    m_canMergeMaxRms(5.f),
    m_minCaloHitsInDaughter(6),
    m_minOccupiedLayersInDaughter(4),
    m_minCaloHitsInParent(3),
    m_nPointsToFit(10),
    m_maxFitChi2(10.f),
    m_nFitProjectionLayers(4),
    m_maxFitDistanceToClosestHit(30.f),
    m_maxClusterApproach(250.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ShowerMipMerging3Algorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    // Loop over possible mip-stub daughter clusters
    for (ClusterVector::iterator iterI = clusterVector.begin(), iterIEnd = clusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        const Cluster *const pDaughterCluster = *iterI;

        // Check to see if cluster has already been changed
        if (NULL == pDaughterCluster)
            continue;

        if (pDaughterCluster->GetNCaloHits() < m_minCaloHitsInDaughter)
            continue;

        if (pDaughterCluster->GetOrderedCaloHitList().size() < m_minOccupiedLayersInDaughter)
            continue;

        if (!ClusterHelper::CanMergeCluster(this->GetPandora(), pDaughterCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        ClusterFitResult daughterClusterFitResult;
        if (STATUS_CODE_SUCCESS != ClusterFitHelper::FitStart(pDaughterCluster, m_nPointsToFit, daughterClusterFitResult))
            continue;

        // Mip stub cluster must be consistent with a straight line fit
        if (daughterClusterFitResult.GetChi2() > m_maxFitChi2)
            continue;

        const Cluster *pBestParentCluster(NULL);
        float bestParentClusterEnergy(0.);
        float minFitDistanceToClosestHit(m_maxFitDistanceToClosestHit);

        const unsigned int daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

        // Find the closest plausible parent cluster, with the smallest cluster approach distance
        for (ClusterVector::const_iterator iterJ = clusterVector.begin(), iterJEnd = clusterVector.end(); iterJ != iterJEnd; ++iterJ)
        {
            const Cluster *const pParentCluster = *iterJ;

            // Check to see if cluster has already been changed
            if ((NULL == pParentCluster) || (pParentCluster == pDaughterCluster))
                continue;

            if (pParentCluster->GetNCaloHits() < m_minCaloHitsInParent)
                continue;

            // We are looking for small mip stub clusters emerging from the parent shower-like cluster
            const unsigned int parentOuterLayer(pParentCluster->GetOuterPseudoLayer());

            if (daughterInnerLayer < parentOuterLayer)
                continue;

            // Cluster approach is the smallest distance between a hit in daughter cluster and a hit in parent cluster
            const float clusterApproach(ClusterHelper::GetDistanceToClosestHit(pDaughterCluster, pParentCluster));

            if (clusterApproach > m_maxClusterApproach)
                continue;

            // Cut on distance between projected fit result and nearest cluster hit
            const unsigned int fitProjectionInnerLayer((parentOuterLayer > m_nFitProjectionLayers) ? parentOuterLayer - m_nFitProjectionLayers : 0);
            const float fitDistanceToClosestHit(ClusterHelper::GetDistanceToClosestHit(daughterClusterFitResult, pParentCluster, fitProjectionInnerLayer, parentOuterLayer));

            const float parentClusterEnergy(pParentCluster->GetHadronicEnergy());

            if ((fitDistanceToClosestHit < minFitDistanceToClosestHit) ||
                ((fitDistanceToClosestHit == minFitDistanceToClosestHit) && (parentClusterEnergy > bestParentClusterEnergy)))
            {
                minFitDistanceToClosestHit = fitDistanceToClosestHit;
                pBestParentCluster = pParentCluster;
                bestParentClusterEnergy = parentClusterEnergy;
            }
        }

        // If parent cluster found, within threshold approach distance, merge the clusters
        if (pBestParentCluster != NULL)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
            *iterI = NULL;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ShowerMipMerging3Algorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsInDaughter", m_minCaloHitsInDaughter));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinOccupiedLayersInDaughter", m_minOccupiedLayersInDaughter));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsInParent", m_minCaloHitsInParent));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NPointsToFit", m_nPointsToFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitChi2", m_maxFitChi2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NFitProjectionLayers", m_nFitProjectionLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitDistanceToClosestHit", m_maxFitDistanceToClosestHit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxClusterApproach", m_maxClusterApproach));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
