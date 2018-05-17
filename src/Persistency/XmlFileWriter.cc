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

XmlFileWriter::XmlFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode) :
    FileWriter(pandora, fileName),
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
    const std::string containerXmlKey((GEOMETRY_CONTAINER == containerId) ? "Geometry" : (EVENT_CONTAINER == containerId) ? "Event" : "Unknown");
    m_pContainerXmlElement = new TiXmlElement(containerXmlKey);
    m_pXmlDocument->LinkEndChild(m_pContainerXmlElement);

    m_containerId = containerId;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteFooter()
{
    if ((EVENT_CONTAINER != m_containerId) && (GEOMETRY_CONTAINER != m_containerId))
        return STATUS_CODE_FAILURE;

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteSubDetector(const SubDetector *const pSubDetector)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("SubDetector");
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pSubDetectorFactory->Write(pSubDetector, *this));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("SubDetectorName", pSubDetector->GetSubDetectorName()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("SubDetectorType", pSubDetector->GetSubDetectorType()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerRCoordinate", pSubDetector->GetInnerRCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerZCoordinate", pSubDetector->GetInnerZCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerPhiCoordinate", pSubDetector->GetInnerPhiCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerSymmetryOrder", pSubDetector->GetInnerSymmetryOrder()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterRCoordinate", pSubDetector->GetOuterRCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterZCoordinate", pSubDetector->GetOuterZCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterPhiCoordinate", pSubDetector->GetOuterPhiCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterSymmetryOrder", pSubDetector->GetOuterSymmetryOrder()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("IsMirroredInZ", pSubDetector->IsMirroredInZ()));

    const unsigned int nLayers(pSubDetector->GetNLayers());
    const SubDetector::SubDetectorLayerVector &subDetectorLayerVector(pSubDetector->GetSubDetectorLayerVector());

    if (subDetectorLayerVector.size() != nLayers)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NLayers", nLayers));

    if (nLayers > 0)
    {
        std::string closestDistanceToIpString, nRadiationLengthsString, nInteractionLengthsString;

        for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
        {
            closestDistanceToIpString += TypeToString(subDetectorLayerVector.at(iLayer).GetClosestDistanceToIp()) + " ";
            nRadiationLengthsString += TypeToString(subDetectorLayerVector.at(iLayer).GetNRadiationLengths()) + " ";
            nInteractionLengthsString += TypeToString(subDetectorLayerVector.at(iLayer).GetNInteractionLengths()) + " ";
        }

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ClosestDistanceToIp", closestDistanceToIpString));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NRadiationLengths", nRadiationLengthsString));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NInteractionLengths", nInteractionLengthsString));
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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pLArTPCFactory->Write(pLArTPC, *this));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("LArTPCVolumeId", pLArTPC->GetLArTPCVolumeId()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CenterX", pLArTPC->GetCenterX()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CenterY", pLArTPC->GetCenterY()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CenterZ", pLArTPC->GetCenterZ()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("WidthX", pLArTPC->GetWidthX()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("WidthY", pLArTPC->GetWidthY()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("WidthZ", pLArTPC->GetWidthZ()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("WirePitchU", pLArTPC->GetWirePitchU()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("WirePitchV", pLArTPC->GetWirePitchV()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("WirePitchW", pLArTPC->GetWirePitchW()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("WireAngleU", pLArTPC->GetWireAngleU()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("WireAngleV", pLArTPC->GetWireAngleV()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("WireAngleW", pLArTPC->GetWireAngleW()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("SigmaUVW", pLArTPC->GetSigmaUVW()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("IsDriftInPositiveX", pLArTPC->IsDriftInPositiveX()));

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
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pLineGapFactory->Write(pLineGap, *this));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("LineGapType", pLineGap->GetLineGapType()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("LineStartX", pLineGap->GetLineStartX()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("LineEndX", pLineGap->GetLineEndX()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("LineStartZ", pLineGap->GetLineStartZ()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("LineEndZ", pLineGap->GetLineEndZ()));

        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
        m_pCurrentXmlElement = nullptr;
    }
    else if (nullptr != pBoxGap)
    {
        m_pCurrentXmlElement = new TiXmlElement("BoxGap");
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pBoxGapFactory->Write(pBoxGap, *this));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Vertex", pBoxGap->GetVertex()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Side1", pBoxGap->GetSide1()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Side2", pBoxGap->GetSide2()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Side3", pBoxGap->GetSide3()));

        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
        m_pCurrentXmlElement = nullptr;
    }
    else if (nullptr != pConcentricGap)
    {
        m_pCurrentXmlElement = new TiXmlElement("ConcentricGap");
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pConcentricGapFactory->Write(pConcentricGap, *this));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MinZCoordinate", pConcentricGap->GetMinZCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MaxZCoordinate", pConcentricGap->GetMaxZCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerRCoordinate", pConcentricGap->GetInnerRCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerPhiCoordinate", pConcentricGap->GetInnerPhiCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerSymmetryOrder", pConcentricGap->GetInnerSymmetryOrder()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterRCoordinate", pConcentricGap->GetOuterRCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterPhiCoordinate", pConcentricGap->GetOuterPhiCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterSymmetryOrder", pConcentricGap->GetOuterSymmetryOrder()));

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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCaloHitFactory->Write(pCaloHit, *this));

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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("HitRegion", pCaloHit->GetHitRegion()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Layer", pCaloHit->GetLayer()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("IsInOuterSamplingLayer", pCaloHit->IsInOuterSamplingLayer()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ParentCaloHitAddress", pCaloHit->GetParentAddress()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CellSize0", pCaloHit->GetCellSize0()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("CellSize1", pCaloHit->GetCellSize1()));

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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pTrackFactory->Write(pTrack, *this));

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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ParentTrackAddress", pTrack->GetParentAddress()));

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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pMCParticleFactory->Write(pMCParticle, *this));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Energy", pMCParticle->GetEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Momentum", pMCParticle->GetMomentum()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Vertex", pMCParticle->GetVertex()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Endpoint", pMCParticle->GetEndpoint()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ParticleId", pMCParticle->GetParticleId()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MCParticleType", pMCParticle->GetMCParticleType()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Uid", pMCParticle->GetUid()));

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

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("RelationshipId", relationshipId));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Address1", address1));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Address2", address2));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Weight", weight));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    m_pCurrentXmlElement = nullptr;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
