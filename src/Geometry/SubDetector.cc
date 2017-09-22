/**
 *  @file   PandoraSDK/src/Geometry/SubDetector.cc
 * 
 *  @brief  Implementation of the sub detector class.
 * 
 *  $Log: $
 */

#include "Geometry/SubDetector.h"

namespace pandora
{

SubDetector::SubDetector(const object_creation::Geometry::SubDetector::Parameters &inputParameters) :
    m_subDetectorName(inputParameters.m_subDetectorName.Get()),
    m_subDetectorType(inputParameters.m_subDetectorType.Get()),
    m_innerRCoordinate(inputParameters.m_innerRCoordinate.Get()),
    m_innerZCoordinate(inputParameters.m_innerZCoordinate.Get()),
    m_innerPhiCoordinate(inputParameters.m_innerPhiCoordinate.Get()),
    m_innerSymmetryOrder(inputParameters.m_innerSymmetryOrder.Get()),
    m_outerRCoordinate(inputParameters.m_outerRCoordinate.Get()),
    m_outerZCoordinate(inputParameters.m_outerZCoordinate.Get()),
    m_outerPhiCoordinate(inputParameters.m_outerPhiCoordinate.Get()),
    m_outerSymmetryOrder(inputParameters.m_outerSymmetryOrder.Get()),
    m_isMirroredInZ(inputParameters.m_isMirroredInZ.Get()),
    m_nLayers(inputParameters.m_nLayers.Get())
{
    if ((m_innerRCoordinate < 0.f) || (m_outerRCoordinate < 0.f) || (m_isMirroredInZ && ((m_innerZCoordinate < 0.f) || (m_outerZCoordinate < 0.f))))
    {
        std::cout << "GeometryPlugin: Invalid coordinate specified for " << m_subDetectorName << std::endl;
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    if (m_nLayers != inputParameters.m_layerParametersVector.size())
    {
        std::cout << "GeometryPlugin: Invalid number of entries in layer parameters list for " << m_subDetectorName << std::endl;
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    for (const object_creation::Geometry::LayerParameters &layerParameters : inputParameters.m_layerParametersVector)
    {
        SubDetectorLayer subDetectorLayer(layerParameters.m_closestDistanceToIp.Get(), layerParameters.m_nRadiationLengths.Get(), layerParameters.m_nInteractionLengths.Get());
        m_subDetectorLayerVector.push_back(subDetectorLayer);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

SubDetector::~SubDetector()
{
}

} // namespace pandora
