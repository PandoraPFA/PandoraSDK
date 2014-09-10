/**
 *  @file   LCContent/src/LCPlugins/LCBFieldPlugin.cc
 * 
 *  @brief  Implementation of the lc bfield plugin class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCPlugins/LCBFieldPlugin.h"

using namespace pandora;

namespace lc_content
{

LCBFieldPlugin::LCBFieldPlugin(const float innerBField, const float muonBarrelBField, const float muonEndCapBField) :
    m_innerBField(innerBField),
    m_muonBarrelBField(muonBarrelBField),
    m_muonEndCapBField(muonEndCapBField),
    m_muonEndCapInnerZ(std::numeric_limits<float>::max()),
    m_coilMidPointR(std::numeric_limits<float>::max())
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

float LCBFieldPlugin::GetBField(const CartesianVector &positionVector) const
{
    if (std::fabs(positionVector.GetZ()) >= m_muonEndCapInnerZ)
        return m_muonEndCapBField;

    if (std::sqrt(positionVector.GetX() * positionVector.GetX() + positionVector.GetY() * positionVector.GetY()) >= m_coilMidPointR)
        return m_muonBarrelBField;

    return m_innerBField;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LCBFieldPlugin::Initialize()
{
    try
    {
        m_muonEndCapInnerZ = this->GetPandora().GetGeometry()->GetSubDetector(MUON_ENDCAP).GetInnerZCoordinate();
        m_coilMidPointR = (0.5f * (this->GetPandora().GetGeometry()->GetSubDetector(COIL).GetInnerRCoordinate() +
            this->GetPandora().GetGeometry()->GetSubDetector(COIL).GetOuterRCoordinate()));
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "LCBFieldPlugin: Unable to extract Muon EndCap and Coil geometry." << std::endl;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LCBFieldPlugin::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
