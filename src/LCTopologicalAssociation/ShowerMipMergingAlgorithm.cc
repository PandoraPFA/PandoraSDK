/**
 *  @file   LCContent/src/LCTopologicalAssociation/ShowerMipMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the shower mip merging algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCTopologicalAssociation/ShowerMipMergingAlgorithm.h"

using namespace pandora;

namespace lc_content
{

StatusCode ShowerMipMergingAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    // Apply preselection and order clusters by inner layer
    ClusterVector clusterVector;
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (pCluster->GetNCaloHits() < m_minHitsInCluster)
            continue;

        if (!ClusterHelper::CanMergeCluster(this->GetPandora(), pCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        clusterVector.push_back(pCluster);
    }

    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    // Loop over all candidate parent clusters
    for (ClusterVector::const_iterator iterI = clusterVector.begin(); iterI != clusterVector.end(); ++iterI)
    {
        Cluster *pParentCluster = *iterI;

        // Check to see if cluster has already been changed
        if (NULL == pParentCluster)
            continue;

        if ((pParentCluster->GetMipFraction() - m_mipFractionCut) < std::numeric_limits<float>::epsilon())
            continue;

        if (pParentCluster->GetOrderedCaloHitList().size() < m_minOccupiedLayersInCluster)
            continue;

        if (!pParentCluster->GetFitToAllHitsResult().IsFitSuccessful() || (pParentCluster->GetFitToAllHitsResult().GetRms() > m_fitToAllHitsRmsCut))
            continue;

        // Calculate properties to compare with possible daughter clusters
        ClusterFitResult parentClusterFitResult;
        if (STATUS_CODE_SUCCESS != ClusterFitHelper::FitEnd(pParentCluster, m_nPointsToFit, parentClusterFitResult))
            continue;

        const unsigned int parentInnerLayer(pParentCluster->GetInnerPseudoLayer());
        const unsigned int parentOuterLayer(pParentCluster->GetOuterPseudoLayer());
        const CartesianVector parentOuterCentroid(pParentCluster->GetCentroid(parentOuterLayer));

        float minDistanceToCentroid(m_maxDistanceToClosestCentroid);
        ClusterVector::iterator bestDaughterClusterIter(clusterVector.end());
        float bestDaughterClusterEnergy(std::numeric_limits<float>::max());

        // Compare this mip candidate cluster with all other clusters
        for (ClusterVector::iterator iterJ = clusterVector.begin(); iterJ != clusterVector.end(); ++iterJ)
        {
            Cluster *pDaughterCluster = *iterJ;

            // Check to see if cluster has already been changed
            if ((NULL == pDaughterCluster) || (pParentCluster == pDaughterCluster))
                continue;

            // Check mip candidate cluster has origin closest to IP
            const unsigned int daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

            if (daughterInnerLayer < parentInnerLayer)
                continue;

            // Cut on physical separation of clusters
            const CartesianVector centroidDifference(parentOuterCentroid - pDaughterCluster->GetCentroid(daughterInnerLayer));

            if (centroidDifference.GetMagnitude() > m_maxCentroidDifference)
                continue;

            // Cut on distance between projected fit result and nearest cluster hit
            const float distanceToClosestHit(ClusterHelper::GetDistanceToClosestHit(parentClusterFitResult, pDaughterCluster, parentOuterLayer,
                parentOuterLayer + m_nFitProjectionLayers));

            if (distanceToClosestHit > m_maxDistanceToClosestHit)
                continue;

            // Also cut on distance between projected fit result and nearest cluster centroid
            const float distanceToClosestCentroid(ClusterHelper::GetDistanceToClosestCentroid(parentClusterFitResult, pDaughterCluster,
                parentOuterLayer, parentOuterLayer + m_nFitProjectionLayers));

            const float daughterClusterEnergy(pDaughterCluster->GetHadronicEnergy());

            if ((distanceToClosestCentroid < minDistanceToCentroid) ||
                ((distanceToClosestCentroid == minDistanceToCentroid) && (daughterClusterEnergy < bestDaughterClusterEnergy)))
            {
                bestDaughterClusterIter = iterJ;
                minDistanceToCentroid = distanceToClosestCentroid;
                bestDaughterClusterEnergy = daughterClusterEnergy;
            }
        }

        if (bestDaughterClusterIter != clusterVector.end())
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pParentCluster, *bestDaughterClusterIter));
            *bestDaughterClusterIter = NULL;
            --iterI;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ShowerMipMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_canMergeMinMipFraction = 0.7f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_nPointsToFit = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NPointsToFit", m_nPointsToFit));

    m_minHitsInCluster = 6;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    m_minOccupiedLayersInCluster = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinOccupiedLayersInCluster", m_minOccupiedLayersInCluster));

    m_mipFractionCut = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipFractionCut", m_mipFractionCut));

    m_fitToAllHitsRmsCut = 10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitToAllHitsRmsCut", m_fitToAllHitsRmsCut));

    m_maxCentroidDifference = 1000.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDifference", m_maxCentroidDifference));

    m_nFitProjectionLayers = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NFitProjectionLayers", m_nFitProjectionLayers));

    m_maxDistanceToClosestHit = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxDistanceToClosestHit", m_maxDistanceToClosestHit));

    m_maxDistanceToClosestCentroid = 25.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxDistanceToClosestCentroid", m_maxDistanceToClosestCentroid));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
