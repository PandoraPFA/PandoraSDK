/**
 *  @file   PandoraSDK/src/Persistency/BinaryFileWriter.cc
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

#include "Persistency/BinaryFileWriter.h"

namespace pandora
{

BinaryFileWriter::BinaryFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode,
    const unsigned int majorVersion, const unsigned int minorVersion) :
    FileWriter(pandora, fileName, majorVersion, minorVersion)
{
    m_fileType = BINARY;

    if (APPEND == fileMode)
    {
        m_fileStream.open(fileName.c_str(), std::ios::out | std::ios::in | std::ios::binary | std::ios::ate);

        if (!m_fileStream.is_open())
        {
            m_fileStream.open(fileName.c_str(), std::ios::out | std::ios::binary);
        }
    }
    else if (OVERWRITE == fileMode)
    {
        m_fileStream.open(fileName.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
    }
    else
    {
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    if (!m_fileStream.is_open() || !m_fileStream.good())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    m_containerPosition = m_fileStream.tellp();
}

//------------------------------------------------------------------------------------------------------------------------------------------

BinaryFileWriter::~BinaryFileWriter()
{
    m_fileStream.close();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteHeader(const ContainerId containerId)
{
    RETURN_ON_ERROR(this->WriteVariable(PANDORA_FILE_HASH));
    RETURN_ON_ERROR(this->WriteVariable(containerId));

    m_containerPosition = m_fileStream.tellp();
    const std::ofstream::pos_type dummyContainerSize(0);
    RETURN_ON_ERROR(this->WriteVariable(dummyContainerSize));

    m_containerId = containerId;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteFooter()
{
    if ((HEADER_CONTAINER != m_containerId) && (EVENT_CONTAINER != m_containerId) && (GEOMETRY_CONTAINER != m_containerId))
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable((HEADER_CONTAINER == m_containerId) ? HEADER_END_COMPONENT
                : (EVENT_CONTAINER == m_containerId)            ? EVENT_END_COMPONENT
                                                                : GEOMETRY_END_COMPONENT));

    m_containerId = UNKNOWN_CONTAINER;

    const std::ofstream::pos_type containerSize(m_fileStream.tellp() - m_containerPosition);
    m_fileStream.seekp(m_containerPosition, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable(containerSize));
    m_fileStream.seekp(0, std::ios::end);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    m_containerPosition = m_fileStream.tellp();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteVersion()
{
    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable(VERSION_COMPONENT));
    RETURN_ON_ERROR(this->WriteVariable(m_fileMajorVersion));
    RETURN_ON_ERROR(this->WriteVariable(m_fileMinorVersion));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteSubDetector(const SubDetector *const pSubDetector)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable(SUB_DETECTOR_COMPONENT));
    RETURN_ON_ERROR(m_pSubDetectorFactory->Write(pSubDetector, *this));

    RETURN_ON_ERROR(this->WriteVariable(pSubDetector->GetSubDetectorName()));
    RETURN_ON_ERROR(this->WriteVariable(pSubDetector->GetSubDetectorType()));
    RETURN_ON_ERROR(this->WriteVariable(pSubDetector->GetInnerRCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable(pSubDetector->GetInnerZCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable(pSubDetector->GetInnerPhiCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable(pSubDetector->GetInnerSymmetryOrder()));
    RETURN_ON_ERROR(this->WriteVariable(pSubDetector->GetOuterRCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable(pSubDetector->GetOuterZCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable(pSubDetector->GetOuterPhiCoordinate()));
    RETURN_ON_ERROR(this->WriteVariable(pSubDetector->GetOuterSymmetryOrder()));
    RETURN_ON_ERROR(this->WriteVariable(pSubDetector->IsMirroredInZ()));

    const unsigned int nLayers(pSubDetector->GetNLayers());
    const SubDetector::SubDetectorLayerVector &subDetectorLayerVector(pSubDetector->GetSubDetectorLayerVector());

    if (subDetectorLayerVector.size() != nLayers)
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable(nLayers));

    for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
    {
        RETURN_ON_ERROR(this->WriteVariable(subDetectorLayerVector.at(iLayer).GetClosestDistanceToIp()));
        RETURN_ON_ERROR(this->WriteVariable(subDetectorLayerVector.at(iLayer).GetNRadiationLengths()));
        RETURN_ON_ERROR(this->WriteVariable(subDetectorLayerVector.at(iLayer).GetNInteractionLengths()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteLArTPC(const LArTPC *const pLArTPC)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable(LAR_TPC_COMPONENT));
    RETURN_ON_ERROR(m_pLArTPCFactory->Write(pLArTPC, *this));

    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetLArTPCVolumeId()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetCenterX()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetCenterY()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetCenterZ()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetWidthX()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetWidthY()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetWidthZ()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetWirePitchU()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetWirePitchV()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetWirePitchW()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetWireAngleU()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetWireAngleV()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetWireAngleW()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->GetSigmaUVW()));
    RETURN_ON_ERROR(this->WriteVariable(pLArTPC->IsDriftInPositiveX()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteDetectorGap(const DetectorGap *const pDetectorGap)
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
        RETURN_ON_ERROR(this->WriteVariable(LINE_GAP_COMPONENT));
        RETURN_ON_ERROR(m_pLineGapFactory->Write(pLineGap, *this));

        RETURN_ON_ERROR(this->WriteVariable(pLineGap->GetLineGapType()));
        RETURN_ON_ERROR(this->WriteVariable(pLineGap->GetLineStartX()));
        RETURN_ON_ERROR(this->WriteVariable(pLineGap->GetLineEndX()));
        RETURN_ON_ERROR(this->WriteVariable(pLineGap->GetLineStartZ()));
        RETURN_ON_ERROR(this->WriteVariable(pLineGap->GetLineEndZ()));
    }
    else if (nullptr != pBoxGap)
    {
        RETURN_ON_ERROR(this->WriteVariable(BOX_GAP_COMPONENT));
        RETURN_ON_ERROR(m_pBoxGapFactory->Write(pBoxGap, *this));

        RETURN_ON_ERROR(this->WriteVariable(pBoxGap->GetVertex()));
        RETURN_ON_ERROR(this->WriteVariable(pBoxGap->GetSide1()));
        RETURN_ON_ERROR(this->WriteVariable(pBoxGap->GetSide2()));
        RETURN_ON_ERROR(this->WriteVariable(pBoxGap->GetSide3()));
    }
    else if (nullptr != pConcentricGap)
    {
        RETURN_ON_ERROR(this->WriteVariable(CONCENTRIC_GAP_COMPONENT));
        RETURN_ON_ERROR(m_pConcentricGapFactory->Write(pConcentricGap, *this));

        RETURN_ON_ERROR(this->WriteVariable(pConcentricGap->GetMinZCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable(pConcentricGap->GetMaxZCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable(pConcentricGap->GetInnerRCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable(pConcentricGap->GetInnerPhiCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable(pConcentricGap->GetInnerSymmetryOrder()));
        RETURN_ON_ERROR(this->WriteVariable(pConcentricGap->GetOuterRCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable(pConcentricGap->GetOuterPhiCoordinate()));
        RETURN_ON_ERROR(this->WriteVariable(pConcentricGap->GetOuterSymmetryOrder()));
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteCaloHit(const CaloHit *const pCaloHit)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable(CALO_HIT_COMPONENT));
    RETURN_ON_ERROR(m_pCaloHitFactory->Write(pCaloHit, *this));

    const CellGeometry cellGeometry(pCaloHit->GetCellGeometry());
    RETURN_ON_ERROR(this->WriteVariable(cellGeometry));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetPositionVector()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetExpectedDirection()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetCellNormalVector()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetCellThickness()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetNCellRadiationLengths()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetNCellInteractionLengths()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetTime()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetInputEnergy()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetMipEquivalentEnergy()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetElectromagneticEnergy()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetHadronicEnergy()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->IsDigital()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetHitType()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetHitRegion()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetLayer()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->IsInOuterSamplingLayer()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetParentAddress()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetCellSize0()));
    RETURN_ON_ERROR(this->WriteVariable(pCaloHit->GetCellSize1()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteTrack(const Track *const pTrack)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable(TRACK_COMPONENT));
    RETURN_ON_ERROR(m_pTrackFactory->Write(pTrack, *this));

    RETURN_ON_ERROR(this->WriteVariable(pTrack->GetD0()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->GetZ0()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->GetParticleId()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->GetCharge()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->GetMass()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->GetMomentumAtDca()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->GetTrackStateAtStart()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->GetTrackStateAtEnd()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->GetTrackStateAtCalorimeter()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->GetTimeAtCalorimeter()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->ReachesCalorimeter()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->IsProjectedToEndCap()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->CanFormPfo()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->CanFormClusterlessPfo()));
    RETURN_ON_ERROR(this->WriteVariable(pTrack->GetParentAddress()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteMCParticle(const MCParticle *const pMCParticle)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable(MC_PARTICLE_COMPONENT));
    RETURN_ON_ERROR(m_pMCParticleFactory->Write(pMCParticle, *this));

    RETURN_ON_ERROR(this->WriteVariable(pMCParticle->GetEnergy()));
    RETURN_ON_ERROR(this->WriteVariable(pMCParticle->GetMomentum()));
    RETURN_ON_ERROR(this->WriteVariable(pMCParticle->GetVertex()));
    RETURN_ON_ERROR(this->WriteVariable(pMCParticle->GetEndpoint()));
    RETURN_ON_ERROR(this->WriteVariable(pMCParticle->GetParticleId()));
    RETURN_ON_ERROR(this->WriteVariable(pMCParticle->GetMCParticleType()));
    RETURN_ON_ERROR(this->WriteVariable(pMCParticle->GetUid()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2, const float weight)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable(RELATIONSHIP_COMPONENT));
    RETURN_ON_ERROR(this->WriteVariable(relationshipId));
    RETURN_ON_ERROR(this->WriteVariable(address1));
    RETURN_ON_ERROR(this->WriteVariable(address2));
    RETURN_ON_ERROR(this->WriteVariable(weight));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteEventInformation()
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    RETURN_ON_ERROR(this->WriteVariable(EVENT_INFO_COMPONENT));
    RETURN_ON_ERROR(this->WriteVariable(m_pPandora->GetRun()));
    RETURN_ON_ERROR(this->WriteVariable(m_pPandora->GetSubrun()));
    RETURN_ON_ERROR(this->WriteVariable(m_pPandora->GetEvent()));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
