/**
 *  @file   LCContent/src/LCTopologicalAssociation/BackscatteredTracks2Algorithm.cc
 * 
 *  @brief  Implementation of the backscattered tracks 2 algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCTopologicalAssociation/BackscatteredTracks2Algorithm.h"

using namespace pandora;

namespace lc_content
{

BackscatteredTracks2Algorithm::BackscatteredTracks2Algorithm() :
    m_canMergeMinMipFraction(0.7f),
    m_canMergeMaxRms(5.f),
    m_minCaloHitsPerCluster(6),
    m_maxFitRms(15.f),
    m_nFitProjectionLayers(2),
    m_maxFitDistanceToClosestHit(30.f),
    m_maxCentroidDistance(1000.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BackscatteredTracks2Algorithm::Run()
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
    for (ClusterVector::const_iterator iterI = clusterVector.begin(), iterIEnd = clusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        const Cluster *const pParentCluster = *iterI;

        // Check to see if cluster has already been changed
        if (NULL == pParentCluster)
            continue;

        if (pParentCluster->GetNCaloHits() < m_minCaloHitsPerCluster)
            continue;

        // Fit the parent cluster candidate from its innermost layer to its showerstart layer
        const unsigned int parentInnerLayer(pParentCluster->GetInnerPseudoLayer());
        const unsigned int parentShowerStartLayer(pParentCluster->GetShowerStartLayer(this->GetPandora()));

        ClusterFitResult parentClusterFitResult;
        if (STATUS_CODE_SUCCESS != ClusterFitHelper::FitLayers(pParentCluster, parentInnerLayer, parentShowerStartLayer, parentClusterFitResult))
            continue;

        if (!parentClusterFitResult.IsFitSuccessful() || (parentClusterFitResult.GetRms() > m_maxFitRms))
            continue;

        float minFitDistanceToClosestHit(m_maxFitDistanceToClosestHit);
        ClusterVector::iterator bestDaughterClusterIter(clusterVector.end());
        float bestDaughterClusterEnergy(std::numeric_limits<float>::max());

        // Find a compatible daughter cluster
        for (ClusterVector::iterator iterJ = clusterVector.begin(), iterJEnd = clusterVector.end(); iterJ != iterJEnd; ++iterJ)
        {
            const Cluster *const pDaughterCluster = *iterJ;

            // Check to see if cluster has already been changed
            if ((NULL == pDaughterCluster) || (pParentCluster == pDaughterCluster))
                continue;

            // Backscattered particle is expected to be daughter of a parent mip section; cut on overlap between relevant layers
            const unsigned int daughterOuterLayer(pDaughterCluster->GetOuterPseudoLayer());

            if ((parentShowerStartLayer <= daughterOuterLayer) || (parentInnerLayer >= daughterOuterLayer))
                continue;

            // Cut on the closest approach within a layer between parent cluster and the daughter cluster candidate
            float centroidDistance(std::numeric_limits<float>::max());
            if (STATUS_CODE_SUCCESS != ClusterHelper::GetDistanceToClosestCentroid(pParentCluster, pDaughterCluster, centroidDistance))
                continue;

            if (centroidDistance > m_maxCentroidDistance)
                continue;

            // Cut on the distance of closest approach between the fit to the parent cluster and the daughter cluster candidate
            const unsigned int daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());
            const unsigned int fitProjectionOuterLayer((daughterOuterLayer > m_nFitProjectionLayers) ? daughterOuterLayer - m_nFitProjectionLayers : 0);

            if (daughterInnerLayer > fitProjectionOuterLayer)
                continue;

            const float fitDistanceToClosestHit(ClusterHelper::GetDistanceToClosestHit(parentClusterFitResult, pDaughterCluster,
                daughterInnerLayer, fitProjectionOuterLayer));

            const float daughterClusterEnergy(pDaughterCluster->GetHadronicEnergy());

            if ((fitDistanceToClosestHit < minFitDistanceToClosestHit) ||
                ((fitDistanceToClosestHit == minFitDistanceToClosestHit) && (daughterClusterEnergy < bestDaughterClusterEnergy)))
            {
                bestDaughterClusterIter = iterJ;
                minFitDistanceToClosestHit = fitDistanceToClosestHit;
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

StatusCode BackscatteredTracks2Algorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerCluster", m_minCaloHitsPerCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitRms", m_maxFitRms));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NFitProjectionLayers", m_nFitProjectionLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitDistanceToClosestHit", m_maxFitDistanceToClosestHit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDistance", m_maxCentroidDistance));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
