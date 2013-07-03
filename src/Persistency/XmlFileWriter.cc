/**
 *  @file   PandoraPFANew/Framework/src/Persistency/XmlFileWriter.cc
 * 
 *  @brief  Implementation of the file writer class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

#include "Helpers/GeometryHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/DetectorGap.h"
#include "Objects/MCParticle.h"
#include "Objects/Track.h"

#include "Persistency/XmlFileWriter.h"

namespace pandora
{

XmlFileWriter::XmlFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode) :
    FileWriter(pandora, fileName),
    m_pContainerXmlElement(NULL),
    m_pCurrentXmlElement(NULL)
{
    if (APPEND == fileMode)
    {
        m_pXmlDocument = new TiXmlDocument(fileName);

        if (!m_pXmlDocument->LoadFile())
        {
            std::cout << "XmlFileWriter, fileMode: Append - Invalid xml file." << std::endl;
            delete m_pXmlDocument;
            throw StatusCodeException(STATUS_CODE_FAILURE);
        }
    }
    else if (OVERWRITE == fileMode)
    {
        m_pXmlDocument = new TiXmlDocument;
    }
    else
    {
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

XmlFileWriter::~XmlFileWriter()
{
    m_pXmlDocument->SaveFile(m_fileName);
    delete m_pXmlDocument;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteHeader(const ContainerId containerId)
{
    const std::string containerXmlKey((GEOMETRY == containerId) ? "Geometry" : (EVENT == containerId) ? "Event" : "Unknown");
    m_pContainerXmlElement = new TiXmlElement(containerXmlKey);
    m_pXmlDocument->LinkEndChild(m_pContainerXmlElement);

    m_containerId = containerId;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteFooter()
{
    if ((EVENT != m_containerId) && (GEOMETRY != m_containerId))
        return STATUS_CODE_FAILURE;

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteTracker()
{
    m_pCurrentXmlElement = new TiXmlElement("Tracker");
    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    try
    {
        const float mainTrackerInnerRadius(GeometryHelper::GetMainTrackerInnerRadius());
        const float mainTrackerOuterRadius(GeometryHelper::GetMainTrackerOuterRadius());
        const float mainTrackerZExtent(GeometryHelper::GetMainTrackerZExtent());
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("TrackerPresent", true));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MainTrackerInnerRadius", mainTrackerInnerRadius));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MainTrackerOuterRadius", mainTrackerOuterRadius));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MainTrackerZExtent", mainTrackerZExtent));
    }
    catch (StatusCodeException &)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("TrackerPresent", false));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteCoil()
{
    m_pCurrentXmlElement = new TiXmlElement("Coil");
    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    try
    {
        const float coilInnerRadius(GeometryHelper::GetCoilInnerRadius());
        const float coilOuterRadius(GeometryHelper::GetCoilOuterRadius());
        const float coilZExtent(GeometryHelper::GetCoilZExtent());
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CoilPresent", true));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CoilInnerRadius", coilInnerRadius));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CoilOuterRadius", coilOuterRadius));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CoilZExtent", coilZExtent));
    }
    catch (StatusCodeException &)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CoilPresent", false));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteDetectorGap(const DetectorGap *const pDetectorGap)
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    const BoxGap *pBoxGap = NULL;
    pBoxGap = dynamic_cast<const BoxGap *>(pDetectorGap);

    const ConcentricGap *pConcentricGap = NULL;
    pConcentricGap = dynamic_cast<const ConcentricGap *>(pDetectorGap);

    if (NULL != pBoxGap)
    {
        m_pCurrentXmlElement = new TiXmlElement("BoxGap");
        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Vertex", pBoxGap->m_vertex));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Side1", pBoxGap->m_side1));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Side2", pBoxGap->m_side2));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Side3", pBoxGap->m_side3));
    }
    else if (NULL != pConcentricGap)
    {
        m_pCurrentXmlElement = new TiXmlElement("ConcentricGap");
        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MinZCoordinate", pConcentricGap->m_minZCoordinate));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MaxZCoordinate", pConcentricGap->m_maxZCoordinate));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerRCoordinate", pConcentricGap->m_innerRCoordinate));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerPhiCoordinate", pConcentricGap->m_innerPhiCoordinate));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerSymmetryOrder", pConcentricGap->m_innerSymmetryOrder));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterRCoordinate", pConcentricGap->m_outerRCoordinate));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterPhiCoordinate", pConcentricGap->m_outerPhiCoordinate));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterSymmetryOrder", pConcentricGap->m_outerSymmetryOrder));
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteAdditionalSubDetectors()
{
    const GeometryHelper::SubDetectorParametersMap &subDetectorParametersMap(GeometryHelper::GetAdditionalSubDetectors());

    m_pCurrentXmlElement = m_pContainerXmlElement;

    const unsigned int nAdditionalSubDetectors(subDetectorParametersMap.size());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NAdditionalSubDetectors", nAdditionalSubDetectors));

    for (GeometryHelper::SubDetectorParametersMap::const_iterator iter = subDetectorParametersMap.begin(), iterEnd = subDetectorParametersMap.end();
        iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector(iter->first, &iter->second));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteSubDetector(const std::string &subDetectorName, const GeometryHelper::SubDetectorParameters *const pSubDetectorParameters)
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("SubDetector");
    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("SubDetectorName", subDetectorName));

    const bool isInitialized(pSubDetectorParameters->IsInitialized());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("IsInitialized", isInitialized));

    if (!isInitialized)
        return STATUS_CODE_SUCCESS;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerRCoordinate", pSubDetectorParameters->GetInnerRCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerZCoordinate", pSubDetectorParameters->GetInnerZCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerPhiCoordinate", pSubDetectorParameters->GetInnerPhiCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerSymmetryOrder", pSubDetectorParameters->GetInnerSymmetryOrder()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterRCoordinate", pSubDetectorParameters->GetOuterRCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterZCoordinate", pSubDetectorParameters->GetOuterZCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterPhiCoordinate", pSubDetectorParameters->GetOuterPhiCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterSymmetryOrder", pSubDetectorParameters->GetOuterSymmetryOrder()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("IsMirroredInZ", pSubDetectorParameters->IsMirroredInZ()));

    const unsigned int nLayers(pSubDetectorParameters->GetNLayers());
    const GeometryHelper::LayerParametersList &layerParametersList(pSubDetectorParameters->GetLayerParametersList());

    if (layerParametersList.size() != nLayers)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NLayers", nLayers));
    std::string closestDistanceToIpString, nRadiationLengthsString, nInteractionLengthsString;

    for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
    {
        closestDistanceToIpString += TypeToString(layerParametersList[iLayer].m_closestDistanceToIp) + " ";
        nRadiationLengthsString += TypeToString(layerParametersList[iLayer].m_nRadiationLengths) + " ";
        nInteractionLengthsString += TypeToString(layerParametersList[iLayer].m_nInteractionLengths) + " ";
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ClosestDistanceToIp", closestDistanceToIpString));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NRadiationLengths", nRadiationLengthsString));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NInteractionLengths", nInteractionLengthsString));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteCaloHit(const CaloHit *const pCaloHit)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("CaloHit");
    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    const CellGeometry cellGeometry(pCaloHit->GetCellGeometry());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CellGeometry", cellGeometry));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("PositionVector", pCaloHit->GetPositionVector()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ExpectedDirection", pCaloHit->GetExpectedDirection()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CellNormalVector", pCaloHit->GetCellNormalVector()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CellThickness", pCaloHit->GetCellThickness()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NCellRadiationLengths", pCaloHit->GetNCellRadiationLengths()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NCellInteractionLengths", pCaloHit->GetNCellInteractionLengths()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Time", pCaloHit->GetTime()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InputEnergy", pCaloHit->GetInputEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MipEquivalentEnergy", pCaloHit->GetMipEquivalentEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ElectromagneticEnergy", pCaloHit->GetElectromagneticEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("HadronicEnergy", pCaloHit->GetHadronicEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("IsDigital", pCaloHit->IsDigital()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("HitType", pCaloHit->GetHitType()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("DetectorRegion", pCaloHit->GetDetectorRegion()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Layer", pCaloHit->GetLayer()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("IsInOuterSamplingLayer", pCaloHit->IsInOuterSamplingLayer()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ParentCaloHitAddress", pCaloHit->GetParentCaloHitAddress()));

    if (RECTANGULAR == cellGeometry)
    {
        const RectangularCaloHit *pRectangularCaloHit = dynamic_cast<const RectangularCaloHit *>(pCaloHit);

        if (NULL == pRectangularCaloHit)
            return STATUS_CODE_FAILURE;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CellSizeU", pRectangularCaloHit->GetCellSizeU()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CellSizeV", pRectangularCaloHit->GetCellSizeV()));
    }
    else if (POINTING == cellGeometry)
    {
        const PointingCaloHit *pPointingCaloHit = dynamic_cast<const PointingCaloHit *>(pCaloHit);

        if (NULL == pPointingCaloHit)
            return STATUS_CODE_FAILURE;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CellSizeEta", pPointingCaloHit->GetCellSizeEta()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CellSizePhi", pPointingCaloHit->GetCellSizePhi()));
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteTrack(const Track *const pTrack)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("Track");
    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("D0", pTrack->GetD0()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Z0", pTrack->GetZ0()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ParticleId", pTrack->GetParticleId()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Charge", pTrack->GetCharge()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Mass", pTrack->GetMass()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MomentumAtDca", pTrack->GetMomentumAtDca()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("TrackStateAtStart", pTrack->GetTrackStateAtStart()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("TrackStateAtEnd", pTrack->GetTrackStateAtEnd()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("TrackStateAtCalorimeter", pTrack->GetTrackStateAtCalorimeter()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("TimeAtCalorimeter", pTrack->GetTimeAtCalorimeter()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ReachesCalorimeter", pTrack->ReachesCalorimeter()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("IsProjectedToEndCap", pTrack->IsProjectedToEndCap()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CanFormPfo", pTrack->CanFormPfo()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CanFormClusterlessPfo", pTrack->CanFormClusterlessPfo()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ParentTrackAddress", pTrack->GetParentTrackAddress()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteMCParticle(const MCParticle *const pMCParticle)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("MCParticle");
    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Energy", pMCParticle->GetEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Momentum", pMCParticle->GetMomentum()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Vertex", pMCParticle->GetVertex()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Endpoint", pMCParticle->GetEndpoint()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ParticleId", pMCParticle->GetParticleId()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MCParticleType", pMCParticle->GetMCParticleType()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Uid", pMCParticle->GetUid()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2)
{
    m_pCurrentXmlElement = new TiXmlElement("Relationship");
    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("RelationshipId", relationshipId));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Address1", address1));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Address2", address2));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
