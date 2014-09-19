/**
 *  @file   LCContent/src/LCTopologicalAssociation/SoftClusterMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the soft cluster merging algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCTopologicalAssociation/SoftClusterMergingAlgorithm.h"

using namespace pandora;

namespace lc_content
{

SoftClusterMergingAlgorithm::SoftClusterMergingAlgorithm() :
    m_shouldUseCurrentClusterList(true),
    m_updateCurrentTrackClusterAssociations(true),
    m_maxHitsInSoftCluster(5),
    m_maxLayersSpannedBySoftCluster(3),
    m_maxHadEnergyForSoftClusterNoTrack(2.f),
    m_minClusterHadEnergy(0.25f),
    m_minClusterEMEnergy(0.025f),
    m_minCosOpeningAngle(0.f),
    m_minHitsInCluster(5),
    m_closestDistanceCut0(50.f),
    m_closestDistanceCut1(100.f),
    m_innerLayerCut1(20),
    m_closestDistanceCut2(250.f),
    m_innerLayerCut2(40),
    m_maxClusterDistanceFine(100.f),
    m_maxClusterDistanceCoarse(250.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SoftClusterMergingAlgorithm::Run()
{
    // Read specified lists of input clusters
    ClusterListToNameMap clusterListToNameMap;

    ClusterList clusterList;

    if (m_shouldUseCurrentClusterList)
    {
        std::string clusterListName;
        const ClusterList *pClusterList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList, clusterListName));

        clusterList.insert(pClusterList->begin(), pClusterList->end());
        clusterListToNameMap.insert(ClusterListToNameMap::value_type(pClusterList, clusterListName));

        if (m_updateCurrentTrackClusterAssociations)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));
        }
    }

    for (StringVector::const_iterator iter = m_additionalClusterListNames.begin(), iterEnd = m_additionalClusterListNames.end(); iter != iterEnd; ++iter)
    {
        const ClusterList *pClusterList = NULL;

        if (STATUS_CODE_SUCCESS == PandoraContentApi::GetList(*this, *iter, pClusterList))
        {
            clusterList.insert(pClusterList->begin(), pClusterList->end());
            clusterListToNameMap.insert(ClusterListToNameMap::value_type(pClusterList, *iter));
        }
        else
        {
            std::cout << "SoftClusterMergingAlgorithm: Failed to obtain cluster list " << *iter << std::endl;
        }
    }

    // Create a vector of input clusters, ordered by inner layer
    ClusterVector clusterVector(clusterList.begin(), clusterList.end());
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    // Loop over soft daughter candidate clusters
    for (ClusterVector::iterator iterI = clusterVector.begin(), iterIEnd = clusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = *iterI;

        if (NULL == pDaughterCluster)
            continue;

        if (!this->IsSoftCluster(pDaughterCluster))
            continue;

        Cluster *pBestParentCluster(NULL);
        float bestParentClusterEnergy(0.);
        float closestDistance(std::numeric_limits<float>::max());

        const CartesianVector &daughterInitialDirection(pDaughterCluster->GetInitialDirection());

        // Find best candidate parent cluster: that with closest distance between a pair of hits in the daughter and parent
        for (ClusterVector::iterator iterJ = clusterVector.begin(), iterJEnd = clusterVector.end(); iterJ != iterJEnd; ++iterJ)
        {
            Cluster *pParentCluster = *iterJ;

            if ((NULL == pParentCluster) || (pDaughterCluster == pParentCluster))
                continue;

            if (pParentCluster->GetNCaloHits() <= m_maxHitsInSoftCluster)
                continue;

            const float parentClusterEnergy(pParentCluster->GetHadronicEnergy());

            if (parentClusterEnergy < m_minClusterHadEnergy)
                continue;

            // Apply simple preselection using cosine of opening angle between the clusters
            const float cosOpeningAngle(pParentCluster->GetInitialDirection().GetCosOpeningAngle(daughterInitialDirection));

            if (cosOpeningAngle < m_minCosOpeningAngle)
                continue;

            const float distance(ClusterHelper::GetDistanceToClosestHit(pParentCluster, pDaughterCluster));

            // In event of equidistant parent candidates, choose highest energy cluster
            if ((distance < closestDistance) || ((distance == closestDistance) && (parentClusterEnergy > bestParentClusterEnergy)))
            {
                closestDistance = distance;
                pBestParentCluster = pParentCluster;
                bestParentClusterEnergy = parentClusterEnergy;
            }
        }

        if (NULL == pBestParentCluster)
            continue;

        if (this->CanMergeSoftCluster(pDaughterCluster, closestDistance))
        {
            if (clusterListToNameMap.size() > 1)
            {
                std::string parentListName, daughterListName;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterListName(pBestParentCluster, clusterListToNameMap, parentListName));
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterListName(pDaughterCluster, clusterListToNameMap, daughterListName));
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster,
                    parentListName, daughterListName));
            }
            else
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
            }

            *iterI = NULL;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool SoftClusterMergingAlgorithm::IsSoftCluster(Cluster *const pDaughterCluster) const
{
    // Note the cuts applied here are order-dependent - use the order defined in original version of pandora
    const unsigned int nCaloHits(pDaughterCluster->GetNCaloHits());

    if (0 == nCaloHits)
        return false;

    // ATTN: Added this cut to prevent merging multiple track-seeded clusters
    if (!pDaughterCluster->GetAssociatedTrackList().empty())
        return false;

    bool isSoftCluster(false);

    if (nCaloHits <= m_maxHitsInSoftCluster)
    {
        isSoftCluster = true;
    }
    else if ((pDaughterCluster->GetOuterPseudoLayer() - pDaughterCluster->GetInnerPseudoLayer()) < m_maxLayersSpannedBySoftCluster)
    {
        isSoftCluster = true;
    }
    else if (pDaughterCluster->GetHadronicEnergy() < m_maxHadEnergyForSoftClusterNoTrack)
    {
        isSoftCluster = true;
    }

    if (pDaughterCluster->GetHadronicEnergy() < m_minClusterHadEnergy)
        isSoftCluster = true;

    if (pDaughterCluster->IsPhotonFast(this->GetPandora()) && (pDaughterCluster->GetElectromagneticEnergy() > m_minClusterEMEnergy))
        isSoftCluster = false;

    return isSoftCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool SoftClusterMergingAlgorithm::CanMergeSoftCluster(const Cluster *const pDaughterCluster, const float closestDistance) const
{
    if (closestDistance < m_closestDistanceCut0)
        return true;

    const unsigned int daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

    if ((closestDistance < m_closestDistanceCut1) && (daughterInnerLayer > m_innerLayerCut1))
        return true;

    if ((closestDistance < m_closestDistanceCut2) && (daughterInnerLayer > m_innerLayerCut2))
        return true;

    const float distanceCut((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pDaughterCluster->GetOuterLayerHitType()) <= FINE) ?
        m_maxClusterDistanceFine : m_maxClusterDistanceCoarse);

    if (closestDistance > distanceCut)
        return false;

    return ((pDaughterCluster->GetHadronicEnergy() < m_minClusterHadEnergy) || (pDaughterCluster->GetNCaloHits() < m_minHitsInCluster));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SoftClusterMergingAlgorithm::GetClusterListName(Cluster *const pCluster, const ClusterListToNameMap &clusterListToNameMap,
    std::string &listName) const
{
    for (ClusterListToNameMap::const_iterator iter = clusterListToNameMap.begin(), iterEnd = clusterListToNameMap.end(); iter != iterEnd; ++iter)
    {
        const ClusterList *const pClusterList = iter->first;

        if (pClusterList->end() != pClusterList->find(pCluster))
        {
            listName = iter->second;
            return STATUS_CODE_SUCCESS;
        }
    }

    return STATUS_CODE_NOT_FOUND;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SoftClusterMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseCurrentClusterList", m_shouldUseCurrentClusterList));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "UpdateCurrentTrackClusterAssociations", m_updateCurrentTrackClusterAssociations));

    if (m_shouldUseCurrentClusterList && m_updateCurrentTrackClusterAssociations)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle, 
            m_trackClusterAssociationAlgName));
    }

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "AdditionalClusterListNames", m_additionalClusterListNames));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxHitsInSoftCluster", m_maxHitsInSoftCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayersSpannedBySoftCluster", m_maxLayersSpannedBySoftCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxHadEnergyForSoftClusterNoTrack", m_maxHadEnergyForSoftClusterNoTrack));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterHadEnergy", m_minClusterHadEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterEMEnergy", m_minClusterEMEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCosOpeningAngle", m_minCosOpeningAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceCut0", m_closestDistanceCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceCut1", m_closestDistanceCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InnerLayerCut1", m_innerLayerCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceCut2", m_closestDistanceCut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InnerLayerCut2", m_innerLayerCut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxClusterDistanceFine", m_maxClusterDistanceFine));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxClusterDistanceCoarse", m_maxClusterDistanceCoarse));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
