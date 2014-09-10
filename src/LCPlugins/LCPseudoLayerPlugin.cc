/**
 *  @file   LCContent/src/LCPlugins/LCPseudoLayerPlugin.cc
 * 
 *  @brief  Implementation of the lc pseudo layer plugin class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "LCPlugins/LCPseudoLayerPlugin.h"

using namespace pandora;

namespace lc_content
{

LCPseudoLayerPlugin::LCPseudoLayerPlugin() :
    m_barrelInnerR(0.f),
    m_endCapInnerZ(0.f),
    m_barrelInnerRMuon(0.f),
    m_endCapInnerZMuon(0.f),
    m_rCorrection(0.f),
    m_zCorrection(0.f),
    m_rCorrectionMuon(0.f),
    m_zCorrectionMuon(0.f),
    m_barrelEdgeR(0.f),
    m_endCapEdgeZ(0.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LCPseudoLayerPlugin::Initialize()
{
    try
    {
        this->StoreLayerPositions();
        this->StoreDetectorOuterEdge();
        this->StorePolygonAngles();
        this->StoreOverlapCorrectionDetails();
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "LCPseudoLayerPlugin: Incomplete geometry - consider using a different PseudoLayerCalculator." << std::endl;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int LCPseudoLayerPlugin::GetPseudoLayer(const CartesianVector &positionVector) const
{
    const float zCoordinate(std::fabs(positionVector.GetZ()));

    if (zCoordinate > m_endCapEdgeZ)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    const float rCoordinate(this->GetMaximumRadius(m_eCalBarrelAngleVector, positionVector.GetX(), positionVector.GetY()));
    const float rCoordinateMuon(this->GetMaximumRadius(m_muonBarrelAngleVector, positionVector.GetX(), positionVector.GetY()));

    if ((rCoordinateMuon > m_barrelEdgeR) || (rCoordinate > m_barrelEdgeR))
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    unsigned int pseudoLayer;

    if ((zCoordinate < m_endCapInnerZMuon) && (rCoordinateMuon < m_barrelInnerRMuon))
    {
        const StatusCode statusCode(this->GetPseudoLayer(rCoordinate, zCoordinate, m_rCorrection, m_zCorrection, m_barrelInnerR,
            m_endCapInnerZ, pseudoLayer));

        if (STATUS_CODE_SUCCESS != statusCode)
            throw StatusCodeException(statusCode);
    }
    else
    {
        const StatusCode statusCode(this->GetPseudoLayer(rCoordinateMuon, zCoordinate, m_rCorrectionMuon, m_zCorrectionMuon,
            m_barrelInnerRMuon, m_endCapInnerZMuon, pseudoLayer));

        if (STATUS_CODE_SUCCESS != statusCode)
            throw StatusCodeException(statusCode);
    }

    // Reserve a pseudo layer for track projections, etc.
    return (1 + pseudoLayer);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LCPseudoLayerPlugin::GetPseudoLayer(const float rCoordinate, const float zCoordinate, const float rCorrection,
    const float zCorrection, const float barrelInnerR, const float endCapInnerZ, unsigned int &pseudoLayer) const
{
    if (zCoordinate < endCapInnerZ)
    {
        return this->FindMatchingLayer(rCoordinate, m_barrelLayerPositions, pseudoLayer);
    }
    else if (rCoordinate < barrelInnerR)
    {
        return this->FindMatchingLayer(zCoordinate, m_endCapLayerPositions, pseudoLayer);
    }
    else
    {
        unsigned int bestBarrelLayer(0);
        const StatusCode barrelStatusCode(this->FindMatchingLayer(rCoordinate - rCorrection, m_barrelLayerPositions, bestBarrelLayer));

        unsigned int bestEndCapLayer(0);
        const StatusCode endCapStatusCode(this->FindMatchingLayer(zCoordinate - zCorrection, m_endCapLayerPositions, bestEndCapLayer));

        if ((STATUS_CODE_SUCCESS != barrelStatusCode) && (STATUS_CODE_SUCCESS != endCapStatusCode))
            return STATUS_CODE_NOT_FOUND;

        pseudoLayer = std::max(bestBarrelLayer, bestEndCapLayer);
        return STATUS_CODE_SUCCESS;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LCPseudoLayerPlugin::FindMatchingLayer(const float position, const LayerPositionList &layerPositionList,
    unsigned int &layer) const
{
    LayerPositionList::const_iterator upperIter = std::upper_bound(layerPositionList.begin(), layerPositionList.end(), position);

    if (layerPositionList.end() == upperIter)
    {
        return STATUS_CODE_NOT_FOUND;
    }

    if (layerPositionList.begin() == upperIter)
    {
        layer = 0;
        return STATUS_CODE_SUCCESS;
    }

    LayerPositionList::const_iterator lowerIter = upperIter - 1;

    if (std::fabs(position - *lowerIter) < std::fabs(position - *upperIter))
    {
        layer = std::distance(layerPositionList.begin(), lowerIter);
    }
    else
    {
        layer = std::distance(layerPositionList.begin(), upperIter);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCPseudoLayerPlugin::StoreLayerPositions()
{
    const GeometryManager *const pGeometryManager(this->GetPandora().GetGeometry());
    this->StoreLayerPositions(pGeometryManager->GetSubDetector(ECAL_BARREL), m_barrelLayerPositions);
    this->StoreLayerPositions(pGeometryManager->GetSubDetector(ECAL_ENDCAP), m_endCapLayerPositions);
    this->StoreLayerPositions(pGeometryManager->GetSubDetector(HCAL_BARREL), m_barrelLayerPositions);
    this->StoreLayerPositions(pGeometryManager->GetSubDetector(HCAL_ENDCAP), m_endCapLayerPositions);
    this->StoreLayerPositions(pGeometryManager->GetSubDetector(MUON_BARREL), m_barrelLayerPositions);
    this->StoreLayerPositions(pGeometryManager->GetSubDetector(MUON_ENDCAP), m_endCapLayerPositions);

    if (m_barrelLayerPositions.empty() || m_endCapLayerPositions.empty())
    {
        std::cout << "LCPseudoLayerPlugin: No layer positions specified." << std::endl;
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
    }

    std::sort(m_barrelLayerPositions.begin(), m_barrelLayerPositions.end());
    std::sort(m_endCapLayerPositions.begin(), m_endCapLayerPositions.end());

    LayerPositionList::const_iterator barrelIter = std::unique(m_barrelLayerPositions.begin(), m_barrelLayerPositions.end());
    LayerPositionList::const_iterator endcapIter = std::unique(m_endCapLayerPositions.begin(), m_endCapLayerPositions.end());

    if ((m_barrelLayerPositions.end() != barrelIter) || (m_endCapLayerPositions.end() != endcapIter))
    {
        std::cout << "LCPseudoLayerPlugin: Duplicate layer position detected." << std::endl;
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCPseudoLayerPlugin::StoreLayerPositions(const SubDetector &subDetector, LayerPositionList &layerPositionList)
{
    if (!subDetector.IsMirroredInZ())
    {
        std::cout << "LCPseudoLayerPlugin: Error, detector must be symmetrical about z=0 plane." << std::endl;
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    const SubDetector::SubDetectorLayerList &subDetectorLayerList(subDetector.GetSubDetectorLayerList());

    for (SubDetector::SubDetectorLayerList::const_iterator iter = subDetectorLayerList.begin(), iterEnd = subDetectorLayerList.end(); iter != iterEnd; ++iter)
    {
        layerPositionList.push_back(iter->GetClosestDistanceToIp());
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCPseudoLayerPlugin::StoreDetectorOuterEdge()
{
    const GeometryManager *const pGeometryManager(this->GetPandora().GetGeometry());

    m_barrelEdgeR = (std::max(pGeometryManager->GetSubDetector(ECAL_BARREL).GetOuterRCoordinate(), std::max(
        pGeometryManager->GetSubDetector(HCAL_BARREL).GetOuterRCoordinate(),
        pGeometryManager->GetSubDetector(MUON_BARREL).GetOuterRCoordinate()) ));

    m_endCapEdgeZ = (std::max(std::fabs(pGeometryManager->GetSubDetector(ECAL_ENDCAP).GetOuterZCoordinate()), std::max(
        std::fabs(pGeometryManager->GetSubDetector(HCAL_ENDCAP).GetOuterZCoordinate()),
        std::fabs(pGeometryManager->GetSubDetector(MUON_ENDCAP).GetOuterZCoordinate())) ));

    if ((m_barrelLayerPositions.end() != std::upper_bound(m_barrelLayerPositions.begin(), m_barrelLayerPositions.end(), m_barrelEdgeR)) ||
        (m_endCapLayerPositions.end() != std::upper_bound(m_endCapLayerPositions.begin(), m_endCapLayerPositions.end(), m_endCapEdgeZ)))
    {
        std::cout << "LCPseudoLayerPlugin: Layers specified outside detector edge." << std::endl;
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }

    m_barrelLayerPositions.push_back(m_barrelEdgeR);
    m_endCapLayerPositions.push_back(m_endCapEdgeZ);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCPseudoLayerPlugin::StorePolygonAngles()
{
    const GeometryManager *const pGeometryManager(this->GetPandora().GetGeometry());

    this->FillAngleVector(pGeometryManager->GetSubDetector(ECAL_BARREL).GetInnerSymmetryOrder(),
        pGeometryManager->GetSubDetector(ECAL_BARREL).GetInnerPhiCoordinate(), m_eCalBarrelAngleVector);

    this->FillAngleVector(pGeometryManager->GetSubDetector(MUON_BARREL).GetInnerSymmetryOrder(),
        pGeometryManager->GetSubDetector(MUON_BARREL).GetInnerPhiCoordinate(), m_muonBarrelAngleVector);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCPseudoLayerPlugin::StoreOverlapCorrectionDetails()
{
    const GeometryManager *const pGeometryManager(this->GetPandora().GetGeometry());

    m_barrelInnerR = pGeometryManager->GetSubDetector(ECAL_BARREL).GetInnerRCoordinate();
    m_endCapInnerZ = std::fabs(pGeometryManager->GetSubDetector(ECAL_ENDCAP).GetInnerZCoordinate());
    m_barrelInnerRMuon = pGeometryManager->GetSubDetector(MUON_BARREL).GetInnerRCoordinate();
    m_endCapInnerZMuon = std::fabs(pGeometryManager->GetSubDetector(MUON_ENDCAP).GetInnerZCoordinate());

    const float barrelOuterZ = std::fabs(pGeometryManager->GetSubDetector(ECAL_BARREL).GetOuterZCoordinate());
    const float endCapOuterR = pGeometryManager->GetSubDetector(ECAL_ENDCAP).GetOuterRCoordinate();
    const float barrelOuterZMuon = std::fabs(pGeometryManager->GetSubDetector(MUON_BARREL).GetOuterZCoordinate());
    const float endCapOuterRMuon = pGeometryManager->GetSubDetector(MUON_ENDCAP).GetOuterRCoordinate();

    const bool IsEnclosingEndCap(endCapOuterR > m_barrelInnerR);
    m_rCorrection = ((!IsEnclosingEndCap) ? 0.f : m_barrelInnerR * ((m_endCapInnerZ / barrelOuterZ) - 1.f));
    m_zCorrection = ((IsEnclosingEndCap) ? 0.f : m_endCapInnerZ * ((m_barrelInnerR / endCapOuterR) - 1.f));
    m_rCorrectionMuon = ((!IsEnclosingEndCap) ? 0.f : m_barrelInnerRMuon * ((m_endCapInnerZMuon / barrelOuterZMuon) - 1.f));
    m_zCorrectionMuon = ((IsEnclosingEndCap) ? 0.f : m_endCapInnerZMuon * ((m_barrelInnerRMuon / endCapOuterRMuon) - 1.f));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float LCPseudoLayerPlugin::GetMaximumRadius(const AngleVector &angleVector, const float x, const float y) const
{
    if (angleVector.size() <= 2)
        return std::sqrt((x * x) + (y * y));

    float maxRadius(0.);
    for (AngleVector::const_iterator iter = angleVector.begin(), iterEnd = angleVector.end(); iter != iterEnd; ++iter)
    {
        const float radius((x * iter->first) + (y * iter->second));

        if (radius > maxRadius)
            maxRadius = radius;
    }

    return maxRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LCPseudoLayerPlugin::FillAngleVector(const unsigned int symmetryOrder, const float phi0, AngleVector &angleVector) const
{
    const float twoPi = static_cast<float>(2. * std::acos(-1.));
    angleVector.clear();

    for (unsigned int iSymmetry = 0; iSymmetry < symmetryOrder; ++iSymmetry)
    {
        const float phi = phi0 + ((twoPi * static_cast<float>(iSymmetry)) / static_cast<float>(symmetryOrder));
        angleVector.push_back(std::make_pair(std::cos(phi), std::sin(phi)));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LCPseudoLayerPlugin::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}

} // namespace lc_content
