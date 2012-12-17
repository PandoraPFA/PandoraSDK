/**
 *  @file   PandoraPFANew/Framework/src/Helpers/CaloHitHelper.cc
 * 
 *  @brief  Implementation of the calo hit helper class.
 * 
 *  $Log: $
 */

#include "Helpers/CaloHitHelper.h"
#include "Helpers/GeometryHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/OrderedCaloHitList.h"

#include <cmath>

namespace pandora
{

float CaloHitHelper::GetDensityWeightContribution(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    float densityWeightContribution = 0.f;
    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const float positionMagnitude(positionVector.GetMagnitude());

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());

        if (positionDifference.GetMagnitudeSquared() > m_caloHitMaxSeparation2)
            continue;

        const CartesianVector crossProduct(positionVector.GetCrossProduct(positionDifference));

        const float r(crossProduct.GetMagnitude() / positionMagnitude);
        float rN(1.);

        for (unsigned int i = 0; i < m_densityWeightPower; ++i)
            rN *= r;

        if (0.f == rN)
            continue;

        densityWeightContribution += (m_densityWeightContribution / rN);
    }

    return densityWeightContribution;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CaloHitHelper::GetSurroundingEnergyContribution(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    float surroundingEnergyContribution = 0.f;
    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const bool isHitInBarrelRegion(pCaloHit->GetDetectorRegion() == BARREL);

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());

        if (positionDifference.GetMagnitudeSquared() > m_caloHitMaxSeparation2)
            continue;

        const float cellLengthScale(pCaloHit->GetCellLengthScale());

        if (isHitInBarrelRegion)
        {
            const float dX(std::fabs(positionDifference.GetX()));
            const float dY(std::fabs(positionDifference.GetY()));
            const float dZ(std::fabs(positionDifference.GetZ()));
            const float dPhi(std::sqrt(dX * dX + dY * dY));

            if ((dZ < (1.5f * cellLengthScale)) && (dPhi < (1.5f * cellLengthScale)))
                surroundingEnergyContribution += (*iter)->GetHadronicEnergy();
        }
        else
        {
            const float dX(std::fabs(positionDifference.GetX()));
            const float dY(std::fabs(positionDifference.GetY()));

            if ((dX < (1.5f * cellLengthScale)) && (dY < (1.5f * cellLengthScale)))
                surroundingEnergyContribution += (*iter)->GetHadronicEnergy();
        }
    }

    return surroundingEnergyContribution;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int CaloHitHelper::IsolationCountNearbyHits(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const float positionMagnitudeSquared(positionVector.GetMagnitudeSquared());
    const float isolationCutDistanceSquared((GeometryHelper::GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
        m_isolationCutDistanceFine2 : m_isolationCutDistanceCoarse2);

    unsigned int nearbyHitsFound = 0;

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());
        const CartesianVector crossProduct(positionVector.GetCrossProduct(positionDifference));

        if (positionDifference.GetMagnitudeSquared() > m_isolationCaloHitMaxSeparation2)
            continue;

        if ((crossProduct.GetMagnitudeSquared() / positionMagnitudeSquared) < isolationCutDistanceSquared)
            ++nearbyHitsFound;
    }

    return nearbyHitsFound;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int CaloHitHelper::MipCountNearbyHits(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    static const float mipNCellsForNearbyHit(m_mipNCellsForNearbyHit + 0.5f);

    unsigned int nearbyHitsFound = 0;
    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const bool isHitInBarrelRegion(pCaloHit->GetDetectorRegion() == BARREL);

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());

        if (positionDifference.GetMagnitudeSquared() > m_caloHitMaxSeparation2)
            continue;

        const float cellLengthScale(pCaloHit->GetCellLengthScale());

        if (isHitInBarrelRegion)
        {
            const float dX(std::fabs(positionDifference.GetX()));
            const float dY(std::fabs(positionDifference.GetY()));
            const float dZ(std::fabs(positionDifference.GetZ()));
            const float dPhi(std::sqrt(dX * dX + dY * dY));

            if ((dZ < (mipNCellsForNearbyHit * cellLengthScale)) && (dPhi < (mipNCellsForNearbyHit * cellLengthScale)))
                ++nearbyHitsFound;
        }
        else
        {
            const float dX(std::fabs(positionDifference.GetX()));
            const float dY(std::fabs(positionDifference.GetY()));

            if ((dX < (mipNCellsForNearbyHit * cellLengthScale)) && (dY < (mipNCellsForNearbyHit * cellLengthScale)))
                ++nearbyHitsFound;
        }
    }

    return nearbyHitsFound;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitHelper::CalculateCaloHitProperties(CaloHit *const pCaloHit, const OrderedCaloHitList *const pOrderedCaloHitList)
{
    // Calculate number of adjacent pseudolayers to examine
    const PseudoLayer pseudoLayer(pCaloHit->GetPseudoLayer());
    const PseudoLayer densityWeightMaxLayer(pseudoLayer + m_densityWeightNLayers);
    const PseudoLayer densityWeightMinLayer((pseudoLayer < m_densityWeightNLayers) ? 0 : pseudoLayer - m_densityWeightNLayers);
    const PseudoLayer isolationMaxLayer(pseudoLayer + m_isolationNLayers);
    const PseudoLayer isolationMinLayer((pseudoLayer < m_isolationNLayers) ? 0 : pseudoLayer - m_isolationNLayers);

    // Initialize variables
    bool isIsolated = true;
    unsigned int isolationNearbyHits = 0;
    float densityWeight(0.f), surroundingEnergy(0.f);

    // Loop over adjacent pseudolayers
    for (PseudoLayer iPseudoLayer = std::min(densityWeightMinLayer, isolationMinLayer),
        iPseudoLayerMax = std::max(densityWeightMaxLayer, isolationMaxLayer); iPseudoLayer <= iPseudoLayerMax; ++iPseudoLayer)
    {
        OrderedCaloHitList::const_iterator adjacentPseudoLayerIter = pOrderedCaloHitList->find(iPseudoLayer);

        if (pOrderedCaloHitList->end() == adjacentPseudoLayerIter)
            continue;

        CaloHitList *pCaloHitList = adjacentPseudoLayerIter->second;

        // IsIsolated flag
        if (isIsolated && (isolationMinLayer <= iPseudoLayer) && (isolationMaxLayer >= iPseudoLayer))
        {
            isolationNearbyHits += CaloHitHelper::IsolationCountNearbyHits(pCaloHit, pCaloHitList);
            isIsolated = isolationNearbyHits < m_isolationMaxNearbyHits;
        }

        // Density weight
        if (m_shouldCalculateDensityWeight && (densityWeightMinLayer <= iPseudoLayer) && (densityWeightMaxLayer >= iPseudoLayer))
        {
            densityWeight += CaloHitHelper::GetDensityWeightContribution(pCaloHit, pCaloHitList);
        }

        // Surrounding energy and possible mip flag
        if (pseudoLayer == iPseudoLayer)
        {
            if (m_shouldCalculateSurroundingEnergy)
            {
                surroundingEnergy += CaloHitHelper::GetSurroundingEnergyContribution(pCaloHit, pCaloHitList);
            }

            if (MUON == pCaloHit->GetHitType())
            {
                pCaloHit->SetPossibleMipFlag(true);
                continue;
            }

            const CartesianVector &positionVector(pCaloHit->GetPositionVector());

            const float x(positionVector.GetX());
            const float y(positionVector.GetY());

            const float angularCorrection( (BARREL == pCaloHit->GetDetectorRegion()) ?
                positionVector.GetMagnitude() / std::sqrt(x * x + y * y) :
                positionVector.GetMagnitude() / std::fabs(positionVector.GetZ()) );

            if ((pCaloHit->GetMipEquivalentEnergy() <= (m_mipLikeMipCut * angularCorrection) || pCaloHit->IsDigital()) &&
                (m_mipMaxNearbyHits >= CaloHitHelper::MipCountNearbyHits(pCaloHit, pCaloHitList)))
            {
                pCaloHit->SetPossibleMipFlag(true);
            }
        }
    }

    if (isIsolated)
        pCaloHit->SetIsolatedFlag(true);

    if (m_shouldCalculateDensityWeight)
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCaloHit->SetDensityWeight(densityWeight));

    if (m_shouldCalculateSurroundingEnergy)
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCaloHit->SetSurroundingEnergy(surroundingEnergy));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CaloHitHelper::m_caloHitMaxSeparation2 = 10000.f;
float CaloHitHelper::m_isolationCaloHitMaxSeparation2 = 1000000.f;
unsigned int CaloHitHelper::m_isolationNLayers = 2;
float CaloHitHelper::m_isolationCutDistanceFine2 = 625.f;
float CaloHitHelper::m_isolationCutDistanceCoarse2 = 40000.f;
unsigned int CaloHitHelper::m_isolationMaxNearbyHits = 2;
bool CaloHitHelper::m_shouldCalculateDensityWeight = true;
float CaloHitHelper::m_densityWeightContribution = 100.f;
unsigned int CaloHitHelper::m_densityWeightPower = 2;
unsigned int CaloHitHelper::m_densityWeightNLayers = 2;
bool CaloHitHelper::m_shouldCalculateSurroundingEnergy = true;
float CaloHitHelper::m_mipLikeMipCut = 5.f;
unsigned int CaloHitHelper::m_mipNCellsForNearbyHit = 2;
unsigned int CaloHitHelper::m_mipMaxNearbyHits = 1;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::ReadSettings(const TiXmlHandle *const pXmlHandle)
{
    TiXmlElement *pXmlElement(pXmlHandle->FirstChild("CaloHitHelper").Element());

    if (NULL != pXmlElement)
    {
        const TiXmlHandle xmlHandle(pXmlElement);

        float caloHitMaxSeparation(std::sqrt(m_caloHitMaxSeparation2));
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "CaloHitMaxSeparation", caloHitMaxSeparation));
        m_caloHitMaxSeparation2 = caloHitMaxSeparation * caloHitMaxSeparation;

        float isolationCaloHitMaxSeparation(std::sqrt(m_isolationCaloHitMaxSeparation2));
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "IsolationCaloHitMaxSeparation", isolationCaloHitMaxSeparation));
        m_isolationCaloHitMaxSeparation2 = isolationCaloHitMaxSeparation * isolationCaloHitMaxSeparation;

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "IsolationNLayers", m_isolationNLayers));

        float isolationCutDistanceFine(std::sqrt(m_isolationCutDistanceFine2));
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "IsolationCutDistanceFine", isolationCutDistanceFine));
        m_isolationCutDistanceFine2 = isolationCutDistanceFine * isolationCutDistanceFine;

        float isolationCutDistanceCoarse(std::sqrt(m_isolationCutDistanceCoarse2));
        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "IsolationCutDistanceCoarse", isolationCutDistanceCoarse));
        m_isolationCutDistanceCoarse2 = isolationCutDistanceCoarse * isolationCutDistanceCoarse;

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "IsolationMaxNearbyHits", m_isolationMaxNearbyHits));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "ShouldCalculateDensityWeight", m_shouldCalculateDensityWeight));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "DensityWeightContribution", m_densityWeightContribution));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "DensityWeightPower", m_densityWeightPower));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "DensityWeightNLayers", m_densityWeightNLayers));

        if (!m_shouldCalculateDensityWeight)
            m_densityWeightNLayers = 0;

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "ShouldCalculateSurroundingEnergy", m_shouldCalculateSurroundingEnergy));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "MipLikeMipCut", m_mipLikeMipCut));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "MipNCellsForNearbyHit", m_mipNCellsForNearbyHit));

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
            "MipMaxNearbyHits", m_mipMaxNearbyHits));
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
