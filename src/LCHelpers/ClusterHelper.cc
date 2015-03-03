/**
 *  @file   PandoraSDK/src/LCHelpers/ClusterHelper.cc
 * 
 *  @brief  Implementation of the cluster helper class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCHelpers/ClusterHelper.h"

using namespace pandora;

namespace lc_content
{

StatusCode ClusterHelper::GetFitResultsClosestApproach(const ClusterFitResult &lhs, const ClusterFitResult &rhs, float &closestApproach)
{
    if (!lhs.IsFitSuccessful() || !rhs.IsFitSuccessful())
        return STATUS_CODE_INVALID_PARAMETER;

    const CartesianVector interceptDifference(lhs.GetIntercept() - rhs.GetIntercept());

    try
    {
        const CartesianVector directionNormal((lhs.GetDirection().GetCrossProduct(rhs.GetDirection())).GetUnitVector());
        closestApproach = std::fabs(directionNormal.GetDotProduct(interceptDifference));
    }
    catch (StatusCodeException &)
    {
        closestApproach = interceptDifference.GetMagnitude();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::GetDistanceToClosestHit(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
    const unsigned int startLayer, const unsigned int endLayer)
{
    if (startLayer > endLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    bool distanceFound(false);
    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (unsigned int iLayer = startLayer; iLayer <= endLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() == iter)
            continue;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CartesianVector interceptDifference((*hitIter)->GetPositionVector() - clusterFitResult.GetIntercept());
            const float distanceSquared(interceptDifference.GetCrossProduct(clusterFitResult.GetDirection()).GetMagnitudeSquared());

            if (distanceSquared < minDistanceSquared)
            {
                minDistanceSquared = distanceSquared;
                distanceFound = true;
            }
        }
    }

    if (!distanceFound)
        return std::numeric_limits<float>::max();

    return std::sqrt(minDistanceSquared);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::GetDistanceToClosestHit(const Cluster *const pClusterI, const Cluster *const pClusterJ)
{
    bool distanceFound(false);
    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitListI(pClusterI->GetOrderedCaloHitList());
    const OrderedCaloHitList &orderedCaloHitListJ(pClusterJ->GetOrderedCaloHitList());

    // Loop over hits in cluster I
    for (OrderedCaloHitList::const_iterator iterI = orderedCaloHitListI.begin(), iterIEnd = orderedCaloHitListI.end(); iterI != iterIEnd; ++iterI)
    {
        for (CaloHitList::const_iterator hitIterI = iterI->second->begin(), hitIterIEnd = iterI->second->end(); hitIterI != hitIterIEnd; ++hitIterI)
        {
            const CartesianVector &positionVectorI((*hitIterI)->GetPositionVector());

            // For each hit in cluster I, find closest distance to a hit in cluster J
            for (OrderedCaloHitList::const_iterator iterJ = orderedCaloHitListJ.begin(), iterJEnd = orderedCaloHitListJ.end(); iterJ != iterJEnd; ++iterJ)
            {
                for (CaloHitList::const_iterator hitIterJ = iterJ->second->begin(), hitIterJEnd = iterJ->second->end(); hitIterJ != hitIterJEnd; ++hitIterJ)
                {
                    const float distanceSquared((positionVectorI - (*hitIterJ)->GetPositionVector()).GetMagnitudeSquared());

                    if (distanceSquared < minDistanceSquared)
                    {
                        minDistanceSquared = distanceSquared;
                        distanceFound = true;
                    }
                }
            }
        }
    }

    if (!distanceFound)
        return std::numeric_limits<float>::max();

    return std::sqrt(minDistanceSquared);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::GetDistanceToClosestCentroid(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
    const unsigned int startLayer, const unsigned int endLayer)
{
    if (startLayer > endLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    bool distanceFound(false);
    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (unsigned int iLayer = startLayer; iLayer <= endLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() == iter)
            continue;

        const CartesianVector interceptDifference(pCluster->GetCentroid(iLayer) - clusterFitResult.GetIntercept());
        const float distanceSquared(interceptDifference.GetCrossProduct(clusterFitResult.GetDirection()).GetMagnitudeSquared());

        if (distanceSquared < minDistanceSquared)
        {
            minDistanceSquared = distanceSquared;
            distanceFound = true;
        }
    }

    if (!distanceFound)
        return std::numeric_limits<float>::max();

    return std::sqrt(minDistanceSquared);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetDistanceToClosestCentroid(const Cluster *const pClusterI, const Cluster *const pClusterJ, float &centroidDistance)
{
    // Return if clusters do not overlap
    if ((pClusterI->GetOuterPseudoLayer() < pClusterJ->GetInnerPseudoLayer()) || (pClusterJ->GetOuterPseudoLayer() < pClusterI->GetInnerPseudoLayer()))
        return STATUS_CODE_NOT_FOUND;

    bool distanceFound(false);
    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitListI(pClusterI->GetOrderedCaloHitList());
    const OrderedCaloHitList &orderedCaloHitListJ(pClusterJ->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iterI = orderedCaloHitListI.begin(), iterIEnd = orderedCaloHitListI.end(); iterI != iterIEnd; ++iterI)
    {
        const CartesianVector centroidI(pClusterI->GetCentroid(iterI->first));

        for (OrderedCaloHitList::const_iterator iterJ = orderedCaloHitListJ.begin(), iterJEnd = orderedCaloHitListJ.end(); iterJ != iterJEnd; ++iterJ)
        {
            if (orderedCaloHitListI.end() == orderedCaloHitListI.find(iterJ->first))
                continue;

            const CartesianVector centroidJ(pClusterJ->GetCentroid(iterJ->first));

            const float distanceSquared((centroidI - centroidJ).GetMagnitudeSquared());

            if (distanceSquared < minDistanceSquared)
            {
                minDistanceSquared = distanceSquared;
                distanceFound = true;
            }
        }
    }

    if (!distanceFound)
        return STATUS_CODE_NOT_FOUND;

    centroidDistance = std::sqrt(minDistanceSquared);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetClosestIntraLayerDistance(const Cluster *const pClusterI, const Cluster *const pClusterJ, float &intraLayerDistance)
{
    // Return if clusters do not overlap
    if ((pClusterI->GetOuterPseudoLayer() < pClusterJ->GetInnerPseudoLayer()) || (pClusterJ->GetOuterPseudoLayer() < pClusterI->GetInnerPseudoLayer()))
        return STATUS_CODE_NOT_FOUND;

    bool distanceFound(false);
    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitListI(pClusterI->GetOrderedCaloHitList());
    const OrderedCaloHitList &orderedCaloHitListJ(pClusterJ->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iterI = orderedCaloHitListI.begin(), iterIEnd = orderedCaloHitListI.end(); iterI != iterIEnd; ++iterI)
    {
        const unsigned int pseudoLayer(iterI->first);
        OrderedCaloHitList::const_iterator iterJ = orderedCaloHitListJ.find(pseudoLayer);

        if (orderedCaloHitListJ.end() == iterJ)
            continue;

        const CartesianVector centroidI(pClusterI->GetCentroid(pseudoLayer));
        const CartesianVector centroidJ(pClusterJ->GetCentroid(pseudoLayer));

        const float distanceSquared((centroidI - centroidJ).GetMagnitudeSquared());

        if (distanceSquared < minDistanceSquared)
        {
            minDistanceSquared = distanceSquared;
            distanceFound = true;
        }
    }

    if (!distanceFound)
        return STATUS_CODE_NOT_FOUND;

    intraLayerDistance = std::sqrt(minDistanceSquared);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetTrackClusterDistance(const Track *const pTrack, const Cluster *const pCluster, const unsigned int maxSearchLayer,
    const float parallelDistanceCut, const float minTrackClusterCosAngle, float &trackClusterDistance)
{
    if ((0 == pCluster->GetNCaloHits()) || (pCluster->GetInnerPseudoLayer() > maxSearchLayer))
        return STATUS_CODE_NOT_FOUND;

    const TrackState &trackState(pTrack->GetTrackStateAtCalorimeter());
    const CartesianVector &trackPosition(trackState.GetPosition());
    const CartesianVector trackDirection(trackState.GetMomentum().GetUnitVector());

    if (trackDirection.GetCosOpeningAngle(pCluster->GetInitialDirection()) < minTrackClusterCosAngle)
        return STATUS_CODE_NOT_FOUND;

    bool distanceFound(false);
    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (iter->first > maxSearchLayer)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CartesianVector positionDifference((*hitIter)->GetPositionVector() - trackPosition);

            if (std::fabs(trackDirection.GetDotProduct(positionDifference)) > parallelDistanceCut)
                continue;

            const float perpendicularDistanceSquared((trackDirection.GetCrossProduct(positionDifference)).GetMagnitudeSquared());

            if (perpendicularDistanceSquared < minDistanceSquared)
            {
                minDistanceSquared = perpendicularDistanceSquared;
                distanceFound = true;
            }
        }
    }

    if (!distanceFound)
        return STATUS_CODE_NOT_FOUND;

    trackClusterDistance = std::sqrt(minDistanceSquared);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ClusterHelper::CanMergeCluster(const Pandora &pandora, const Cluster *const pCluster, const float minMipFraction, const float maxAllHitsFitRms)
{
    if (0 == pCluster->GetNCaloHits())
        return false;

    if (!(pCluster->IsPhotonFast(pandora)))
        return true;

    if (pCluster->GetMipFraction() - minMipFraction > std::numeric_limits<float>::epsilon())
        return true;

    return (pCluster->GetFitToAllHitsResult().IsFitSuccessful() && (pCluster->GetFitToAllHitsResult().GetRms() < maxAllHitsFitRms));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::GetEnergyWeightedMeanTime(const Cluster *const pCluster)
{
    if (0 == pCluster->GetNCaloHits())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    float energySum(0.f);
    float energyTimeProductSum(0.f);

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const float hadronicEnergy((*hitIter)->GetHadronicEnergy());
            energySum += hadronicEnergy;
            energyTimeProductSum += (hadronicEnergy * (*hitIter)->GetTime());
        }
    }

    if ((energySum < std::numeric_limits<float>::epsilon()) || (energyTimeProductSum < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_FAILURE);

    return energyTimeProductSum / energySum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ClusterHelper::DoesClusterCrossGapRegion(const Pandora &pandora, const Cluster *const pCluster, const unsigned int startLayer,
    const unsigned int endLayer, const unsigned int nSamplingPoints)
{
    const unsigned int fitStartLayer(std::max(startLayer, pCluster->GetInnerPseudoLayer()));
    const unsigned int fitEndLayer(std::min(endLayer, pCluster->GetOuterPseudoLayer()));

    if (fitStartLayer > fitEndLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    ClusterFitResult fitResult;
    if (STATUS_CODE_SUCCESS != ClusterFitHelper::FitLayers(pCluster, fitStartLayer, fitEndLayer, fitResult))
        return false;

    const CartesianVector startLayerCentroid(pCluster->GetCentroid(fitStartLayer));
    const float propagationDistance((pCluster->GetCentroid(fitEndLayer) - startLayerCentroid).GetDotProduct(fitResult.GetDirection()));

    return ClusterHelper::DoesFitCrossGapRegion(pandora, fitResult, startLayerCentroid, propagationDistance, nSamplingPoints);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ClusterHelper::DoesFitCrossGapRegion(const Pandora &pandora, const ClusterFitResult &clusterFitResult, const CartesianVector &startPosition,
    const float propagationDistance, const unsigned int nSamplingPoints)
{
    const CartesianVector &fitDirection(clusterFitResult.GetDirection());
    const CartesianVector &fitIntercept(clusterFitResult.GetIntercept());

    const float fitStartDistance((startPosition - fitIntercept).GetDotProduct(fitDirection));
    const CartesianVector fitStartPosition(fitIntercept + (fitDirection * fitStartDistance));
    const CartesianVector fitPropagation(fitDirection * propagationDistance);

    const DetectorGapList &detectorGapList(pandora.GetGeometry()->GetDetectorGapList());

    for (unsigned int i = 0; i < nSamplingPoints; ++i)
    {
        const CartesianVector fitPosition(fitStartPosition + (fitPropagation * (static_cast<float>(i) / static_cast<float>(nSamplingPoints))));

        for (DetectorGapList::const_iterator iter = detectorGapList.begin(), iterEnd = detectorGapList.end(); iter != iterEnd; ++iter)
        {
            if ((*iter)->IsInGap(fitPosition))
                return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ClusterHelper::IsClusterLeavingDetector(const Cluster *const pCluster, const unsigned int nOuterLayersToExamine,
    const unsigned int nMipLikeOccupiedLayers, const unsigned int nShowerLikeOccupiedLayers, const float showerLikeEnergyInOuterLayers)
{
    if (!ClusterHelper::ContainsHitInOuterSamplingLayer(pCluster))
        return false;

    if (ClusterHelper::ContainsHitType(pCluster, MUON))
        return true;

    // Examine occupancy and energy content of outer layers
    const unsigned int outerLayer(pCluster->GetOuterPseudoLayer());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    if (nOuterLayersToExamine > outerLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    unsigned int nOccupiedOuterLayers(0);
    float hadronicEnergyInOuterLayers(0.f);

    for (unsigned int iLayer = outerLayer - nOuterLayersToExamine; iLayer <= outerLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() != iter)
        {
            nOccupiedOuterLayers++;

            for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                hadronicEnergyInOuterLayers += (*hitIter)->GetHadronicEnergy();
            }
        }
    }

    if ((nOccupiedOuterLayers >= nMipLikeOccupiedLayers) ||
        ((nOccupiedOuterLayers == nShowerLikeOccupiedLayers) && (hadronicEnergyInOuterLayers > showerLikeEnergyInOuterLayers)))
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ClusterHelper::ContainsHitType(const Cluster *const pCluster, const HitType hitType)
{
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_reverse_iterator iter = orderedCaloHitList.rbegin(), iterEnd = orderedCaloHitList.rend(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hIter = iter->second->begin(), hIterEnd = iter->second->end(); hIter != hIterEnd; ++hIter)
        {
            const CaloHit *const pCaloHit(*hIter);

            if (hitType == pCaloHit->GetHitType())
                return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ClusterHelper::ContainsHitInOuterSamplingLayer(const Cluster *const pCluster)
{
    return (0 != pCluster->GetNHitsInOuterLayer());
}

} // namespace lc_content
