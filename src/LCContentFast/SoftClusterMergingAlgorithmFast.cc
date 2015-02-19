/**
 *  @file   LCContent/src/LCContentFast/SoftClusterMergingAlgorithmFast.cc
 * 
 *  @brief  Implementation of the soft cluster merging algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCContentFast/KDTreeLinkerAlgoT.h"
#include "LCContentFast/SoftClusterMergingAlgorithmFast.h"
#include "LCContentFast/QuickUnion.h"

using namespace pandora;

namespace lc_content_fast
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
    m_maxClusterDistanceCoarse(250.f),
    m_hitsToHitsCacheMap(new HitsToHitsCacheMap),
    m_hitNodes3D(new std::vector<HitKDNode3D>),
    m_hitsKdTree3D(new HitKDTree3D)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

SoftClusterMergingAlgorithm::~SoftClusterMergingAlgorithm()
{
    delete m_hitsToHitsCacheMap;
    delete m_hitNodes3D;
    delete m_hitsKdTree3D;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SoftClusterMergingAlgorithm::Run()
{
    ClusterList clusterList;
    ClusterListToNameMap clusterListToNameMap;
    this->GetInputClusters(clusterList, clusterListToNameMap);

    ClusterVector clusterVector(clusterList.begin(), clusterList.end());
    std::sort(clusterVector.begin(), clusterVector.end(), lc_content::SortingHelper::SortClustersByInnerLayer);
    QuickUnion quickUnion(clusterVector.size());

    CaloHitList fullCaloHitList;
    HitToClusterMap hitToClusterMap;
    this->GetInputCaloHits(clusterVector, fullCaloHitList, hitToClusterMap);
    this->InitializeKDTree(&fullCaloHitList);

    int index(-1);

    for (const Cluster *const pDaughterCluster : clusterVector)
    {
        ++index;

        if (!this->IsSoftCluster(pDaughterCluster))
            continue;

        CaloHitList theseHits;
        pDaughterCluster->GetOrderedCaloHitList().GetCaloHitList(theseHits);

        float closestDistance(std::numeric_limits<float>::max());
        const int parentIndex(this->FindBestParentCluster(clusterVector, hitToClusterMap, quickUnion, pDaughterCluster, theseHits, closestDistance));

        if ((parentIndex >= 0) && this->CanMergeSoftCluster(pDaughterCluster, closestDistance))
        {
            this->MergeClusters(clusterVector.at(parentIndex), pDaughterCluster, clusterListToNameMap);
            quickUnion.Unite(index, parentIndex);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void SoftClusterMergingAlgorithm::GetInputClusters(ClusterList &clusterList, ClusterListToNameMap &clusterListToNameMap) const
{
    if (m_shouldUseCurrentClusterList)
    {
        std::string clusterListName;
        const ClusterList *pClusterList = nullptr;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList, clusterListName));

        clusterList.insert(pClusterList->begin(), pClusterList->end());
        clusterListToNameMap.insert(ClusterListToNameMap::value_type(pClusterList, clusterListName));

        if (m_updateCurrentTrackClusterAssociations)
        {
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));
        }
    }

    for (const std::string &listName : m_additionalClusterListNames)
    {
        const ClusterList *pClusterList = nullptr;

        if (STATUS_CODE_SUCCESS == PandoraContentApi::GetList(*this, listName, pClusterList))
        {
            clusterList.insert(pClusterList->begin(), pClusterList->end());
            clusterListToNameMap.insert(ClusterListToNameMap::value_type(pClusterList, listName));
        }
        else
        {
            std::cout << "SoftClusterMergingAlgorithm: Failed to obtain cluster list " << listName << std::endl;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void SoftClusterMergingAlgorithm::GetInputCaloHits(const ClusterVector &clusterVector, CaloHitList &fullCaloHitList, HitToClusterMap &hitToClusterMap) const
{
    int index(-1);

    for (const Cluster *const pCluster : clusterVector)
    {
        ++index;

        CaloHitList caloHitList;
        pCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);
        fullCaloHitList.insert(caloHitList.begin(), caloHitList.end());

        for (const CaloHit *const pCaloHit : caloHitList)
            (void) hitToClusterMap.insert(HitToClusterMap::value_type(pCaloHit, index));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void SoftClusterMergingAlgorithm::InitializeKDTree(const CaloHitList *const pCaloHitList) 
{
    m_hitsToHitsCacheMap->clear();
    m_hitsKdTree3D->clear();
    m_hitNodes3D->clear();
    KDTreeCube hitsBoundingRegion3D = fill_and_bound_3d_kd_tree(this, *pCaloHitList, *m_hitNodes3D, true);
    m_hitsKdTree3D->build(*m_hitNodes3D, hitsBoundingRegion3D);
    m_hitNodes3D->clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool SoftClusterMergingAlgorithm::IsSoftCluster(const Cluster *const pDaughterCluster) const
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

int SoftClusterMergingAlgorithm::FindBestParentCluster(const ClusterVector &clusterVector, const HitToClusterMap &hitToClusterMap,
    QuickUnion &quickUnion, const Cluster *const pDaughterCluster, const CaloHitList &daughterHits, float &closestDistance) const
{
    int bestParentIndex(-1);
    closestDistance = std::numeric_limits<float>::max();

    const float searchDistance((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pDaughterCluster->GetOuterLayerHitType()) <= FINE) ?
        m_maxClusterDistanceFine : m_maxClusterDistanceCoarse);

    float bestParentClusterEnergy(0.);
    float minDistanceSquared(std::numeric_limits<float>::max());

    for (const CaloHit *const pCaloHitI : daughterHits)
    {
        const CartesianVector &positionVectorI(pCaloHitI->GetPositionVector());
        CaloHitList nearby_hits;

        if (m_hitsToHitsCacheMap->count(pCaloHitI))
        {
            const auto range = m_hitsToHitsCacheMap->equal_range(pCaloHitI);

            for (auto rangeIter = range.first; rangeIter != range.second; ++rangeIter)
                nearby_hits.insert(rangeIter->second);
        }
        else
        {
            KDTreeCube searchRegionHits = build_3d_kd_search_region(pCaloHitI, searchDistance, searchDistance, searchDistance);
            std::vector<HitKDNode3D> found;
            m_hitsKdTree3D->search(searchRegionHits, found);

            for (const auto &hit : found)
            {
                m_hitsToHitsCacheMap->emplace(pCaloHitI, hit.data);
                nearby_hits.insert(hit.data);
            }
        }

        for (const CaloHit *const pCaloHitJ : nearby_hits)
        {
            if (daughterHits.count(pCaloHitJ))
                continue;

            const int parentIndex(static_cast<int>(quickUnion.Find(hitToClusterMap.at(pCaloHitJ))));
            const Cluster *const pClusterJ = clusterVector.at(parentIndex);
            const float clusterEnergyJ(pClusterJ->GetHadronicEnergy());

            if (clusterEnergyJ < m_minClusterHadEnergy)
                continue;

            if (pClusterJ->GetNCaloHits() <= m_maxHitsInSoftCluster)
                continue;

            const float distanceSquared((positionVectorI - pCaloHitJ->GetPositionVector()).GetMagnitudeSquared());

            if ((distanceSquared < minDistanceSquared) || ((distanceSquared == minDistanceSquared) && (clusterEnergyJ > bestParentClusterEnergy)))
            {
                minDistanceSquared = distanceSquared;
                bestParentClusterEnergy = clusterEnergyJ;
                bestParentIndex = parentIndex;
            }
        }
    }

    if (bestParentIndex >= 0)
        closestDistance = std::sqrt(minDistanceSquared);

    return bestParentIndex;
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

void SoftClusterMergingAlgorithm::MergeClusters(const Cluster *const pParentCluster, const Cluster *const pDaughterCluster,
    const ClusterListToNameMap &clusterListToNameMap) const
{
    if (clusterListToNameMap.size() > 1)
    {
        std::string parentListName, daughterListName;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterListName(pParentCluster, clusterListToNameMap, parentListName));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterListName(pDaughterCluster, clusterListToNameMap, daughterListName));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pParentCluster, pDaughterCluster,
            parentListName, daughterListName));
    }
    else
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pParentCluster, pDaughterCluster));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SoftClusterMergingAlgorithm::GetClusterListName(const Cluster *const pCluster, const ClusterListToNameMap &clusterListToNameMap,
    std::string &listName) const
{
    for (auto mapIter : clusterListToNameMap)
    {
        if (mapIter.first->end() != mapIter.first->find(pCluster))
        {
            listName = mapIter.second;
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

} // namespace lc_content_fast
