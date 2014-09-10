/**
 *  @file   LCContent/src/LCTopologicalAssociation/BrokenTracksAlgorithm.cc
 * 
 *  @brief  Implementation of the broken tracks algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCTopologicalAssociation/BrokenTracksAlgorithm.h"

using namespace pandora;

namespace lc_content
{

StatusCode BrokenTracksAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    // Fit a straight line to start and end of all clusters in the current list
    ClusterFitRelationList clusterFitRelationList;

    for (ClusterVector::const_iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (!ClusterHelper::CanMergeCluster(this->GetPandora(), pCluster, m_canMergeMinMipFraction, m_maxFitRms))
            continue;

        if (pCluster->GetNCaloHits() < m_minHitsInCluster)
            continue;

        ClusterFitResult startFitResult, endFitResult;
        const unsigned int nOccupiedLayers(pCluster->GetOrderedCaloHitList().size());
 
        if (nOccupiedLayers >= m_minOccupiedLayersForStartFit)
            (void) ClusterFitHelper::FitStart(pCluster, m_nStartLayersToFit, startFitResult);

        if (nOccupiedLayers >= m_minOccupiedLayersForEndFit)
            (void) ClusterFitHelper::FitEnd(pCluster, m_nEndLayersToFit, endFitResult);

        if ((startFitResult.IsFitSuccessful() && (startFitResult.GetRms() < m_maxFitRms)) &&
            (endFitResult.IsFitSuccessful() && (endFitResult.GetRms() < m_maxFitRms)))
        {
            clusterFitRelationList.push_back(new ClusterFitRelation(pCluster, startFitResult, endFitResult));
        }
    }

    // Loop over cluster combinations, comparing fit results to determine whether clusters should be merged
    for (ClusterFitRelationList::const_iterator iterI = clusterFitRelationList.begin(), iterIEnd = clusterFitRelationList.end(); iterI != iterIEnd; ++iterI)
    {
        if ((*iterI)->IsDefunct())
            continue;

        Cluster *pParentCluster((*iterI)->GetCluster());
        const ClusterFitResult &parentClusterFitResult((*iterI)->GetEndFitResult());

        const unsigned int parentOuterLayer(pParentCluster->GetOuterPseudoLayer());
        const CartesianVector parentOuterCentroid(pParentCluster->GetCentroid(parentOuterLayer));

        ClusterFitRelation *pBestClusterFitRelation(NULL);
        float minDistanceProduct(std::numeric_limits<float>::max());

        // For each end fit, examine start fits for all other clusters
        for (ClusterFitRelationList::const_iterator iterJ = clusterFitRelationList.begin(), iterJEnd = clusterFitRelationList.end(); iterJ != iterJEnd; ++iterJ)
        {
            // Check to see if cluster has already been changed
            if ((*iterJ)->IsDefunct())
                continue;

            Cluster *pDaughterCluster((*iterJ)->GetCluster());
            const ClusterFitResult &daughterClusterFitResult((*iterJ)->GetStartFitResult());

            if (pParentCluster == pDaughterCluster)
                continue;

            const unsigned int daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

            // Basic cut on layer separation between the two clusters
            if (daughterInnerLayer <= parentOuterLayer)
                continue;

            // Check that cluster fit directions are compatible
            const float fitDirectionDotProduct(parentClusterFitResult.GetDirection().GetDotProduct(daughterClusterFitResult.GetDirection()));

            if (fitDirectionDotProduct < m_fitDirectionDotProductCut)
                continue;

            // Cut on distance of closest approach between start and end fits
            float fitResultsClosestApproach(std::numeric_limits<float>::max());

            if (STATUS_CODE_SUCCESS != ClusterHelper::GetFitResultsClosestApproach(parentClusterFitResult, daughterClusterFitResult, fitResultsClosestApproach))
                continue;

            const bool isDaughterFineGranularity(PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pDaughterCluster->GetInnerLayerHitType()) <= FINE);
            const float trackMergeCut(isDaughterFineGranularity ? m_trackMergeCutFine : m_trackMergeCutCoarse);

            if (fitResultsClosestApproach > trackMergeCut)
                continue;

            // Cut on perpendicular distance between fit directions and centroid difference vector.
            const CartesianVector daughterInnerCentroid(pDaughterCluster->GetCentroid(daughterInnerLayer));
            const CartesianVector centroidDifference(daughterInnerCentroid - parentOuterCentroid);
            const float trackMergePerpCut(isDaughterFineGranularity ? m_trackMergePerpCutFine : m_trackMergePerpCutCoarse);

            const CartesianVector parentCrossProduct(parentClusterFitResult.GetDirection().GetCrossProduct(centroidDifference));
            const float parentPerpendicularDistance(parentCrossProduct.GetMagnitude());

            const CartesianVector daughterCrossProduct(daughterClusterFitResult.GetDirection().GetCrossProduct(centroidDifference));
            const float daughterPerpendicularDistance(daughterCrossProduct.GetMagnitude());

            if ((parentPerpendicularDistance > trackMergePerpCut) && (daughterPerpendicularDistance > trackMergePerpCut))
                continue;

            // More detailed (and potentially time-consuming) examination of cluster separation
            const float centroidSeparation(centroidDifference.GetMagnitude());
            if ((daughterInnerLayer - parentOuterLayer > m_maxLayerDifference) || (centroidSeparation > m_maxCentroidDifference))
            {
                if (!m_shouldPerformGapCheck)
                    continue;

                if (parentClusterFitResult.GetChi2() > m_maxChi2ForGapCheck || daughterClusterFitResult.GetChi2() > m_maxChi2ForGapCheck)
                    continue;

                if (!ClusterHelper::DoesFitCrossGapRegion(this->GetPandora(), parentClusterFitResult, parentOuterCentroid, centroidSeparation) &&
                    !ClusterHelper::DoesFitCrossGapRegion(this->GetPandora(), daughterClusterFitResult, daughterInnerCentroid, -centroidSeparation))
                {
                    continue;
                }
            }

            const float distanceProduct(parentPerpendicularDistance * daughterPerpendicularDistance);

            if (distanceProduct < minDistanceProduct)
            {
                pBestClusterFitRelation = *iterJ;
                minDistanceProduct = distanceProduct;
            }
        }

        if (NULL != pBestClusterFitRelation)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pParentCluster, pBestClusterFitRelation->GetCluster()));
            pBestClusterFitRelation->SetAsDefunct();

            // Re-fit and re-use modified parent cluster
            ClusterFitResult endFitResult;
            (void) ClusterFitHelper::FitEnd(pParentCluster, m_nEndLayersToFit, endFitResult);

            if (endFitResult.IsFitSuccessful() && (endFitResult.GetRms() < m_maxFitRms))
            {
                (*iterI)->SetEndFitResult(endFitResult);
                --iterI;
            }
        }
    }

    // Tidy up
    for (ClusterFitRelationList::const_iterator iter = clusterFitRelationList.begin(), iterEnd = clusterFitRelationList.end(); iter != iterEnd; ++iter)
        delete (*iter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BrokenTracksAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_canMergeMinMipFraction = 0.7f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_minHitsInCluster = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    m_minOccupiedLayersForStartFit = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinOccupiedLayersForStartFit", m_minOccupiedLayersForStartFit));

    m_minOccupiedLayersForEndFit = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinOccupiedLayersForEndFit", m_minOccupiedLayersForEndFit));

    m_nStartLayersToFit = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NStartLayersToFit", m_nStartLayersToFit));

    m_nEndLayersToFit = 8;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NEndLayersToFit", m_nEndLayersToFit));

    m_maxFitRms = 35.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitRms", m_maxFitRms));

    m_fitDirectionDotProductCut = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitDirectionDotProductCut", m_fitDirectionDotProductCut));

    m_trackMergeCutFine = 45.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergeCutFine", m_trackMergeCutFine));

    m_trackMergeCutCoarse = 45.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergeCutCoarse", m_trackMergeCutCoarse));

    m_trackMergePerpCutFine = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergePerpCutFine", m_trackMergePerpCutFine));

    m_trackMergePerpCutCoarse = 75.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergePerpCutCoarse", m_trackMergePerpCutCoarse));

    m_maxLayerDifference = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayerDifference", m_maxLayerDifference));

    m_maxCentroidDifference = 2000.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDifference", m_maxCentroidDifference));

    m_maxChi2ForGapCheck = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxChi2ForGapCheck", m_maxChi2ForGapCheck));

    m_shouldPerformGapCheck = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldPerformGapCheck", m_shouldPerformGapCheck));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
