/**
 *    @file PandoraPFANew/Framework/src/Objects/CaloHit.cc
 * 
 *    @brief Implementation of the calo hit class.
 * 
 *  $Log: $
 */

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

#include <cmath>

namespace pandora
{

CaloHit::CaloHit(const PandoraApi::CaloHitBaseParameters &caloHitParameters) :
    m_positionVector(caloHitParameters.m_positionVector.Get()),
    m_expectedDirection(caloHitParameters.m_expectedDirection.Get().GetUnitVector()),
    m_cellNormalVector(caloHitParameters.m_cellNormalVector.Get().GetUnitVector()),
    m_cellThickness(caloHitParameters.m_cellThickness.Get()),
    m_nCellRadiationLengths(caloHitParameters.m_nCellRadiationLengths.Get()),
    m_nCellInteractionLengths(caloHitParameters.m_nCellInteractionLengths.Get()),
    m_time(caloHitParameters.m_time.Get()),
    m_inputEnergy(caloHitParameters.m_inputEnergy.Get()),
    m_mipEquivalentEnergy(caloHitParameters.m_mipEquivalentEnergy.Get()),
    m_electromagneticEnergy(caloHitParameters.m_electromagneticEnergy.Get()),
    m_hadronicEnergy(caloHitParameters.m_hadronicEnergy.Get()),
    m_isDigital(caloHitParameters.m_isDigital.Get()),
    m_hitType(caloHitParameters.m_hitType.Get()),
    m_detectorRegion(caloHitParameters.m_detectorRegion.Get()),
    m_layer(caloHitParameters.m_layer.Get()),
    m_isInOuterSamplingLayer(caloHitParameters.m_isInOuterSamplingLayer.Get()),
    m_isPossibleMip(false),
    m_isIsolated(false),
    m_isAvailable(true),
    m_weight(1.f),
    m_cellGeometry(UNKNOWN_CELL_GEOMETRY),
    m_pMCParticle(NULL),
    m_pParentAddress(caloHitParameters.m_pParentAddress.Get())
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::CaloHit(CaloHit *pCaloHit, const float weight) :
    m_positionVector(pCaloHit->m_positionVector),
    m_expectedDirection(pCaloHit->m_expectedDirection),
    m_cellNormalVector(pCaloHit->m_cellNormalVector),
    m_cellThickness(pCaloHit->m_cellThickness),
    m_nCellRadiationLengths(pCaloHit->m_nCellRadiationLengths),
    m_nCellInteractionLengths(pCaloHit->m_nCellInteractionLengths),
    m_time(pCaloHit->m_time),
    m_inputEnergy(weight * pCaloHit->m_inputEnergy),
    m_mipEquivalentEnergy(weight * pCaloHit->m_mipEquivalentEnergy),
    m_electromagneticEnergy(weight * pCaloHit->m_electromagneticEnergy),
    m_hadronicEnergy(weight * pCaloHit->m_hadronicEnergy),
    m_isDigital(pCaloHit->m_isDigital),
    m_hitType(pCaloHit->m_hitType),
    m_detectorRegion(pCaloHit->m_detectorRegion),
    m_layer(pCaloHit->m_layer),
    m_pseudoLayer(pCaloHit->m_pseudoLayer),
    m_isInOuterSamplingLayer(pCaloHit->m_isInOuterSamplingLayer),
    m_densityWeight(pCaloHit->m_densityWeight),
    m_surroundingEnergy(pCaloHit->m_surroundingEnergy),
    m_isPossibleMip(pCaloHit->m_isPossibleMip),
    m_isIsolated(pCaloHit->m_isIsolated),
    m_isAvailable(pCaloHit->m_isAvailable),
    m_weight(weight * pCaloHit->m_weight),
    m_cellGeometry(pCaloHit->m_cellGeometry),
    m_pMCParticle(pCaloHit->m_pMCParticle),
    m_pParentAddress(pCaloHit->m_pParentAddress)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::~CaloHit()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::SetPseudoLayer(PseudoLayer pseudoLayer)
{
    if (!(m_pseudoLayer = pseudoLayer))
        return STATUS_CODE_NOT_INITIALIZED;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::SetDensityWeight(float densityWeight)
{
    if (!(m_densityWeight = densityWeight))
        return STATUS_CODE_NOT_INITIALIZED;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::SetSurroundingEnergy(float surroundingEnergy)
{
    if (!(m_surroundingEnergy = surroundingEnergy))
        return STATUS_CODE_NOT_INITIALIZED;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::SetIsolatedFlag(bool isolatedFlag)
{
    m_isIsolated = isolatedFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::SetPossibleMipFlag(bool possibleMipFlag)
{
    m_isPossibleMip = possibleMipFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::SetMCParticle(MCParticle *const pMCParticle)
{
    if (NULL == pMCParticle)
        return STATUS_CODE_FAILURE;

    m_pMCParticle = pMCParticle;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::RemoveMCParticle()
{
    m_pMCParticle = NULL;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

RectangularCaloHit::RectangularCaloHit(const PandoraApi::RectangularCaloHitParameters &parameters) :
    CaloHit(parameters),
    m_cellSizeU(parameters.m_cellSizeU.Get()),
    m_cellSizeV(parameters.m_cellSizeV.Get()),
    m_cellLengthScale(std::sqrt(m_cellSizeU * m_cellSizeV))
{
     m_cellGeometry = RECTANGULAR;
}

//------------------------------------------------------------------------------------------------------------------------------------------

RectangularCaloHit::RectangularCaloHit(RectangularCaloHit *pCaloHit, const float weight) :
    CaloHit(pCaloHit, weight),
    m_cellSizeU(pCaloHit->m_cellSizeU),
    m_cellSizeV(pCaloHit->m_cellSizeV),
    m_cellLengthScale(pCaloHit->m_cellLengthScale)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void RectangularCaloHit::GetCellCorners(CartesianPointList &cartesianPointList) const
{
    const CartesianVector &position(this->GetPositionVector());

    CartesianVector normal(this->GetCellNormalVector());
    CartesianVector dirU((ENDCAP == this->GetDetectorRegion()) ? CartesianVector(0.f, 1.f, 0.f) : CartesianVector(0.f, 0.f, 1.f));
    CartesianVector dirV(normal.GetCrossProduct(dirU));

    dirU *= (this->GetCellSizeU() / 2.);
    dirV *= (this->GetCellSizeV() / 2.);
    normal *= (this->GetCellThickness() / 2.);

    cartesianPointList.push_back(CartesianVector(position - dirU - dirV - normal));
    cartesianPointList.push_back(CartesianVector(position + dirU - dirV - normal));
    cartesianPointList.push_back(CartesianVector(position + dirU + dirV - normal));
    cartesianPointList.push_back(CartesianVector(position - dirU + dirV - normal));

    cartesianPointList.push_back(CartesianVector(position - dirU - dirV + normal));
    cartesianPointList.push_back(CartesianVector(position + dirU - dirV + normal));
    cartesianPointList.push_back(CartesianVector(position + dirU + dirV + normal));
    cartesianPointList.push_back(CartesianVector(position - dirU + dirV + normal));
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

PointingCaloHit::PointingCaloHit(const PandoraApi::PointingCaloHitParameters &parameters) :
    CaloHit(parameters),
    m_cellSizeEta(parameters.m_cellSizeEta.Get()),
    m_cellSizePhi(parameters.m_cellSizePhi.Get()),
    m_cellLengthScale(this->CalculateCellLengthScale())
{
    m_cellGeometry = POINTING;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PointingCaloHit::PointingCaloHit(PointingCaloHit *pCaloHit, const float weight) :
    CaloHit(pCaloHit, weight),
    m_cellSizeEta(pCaloHit->m_cellSizeEta),
    m_cellSizePhi(pCaloHit->m_cellSizePhi),
    m_cellLengthScale(pCaloHit->m_cellLengthScale)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

float PointingCaloHit::CalculateCellLengthScale() const
{
    float radius(0.f), phi(0.f), theta(0.f);
    this->GetPositionVector().GetSphericalCoordinates(radius, phi, theta);
    const float centralEta(-1. * std::log(std::tan(theta / 2.)));

    const float etaMin(centralEta - this->GetCellSizeEta() / 2.), etaMax(centralEta + this->GetCellSizeEta() / 2.);
    const float thetaMin(2. * std::atan(std::exp(-1. * etaMin))), thetaMax(2. * std::atan(std::exp(-1. * etaMax)));

    return std::sqrt(std::fabs(radius * this->GetCellSizePhi() * radius * (thetaMax - thetaMin)));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PointingCaloHit::GetCellCorners(CartesianPointList &cartesianPointList) const
{
    float radius(0.f), phi(0.f), theta(0.f);
    this->GetPositionVector().GetSphericalCoordinates(radius, phi, theta);
    const float centralEta(-1. * std::log(std::tan(theta / 2.)));

    const float rMin(radius - this->GetCellThickness() / 2.), rMax(radius + this->GetCellThickness() / 2.);
    const float phiMin(phi - this->GetCellSizePhi() / 2.), phiMax(phi + this->GetCellSizePhi() / 2.);
    const float etaMin(centralEta - this->GetCellSizeEta() / 2.), etaMax(centralEta + this->GetCellSizeEta() / 2.);
    const float thetaMin(2. * std::atan(std::exp(-1. * etaMin))), thetaMax(2. * std::atan(std::exp(-1. * etaMax)));

    const float sinTheta(std::sin(theta)), cosTheta(std::cos(theta));
    const float sinThetaMin(std::sin(thetaMin)), cosThetaMin(std::cos(thetaMin)), sinPhiMin(std::sin(phiMin)), cosPhiMin(std::cos(phiMin));
    const float sinThetaMax(std::sin(thetaMax)), cosThetaMax(std::cos(thetaMax)), sinPhiMax(std::sin(phiMax)), cosPhiMax(std::cos(phiMax));

    float thetaMinRScale(1.f), thetaMaxRScale(1.f);

    if (ENDCAP == this->GetDetectorRegion())
    {
        if (std::fabs(cosThetaMin) > std::numeric_limits<float>::epsilon())
            thetaMinRScale = std::fabs(cosTheta / cosThetaMin);

        if (std::fabs(cosThetaMax) > std::numeric_limits<float>::epsilon())
            thetaMaxRScale = std::fabs(cosTheta / cosThetaMax);
    }
    else
    {
        if (std::fabs(sinThetaMin) > std::numeric_limits<float>::epsilon())
            thetaMinRScale = std::fabs(sinTheta / sinThetaMin);

        if (std::fabs(sinThetaMax) > std::numeric_limits<float>::epsilon())
            thetaMaxRScale = std::fabs(sinTheta / sinThetaMax);
    }

    const float rMinAtThetaMin(thetaMinRScale * rMin), rMinAtThetaMax(thetaMaxRScale * rMin);
    const float rMaxAtThetaMin(thetaMinRScale * rMax), rMaxAtThetaMax(thetaMaxRScale * rMax);

    cartesianPointList.push_back(CartesianVector(rMinAtThetaMin * sinThetaMin * cosPhiMin, rMinAtThetaMin * sinThetaMin * sinPhiMin, rMinAtThetaMin * cosThetaMin));
    cartesianPointList.push_back(CartesianVector(rMinAtThetaMax * sinThetaMax * cosPhiMin, rMinAtThetaMax * sinThetaMax * sinPhiMin, rMinAtThetaMax * cosThetaMax));
    cartesianPointList.push_back(CartesianVector(rMinAtThetaMax * sinThetaMax * cosPhiMax, rMinAtThetaMax * sinThetaMax * sinPhiMax, rMinAtThetaMax * cosThetaMax));
    cartesianPointList.push_back(CartesianVector(rMinAtThetaMin * sinThetaMin * cosPhiMax, rMinAtThetaMin * sinThetaMin * sinPhiMax, rMinAtThetaMin * cosThetaMin));

    cartesianPointList.push_back(CartesianVector(rMaxAtThetaMin * sinThetaMin * cosPhiMin, rMaxAtThetaMin * sinThetaMin * sinPhiMin, rMaxAtThetaMin * cosThetaMin));
    cartesianPointList.push_back(CartesianVector(rMaxAtThetaMax * sinThetaMax * cosPhiMin, rMaxAtThetaMax * sinThetaMax * sinPhiMin, rMaxAtThetaMax * cosThetaMax));
    cartesianPointList.push_back(CartesianVector(rMaxAtThetaMax * sinThetaMax * cosPhiMax, rMaxAtThetaMax * sinThetaMax * sinPhiMax, rMaxAtThetaMax * cosThetaMax));
    cartesianPointList.push_back(CartesianVector(rMaxAtThetaMin * sinThetaMin * cosPhiMax, rMaxAtThetaMin * sinThetaMin * sinPhiMax, rMaxAtThetaMin * cosThetaMin));
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &stream, const CaloHit &caloHit)
{
    stream  << " CaloHit: " << std::endl
            << " position " << caloHit.GetPositionVector()
            << " energy   " << caloHit.GetInputEnergy() << std::endl;

    return stream;
}

} // namespace pandora
