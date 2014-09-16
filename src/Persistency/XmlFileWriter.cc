/**
 *  @file   PandoraSDK/src/Persistency/XmlFileWriter.cc
 * 
 *  @brief  Implementation of the file writer class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

#include "Objects/CaloHit.h"
#include "Objects/DetectorGap.h"
#include "Objects/MCParticle.h"
#include "Objects/SubDetector.h"
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

StatusCode XmlFileWriter::WriteSubDetector(const SubDetector *const pSubDetector)
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("SubDetector");

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
    const SubDetector::SubDetectorLayerList &subDetectorLayerList(pSubDetector->GetSubDetectorLayerList());

    if (subDetectorLayerList.size() != nLayers)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NLayers", nLayers));

    if (nLayers > 0)
    {
        std::string closestDistanceToIpString, nRadiationLengthsString, nInteractionLengthsString;

        for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
        {
            closestDistanceToIpString += TypeToString(subDetectorLayerList[iLayer].GetClosestDistanceToIp()) + " ";
            nRadiationLengthsString += TypeToString(subDetectorLayerList[iLayer].GetNRadiationLengths()) + " ";
            nInteractionLengthsString += TypeToString(subDetectorLayerList[iLayer].GetNInteractionLengths()) + " ";
        }

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ClosestDistanceToIp", closestDistanceToIpString));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NRadiationLengths", nRadiationLengthsString));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("NInteractionLengths", nInteractionLengthsString));
    }

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

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

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Vertex", pBoxGap->GetVertex()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Side1", pBoxGap->GetSide1()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Side2", pBoxGap->GetSide2()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Side3", pBoxGap->GetSide3()));

        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
    }
    else if (NULL != pConcentricGap)
    {
        m_pCurrentXmlElement = new TiXmlElement("ConcentricGap");

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MinZCoordinate", pConcentricGap->GetMinZCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MaxZCoordinate", pConcentricGap->GetMaxZCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerRCoordinate", pConcentricGap->GetInnerRCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerPhiCoordinate", pConcentricGap->GetInnerPhiCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("InnerSymmetryOrder", pConcentricGap->GetInnerSymmetryOrder()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterRCoordinate", pConcentricGap->GetOuterRCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterPhiCoordinate", pConcentricGap->GetOuterPhiCoordinate()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("OuterSymmetryOrder", pConcentricGap->GetOuterSymmetryOrder()));

        m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);
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
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("CaloHit");

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

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteTrack(const Track *const pTrack)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("Track");

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

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteMCParticle(const MCParticle *const pMCParticle)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("MCParticle");

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Energy", pMCParticle->GetEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Momentum", pMCParticle->GetMomentum()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Vertex", pMCParticle->GetVertex()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Endpoint", pMCParticle->GetEndpoint()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("ParticleId", pMCParticle->GetParticleId()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("MCParticleType", pMCParticle->GetMCParticleType()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Uid", pMCParticle->GetUid()));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileWriter::WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2, const float weight)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    m_pCurrentXmlElement = new TiXmlElement("Relationship");

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("RelationshipId", relationshipId));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Address1", address1));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Address2", address2));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable("Weight", weight));

    m_pContainerXmlElement->LinkEndChild(m_pCurrentXmlElement);

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
