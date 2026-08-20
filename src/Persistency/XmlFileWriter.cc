/**
 *  @file   PandoraSDK/src/Persistency/XmlFileWriter.cc
 *
 *  @brief  Implementation of the file writer class.
 *
 *  $Log: $
 */

#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

#include "Geometry/DetectorGap.h"
#include "Geometry/LArTPC.h"
#include "Geometry/SubDetector.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"
#include "Objects/Track.h"

#include "Persistency/XmlFileWriter.h"

namespace pandora
{

XmlFileWriter::XmlFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode,
    const unsigned int majorVersion, const unsigned int minorVersion) :
    FileWriter(pandora, fileName, majorVersion, minorVersion),
    m_pContainerXmlElement(nullptr),
    m_pCurrentXmlElement(nullptr)
{
    m_fileType = XML;

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
    const std::string containerXmlKey((HEADER_CONTAINER == containerId) ? "Header"
            : (GEOMETRY_CONTAINER == containerId)                       ? "Geometry"
            : (EVENT_CONTAINER == containerId)                          ? "Event"
                                                                        : "Unknown");

    m_pContainerXmlElement = new TiXmlElement(containerXmlKey);
    m_pXmlDocument->LinkEndChild(m_pContainerXmlElement);

    m_containerId = containerId;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteFooter()
{
    if ((HEADER_CONTAINER != m_containerId) && (EVENT_CONTAINER != m_containerId) && (GEOMETRY_CONTAINER != m_containerId))
        return STATUS_CODE_FAILURE;

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteVersion()
{
    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("Version");
    RETURN_ON_ERROR(this->WriteVariable("MajorVersion", m_fileMajorVersion));
    RETURN_ON_ERROR(this->WriteVariable("MinorVersion", m_fileMinorVersion));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteSubDetector(const SubDetector *const pSubDetector)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("SubDetector");
    RETURN_ON_ERROR(m_pSubDetectorFactory->Write(pSubDetector, *this));

    RETURN_ON_ERROR(this->WriteVariable("SubDetectorName", pSubDetector->GetSubDetectorName()));
    RETURN_ON_ERROR(this->WriteVariable("SubDetectorType", pSubDetector->GetSubDetectorType()));
    RETURN_ON_ERROR(this->WriteVariable("InnerRCoordinate", pSubDetector->GetInnerRCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable("InnerZCoordinate", pSubDetector->GetInnerZCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable("InnerPhiCoordinate", pSubDetector->GetInnerPhiCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable("InnerSymmetryOrder", pSubDetector->GetInnerSymmetryOrder()));
    RETURN_ON_ERROR(this->WriteVariable("OuterRCoordinate", pSubDetector->GetOuterRCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable("OuterZCoordinate", pSubDetector->GetOuterZCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable("OuterPhiCoordinate", pSubDetector->GetOuterPhiCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable("OuterSymmetryOrder", pSubDetector->GetOuterSymmetryOrder()));
    RETURN_ON_ERROR(this->WriteVariable("IsMirroredInZ", pSubDetector->IsMirroredInZ()));

    const unsigned int nLayers(pSubDetector->GetNLayers());
    const SubDetector::SubDetectorLayerVector &subDetectorLayerVector(pSubDetector->GetSubDetectorLayerVector());

    if (subDetectorLayerVector.size() != nLayers)
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable("NLayers", nLayers));

    if (nLayers > 0)
    {
        std::string closestDistanceToIpString, nRadiationLengthsString, nInteractionLengthsString;

        for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
        {
            closestDistanceToIpString += TypeToString(subDetectorLayerVector.at(iLayer).GetClosestDistanceToIp()) + " ";
            nRadiationLengthsString += TypeToString(subDetectorLayerVector.at(iLayer).GetNRadiationLengths()) + " ";
            nInteractionLengthsString += TypeToString(subDetectorLayerVector.at(iLayer).GetNInteractionLengths()) + " ";
        }

        RETURN_ON_ERROR(this->WriteVariable("ClosestDistanceToIp", closestDistanceToIpString));
        RETURN_ON_ERROR(this->WriteVariable("NRadiationLengths", nRadiationLengthsString));
        RETURN_ON_ERROR(this->WriteVariable("NInteractionLengths", nInteractionLengthsString));
    }

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteLArTPC(const LArTPC *const pLArTPC)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("LArTPC");
    RETURN_ON_ERROR(m_pLArTPCFactory->Write(pLArTPC, *this));

    RETURN_ON_ERROR(this->WriteVariable("LArTPCVolumeId", pLArTPC->GetLArTPCVolumeId()));
    RETURN_ON_ERROR(this->WriteVariable("CenterX", pLArTPC->GetCenterX()));
    RETURN_ON_ERROR(this->WriteVariable("CenterY", pLArTPC->GetCenterY()));
    RETURN_ON_ERROR(this->WriteVariable("CenterZ", pLArTPC->GetCenterZ()));
    RETURN_ON_ERROR(this->WriteVariable("WidthX", pLArTPC->GetWidthX()));
    RETURN_ON_ERROR(this->WriteVariable("WidthY", pLArTPC->GetWidthY()));
    RETURN_ON_ERROR(this->WriteVariable("WidthZ", pLArTPC->GetWidthZ()));
    RETURN_ON_ERROR(this->WriteVariable("WirePitchU", pLArTPC->GetWirePitchU()));
    RETURN_ON_ERROR(this->WriteVariable("WirePitchV", pLArTPC->GetWirePitchV()));
    RETURN_ON_ERROR(this->WriteVariable("WirePitchW", pLArTPC->GetWirePitchW()));
    RETURN_ON_ERROR(this->WriteVariable("WireAngleU", pLArTPC->GetWireAngleU()));
    RETURN_ON_ERROR(this->WriteVariable("WireAngleV", pLArTPC->GetWireAngleV()));
    RETURN_ON_ERROR(this->WriteVariable("WireAngleW", pLArTPC->GetWireAngleW()));
    RETURN_ON_ERROR(this->WriteVariable("SigmaUVW", pLArTPC->GetSigmaUVW()));
    RETURN_ON_ERROR(this->WriteVariable("IsDriftInPositiveX", pLArTPC->IsDriftInPositiveX()));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteDetectorGap(const DetectorGap *const pDetectorGap)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    const LineGap *pLineGap(nullptr);
    pLineGap = dynamic_cast<const LineGap *>(pDetectorGap);

    const BoxGap *pBoxGap(nullptr);
    pBoxGap = dynamic_cast<const BoxGap *>(pDetectorGap);

    const ConcentricGap *pConcentricGap(nullptr);
    pConcentricGap = dynamic_cast<const ConcentricGap *>(pDetectorGap);

    if (nullptr != pLineGap)
    {
        m_pCurrentXmlElement = new TiXmlElement("LineGap");
        RETURN_ON_ERROR(m_pLineGapFactory->Write(pLineGap, *this));

        RETURN_ON_ERROR(this->WriteVariable("LineGapType", pLineGap->GetLineGapType()));
        RETURN_ON_ERROR(this->WriteVariable("LineStartX", pLineGap->GetLineStartX()));
        RETURN_ON_ERROR(this->WriteVariable("LineEndX", pLineGap->GetLineEndX()));
        RETURN_ON_ERROR(this->WriteVariable("LineStartZ", pLineGap->GetLineStartZ()));
        RETURN_ON_ERROR(this->WriteVariable("LineEndZ", pLineGap->GetLineEndZ()));

        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
        m_pCurrentXmlElement = nullptr;
    }
    else if (nullptr != pBoxGap)
    {
        m_pCurrentXmlElement = new TiXmlElement("BoxGap");
        RETURN_ON_ERROR(m_pBoxGapFactory->Write(pBoxGap, *this));

        RETURN_ON_ERROR(this->WriteVariable("Vertex", pBoxGap->GetVertex()));
        RETURN_ON_ERROR(this->WriteVariable("Side1", pBoxGap->GetSide1()));
        RETURN_ON_ERROR(this->WriteVariable("Side2", pBoxGap->GetSide2()));
        RETURN_ON_ERROR(this->WriteVariable("Side3", pBoxGap->GetSide3()));

        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
        m_pCurrentXmlElement = nullptr;
    }
    else if (nullptr != pConcentricGap)
    {
        m_pCurrentXmlElement = new TiXmlElement("ConcentricGap");
        RETURN_ON_ERROR(m_pConcentricGapFactory->Write(pConcentricGap, *this));

        RETURN_ON_ERROR(this->WriteVariable("MinZCoordinate", pConcentricGap->GetMinZCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable("MaxZCoordinate", pConcentricGap->GetMaxZCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable("InnerRCoordinate", pConcentricGap->GetInnerRCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable("InnerPhiCoordinate", pConcentricGap->GetInnerPhiCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable("InnerSymmetryOrder", pConcentricGap->GetInnerSymmetryOrder()));
        RETURN_ON_ERROR(this->WriteVariable("OuterRCoordinate", pConcentricGap->GetOuterRCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable("OuterPhiCoordinate", pConcentricGap->GetOuterPhiCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable("OuterSymmetryOrder", pConcentricGap->GetOuterSymmetryOrder()));

        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
        m_pCurrentXmlElement = nullptr;
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteCaloHit(const CaloHit *const pCaloHit)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("CaloHit");
    RETURN_ON_ERROR(m_pCaloHitFactory->Write(pCaloHit, *this));

    const CellGeometry cellGeometry(pCaloHit->GetCellGeometry());
    RETURN_ON_ERROR(this->WriteVariable("CellGeometry", cellGeometry));
    RETURN_ON_ERROR(this->WriteVariable("PositionVector", pCaloHit->GetPositionVector()));
    RETURN_ON_ERROR(this->WriteVariable("ExpectedDirection", pCaloHit->GetExpectedDirection()));
    RETURN_ON_ERROR(this->WriteVariable("CellNormalVector", pCaloHit->GetCellNormalVector()));
    RETURN_ON_ERROR(this->WriteVariable("CellThickness", pCaloHit->GetCellThickness()));
    RETURN_ON_ERROR(this->WriteVariable("NCellRadiationLengths", pCaloHit->GetNCellRadiationLengths()));
    RETURN_ON_ERROR(this->WriteVariable("NCellInteractionLengths", pCaloHit->GetNCellInteractionLengths()));
    RETURN_ON_ERROR(this->WriteVariable("Time", pCaloHit->GetTime()));
    RETURN_ON_ERROR(this->WriteVariable("InputEnergy", pCaloHit->GetInputEnergy()));
    RETURN_ON_ERROR(this->WriteVariable("MipEquivalentEnergy", pCaloHit->GetMipEquivalentEnergy()));
    RETURN_ON_ERROR(this->WriteVariable("ElectromagneticEnergy", pCaloHit->GetElectromagneticEnergy()));
    RETURN_ON_ERROR(this->WriteVariable("HadronicEnergy", pCaloHit->GetHadronicEnergy()));
    RETURN_ON_ERROR(this->WriteVariable("IsDigital", pCaloHit->IsDigital()));
    RETURN_ON_ERROR(this->WriteVariable("HitType", pCaloHit->GetHitType()));
    RETURN_ON_ERROR(this->WriteVariable("HitRegion", pCaloHit->GetHitRegion()));
    RETURN_ON_ERROR(this->WriteVariable("Layer", pCaloHit->GetLayer()));
    RETURN_ON_ERROR(this->WriteVariable("IsInOuterSamplingLayer", pCaloHit->IsInOuterSamplingLayer()));
    RETURN_ON_ERROR(this->WriteVariable("ParentCaloHitAddress", pCaloHit->GetParentAddress()));
    RETURN_ON_ERROR(this->WriteVariable("CellSize0", pCaloHit->GetCellSize0()));
    RETURN_ON_ERROR(this->WriteVariable("CellSize1", pCaloHit->GetCellSize1()));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteTrack(const Track *const pTrack)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("Track");
    RETURN_ON_ERROR(m_pTrackFactory->Write(pTrack, *this));

    RETURN_ON_ERROR(this->WriteVariable("D0", pTrack->GetD0()));
    RETURN_ON_ERROR(this->WriteVariable("Z0", pTrack->GetZ0()));
    RETURN_ON_ERROR(this->WriteVariable("ParticleId", pTrack->GetParticleId()));
    RETURN_ON_ERROR(this->WriteVariable("Charge", pTrack->GetCharge()));
    RETURN_ON_ERROR(this->WriteVariable("Mass", pTrack->GetMass()));
    RETURN_ON_ERROR(this->WriteVariable("MomentumAtDca", pTrack->GetMomentumAtDca()));
    RETURN_ON_ERROR(this->WriteVariable("TrackStateAtStart", pTrack->GetTrackStateAtStart()));
    RETURN_ON_ERROR(this->WriteVariable("TrackStateAtEnd", pTrack->GetTrackStateAtEnd()));
    RETURN_ON_ERROR(this->WriteVariable("TrackStateAtCalorimeter", pTrack->GetTrackStateAtCalorimeter()));
    RETURN_ON_ERROR(this->WriteVariable("TimeAtCalorimeter", pTrack->GetTimeAtCalorimeter()));
    RETURN_ON_ERROR(this->WriteVariable("ReachesCalorimeter", pTrack->ReachesCalorimeter()));
    RETURN_ON_ERROR(this->WriteVariable("IsProjectedToEndCap", pTrack->IsProjectedToEndCap()));
    RETURN_ON_ERROR(this->WriteVariable("CanFormPfo", pTrack->CanFormPfo()));
    RETURN_ON_ERROR(this->WriteVariable("CanFormClusterlessPfo", pTrack->CanFormClusterlessPfo()));
    RETURN_ON_ERROR(this->WriteVariable("ParentTrackAddress", pTrack->GetParentAddress()));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteMCParticle(const MCParticle *const pMCParticle)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("MCParticle");
    RETURN_ON_ERROR(m_pMCParticleFactory->Write(pMCParticle, *this));

    RETURN_ON_ERROR(this->WriteVariable("Energy", pMCParticle->GetEnergy()));
    RETURN_ON_ERROR(this->WriteVariable("Momentum", pMCParticle->GetMomentum()));
    RETURN_ON_ERROR(this->WriteVariable("Vertex", pMCParticle->GetVertex()));
    RETURN_ON_ERROR(this->WriteVariable("Endpoint", pMCParticle->GetEndpoint()));
    RETURN_ON_ERROR(this->WriteVariable("ParticleId", pMCParticle->GetParticleId()));
    RETURN_ON_ERROR(this->WriteVariable("MCParticleType", pMCParticle->GetMCParticleType()));
    RETURN_ON_ERROR(this->WriteVariable("Uid", pMCParticle->GetUid()));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2, const float weight)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("Relationship");

    RETURN_ON_ERROR(this->WriteVariable("RelationshipId", relationshipId));
    RETURN_ON_ERROR(this->WriteVariable("Address1", address1));
    RETURN_ON_ERROR(this->WriteVariable("Address2", address2));
    RETURN_ON_ERROR(this->WriteVariable("Weight", weight));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteEventInformation()
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("EventInfo");

    RETURN_ON_ERROR(this->WriteVariable("Run", m_pPandora->GetRun()));
    RETURN_ON_ERROR(this->WriteVariable("Subrun", m_pPandora->GetSubrun()));
    RETURN_ON_ERROR(this->WriteVariable("Event", m_pPandora->GetEvent()));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
