/**
 *  @file   LCContent/src/LCClustering/ConeClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCClustering/ConeClusteringAlgorithm.h"

//#define DEBUG 1

#ifdef DEBUG
#define DEBUG_PRINT(X) std::cout << X << std::endl
#else
#define DEBUG_PRINT(X) 
#endif

using namespace pandora;

namespace lc_content
{

ConeClusteringAlgorithm::ConeClusteringAlgorithm() :
    m_clusterSeedStrategy(2),
    m_shouldUseOnlyECalHits(false),
    m_shouldUseIsolatedHits(false),
    m_layersToStepBackFine(3),
    m_layersToStepBackCoarse(3),
    m_clusterFormationStrategy(0),
    m_genericDistanceCut(1.f),
    m_minHitTrackCosAngle(0.f),
    m_minHitClusterCosAngle(0.f),
    m_shouldUseTrackSeed(true),
    m_trackSeedCutOffLayer(0),
    m_shouldFollowInitialDirection(false),
    m_sameLayerPadWidthsFine(2.8f),
    m_sameLayerPadWidthsCoarse(1.8f),
    m_coneApproachMaxSeparation2(1000.f * 1000.f),
    m_tanConeAngleFine(0.3f),
    m_tanConeAngleCoarse(0.5f),
    m_additionalPadWidthsFine(2.5f),
    m_additionalPadWidthsCoarse(2.5f),
    m_maxClusterDirProjection(200.f),
    m_minClusterDirProjection(-10.f),
    m_trackPathWidth(2.f),
    m_maxTrackSeedSeparation2(250.f * 250.f),
    m_maxLayersToTrackSeed(3),
    m_maxLayersToTrackLikeHit(3),
    m_nLayersSpannedForFit(6),
    m_nLayersSpannedForApproxFit(10),
    m_nLayersToFit(8),
    m_nLayersToFitLowMipCut(0.5f),
    m_nLayersToFitLowMipMultiplier(2),
    m_fitSuccessDotProductCut1(0.75f),
    m_fitSuccessChi2Cut1(5.0f),
    m_fitSuccessDotProductCut2(0.50f),
    m_fitSuccessChi2Cut2(2.5f),
    m_mipTrackChi2Cut(2.5f)
{
  DEBUG_PRINT("Built ConeClusteringAlgorithm!");
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::Run()
{
  DEBUG_PRINT("Start ConeClusteringAlgorithm!");
    const CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));

    if (pCaloHitList->empty())
        return STATUS_CODE_SUCCESS;

    OrderedCaloHitList orderedCaloHitList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(*pCaloHitList));

    DEBUG_PRINT("Seeding clusters with tracks");
    ClusterVector clusterVector;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SeedClustersWithTracks(clusterVector));
    DEBUG_PRINT("Seeded clusters with tracks");

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const unsigned int pseudoLayer(iter->first);
	DEBUG_PRINT("\trunning ConeClusteringAlgorithm on pseudolayer: " << pseudoLayer);
        CustomSortedCaloHitList customSortedCaloHitList;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CaloHit *const pCaloHit = *hitIter;

            if ((m_shouldUseIsolatedHits || !pCaloHit->IsIsolated()) &&
                (!m_shouldUseOnlyECalHits || (ECAL == pCaloHit->GetHitType())) &&
                (PandoraContentApi::IsAvailable(*this, pCaloHit)))
            {
                customSortedCaloHitList.insert(pCaloHit);
            }
        }

	DEBUG_PRINT("\tbuilt list in PS: " << pseudoLayer << " with " << customSortedCaloHitList.size() << " hits");

        ClusterFitResultMap clusterFitResultMap;
	DEBUG_PRINT("\tGetting ClusterFitResults");
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetCurrentClusterFitResults(clusterVector, clusterFitResultMap));
	DEBUG_PRINT("\tGot ClusterFitResults");
	DEBUG_PRINT("\tFinding HitsInPreviousLayers");
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindHitsInPreviousLayers(pseudoLayer, &customSortedCaloHitList, clusterFitResultMap, clusterVector));
	DEBUG_PRINT("\tFound HitsInPreviousLayers");
	DEBUG_PRINT("\tFinding HitsInSameLayer");
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindHitsInSameLayer(pseudoLayer, &customSortedCaloHitList, clusterFitResultMap, clusterVector));
	DEBUG_PRINT("\tFound HitsInSameLayer");
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveEmptyClusters(clusterVector));

    DEBUG_PRINT("End ConeClusteringAlgorithm");

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::SeedClustersWithTracks(ClusterVector &clusterVector) const
{
    if (0 == m_clusterSeedStrategy)
        return STATUS_CODE_SUCCESS;

    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pTrackList));

    for (TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
    {
        const Track *const pTrack = *iter;

        if (!pTrack->CanFormPfo())
            continue;

        bool useTrack(false);

        if (2 == m_clusterSeedStrategy)
        {
            useTrack = true;
        }
        else if ((1 == m_clusterSeedStrategy) && pTrack->IsProjectedToEndCap())
        {
            useTrack = true;
        }

        if (useTrack)
        {
            const Cluster *pCluster = NULL;
            PandoraContentApi::Cluster::Parameters parameters;
            parameters.m_pTrack = pTrack;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pCluster));
            clusterVector.push_back(pCluster);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetCurrentClusterFitResults(const ClusterVector &clusterVector, ClusterFitResultMap &clusterFitResultMap) const
{
    if (!clusterFitResultMap.empty())
        return STATUS_CODE_INVALID_PARAMETER;

    for (ClusterVector::const_iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        const Cluster *const pCluster = *iter;
        ClusterFitResult clusterFitResult;

        if (pCluster->GetNCaloHits() > 1)
        {
            const unsigned int innerLayer(pCluster->GetInnerPseudoLayer());
            const unsigned int outerLayer(pCluster->GetOuterPseudoLayer());
            const unsigned int nLayersSpanned(outerLayer - innerLayer);

            if (nLayersSpanned > m_nLayersSpannedForFit)
            {
                unsigned int nLayersToFit(m_nLayersToFit);

                if (pCluster->GetMipFraction() - m_nLayersToFitLowMipCut < std::numeric_limits<float>::epsilon())
                    nLayersToFit *= m_nLayersToFitLowMipMultiplier;

                const unsigned int startLayer( (nLayersSpanned > nLayersToFit) ? (outerLayer - nLayersToFit) : innerLayer);
                (void) ClusterFitHelper::FitLayerCentroids(pCluster, startLayer, outerLayer, clusterFitResult);

                if (clusterFitResult.IsFitSuccessful())
                {
                    const float dotProduct(clusterFitResult.GetDirection().GetDotProduct(pCluster->GetInitialDirection()));
                    const float chi2(clusterFitResult.GetChi2());

                    if (((dotProduct < m_fitSuccessDotProductCut1) && (chi2 > m_fitSuccessChi2Cut1)) ||
                        ((dotProduct < m_fitSuccessDotProductCut2) && (chi2 > m_fitSuccessChi2Cut2)) )
                    {
                        clusterFitResult.SetSuccessFlag(false);
                    }
                }
            }
            else if (nLayersSpanned > m_nLayersSpannedForApproxFit)
            {
                const CartesianVector centroidChange(pCluster->GetCentroid(outerLayer) - pCluster->GetCentroid(innerLayer));
                clusterFitResult.Reset();
                clusterFitResult.SetDirection(centroidChange.GetUnitVector());
                clusterFitResult.SetSuccessFlag(true);
            }
        }

        if (!clusterFitResultMap.insert(ClusterFitResultMap::value_type(pCluster, clusterFitResult)).second)
            return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::FindHitsInPreviousLayers(unsigned int pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
    const ClusterFitResultMap &clusterFitResultMap, ClusterVector &clusterVector) const
{
    for (CustomSortedCaloHitList::iterator iter = pCustomSortedCaloHitList->begin(), iterEnd = pCustomSortedCaloHitList->end();
        iter != iterEnd;)
    {
        const CaloHit *const pCaloHit = *iter;

        const Cluster *pBestCluster = NULL;
        float bestClusterEnergy(0.f);
        float smallestGenericDistance(m_genericDistanceCut);
        const unsigned int layersToStepBack((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
            m_layersToStepBackFine : m_layersToStepBackCoarse);

        // Associate with existing clusters in stepBack layers. If stepBackLayer == pseudoLayer, will examine track projections
        for (unsigned int stepBackLayer = 1; (stepBackLayer <= layersToStepBack) && (stepBackLayer <= pseudoLayer); ++stepBackLayer)
        {
            const unsigned int searchLayer(pseudoLayer - stepBackLayer);

            // See if hit should be associated with any existing clusters
            for (ClusterVector::iterator clusterIter = clusterVector.begin(), clusterIterEnd = clusterVector.end();
                clusterIter != clusterIterEnd; ++clusterIter)
            {
                const Cluster *const pCluster = *clusterIter;
                float genericDistance(std::numeric_limits<float>::max());
                const float clusterEnergy(pCluster->GetHadronicEnergy());

                PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetGenericDistanceToHit(pCluster,
                    pCaloHit, searchLayer, clusterFitResultMap, genericDistance));

                if ((genericDistance < smallestGenericDistance) || ((genericDistance == smallestGenericDistance) && (clusterEnergy > bestClusterEnergy)))
                {
                    pBestCluster = pCluster;
                    bestClusterEnergy = clusterEnergy;
                    smallestGenericDistance = genericDistance;
                }
            }

            // Add best hit found after completing examination of a stepback layer
            if ((0 == m_clusterFormationStrategy) && (NULL != pBestCluster))
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pBestCluster, pCaloHit));
                break;
            }
        }

        // Add best hit found after examining all stepback layers
        if ((1 == m_clusterFormationStrategy) && (NULL != pBestCluster))
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pBestCluster, pCaloHit));
        }

        // Tidy the energy sorted calo hit list
        if (!PandoraContentApi::IsAvailable(*this, pCaloHit))
        {
            pCustomSortedCaloHitList->erase(iter++);
        }
        else
        {
            iter++;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::FindHitsInSameLayer(unsigned int pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
    const ClusterFitResultMap &clusterFitResultMap, ClusterVector &clusterVector) const
{
    while (!pCustomSortedCaloHitList->empty())
    {
        bool clustersModified = true;

        while (clustersModified)
        {
            clustersModified = false;

            for (CustomSortedCaloHitList::iterator iter = pCustomSortedCaloHitList->begin(), iterEnd = pCustomSortedCaloHitList->end();
                iter != iterEnd;)
            {
                const CaloHit *const pCaloHit = *iter;

                const Cluster *pBestCluster = NULL;
                float bestClusterEnergy(0.f);
                float smallestGenericDistance(m_genericDistanceCut);

                // See if hit should be associated with any existing clusters
                for (ClusterVector::const_iterator clusterIter = clusterVector.begin(), clusterIterEnd = clusterVector.end();
                    clusterIter != clusterIterEnd; ++clusterIter)
                {
                    const Cluster *const pCluster = *clusterIter;
                    float genericDistance(std::numeric_limits<float>::max());
                    const float clusterEnergy(pCluster->GetHadronicEnergy());

                    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetGenericDistanceToHit(pCluster,
                        pCaloHit, pseudoLayer, clusterFitResultMap, genericDistance));

                    if ((genericDistance < smallestGenericDistance) || ((genericDistance == smallestGenericDistance) && (clusterEnergy > bestClusterEnergy)))
                    {
                        pBestCluster = pCluster;
                        bestClusterEnergy = clusterEnergy;
                        smallestGenericDistance = genericDistance;
                    }
                }

                if (NULL != pBestCluster)
                {
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddToCluster(*this, pBestCluster, pCaloHit));
                    pCustomSortedCaloHitList->erase(iter++);
                    clustersModified = true;
                }
                else
                {
                    iter++;
                }
            }
        }

        // Seed a new cluster
        if (!pCustomSortedCaloHitList->empty())
        {
            const CaloHit *const pCaloHit = *(pCustomSortedCaloHitList->begin());
            pCustomSortedCaloHitList->erase(pCaloHit);

            const Cluster *pCluster = NULL;
            PandoraContentApi::Cluster::Parameters parameters;
            parameters.m_caloHitList.insert(pCaloHit);
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, parameters, pCluster));
            clusterVector.push_back(pCluster);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetGenericDistanceToHit(const Cluster *const pCluster, const CaloHit *const pCaloHit, const unsigned int searchLayer,
    const ClusterFitResultMap &clusterFitResultMap, float &genericDistance) const
{
    const unsigned int firstLayer(PandoraContentApi::GetPlugins(*this)->GetPseudoLayerPlugin()->GetPseudoLayerAtIp());

    // Use position of track projection at calorimeter. Proceed only if projection is reasonably compatible with calo hit
    if (((searchLayer == 0) || (searchLayer < firstLayer)) && pCluster->IsTrackSeeded())
    {
        const TrackState &trackState(pCluster->GetTrackSeed()->GetTrackStateAtCalorimeter());
        const CartesianVector trackDirection(trackState.GetMomentum().GetUnitVector());

        if (pCaloHit->GetExpectedDirection().GetCosOpeningAngle(trackDirection) < m_minHitTrackCosAngle)
            return STATUS_CODE_UNCHANGED;

        return this->GetConeApproachDistanceToHit(pCaloHit, trackState.GetPosition(), trackDirection, genericDistance);
    }

    // Check that cluster is occupied in the searchlayer and is reasonably compatible with calo hit
    OrderedCaloHitList::const_iterator clusterHitListIter = pCluster->GetOrderedCaloHitList().find(searchLayer);

    if (pCluster->GetOrderedCaloHitList().end() == clusterHitListIter)
        return STATUS_CODE_UNCHANGED;

    ClusterFitResultMap::const_iterator fitResultIter(clusterFitResultMap.find(pCluster));
    const ClusterFitResult clusterFitResult((clusterFitResultMap.end() != fitResultIter) ? fitResultIter->second : ClusterFitResult());
    const CartesianVector &clusterDirection(clusterFitResult.IsFitSuccessful() ?  clusterFitResult.GetDirection() : pCluster->GetInitialDirection());

    if (pCaloHit->GetExpectedDirection().GetCosOpeningAngle(clusterDirection) < m_minHitClusterCosAngle)
        return STATUS_CODE_UNCHANGED;

    // Cone approach measurements
    float initialDirectionDistance(std::numeric_limits<float>::max());
    float currentDirectionDistance(std::numeric_limits<float>::max());
    float trackSeedDistance(std::numeric_limits<float>::max());

    const bool useTrackSeed(m_shouldUseTrackSeed && pCluster->IsTrackSeeded());
    const bool followInitialDirection(m_shouldFollowInitialDirection && pCluster->IsTrackSeeded() && (searchLayer > m_trackSeedCutOffLayer));

    if (!useTrackSeed || (searchLayer > m_trackSeedCutOffLayer))
    {
        const CaloHitList *const pClusterCaloHitList = clusterHitListIter->second;

        if (searchLayer == pCaloHit->GetPseudoLayer())
        {
            return this->GetDistanceToHitInSameLayer(pCaloHit, pClusterCaloHitList, genericDistance);
        }

        // Measurement using initial cluster direction
        StatusCode initialStatusCode = this->GetConeApproachDistanceToHit(pCaloHit, pClusterCaloHitList, pCluster->GetInitialDirection(),
            initialDirectionDistance);

        if (STATUS_CODE_SUCCESS == initialStatusCode)
        {
            if (followInitialDirection)
                initialDirectionDistance /= 5.;
        }
        else if (STATUS_CODE_UNCHANGED != initialStatusCode)
        {
            return initialStatusCode;
        }

        // Measurement using current cluster direction
        if (clusterFitResult.IsFitSuccessful())
        {
            StatusCode currentStatusCode = this->GetConeApproachDistanceToHit(pCaloHit, pClusterCaloHitList, clusterFitResult.GetDirection(),
                currentDirectionDistance);

            if (STATUS_CODE_SUCCESS == currentStatusCode)
            {
                if ((currentDirectionDistance < m_genericDistanceCut) && pCluster->IsTrackSeeded())
                {
                    try
                    {
                        if (clusterFitResult.GetChi2() < m_mipTrackChi2Cut)
                            currentDirectionDistance /= 5.;
                    }
                    catch (StatusCodeException &) {}
                }
            }
            else if (STATUS_CODE_UNCHANGED != currentStatusCode)
            {
                return currentStatusCode;
            }
        }
    }

    // Seed track distance measurements
    if (useTrackSeed && !followInitialDirection)
    {
        StatusCode trackStatusCode = this->GetDistanceToTrackSeed(pCluster, pCaloHit, searchLayer, trackSeedDistance);

        if (STATUS_CODE_SUCCESS == trackStatusCode)
        {
            if (trackSeedDistance < m_genericDistanceCut)
                trackSeedDistance /= 5.;
        }
        else if (STATUS_CODE_UNCHANGED != trackStatusCode)
        {
            return trackStatusCode;
        }
    }

    // Identify best measurement of generic distance
    const float smallestDistance(std::min(trackSeedDistance, std::min(initialDirectionDistance, currentDirectionDistance)));
    if (smallestDistance < genericDistance)
    {
        genericDistance = smallestDistance;

        if (std::numeric_limits<float>::max() != genericDistance)
            return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetDistanceToHitInSameLayer(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList,
    float &distance) const
{
    const float dCut ((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
        (m_sameLayerPadWidthsFine * pCaloHit->GetCellLengthScale()) :
        (m_sameLayerPadWidthsCoarse * pCaloHit->GetCellLengthScale()) );

    if (dCut < std::numeric_limits<float>::epsilon())
        return STATUS_CODE_FAILURE;

    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());

    bool hitFound(false);
    float smallestDistanceSquared(std::numeric_limits<float>::max());
    const float rDCutSquared(1.f / (dCut * dCut));

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        const CaloHit *const pHitInCluster = *iter;
        const CartesianVector &hitInClusterPosition(pHitInCluster->GetPositionVector());
        const float separationSquared((hitPosition - hitInClusterPosition).GetMagnitudeSquared());
        const float hitDistanceSquared(separationSquared * rDCutSquared);

        if (hitDistanceSquared < smallestDistanceSquared)
        {
            smallestDistanceSquared = hitDistanceSquared;
            hitFound = true;
        }
    }

    if (!hitFound)
        return STATUS_CODE_UNCHANGED;

    distance = std::sqrt(smallestDistanceSquared);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetConeApproachDistanceToHit(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList,
    const CartesianVector &clusterDirection, float &distance) const
{
    bool hitFound(false);
    float smallestDistance(std::numeric_limits<float>::max());

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        const CaloHit *const pHitInCluster = *iter;
        float hitDistance(std::numeric_limits<float>::max());

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetConeApproachDistanceToHit(pCaloHit,
            pHitInCluster->GetPositionVector(), clusterDirection, hitDistance));

        if (hitDistance < smallestDistance)
        {
            smallestDistance = hitDistance;
            hitFound = true;
        }
    }

    if (!hitFound)
        return STATUS_CODE_UNCHANGED;

    distance = smallestDistance;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetConeApproachDistanceToHit(const CaloHit *const pCaloHit, const CartesianVector &clusterPosition,
    const CartesianVector &clusterDirection, float &distance) const
{
    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());
    const CartesianVector positionDifference(hitPosition - clusterPosition);

    if (positionDifference.GetMagnitudeSquared() > m_coneApproachMaxSeparation2)
        return STATUS_CODE_UNCHANGED;

    const float dAlong(clusterDirection.GetDotProduct(positionDifference));

    if ((dAlong < m_maxClusterDirProjection) && (dAlong > m_minClusterDirProjection))
    {
        const float dCut ((PandoraContentApi::GetGeometry(*this)->GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
            (std::fabs(dAlong) * m_tanConeAngleFine) + (m_additionalPadWidthsFine * pCaloHit->GetCellLengthScale()) :
            (std::fabs(dAlong) * m_tanConeAngleCoarse) + (m_additionalPadWidthsCoarse * pCaloHit->GetCellLengthScale()) );

        if (dCut < std::numeric_limits<float>::epsilon())
            return STATUS_CODE_FAILURE;

        const float dPerp (clusterDirection.GetCrossProduct(positionDifference).GetMagnitude());

        distance = dPerp / dCut;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetDistanceToTrackSeed(const Cluster *const pCluster, const CaloHit *const pCaloHit, unsigned int searchLayer,
    float &distance) const
{
    if (searchLayer < m_maxLayersToTrackSeed)
        return this->GetDistanceToTrackSeed(pCluster, pCaloHit, distance);

    const int searchLayerInt(static_cast<int>(searchLayer));
    const int startLayer(std::max(0, searchLayerInt - static_cast<int>(m_maxLayersToTrackLikeHit)));

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();

    for (int iLayer = startLayer; iLayer < searchLayerInt; ++iLayer)
    {
        OrderedCaloHitList::const_iterator listIter = orderedCaloHitList.find(iLayer);
        if (orderedCaloHitList.end() == listIter)
            continue;

        for (CaloHitList::const_iterator iter = listIter->second->begin(), iterEnd = listIter->second->end(); iter != iterEnd; ++iter)
        {
            float tempDistance(std::numeric_limits<float>::max());
            PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetDistanceToTrackSeed(pCluster, *iter,
                tempDistance));

            if (tempDistance < m_genericDistanceCut)
                return this->GetDistanceToTrackSeed(pCluster, pCaloHit, distance);
        }
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::GetDistanceToTrackSeed(const Cluster *const pCluster, const CaloHit *const pCaloHit, float &distance) const
{
    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());
    const CartesianVector &trackSeedPosition(pCluster->GetTrackSeed()->GetTrackStateAtCalorimeter().GetPosition());

    const CartesianVector positionDifference(hitPosition - trackSeedPosition);
    const float separationSquared(positionDifference.GetMagnitudeSquared());

    if (separationSquared < m_maxTrackSeedSeparation2)
    {
        const float flexibility(1.f + (m_trackPathWidth * std::sqrt(separationSquared / m_maxTrackSeedSeparation2)));

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

StatusCode ConeClusteringAlgorithm::RemoveEmptyClusters(const ClusterVector &clusterVector) const
{
    ClusterList clusterDeletionList;

    for (ClusterVector::const_iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        if (0 == (*iter)->GetNCaloHits())
        {
            clusterDeletionList.insert(*iter);
        }
    }

    if (!clusterDeletionList.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Delete(*this, &clusterDeletionList));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Track seeding parameters
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterSeedStrategy", m_clusterSeedStrategy));

    // High level clustering parameters
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseOnlyECalHits", m_shouldUseOnlyECalHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseIsolatedHits", m_shouldUseIsolatedHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayersToStepBackFine", m_layersToStepBackFine));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayersToStepBackCoarse", m_layersToStepBackCoarse));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterFormationStrategy", m_clusterFormationStrategy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GenericDistanceCut", m_genericDistanceCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitTrackCosAngle", m_minHitTrackCosAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitClusterCosAngle", m_minHitClusterCosAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseTrackSeed", m_shouldUseTrackSeed));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackSeedCutOffLayer", m_trackSeedCutOffLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldFollowInitialDirection", m_shouldFollowInitialDirection));

    // Same layer distance parameters
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SameLayerPadWidthsFine", m_sameLayerPadWidthsFine));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SameLayerPadWidthsCoarse", m_sameLayerPadWidthsCoarse));

    // Cone approach distance parameters
    float coneApproachMaxSeparation = std::sqrt(m_coneApproachMaxSeparation2);
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeApproachMaxSeparation", coneApproachMaxSeparation));
    m_coneApproachMaxSeparation2 = coneApproachMaxSeparation * coneApproachMaxSeparation;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TanConeAngleFine", m_tanConeAngleFine));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TanConeAngleCoarse", m_tanConeAngleCoarse));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsFine", m_additionalPadWidthsFine));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsCoarse", m_additionalPadWidthsCoarse));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxClusterDirProjection", m_maxClusterDirProjection));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterDirProjection", m_minClusterDirProjection));

    // Track seed distance parameters
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackPathWidth", m_trackPathWidth));

    float maxTrackSeedSeparation = std::sqrt(m_maxTrackSeedSeparation2);
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackSeedSeparation", maxTrackSeedSeparation));
    m_maxTrackSeedSeparation2 = maxTrackSeedSeparation * maxTrackSeedSeparation;

    if (m_shouldUseTrackSeed && (m_maxTrackSeedSeparation2 < std::numeric_limits<float>::epsilon()))
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayersToTrackSeed", m_maxLayersToTrackSeed));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayersToTrackLikeHit", m_maxLayersToTrackLikeHit));

    // Cluster current direction and mip track parameters
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersSpannedForFit", m_nLayersSpannedForFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersSpannedForApproxFit", m_nLayersSpannedForApproxFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersToFit", m_nLayersToFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersToFitLowMipCut", m_nLayersToFitLowMipCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersToFitLowMipMultiplier", m_nLayersToFitLowMipMultiplier));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitSuccessDotProductCut1", m_fitSuccessDotProductCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitSuccessChi2Cut1", m_fitSuccessChi2Cut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitSuccessDotProductCut2", m_fitSuccessDotProductCut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitSuccessChi2Cut2", m_fitSuccessChi2Cut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipTrackChi2Cut", m_mipTrackChi2Cut));

    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
