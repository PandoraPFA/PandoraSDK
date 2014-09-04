/**
 *    @file PandoraSDK/src/Objects/CaloHit.cc
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

void CaloHit::SetIsolatedFlag(const bool isolatedFlag)
{
    m_isIsolated = isolatedFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::SetPossibleMipFlag(const bool possibleMipFlag)
{
    m_isPossibleMip = possibleMipFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::CaloHit(const PandoraApi::CaloHitBaseParameters &parameters) :
    m_positionVector(parameters.m_positionVector.Get()),
    m_expectedDirection(parameters.m_expectedDirection.Get().GetUnitVector()),
    m_cellNormalVector(parameters.m_cellNormalVector.Get().GetUnitVector()),
    m_cellThickness(parameters.m_cellThickness.Get()),
    m_nCellRadiationLengths(parameters.m_nCellRadiationLengths.Get()),
    m_nCellInteractionLengths(parameters.m_nCellInteractionLengths.Get()),
    m_time(parameters.m_time.Get()),
    m_inputEnergy(parameters.m_inputEnergy.Get()),
    m_mipEquivalentEnergy(parameters.m_mipEquivalentEnergy.Get()),
    m_electromagneticEnergy(parameters.m_electromagneticEnergy.Get()),
    m_hadronicEnergy(parameters.m_hadronicEnergy.Get()),
    m_isDigital(parameters.m_isDigital.Get()),
    m_hitType(parameters.m_hitType.Get()),
    m_hitRegion(parameters.m_hitRegion.Get()),
    m_layer(parameters.m_layer.Get()),
    m_isInOuterSamplingLayer(parameters.m_isInOuterSamplingLayer.Get()),
    m_isPossibleMip(false),
    m_isIsolated(false),
    m_isAvailable(true),
    m_weight(1.f),
    m_cellGeometry(UNKNOWN_CELL_GEOMETRY),
    m_pParentAddress(parameters.m_pParentAddress.Get())
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
    m_hitRegion(pCaloHit->m_hitRegion),
    m_layer(pCaloHit->m_layer),
    m_pseudoLayer(pCaloHit->m_pseudoLayer),
    m_isInOuterSamplingLayer(pCaloHit->m_isInOuterSamplingLayer),
    m_isPossibleMip(pCaloHit->m_isPossibleMip),
    m_isIsolated(pCaloHit->m_isIsolated),
    m_isAvailable(pCaloHit->m_isAvailable),
    m_weight(weight * pCaloHit->m_weight),
    m_cellGeometry(pCaloHit->m_cellGeometry),
    m_mcParticleWeightMap(pCaloHit->m_mcParticleWeightMap),
    m_pParentAddress(pCaloHit->m_pParentAddress)
{
    for (MCParticleWeightMap::iterator iter = m_mcParticleWeightMap.begin(), iterEnd = m_mcParticleWeightMap.end(); iter != iterEnd; ++iter)
        iter->second = iter->second * weight;
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::~CaloHit()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::SetPseudoLayer(const unsigned int pseudoLayer)
{
    if (!(m_pseudoLayer = pseudoLayer))
        return STATUS_CODE_NOT_INITIALIZED;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::SetMCParticleWeightMap(const MCParticleWeightMap &mcParticleWeightMap)
{
    m_mcParticleWeightMap = mcParticleWeightMap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::RemoveMCParticles()
{
    m_mcParticleWeightMap.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

RectangularCaloHit::RectangularCaloHit(const PandoraApi::RectangularCaloHit::Parameters &parameters) :
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
    CartesianVector dirU((BARREL == this->GetHitRegion()) ? CartesianVector(0.f, 0.f, 1.f) : CartesianVector(0.f, 1.f, 0.f) );
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

PointingCaloHit::PointingCaloHit(const PandoraApi::PointingCaloHit::Parameters &parameters) :
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

    if (BARREL == this->GetHitRegion())
    {
        if (std::fabs(sinThetaMin) > std::numeric_limits<float>::epsilon())
            thetaMinRScale = std::fabs(sinTheta / sinThetaMin);

        if (std::fabs(sinThetaMax) > std::numeric_limits<float>::epsilon())
            thetaMaxRScale = std::fabs(sinTheta / sinThetaMax);
    }
    else
    {
        if (std::fabs(cosThetaMin) > std::numeric_limits<float>::epsilon())
            thetaMinRScale = std::fabs(cosTheta / cosThetaMin);

        if (std::fabs(cosThetaMax) > std::numeric_limits<float>::epsilon())
            thetaMaxRScale = std::fabs(cosTheta / cosThetaMax);
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
