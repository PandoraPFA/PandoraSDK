/**
 *  @file   LCContent/src/LCTopologicalAssociation/MipPhotonSeparationAlgorithm.cc
 * 
 *  @brief  Implementation of the mip-photon separation algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"
#include "LCHelpers/ReclusterHelper.h"
#include "LCHelpers/SortingHelper.h"

#include "LCTopologicalAssociation/MipPhotonSeparationAlgorithm.h"

using namespace pandora;

namespace lc_content
{

MipPhotonSeparationAlgorithm::MipPhotonSeparationAlgorithm() :
    m_nLayersForMipRegion(2),
    m_nLayersForShowerRegion(2),
    m_maxLayersMissed(1),
    m_minMipRegion2Span(4),
    m_maxShowerStartLayer(20),
    m_minShowerRegionSpan(4),
    m_maxShowerStartLayer2(5),
    m_minShowerRegionSpan2(200),
    m_nonPhotonDeltaChi2Cut(0.f),
    m_photonDeltaChi2Cut(1.f),
    m_minHitsInPhotonCluster(6),
    m_genericDistanceCut(1.f),
    m_trackPathWidth(2.f),
    m_maxTrackSeparation2(1000.f * 1000.f),
    m_additionalPadWidthsFine(2.5f),
    m_additionalPadWidthsCoarse(2.5f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MipPhotonSeparationAlgorithm::Run()
{
    // Begin by recalculating track-cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    // Create ordered vector of current clusters
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pClusterList));

    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), SortingHelper::SortClustersByInnerLayer);

    // Examine fragmentation possibilities for each cluster
    for (ClusterVector::iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;
        *iter = NULL;

        const TrackList trackList(pCluster->GetAssociatedTrackList());

        if (trackList.empty() || (trackList.size() > 1))
            continue;

        // Decide whether to fragment cluster, simultaneously determining cluster shower start/end layers
        Track *pTrack = *(trackList.begin());
        unsigned int showerStartLayer(std::numeric_limits<unsigned int>::max()), showerEndLayer(std::numeric_limits<unsigned int>::max());

        if (!this->ShouldFragmentCluster(pCluster, pTrack, showerStartLayer, showerEndLayer))
            continue;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PerformFragmentation(pCluster, pTrack, showerStartLayer, showerEndLayer));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool MipPhotonSeparationAlgorithm::ShouldFragmentCluster(Cluster *const pCluster, Track *const pTrack, unsigned int &showerStartLayer,
    unsigned int &showerEndLayer) const
{
    const unsigned int firstPseudoLayer(PandoraContentApi::GetPlugins(*this)->GetPseudoLayerPlugin()->GetPseudoLayerAtIp());
    const unsigned int maxPseudoLayer(std::numeric_limits<unsigned int>::max());

    unsigned int mipRegion1StartLayer(maxPseudoLayer);
    unsigned int mipRegion2StartLayer(maxPseudoLayer), mipRegion2EndLayer(maxPseudoLayer);

    const unsigned int lastPseudoLayer(pCluster->GetOuterPseudoLayer());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    bool shouldContinue(true);
    unsigned int layersMissed(0), mipCount(0), showerCount(0);
    bool mipRegion1(true), mipRegion2(false), showerRegion(false);

    // Loop over pseudo layers, looking for hits consistent with track projection
    for (unsigned int iLayer = firstPseudoLayer; (iLayer <= lastPseudoLayer) && shouldContinue; ++iLayer)
    {
        // Find hits consistent with track projection
        bool trackHitFound(false), mipTrackHitFound(false), showerTrackHitFound(false);
        OrderedCaloHitList::const_iterator hitListIter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() != hitListIter)
        {
            for (CaloHitList::const_iterator iter = hitListIter->second->begin(), iterEnd = hitListIter->second->end(); iter != iterEnd; ++iter)
            {
                CaloHit *pCaloHit = *iter;
                float distance(std::numeric_limits<float>::max());

                if (STATUS_CODE_SUCCESS != this->GetDistanceToTrack(pCluster, pTrack, pCaloHit, distance))
                    continue;

                if (distance < m_genericDistanceCut)
                {
                    trackHitFound = true;
                    pCaloHit->IsPossibleMip() ? mipTrackHitFound = true : showerTrackHitFound = true;
                }
            }
        }

        // Use results to identify start and end layers for mip and shower regions
        if (trackHitFound)
        {
            layersMissed = 0;
        }
        else
        {
            ++layersMissed;
        }

        if (mipTrackHitFound)
        {
            if (mipRegion2)
                mipRegion2EndLayer = iLayer;
        }

        if (showerTrackHitFound && showerRegion)
        {
            showerEndLayer = iLayer;
        }

        if (mipTrackHitFound && !showerTrackHitFound)
        {
            if (mipRegion1 && (iLayer < mipRegion1StartLayer))
                mipRegion1StartLayer = iLayer;

            if (mipRegion1 || mipRegion2)
                showerCount = 0;

            if (showerRegion)
            {
                if (++mipCount == m_nLayersForMipRegion)
                {
                    mipRegion2 = true;
                    showerRegion = false;
                    showerCount = 0;
                }
                else
                {
                    mipRegion2StartLayer = iLayer;
                }
            }
        }

        if (!mipTrackHitFound && showerTrackHitFound)
        {
            if (showerRegion)
                mipCount = 0;

            if (mipRegion1 || mipRegion2)
            {
                if (++showerCount == m_nLayersForShowerRegion)
                {
                    if (mipRegion1)
                    {
                        showerRegion = true;
                        mipRegion1 = false;
                        showerCount = 0;
                    }

                    if (mipRegion2)
                        shouldContinue = false;
                }
                else if (mipRegion1)
                {
                    showerStartLayer = iLayer;
                }
            }
        }

        if (layersMissed > m_maxLayersMissed)
            shouldContinue = false;
    }

    // Use above findings to determine whether to fragment cluster
    if (mipRegion2EndLayer == maxPseudoLayer)
        return false;

    if ((showerEndLayer != maxPseudoLayer) && (showerStartLayer == maxPseudoLayer))
        return true;

    if (((mipRegion2EndLayer != maxPseudoLayer) && (mipRegion2EndLayer - mipRegion2StartLayer > m_minMipRegion2Span)) &&
        ((showerStartLayer < m_maxShowerStartLayer) && (((showerEndLayer != maxPseudoLayer) && (showerEndLayer - showerStartLayer > m_minShowerRegionSpan)))) )
    {
        return true;
    }

    if (((showerStartLayer < m_maxShowerStartLayer2) && (((showerEndLayer != maxPseudoLayer) && (showerEndLayer - showerStartLayer > m_minShowerRegionSpan2)))) )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MipPhotonSeparationAlgorithm::PerformFragmentation(Cluster *const pOriginalCluster, Track *const pTrack, unsigned int showerStartLayer,
    unsigned int showerEndLayer) const
{
    ClusterList clusterList;
    clusterList.insert(pOriginalCluster);
    std::string originalClustersListName, fragmentClustersListName;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, clusterList, originalClustersListName,
        fragmentClustersListName));

    // Make the cluster fragments
    Cluster *pMipCluster = NULL, *pPhotonCluster = NULL;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->MakeClusterFragments(showerStartLayer, showerEndLayer, pOriginalCluster,
        pMipCluster, pPhotonCluster));

    // Decide whether to keep original cluster or the fragments
    std::string clusterListToSaveName(originalClustersListName), clusterListToDeleteName(fragmentClustersListName);

    if ((NULL != pMipCluster) && (NULL != pPhotonCluster) && (pPhotonCluster->GetNCaloHits() >= m_minHitsInPhotonCluster))
    {
        const float trackEnergy(pTrack->GetEnergyAtDca());
        const float originalChi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), pOriginalCluster->GetTrackComparisonEnergy(this->GetPandora()), trackEnergy));
        const float newChi(ReclusterHelper::GetTrackClusterCompatibility(this->GetPandora(), pMipCluster->GetTrackComparisonEnergy(this->GetPandora()), trackEnergy));
        const float dChi2(newChi * newChi - originalChi * originalChi);

        const bool passChi2Cuts((dChi2 < m_nonPhotonDeltaChi2Cut) || (pPhotonCluster->IsPhotonFast(this->GetPandora()) && (dChi2 < m_photonDeltaChi2Cut)));

        if (passChi2Cuts)
        {
            clusterListToSaveName = fragmentClustersListName;
            clusterListToDeleteName = originalClustersListName;
        }
    }

    // End cluster fragmentation operations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, clusterListToSaveName,
        clusterListToDeleteName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MipPhotonSeparationAlgorithm::MakeClusterFragments(const unsigned int showerStartLayer, const unsigned int showerEndLayer,
    Cluster *const pOriginalCluster, Cluster *&pMipCluster, Cluster *&pPhotonCluster) const
{
    Track *pTrack = *(pOriginalCluster->GetAssociatedTrackList().begin());
    OrderedCaloHitList orderedCaloHitList(pOriginalCluster->GetOrderedCaloHitList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(pOriginalCluster->GetIsolatedCaloHitList()));

    for (OrderedCaloHitList::const_iterator iter =  orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const unsigned int iLayer = iter->first;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;
            float distance(0.f);

            PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetDistanceToTrack(pOriginalCluster, 
                pTrack, pCaloHit, distance));

            if ((distance < m_genericDistanceCut) || (iLayer < showerStartLayer) || (iLayer > showerEndLayer))
            {
                if (NULL == pMipCluster)
                {
                    PandoraContentApi::Cluster::Parameters parameters;
                    parameters.m_pTrack = pTrack;
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pMipCluster));
                }

                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pMipCluster, pCaloHit));
            }
            else
            {
                if (NULL == pPhotonCluster)
                {
                    PandoraContentApi::Cluster::Parameters parameters;
                    parameters.m_caloHitList.insert(pCaloHit);
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pPhotonCluster));
                }
                else
                {
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pPhotonCluster, pCaloHit));
                }
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MipPhotonSeparationAlgorithm::GetDistanceToTrack(Cluster *const pCluster, Track *const pTrack, CaloHit *const pCaloHit,
    float &distance) const
{
    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());
    const CartesianVector &trackSeedPosition(pTrack->GetTrackStateAtCalorimeter().GetPosition());

    const CartesianVector positionDifference(hitPosition - trackSeedPosition);
    const float separationSquared(positionDifference.GetMagnitudeSquared());

    if (separationSquared < m_maxTrackSeparation2)
    {
        const float flexibility(1.f + (m_trackPathWidth * std::sqrt(separationSquared / m_maxTrackSeparation2)));

        const float dCut ((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
            flexibility * (m_additionalPadWidthsFine * pCaloHit->GetCellLengthScale()) :
            flexibility * (m_additionalPadWidthsCoarse * pCaloHit->GetCellLengthScale()) );

        if (dCut < std::numeric_limits<float>::epsilon())
            return STATUS_CODE_FAILURE;

        const float dPerp((pCluster->GetInitialDirection().GetCrossProduct(positionDifference)).GetMagnitude());

        distance = dPerp / dCut;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MipPhotonSeparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle, m_trackClusterAssociationAlgName));

    // Parameters aiding decision whether to proceed with fragmentation
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersForMipRegion", m_nLayersForMipRegion));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersForShowerRegion", m_nLayersForShowerRegion));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayersMissed", m_maxLayersMissed));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinMipRegion2Span", m_minMipRegion2Span));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxShowerStartLayer", m_maxShowerStartLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinShowerRegionSpan", m_minShowerRegionSpan));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxShowerStartLayer2", m_maxShowerStartLayer2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinShowerRegionSpan2", m_minShowerRegionSpan2));

    // Parameters aiding selection of original clusters or new fragments
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NonPhotonDeltaChi2Cut", m_nonPhotonDeltaChi2Cut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonDeltaChi2Cut", m_photonDeltaChi2Cut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInPhotonCluster", m_minHitsInPhotonCluster));

    // Generic distance to track parameters
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GenericDistanceCut", m_genericDistanceCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackPathWidth", m_trackPathWidth));

    float maxTrackSeparation = std::sqrt(m_maxTrackSeparation2);
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackSeparation", maxTrackSeparation));
    m_maxTrackSeparation2 = maxTrackSeparation * maxTrackSeparation;

    if (m_maxTrackSeparation2 < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsFine", m_additionalPadWidthsFine));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsCoarse", m_additionalPadWidthsCoarse));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
