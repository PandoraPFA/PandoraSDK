/**
 *  @file   LCContent/src/LCTopologicalAssociation/ShowerMipMerging2Algorithm.cc
 * 
 *  @brief  Implementation of the shower mip merging 2 algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCTopologicalAssociation/ShowerMipMerging2Algorithm.h"

using namespace pandora;

namespace lc_content
{

ShowerMipMerging2Algorithm::ShowerMipMerging2Algorithm() :
    m_canMergeMinMipFraction(0.7f),
    m_canMergeMaxRms(5.f),
    m_minHitsInCluster(4),
    m_minOccupiedLayersInCluster(2),
    m_fitToAllHitsChi2Cut(5.f),
    m_nPointsToFit(8),
    m_maxLayerDifference(6),
    m_maxCentroidDifference(2000.f),
    m_maxFitDirectionDotProduct(-0.8f),
    m_perpendicularDistanceCutFine(50.f),
    m_perpendicularDistanceCutCoarse(75.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ShowerMipMerging2Algorithm::Run()
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

    // Loop over all candidate parent clusters
    for (ClusterVector::const_iterator iterI = clusterVector.begin(); iterI != clusterVector.end(); ++iterI)
    {
        const Cluster *const pParentCluster = *iterI;

        // Check to see if cluster has already been changed
        if (NULL == pParentCluster)
            continue;

        if ((pParentCluster->GetNCaloHits() < m_minHitsInCluster) || (pParentCluster->GetOrderedCaloHitList().size() < m_minOccupiedLayersInCluster))
            continue;

        ClusterFitResult parentClusterFitResult;
        if (STATUS_CODE_SUCCESS != ClusterFitHelper::FitEnd(pParentCluster, m_nPointsToFit, parentClusterFitResult))
            continue;

        if (!parentClusterFitResult.IsFitSuccessful() || (parentClusterFitResult.GetChi2() > m_fitToAllHitsChi2Cut))
            continue;

        const unsigned int parentOuterLayer(pParentCluster->GetOuterPseudoLayer());
        const CartesianVector parentOuterCentroid(pParentCluster->GetCentroid(parentOuterLayer));

        float minPerpendicularDistance(std::numeric_limits<float>::max());
        ClusterVector::iterator bestDaughterClusterIter(clusterVector.end());
        float bestDaughterClusterEnergy(std::numeric_limits<float>::max());

        // Compare this successfully fitted cluster with all others
        for (ClusterVector::iterator iterJ = clusterVector.begin(); iterJ != clusterVector.end(); ++iterJ)
        {
            const Cluster *const pDaughterCluster = *iterJ;

            // Check to see if cluster has already been changed
            if ((NULL == pDaughterCluster) || (pParentCluster == pDaughterCluster))
                continue;

            // Cut on layer separation between the two clusters
            const unsigned int daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

            if ((daughterInnerLayer <= parentOuterLayer) || ((daughterInnerLayer - parentOuterLayer) > m_maxLayerDifference))
                continue;

            // Also cut on physical separation between the two clusters
            const CartesianVector daughterInnerCentroid(pDaughterCluster->GetCentroid(daughterInnerLayer));
            const CartesianVector centroidDifference(parentOuterCentroid - daughterInnerCentroid);

            if (centroidDifference.GetMagnitude() > m_maxCentroidDifference)
                continue;

            // Require clusters to point at one another
            if (centroidDifference.GetUnitVector().GetDotProduct(parentClusterFitResult.GetDirection()) > m_maxFitDirectionDotProduct)
                continue;

            // Cut on perpendicular distance between fit direction and centroid difference vector.
            const CartesianVector parentCrossProduct(parentClusterFitResult.GetDirection().GetCrossProduct(centroidDifference));
            const float perpendicularDistance(parentCrossProduct.GetMagnitude());

            const float perpendicularDistanceCut((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pDaughterCluster->GetInnerLayerHitType()) <= FINE) ?
                m_perpendicularDistanceCutFine : m_perpendicularDistanceCutCoarse);

            if (perpendicularDistance > perpendicularDistanceCut)
                continue;

            const float daughterClusterEnergy(pDaughterCluster->GetHadronicEnergy());

            if ((perpendicularDistance < minPerpendicularDistance) ||
                ((perpendicularDistance == minPerpendicularDistance) && (daughterClusterEnergy < bestDaughterClusterEnergy)))
            {
                bestDaughterClusterIter = iterJ;
                minPerpendicularDistance = perpendicularDistance;
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

StatusCode ShowerMipMerging2Algorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinOccupiedLayersInCluster", m_minOccupiedLayersInCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitToAllHitsChi2Cut", m_fitToAllHitsChi2Cut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NPointsToFit", m_nPointsToFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayerDifference", m_maxLayerDifference));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDifference", m_maxCentroidDifference));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitDirectionDotProduct", m_maxFitDirectionDotProduct));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PerpendicularDistanceCutFine", m_perpendicularDistanceCutFine));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PerpendicularDistanceCutCoarse", m_perpendicularDistanceCutCoarse));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
