/**
 *  @file   PandoraPFANew/Framework/src/Helpers/ClusterHelper.cc
 * 
 *  @brief  Implementation of the cluster helper class.
 * 
 *  $Log: $
 */

#include "Helpers/ClusterHelper.h"
#include "Helpers/GeometryHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/OrderedCaloHitList.h"
#include "Objects/Track.h"

#include <cmath>
#include <limits>

namespace pandora
{

StatusCode ClusterHelper::FitStart(const Cluster *const pCluster, const unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult)
{
    if (maxOccupiedLayers < 2)
        return STATUS_CODE_INVALID_PARAMETER;

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listSize < 2)
        return STATUS_CODE_OUT_OF_RANGE;

    unsigned int occupiedLayerCount(0);

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (++occupiedLayerCount > maxOccupiedLayers)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitEnd(const Cluster *const pCluster, const unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult)
{
    if (maxOccupiedLayers < 2)
        return STATUS_CODE_INVALID_PARAMETER;

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listSize < 2)
        return STATUS_CODE_OUT_OF_RANGE;

    unsigned int occupiedLayerCount(0);

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_reverse_iterator iter = orderedCaloHitList.rbegin(), iterEnd = orderedCaloHitList.rend(); iter != iterEnd; ++iter)
    {
        if (++occupiedLayerCount > maxOccupiedLayers)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitFullCluster(const Cluster *const pCluster, ClusterFitResult &clusterFitResult)
{
    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listSize < 2)
        return STATUS_CODE_OUT_OF_RANGE;

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitLayers(const Cluster *const pCluster, const PseudoLayer startLayer, const PseudoLayer endLayer,
    ClusterFitResult &clusterFitResult)
{
    if (startLayer >= endLayer)
        return STATUS_CODE_INVALID_PARAMETER;

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listSize < 2)
        return STATUS_CODE_OUT_OF_RANGE;

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const PseudoLayer pseudoLayer(iter->first);

        if (startLayer > pseudoLayer)
            continue;

        if (endLayer < pseudoLayer)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitLayerCentroids(const Cluster *const pCluster, const PseudoLayer startLayer, const PseudoLayer endLayer,
    ClusterFitResult &clusterFitResult)
{
    try
    {
        if (startLayer >= endLayer)
            return STATUS_CODE_INVALID_PARAMETER;

        const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
        const unsigned int listSize(orderedCaloHitList.size());

        if (0 == listSize)
            return STATUS_CODE_NOT_INITIALIZED;

        if (listSize < 2)
            return STATUS_CODE_OUT_OF_RANGE;

        ClusterFitPointList clusterFitPointList;
        for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
        {
            const PseudoLayer pseudoLayer(iter->first);

            if (startLayer > pseudoLayer)
                continue;

            if (endLayer < pseudoLayer)
                break;

            float cellLengthScaleSum(0.f), cellEnergySum(0.f);
            CartesianVector cellNormalVectorSum(0.f, 0.f, 0.f);

            for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                cellLengthScaleSum += (*hitIter)->GetCellLengthScale();
                cellNormalVectorSum += (*hitIter)->GetCellNormalVector();
                cellEnergySum += (*hitIter)->GetInputEnergy();
            }

            clusterFitPointList.push_back(ClusterFitPoint(pCluster->GetCentroid(pseudoLayer), cellNormalVectorSum.GetUnitVector(),
                cellLengthScaleSum / static_cast<float>(iter->second->size()), cellEnergySum / static_cast<float>(iter->second->size()),
                pseudoLayer));
        }

        return FitPoints(clusterFitPointList, clusterFitResult);
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "ClusterHelper: linear fit to cluster failed. " << std::endl;
        clusterFitResult.SetSuccessFlag(false);
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitPoints(const ClusterFitPointList &clusterFitPointList, ClusterFitResult &clusterFitResult)
{
    try
    {
        const unsigned int nFitPoints(clusterFitPointList.size());

        if (nFitPoints < 2)
            return STATUS_CODE_INVALID_PARAMETER;

        clusterFitResult.Reset();
        CartesianVector positionSum(0.f, 0.f, 0.f);
        CartesianVector normalVectorSum(0.f, 0.f, 0.f);

        for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
        {
            positionSum += iter->GetPosition();
            normalVectorSum += iter->GetCellNormalVector();
        }

        return PerformLinearFit(clusterFitPointList, positionSum * (1.f / static_cast<float>(nFitPoints)), normalVectorSum.GetUnitVector(),
            clusterFitResult);
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "ClusterHelper: linear fit to cluster failed. " << std::endl;
        clusterFitResult.SetSuccessFlag(false);
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::PerformLinearFit(const ClusterFitPointList &clusterFitPointList, const CartesianVector &centralPosition,
    const CartesianVector &centralDirection, ClusterFitResult &clusterFitResult)
{
    // Extract the data
    double sumP(0.), sumQ(0.), sumR(0.), sumWeights(0.);
    double sumPR(0.), sumQR(0.), sumRR(0.);

    static const CartesianVector chosenAxis(0.f, 0.f, 1.f);
    const double cosTheta(centralDirection.GetCosOpeningAngle(chosenAxis));
    const double sinTheta(std::sin(std::acos(cosTheta)));

    const CartesianVector rotationAxis((std::fabs(cosTheta) > 0.99) ? CartesianVector(1.f, 0.f, 0.f) :
        centralDirection.GetCrossProduct(chosenAxis).GetUnitVector());

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const CartesianVector position(iter->GetPosition() - centralPosition);
        const double weight(1.);

        const double p( (cosTheta + rotationAxis.GetX() * rotationAxis.GetX() * (1. - cosTheta)) * position.GetX() +
            (rotationAxis.GetX() * rotationAxis.GetY() * (1. - cosTheta) - rotationAxis.GetZ() * sinTheta) * position.GetY() +
            (rotationAxis.GetX() * rotationAxis.GetZ() * (1. - cosTheta) + rotationAxis.GetY() * sinTheta) * position.GetZ() );
        const double q( (rotationAxis.GetY() * rotationAxis.GetX() * (1. - cosTheta) + rotationAxis.GetZ() * sinTheta) * position.GetX() +
            (cosTheta + rotationAxis.GetY() * rotationAxis.GetY() * (1. - cosTheta)) * position.GetY() +
            (rotationAxis.GetY() * rotationAxis.GetZ() * (1. - cosTheta) - rotationAxis.GetX() * sinTheta) * position.GetZ() );
        const double r( (rotationAxis.GetZ() * rotationAxis.GetX() * (1. - cosTheta) - rotationAxis.GetY() * sinTheta) * position.GetX() +
            (rotationAxis.GetZ() * rotationAxis.GetY() * (1. - cosTheta) + rotationAxis.GetX() * sinTheta) * position.GetY() +
            (cosTheta + rotationAxis.GetZ() * rotationAxis.GetZ() * (1. - cosTheta)) * position.GetZ() );

        sumP += p * weight; sumQ += q * weight; sumR += r * weight;
        sumPR += p * r * weight; sumQR += q * r * weight; sumRR += r * r * weight;
        sumWeights += weight;
    }

    // Perform the fit
    const double denominatorR(sumR * sumR - sumWeights * sumRR);

    if (0. == denominatorR)
        return STATUS_CODE_FAILURE;

    const double aP((sumR * sumP - sumWeights * sumPR) / denominatorR);
    const double bP((sumP - aP * sumR) / sumWeights);
    const double aQ((sumR * sumQ - sumWeights * sumQR) / denominatorR);
    const double bQ((sumQ - aQ * sumR) / sumWeights);

    // Extract direction and intercept
    const double magnitude(std::sqrt(1. + aP * aP + aQ * aQ));
    const double dirP(aP / magnitude), dirQ(aQ / magnitude), dirR(1. / magnitude);

    CartesianVector direction(
        static_cast<float>((cosTheta + rotationAxis.GetX() * rotationAxis.GetX() * (1. - cosTheta)) * dirP +
            (rotationAxis.GetX() * rotationAxis.GetY() * (1. - cosTheta) + rotationAxis.GetZ() * sinTheta) * dirQ +
            (rotationAxis.GetX() * rotationAxis.GetZ() * (1. - cosTheta) - rotationAxis.GetY() * sinTheta) * dirR),
        static_cast<float>((rotationAxis.GetY() * rotationAxis.GetX() * (1. - cosTheta) - rotationAxis.GetZ() * sinTheta) * dirP +
            (cosTheta + rotationAxis.GetY() * rotationAxis.GetY() * (1. - cosTheta)) * dirQ +
            (rotationAxis.GetY() * rotationAxis.GetZ() * (1. - cosTheta) + rotationAxis.GetX() * sinTheta) * dirR),
        static_cast<float>((rotationAxis.GetZ() * rotationAxis.GetX() * (1. - cosTheta) + rotationAxis.GetY() * sinTheta) * dirP +
            (rotationAxis.GetZ() * rotationAxis.GetY() * (1. - cosTheta) - rotationAxis.GetX() * sinTheta) * dirQ +
            (cosTheta + rotationAxis.GetZ() * rotationAxis.GetZ() * (1. - cosTheta)) * dirR) );

    CartesianVector intercept(centralPosition + CartesianVector(
        static_cast<float>((cosTheta + rotationAxis.GetX() * rotationAxis.GetX() * (1. - cosTheta)) * bP +
            (rotationAxis.GetX() * rotationAxis.GetY() * (1. - cosTheta) + rotationAxis.GetZ() * sinTheta) * bQ),
        static_cast<float>((rotationAxis.GetY() * rotationAxis.GetX() * (1. - cosTheta) - rotationAxis.GetZ() * sinTheta) * bP +
            (cosTheta + rotationAxis.GetY() * rotationAxis.GetY() * (1. - cosTheta)) * bQ),
        static_cast<float>((rotationAxis.GetZ() * rotationAxis.GetX() * (1. - cosTheta) + rotationAxis.GetY() * sinTheta) * bP +
            (rotationAxis.GetZ() * rotationAxis.GetY() * (1. - cosTheta) - rotationAxis.GetX() * sinTheta) * bQ) ));

    // Extract radial direction cosine
    float dirCosR(direction.GetDotProduct(intercept) / intercept.GetMagnitude());

    if (0.f > dirCosR)
    {
        dirCosR = -dirCosR;
        direction = direction * -1.f;
    }

    // Now calculate something like a chi2
    double chi2_P(0.), chi2_Q(0.), rms(0.);
    double sumA(0.), sumL(0.), sumAL(0.), sumLL(0.);

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const CartesianVector position(iter->GetPosition() - centralPosition);

        const double p( (cosTheta + rotationAxis.GetX() * rotationAxis.GetX() * (1. - cosTheta)) * position.GetX() +
            (rotationAxis.GetX() * rotationAxis.GetY() * (1. - cosTheta) - rotationAxis.GetZ() * sinTheta) * position.GetY() +
            (rotationAxis.GetX() * rotationAxis.GetZ() * (1. - cosTheta) + rotationAxis.GetY() * sinTheta) * position.GetZ() );
        const double q( (rotationAxis.GetY() * rotationAxis.GetX() * (1. - cosTheta) + rotationAxis.GetZ() * sinTheta) * position.GetX() +
            (cosTheta + rotationAxis.GetY() * rotationAxis.GetY() * (1. - cosTheta)) * position.GetY() +
            (rotationAxis.GetY() * rotationAxis.GetZ() * (1. - cosTheta) - rotationAxis.GetX() * sinTheta) * position.GetZ() );
        const double r( (rotationAxis.GetZ() * rotationAxis.GetX() * (1. - cosTheta) - rotationAxis.GetY() * sinTheta) * position.GetX() +
            (rotationAxis.GetZ() * rotationAxis.GetY() * (1. - cosTheta) + rotationAxis.GetX() * sinTheta) * position.GetY() +
            (cosTheta + rotationAxis.GetZ() * rotationAxis.GetZ() * (1. - cosTheta)) * position.GetZ() );

        const double error(iter->GetCellSize() / 3.46);
        const double chiP((p - aP * r - bP) / error);
        const double chiQ((q - aQ * r - bQ) / error);

        chi2_P += chiP * chiP;
        chi2_Q += chiQ * chiQ;

        const CartesianVector difference(iter->GetPosition() - intercept);
        rms += (direction.GetCrossProduct(difference)).GetMagnitudeSquared();

        const float a(direction.GetDotProduct(difference));
        const float l(static_cast<float>(iter->GetPseudoLayer()));
        sumA += a; sumL += l; sumAL += a * l; sumLL += l * l;
    }

    const double nPoints(static_cast<double>(clusterFitPointList.size()));
    const double denominatorL(sumL * sumL - nPoints * sumLL);

    if (0. != denominatorL)
    {
        if (0. > ((sumL * sumA - nPoints * sumAL) / denominatorL))
            direction = direction * -1.f;
    }

    clusterFitResult.SetDirection(direction);
    clusterFitResult.SetIntercept(intercept);
    clusterFitResult.SetChi2(static_cast<float>((chi2_P + chi2_Q) / nPoints));
    clusterFitResult.SetRms(static_cast<float>(std::sqrt(rms / nPoints)));
    clusterFitResult.SetRadialDirectionCosine(dirCosR);
    clusterFitResult.SetSuccessFlag(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetFitResultsClosestApproach(const ClusterHelper::ClusterFitResult &lhs, const ClusterHelper::ClusterFitResult &rhs,
    float &closestApproach)
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
    const PseudoLayer startLayer, const PseudoLayer endLayer)
{
    if (startLayer > endLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    bool distanceFound(false);
    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (PseudoLayer iLayer = startLayer; iLayer <= endLayer; ++iLayer)
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
    const PseudoLayer startLayer, const PseudoLayer endLayer)
{
    if (startLayer > endLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    bool distanceFound(false);
    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (PseudoLayer iLayer = startLayer; iLayer <= endLayer; ++iLayer)
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
        const PseudoLayer pseudoLayer(iterI->first);
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

StatusCode ClusterHelper::GetTrackClusterDistance(const Track *const pTrack, const Cluster *const pCluster, const PseudoLayer maxSearchLayer,
    const float parallelDistanceCut, float &trackClusterDistance)
{
    if ((0 == pCluster->GetNCaloHits()) || (pCluster->GetInnerPseudoLayer() > maxSearchLayer))
        return STATUS_CODE_NOT_FOUND;

    const TrackState &trackState(pTrack->GetTrackStateAtCalorimeter());
    const CartesianVector &trackPosition(trackState.GetPosition());
    const CartesianVector trackDirection(trackState.GetMomentum().GetUnitVector());

    if (trackDirection.GetCosOpeningAngle(pCluster->GetInitialDirection()) < m_minTrackClusterCosAngle)
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

bool ClusterHelper::CanMergeCluster(const Cluster *const pCluster, const float minMipFraction, const float maxAllHitsFitRms)
{
    if (0 == pCluster->GetNCaloHits())
        return false;

    if (!(pCluster->IsPhotonFast()))
        return true;

    if (pCluster->GetMipFraction() - minMipFraction > std::numeric_limits<float>::epsilon())
        return true;

    return (pCluster->GetFitToAllHitsResult().IsFitSuccessful() && (pCluster->GetFitToAllHitsResult().GetRms() < maxAllHitsFitRms));
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer ClusterHelper::GetShowerStartLayer(const Cluster *const pCluster)
{
    const PseudoLayer innerLayer(pCluster->GetInnerPseudoLayer());
    const PseudoLayer outerLayer(pCluster->GetOuterPseudoLayer());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    PseudoLayer currentShowerLayers(0);
    PseudoLayer lastForwardLayer(outerLayer);
    bool foundLastForwardLayer(false);

    // Find two consecutive shower layers
    for (PseudoLayer iLayer = innerLayer; iLayer <= outerLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);
        const bool isLayerPopulated((orderedCaloHitList.end() != iter) && !iter->second->empty());
        float mipFraction(0.f);

        if (isLayerPopulated)
        {
            unsigned int nMipHits(0);

            for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                if ((*hitIter)->IsPossibleMip())
                    nMipHits++;
            }

            mipFraction = static_cast<float>(nMipHits) / static_cast<float>(iter->second->size());
        }

        if (mipFraction - m_showerStartMipFraction > std::numeric_limits<float>::epsilon())
        {
            currentShowerLayers = 0;
        }
        else if (++currentShowerLayers >= m_showerStartNonMipLayers)
        {
            if (isLayerPopulated)
                lastForwardLayer = iLayer;

            foundLastForwardLayer = true;
            break;
        }
    }

    if (!foundLastForwardLayer)
        return outerLayer;

    PseudoLayer currentMipLayers(0);
    PseudoLayer showerStartLayer(lastForwardLayer);

    // Now go backwards to find two consecutive mip layers
    for (PseudoLayer iLayer = lastForwardLayer; iLayer >= innerLayer; --iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);
        const bool isLayerPopulated((orderedCaloHitList.end() != iter) && !iter->second->empty());

        if (!isLayerPopulated)
            continue;

        unsigned int nMipHits(0);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            if ((*hitIter)->IsPossibleMip())
                nMipHits++;
        }

        const float mipFraction(static_cast<float>(nMipHits) / static_cast<float>(iter->second->size()));

        if (mipFraction - m_showerStartMipFraction < std::numeric_limits<float>::epsilon())
        {
            currentMipLayers = 0;
            showerStartLayer = iLayer;
        }
        else if (++currentMipLayers >= m_showerStartNonMipLayers)
        {
            return showerStartLayer;
        }

        // Be careful when decrementing unsigned ints
        if (0 == iLayer)
            break;
    }

    return showerStartLayer;
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

bool ClusterHelper::IsClusterLeavingDetector(const Cluster *const pCluster)
{
    if (!pCluster->ContainsHitInOuterSamplingLayer())
        return false;

    if (pCluster->ContainsHitType(MUON))
        return true;

    // Examine occupancy and energy content of outer layers
    const PseudoLayer outerLayer(pCluster->GetOuterPseudoLayer());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    if (m_leavingNOuterLayersToExamine > outerLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    unsigned int nOccupiedOuterLayers(0);
    float hadronicEnergyInOuterLayers(0.f);

    for (PseudoLayer iLayer = outerLayer - m_leavingNOuterLayersToExamine; iLayer <= outerLayer; ++iLayer)
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

    if ((nOccupiedOuterLayers >= m_leavingMipLikeNOccupiedLayers) ||
        ((nOccupiedOuterLayers == m_leavingShowerLikeNOccupiedLayers) && (hadronicEnergyInOuterLayers > m_leavingShowerLikeEnergyInOuterLayers)))
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ClusterHelper::DoesClusterCrossGapRegion(const Cluster *const pCluster, const PseudoLayer startLayer, const PseudoLayer endLayer,
    const unsigned int nSamplingPoints)
{
    const PseudoLayer fitStartLayer(std::max(startLayer, pCluster->GetInnerPseudoLayer()));
    const PseudoLayer fitEndLayer(std::min(endLayer, pCluster->GetOuterPseudoLayer()));

    if (fitStartLayer > fitEndLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    ClusterFitResult fitResult;
    if (STATUS_CODE_SUCCESS != ClusterHelper::FitLayers(pCluster, fitStartLayer, fitEndLayer, fitResult))
        return false;

    const CartesianVector startLayerCentroid(pCluster->GetCentroid(fitStartLayer));
    const float propagationDistance((pCluster->GetCentroid(fitEndLayer) - startLayerCentroid).GetDotProduct(fitResult.GetDirection()));

    return ClusterHelper::DoesFitCrossGapRegion(fitResult, startLayerCentroid, propagationDistance, nSamplingPoints);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ClusterHelper::DoesFitCrossGapRegion(const ClusterFitResult &clusterFitResult, const CartesianVector &startPosition,
    const float propagationDistance, const unsigned int nSamplingPoints)
{
    const CartesianVector &fitDirection(clusterFitResult.GetDirection());
    const CartesianVector &fitIntercept(clusterFitResult.GetIntercept());

    const float fitStartDistance((startPosition - fitIntercept).GetDotProduct(fitDirection));
    const CartesianVector fitStartPosition(fitIntercept + (fitDirection * fitStartDistance));
    const CartesianVector fitPropagation(fitDirection * propagationDistance);

    for (unsigned int i = 0; i < nSamplingPoints; ++i)
    {
        const CartesianVector fitPosition(fitStartPosition + (fitPropagation * (static_cast<float>(i) / static_cast<float>(nSamplingPoints))));

        if (GeometryHelper::IsInDetectorGapRegion(fitPosition))
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ClusterHelper::ClusterFitPoint::ClusterFitPoint(const CaloHit *const pCaloHit) :
    m_position(pCaloHit->GetPositionVector()),
    m_cellNormalVector(pCaloHit->GetCellNormalVector()),
    m_cellSize(pCaloHit->GetCellLengthScale()),
    m_energy(pCaloHit->GetInputEnergy()),
    m_pseudoLayer(pCaloHit->GetPseudoLayer())
{
    if (m_cellSize < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

ClusterHelper::ClusterFitPoint::ClusterFitPoint(const CartesianVector &position, const CartesianVector &cellNormalVector, const float cellSize,
        const float energy, const PseudoLayer pseudoLayer) :
    m_position(position),
    m_cellNormalVector(cellNormalVector.GetUnitVector()),
    m_cellSize(cellSize),
    m_energy(energy),
    m_pseudoLayer(pseudoLayer)
{
    if (m_cellSize < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::m_minTrackClusterCosAngle = 0.f;
float ClusterHelper::m_showerStartMipFraction = 0.8f;
unsigned int ClusterHelper::m_showerStartNonMipLayers = 2;
unsigned int ClusterHelper::m_leavingNOuterLayersToExamine = 4;
unsigned int ClusterHelper::m_leavingMipLikeNOccupiedLayers = 4;
unsigned int ClusterHelper::m_leavingShowerLikeNOccupiedLayers = 3;
float ClusterHelper::m_leavingShowerLikeEnergyInOuterLayers = 1.f;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::ReadSettings(const TiXmlHandle *const pXmlHandle)
{
    TiXmlElement *pXmlElement(pXmlHandle->FirstChild("ClusterHelper").Element());

    if (NULL != pXmlElement)
    {
        const TiXmlHandle xmlHandle(pXmlElement);

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "MinTrackClusterCosAngle", m_minTrackClusterCosAngle));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "ShowerStartMipFraction", m_showerStartMipFraction));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "ShowerStartNonMipLayers", m_showerStartNonMipLayers));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "LeavingNOuterLayersToExamine", m_leavingNOuterLayersToExamine));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "LeavingMipLikeNOccupiedLayers", m_leavingMipLikeNOccupiedLayers));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "LeavingShowerLikeNOccupiedLayers", m_leavingShowerLikeNOccupiedLayers));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "LeavingShowerLikeEnergyInOuterLayers", m_leavingShowerLikeEnergyInOuterLayers));
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
