/**
 *    @file PandoraSDK/src/Objects/CaloHit.cc
 * 
 *    @brief Implementation of the calo hit class.
 * 
 *  $Log: $
 */

#include "Objects/CaloHit.h"

#include <cmath>

namespace pandora
{

void CaloHit::GetCellCorners(CartesianPointVector &cartesianPointVector) const
{
    if (RECTANGULAR == this->GetCellGeometry())
    {
        this->GetRectangularCellCorners(cartesianPointVector);
    }
    else if (POINTING == this->GetCellGeometry())
    {
        this->GetPointingCellCorners(cartesianPointVector);
    }
    else
    {
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHit::operator< (const CaloHit &rhs) const
{
    const CartesianVector deltaPosition(rhs.GetPositionVector() - this->GetPositionVector());

    if (std::fabs(deltaPosition.GetZ()) > std::numeric_limits<float>::epsilon())
        return (deltaPosition.GetZ() > std::numeric_limits<float>::epsilon());

    if (std::fabs(deltaPosition.GetX()) > std::numeric_limits<float>::epsilon())
        return (deltaPosition.GetX() > std::numeric_limits<float>::epsilon());

    if (std::fabs(deltaPosition.GetY()) > std::numeric_limits<float>::epsilon())
        return (deltaPosition.GetY() > std::numeric_limits<float>::epsilon());

    return (this->GetInputEnergy() > rhs.GetInputEnergy());
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::CaloHit(const object_creation::CaloHit::Parameters &parameters) :
    m_positionVector(parameters.m_positionVector.Get()),
    m_x0(0.f),
    m_expectedDirection(parameters.m_expectedDirection.Get().GetUnitVector()),
    m_cellNormalVector(parameters.m_cellNormalVector.Get().GetUnitVector()),
    m_cellGeometry(parameters.m_cellGeometry.Get()),
    m_cellSize0(parameters.m_cellSize0.Get()),
    m_cellSize1(parameters.m_cellSize1.Get()),
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
    m_cellLengthScale(0.f),
    m_isPossibleMip(false),
    m_isIsolated(false),
    m_isAvailable(true),
    m_weight(1.f),
    m_pParentAddress(parameters.m_pParentAddress.Get()),
    m_propertiesMap()
{
    m_cellLengthScale = this->CalculateCellLengthScale();
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::CaloHit(const object_creation::CaloHitFragment::Parameters &parameters) :
    m_positionVector(parameters.m_pOriginalCaloHit->m_positionVector),
    m_x0(parameters.m_pOriginalCaloHit->m_x0),
    m_expectedDirection(parameters.m_pOriginalCaloHit->m_expectedDirection),
    m_cellNormalVector(parameters.m_pOriginalCaloHit->m_cellNormalVector),
    m_cellGeometry(parameters.m_pOriginalCaloHit->m_cellGeometry),
    m_cellSize0(parameters.m_pOriginalCaloHit->m_cellSize0),
    m_cellSize1(parameters.m_pOriginalCaloHit->m_cellSize1),
    m_cellThickness(parameters.m_pOriginalCaloHit->m_cellThickness),
    m_nCellRadiationLengths(parameters.m_pOriginalCaloHit->m_nCellRadiationLengths),
    m_nCellInteractionLengths(parameters.m_pOriginalCaloHit->m_nCellInteractionLengths),
    m_time(parameters.m_pOriginalCaloHit->m_time),
    m_inputEnergy(parameters.m_weight.Get() * parameters.m_pOriginalCaloHit->m_inputEnergy),
    m_mipEquivalentEnergy(parameters.m_weight.Get() * parameters.m_pOriginalCaloHit->m_mipEquivalentEnergy),
    m_electromagneticEnergy(parameters.m_weight.Get() * parameters.m_pOriginalCaloHit->m_electromagneticEnergy),
    m_hadronicEnergy(parameters.m_weight.Get() * parameters.m_pOriginalCaloHit->m_hadronicEnergy),
    m_isDigital(parameters.m_pOriginalCaloHit->m_isDigital),
    m_hitType(parameters.m_pOriginalCaloHit->m_hitType),
    m_hitRegion(parameters.m_pOriginalCaloHit->m_hitRegion),
    m_layer(parameters.m_pOriginalCaloHit->m_layer),
    m_pseudoLayer(parameters.m_pOriginalCaloHit->m_pseudoLayer),
    m_isInOuterSamplingLayer(parameters.m_pOriginalCaloHit->m_isInOuterSamplingLayer),
    m_cellLengthScale(parameters.m_pOriginalCaloHit->m_cellLengthScale),
    m_isPossibleMip(parameters.m_pOriginalCaloHit->m_isPossibleMip),
    m_isIsolated(parameters.m_pOriginalCaloHit->m_isIsolated),
    m_isAvailable(parameters.m_pOriginalCaloHit->m_isAvailable),
    m_weight(parameters.m_weight.Get() * parameters.m_pOriginalCaloHit->m_weight),
    m_mcParticleWeightMap(parameters.m_pOriginalCaloHit->m_mcParticleWeightMap),
    m_pParentAddress(parameters.m_pOriginalCaloHit->m_pParentAddress),
    m_propertiesMap(parameters.m_pOriginalCaloHit->m_propertiesMap)
{
    for (MCParticleWeightMap::value_type &mapEntry : m_mcParticleWeightMap)
        mapEntry.second = mapEntry.second * parameters.m_weight.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::~CaloHit()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::AlterMetadata(const object_creation::CaloHit::Metadata &metadata)
{
    if (!metadata.m_propertiesToAdd.empty() || !metadata.m_propertiesToRemove.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->UpdatePropertiesMap(metadata));
    }

    if (metadata.m_x0.IsInitialized())
    {
        const float oldX0(m_x0);
        m_x0 = metadata.m_x0.Get();
        m_positionVector += CartesianVector(m_x0 - oldX0, 0.f, 0.f);
    }

    if (metadata.m_isPossibleMip.IsInitialized())
        m_isPossibleMip = metadata.m_isPossibleMip.Get();

    if (metadata.m_isIsolated.IsInitialized())
        m_isIsolated = metadata.m_isIsolated.Get();

    return STATUS_CODE_SUCCESS;
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

float CaloHit::CalculateCellLengthScale() const
{
    if (RECTANGULAR == this->GetCellGeometry())
    {
        return std::sqrt(this->GetCellSize0() * this->GetCellSize1());
    }
    else if (POINTING == this->GetCellGeometry())
    {
        float radius(0.f), phi(0.f), theta(0.f);
        this->GetPositionVector().GetSphericalCoordinates(radius, phi, theta);
        const float centralEta(-1.f * std::log(std::tan(theta / 2.f)));

        const float etaMin(centralEta - this->GetCellSize0() / 2.f), etaMax(centralEta + this->GetCellSize0() / 2.f);
        const float thetaMin(2.f * std::atan(std::exp(-1.f * etaMin))), thetaMax(2.f * std::atan(std::exp(-1.f * etaMax)));

        return std::sqrt(std::fabs(radius * this->GetCellSize1() * radius * (thetaMax - thetaMin)));
    }
    else
    {
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::GetRectangularCellCorners(CartesianPointVector &cartesianPointVector) const
{
    const CartesianVector &position(this->GetPositionVector());

    CartesianVector normal(this->GetCellNormalVector());
    CartesianVector dirU((BARREL == this->GetHitRegion()) ? CartesianVector(0.f, 0.f, 1.f) : CartesianVector(0.f, 1.f, 0.f) );
    CartesianVector dirV(normal.GetCrossProduct(dirU));

    dirU *= (this->GetCellSize0() / 2.f);
    dirV *= (this->GetCellSize1() / 2.f);
    normal *= (this->GetCellThickness() / 2.f);

    cartesianPointVector.push_back(CartesianVector(position - dirU - dirV - normal));
    cartesianPointVector.push_back(CartesianVector(position + dirU - dirV - normal));
    cartesianPointVector.push_back(CartesianVector(position + dirU + dirV - normal));
    cartesianPointVector.push_back(CartesianVector(position - dirU + dirV - normal));

    cartesianPointVector.push_back(CartesianVector(position - dirU - dirV + normal));
    cartesianPointVector.push_back(CartesianVector(position + dirU - dirV + normal));
    cartesianPointVector.push_back(CartesianVector(position + dirU + dirV + normal));
    cartesianPointVector.push_back(CartesianVector(position - dirU + dirV + normal));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::GetPointingCellCorners(CartesianPointVector &cartesianPointVector) const
{
    float radius(0.f), phi(0.f), theta(0.f);
    this->GetPositionVector().GetSphericalCoordinates(radius, phi, theta);
    const float centralEta(-1.f * std::log(std::tan(theta / 2.f)));

    const float rMin(radius - this->GetCellThickness() / 2.f), rMax(radius + this->GetCellThickness() / 2.f);
    const float phiMin(phi - this->GetCellSize1() / 2.f), phiMax(phi + this->GetCellSize1() / 2.f);
    const float etaMin(centralEta - this->GetCellSize0() / 2.f), etaMax(centralEta + this->GetCellSize0() / 2.f);
    const float thetaMin(2.f * std::atan(std::exp(-1.f * etaMin))), thetaMax(2.f * std::atan(std::exp(-1.f * etaMax)));

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

    cartesianPointVector.push_back(CartesianVector(rMinAtThetaMin * sinThetaMin * cosPhiMin, rMinAtThetaMin * sinThetaMin * sinPhiMin, rMinAtThetaMin * cosThetaMin));
    cartesianPointVector.push_back(CartesianVector(rMinAtThetaMax * sinThetaMax * cosPhiMin, rMinAtThetaMax * sinThetaMax * sinPhiMin, rMinAtThetaMax * cosThetaMax));
    cartesianPointVector.push_back(CartesianVector(rMinAtThetaMax * sinThetaMax * cosPhiMax, rMinAtThetaMax * sinThetaMax * sinPhiMax, rMinAtThetaMax * cosThetaMax));
    cartesianPointVector.push_back(CartesianVector(rMinAtThetaMin * sinThetaMin * cosPhiMax, rMinAtThetaMin * sinThetaMin * sinPhiMax, rMinAtThetaMin * cosThetaMin));

    cartesianPointVector.push_back(CartesianVector(rMaxAtThetaMin * sinThetaMin * cosPhiMin, rMaxAtThetaMin * sinThetaMin * sinPhiMin, rMaxAtThetaMin * cosThetaMin));
    cartesianPointVector.push_back(CartesianVector(rMaxAtThetaMax * sinThetaMax * cosPhiMin, rMaxAtThetaMax * sinThetaMax * sinPhiMin, rMaxAtThetaMax * cosThetaMax));
    cartesianPointVector.push_back(CartesianVector(rMaxAtThetaMax * sinThetaMax * cosPhiMax, rMaxAtThetaMax * sinThetaMax * sinPhiMax, rMaxAtThetaMax * cosThetaMax));
    cartesianPointVector.push_back(CartesianVector(rMaxAtThetaMin * sinThetaMin * cosPhiMax, rMaxAtThetaMin * sinThetaMin * sinPhiMax, rMaxAtThetaMin * cosThetaMin));
}

//------------------------------------------------------------------------------------------------------------------------------------------
StatusCode CaloHit::UpdatePropertiesMap(const object_creation::CaloHit::Metadata &metadata)
{
    for (const std::string &propertyName : metadata.m_propertiesToRemove)
    {
        if (metadata.m_propertiesToAdd.count(propertyName))
            return STATUS_CODE_INVALID_PARAMETER;

        if(!m_propertiesMap.count(propertyName))
            return STATUS_CODE_NOT_FOUND;
    }

    for (const std::string &propertyName : metadata.m_propertiesToRemove)
        m_propertiesMap.erase(propertyName);

    for (const PropertiesMap::value_type &entryToAdd : metadata.m_propertiesToAdd)
        m_propertiesMap[entryToAdd.first] = entryToAdd.second;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
