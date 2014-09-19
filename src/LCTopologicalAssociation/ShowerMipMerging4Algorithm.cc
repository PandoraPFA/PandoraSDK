/**
 *  @file   LCContent/src/LCTopologicalAssociation/ShowerMipMerging4Algorithm.cc
 * 
 *  @brief  Implementation of the shower mip merging 4 algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCTopologicalAssociation/ShowerMipMerging4Algorithm.h"

using namespace pandora;

namespace lc_content
{

ShowerMipMerging4Algorithm::ShowerMipMerging4Algorithm() :
    m_canMergeMinMipFraction(0.7f),
    m_canMergeMaxRms(5.f),
    m_minCaloHitsPerDaughterCluster(6),
    m_minCaloHitsPerParentCluster(11),
    m_maxLayerDifference(4),
    m_maxProjectionDistance(50.f),
    m_maxProjectionDistanceRatio(0.9f),
    m_nProjectionExaminationLayers(4),
    m_maxCentroidDistance(500.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ShowerMipMerging4Algorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    // Loop over possible mip-stub daughter clusters
    for (ClusterVector::iterator iterI = clusterVector.begin(), iterIEnd = clusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = *iterI;

        // Check to see if cluster has already been changed
        if (NULL == pDaughterCluster)
            continue;

        if (pDaughterCluster->GetNCaloHits() < m_minCaloHitsPerDaughterCluster)
            continue;

        if (!ClusterHelper::CanMergeCluster(this->GetPandora(), pDaughterCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        Cluster *pBestParentCluster(NULL);
        float bestParentClusterEnergy(0.);
        float minProjectionDistance(m_maxProjectionDistance);

        const unsigned int daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

        // Find the closest plausible parent cluster, with the smallest cluster approach distance
        for (ClusterVector::const_iterator iterJ = clusterVector.begin(), iterJEnd = clusterVector.end(); iterJ != iterJEnd; ++iterJ)
        {
            Cluster *pParentCluster = *iterJ;

            // Check to see if cluster has already been changed
            if ((NULL == pParentCluster) || (pParentCluster == pDaughterCluster))
                continue;

            if (pParentCluster->GetNCaloHits() < m_minCaloHitsPerParentCluster)
                continue;

            // Cut on layer separation between the two clusters
            const unsigned int parentOuterLayer(pParentCluster->GetOuterPseudoLayer());
            const unsigned int minDaughterInnerLayer((parentOuterLayer > m_maxLayerDifference) ? parentOuterLayer - m_maxLayerDifference : 0);

            if (daughterInnerLayer < minDaughterInnerLayer)
                continue;

            // Get the smallest distance between a hit in the daughter cluster and the projected initial direction of the parent cluster
            const float projectionDistance(this->GetDistanceFromInitialProjection(pParentCluster, pDaughterCluster));

            const float parentClusterEnergy(pParentCluster->GetHadronicEnergy());

            if ((projectionDistance < minProjectionDistance) ||
                ((projectionDistance == minProjectionDistance) && (parentClusterEnergy > bestParentClusterEnergy)))
            {
                minProjectionDistance = projectionDistance;
                pBestParentCluster = pParentCluster;
                bestParentClusterEnergy = parentClusterEnergy;
            }
        }

        if (pBestParentCluster == NULL)
            continue;

        // Check closest approach within a layer between best parent cluster and the daughter cluster
        float centroidDistance(std::numeric_limits<float>::max());
        if (STATUS_CODE_SUCCESS != ClusterHelper::GetDistanceToClosestCentroid(pBestParentCluster, pDaughterCluster, centroidDistance))
            continue;

        if (centroidDistance < m_maxCentroidDistance)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
            *iterI = NULL;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ShowerMipMerging4Algorithm::GetDistanceFromInitialProjection(const Cluster *const pClusterToProject, const Cluster *const pClusterToExamine) const
{
    const CartesianVector innerCentroidI(pClusterToProject->GetCentroid(pClusterToProject->GetInnerPseudoLayer()));
    const CartesianVector &projectedDirection(pClusterToProject->GetInitialDirection());

    float minDistance(std::numeric_limits<float>::max());
    const unsigned int lastLayer(pClusterToExamine->GetInnerPseudoLayer() + m_nProjectionExaminationLayers);
    const OrderedCaloHitList &orderedCaloHitList(pClusterToExamine->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (iter->first > lastLayer)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CartesianVector separation((*hitIter)->GetPositionVector() - innerCentroidI);
            const CartesianVector directionCrossSeparation(projectedDirection.GetCrossProduct(separation));

            const float perpendicularDistance(directionCrossSeparation.GetMagnitude());
            const float parallelDistance(std::fabs(projectedDirection.GetDotProduct(separation)));

            if ((0 == parallelDistance) || ((perpendicularDistance / parallelDistance) > m_maxProjectionDistanceRatio))
                continue;

            if (perpendicularDistance < minDistance)
            {
                minDistance = perpendicularDistance;
            }
        }
    }

    return minDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ShowerMipMerging4Algorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerDaughterCluster", m_minCaloHitsPerDaughterCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerParentCluster", m_minCaloHitsPerParentCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayerDifference", m_maxLayerDifference));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxProjectionDistance", m_maxProjectionDistance));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxProjectionDistanceRatio", m_maxProjectionDistanceRatio));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NProjectionExaminationLayers", m_nProjectionExaminationLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDistance", m_maxCentroidDistance));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
