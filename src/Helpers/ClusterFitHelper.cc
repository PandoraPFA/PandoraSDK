/**
 *  @file   PandoraSDK/src/Helpers/ClusterFitHelper.cc
 * 
 *  @brief  Implementation of the cluster fit helper class.
 * 
 *  $Log: $
 */

#include "Helpers/ClusterFitHelper.h"

#include "Objects/Cluster.h"

#include <cmath>
#include <limits>

namespace pandora
{

StatusCode ClusterFitHelper::FitStart(const Cluster *const pCluster, const unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult)
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

StatusCode ClusterFitHelper::FitEnd(const Cluster *const pCluster, const unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult)
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

StatusCode ClusterFitHelper::FitFullCluster(const Cluster *const pCluster, ClusterFitResult &clusterFitResult)
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

StatusCode ClusterFitHelper::FitLayers(const Cluster *const pCluster, const unsigned int startLayer, const unsigned int endLayer,
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
        const unsigned int pseudoLayer(iter->first);

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

StatusCode ClusterFitHelper::FitLayerCentroids(const Cluster *const pCluster, const unsigned int startLayer, const unsigned int endLayer,
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
            const unsigned int pseudoLayer(iter->first);

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
        std::cout << "ClusterFitHelper: linear fit to cluster failed. " << std::endl;
        clusterFitResult.SetSuccessFlag(false);
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterFitHelper::FitPoints(const ClusterFitPointList &clusterFitPointList, ClusterFitResult &clusterFitResult)
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
        std::cout << "ClusterFitHelper: linear fit to cluster failed. " << std::endl;
        clusterFitResult.SetSuccessFlag(false);
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterFitHelper::PerformLinearFit(const ClusterFitPointList &clusterFitPointList, const CartesianVector &centralPosition,
    const CartesianVector &centralDirection, ClusterFitResult &clusterFitResult)
{
    // Extract the data
    double sumP(0.), sumQ(0.), sumR(0.), sumWeights(0.);
    double sumPR(0.), sumQR(0.), sumRR(0.);

    const CartesianVector chosenAxis(0.f, 0.f, 1.f);
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

    if (std::fabs(denominatorR) < std::numeric_limits<double>::epsilon())
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
//------------------------------------------------------------------------------------------------------------------------------------------

ClusterFitPoint::ClusterFitPoint(const CaloHit *const pCaloHit) :
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

ClusterFitPoint::ClusterFitPoint(const CartesianVector &position, const CartesianVector &cellNormalVector, const float cellSize,
        const float energy, const unsigned int pseudoLayer) :
    m_position(position),
    m_cellNormalVector(cellNormalVector.GetUnitVector()),
    m_cellSize(cellSize),
    m_energy(energy),
    m_pseudoLayer(pseudoLayer)
{
    if (m_cellSize < std::numeric_limits<float>::epsilon())
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

} // namespace pandora
