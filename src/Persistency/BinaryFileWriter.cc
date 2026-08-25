/**
 *  @file   PandoraSDK/src/Persistency/BinaryFileWriter.cc
 *
 *  @brief  Implementation of the binary file writer class.
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

#include <cstdint>

namespace pandora
{

//------------------------------------------------------------------------------------------------------------------------------------------

BinaryFileWriter::BinaryFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode) :
    FileWriter(pandora, fileName),
    m_containerPosition(0)
{
    m_fileType = BINARY;

    if (APPEND == fileMode)
    {
        m_fileStream.open(fileName.c_str(), std::ios::out | std::ios::in | std::ios::binary | std::ios::ate);

        if (!m_fileStream.is_open())
            m_fileStream.open(fileName.c_str(), std::ios::out | std::ios::binary);
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

    // Schema version table lives once, in FileWriter (see FileWriter::GetSchemaVersion), so binary and XML writers cannot drift apart.
    this->PopulateSchemaRegistry();
}

//------------------------------------------------------------------------------------------------------------------------------------------

BinaryFileWriter::~BinaryFileWriter()
{
    m_fileStream.close();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteHeader(const ContainerId containerId)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(PANDORA_FILE_HASH));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(PANDORA_BINARY_FORMAT_VERSION));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(containerId));

    m_containerPosition = m_fileStream.tellp();
    const std::ofstream::pos_type dummyContainerSize(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(dummyContainerSize));

    m_containerId = containerId;

    // Tag ids are container-local, so that a reader seeking straight to this container can resolve them.
    m_tagDictionary.clear();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteFooter()
{
    if ((HEADER_CONTAINER != m_containerId) && (EVENT_CONTAINER != m_containerId) && (GEOMETRY_CONTAINER != m_containerId))
        return STATUS_CODE_FAILURE;

    const ComponentId endComponentId =
        (HEADER_CONTAINER == m_containerId) ? HEADER_END_COMPONENT
      : (EVENT_CONTAINER  == m_containerId) ? EVENT_END_COMPONENT
                                             : GEOMETRY_END_COMPONENT;

    const FieldMap emptyFields;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteComponent(endComponentId, 0u, emptyFields));

    m_containerId = UNKNOWN_CONTAINER;

    const std::ofstream::pos_type containerSize(m_fileStream.tellp() - m_containerPosition);
    m_fileStream.seekp(m_containerPosition, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(containerSize));
    m_fileStream.seekp(0, std::ios::end);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    m_containerPosition = m_fileStream.tellp();
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteComponent(const ComponentId componentId, const unsigned int schemaVersion, const FieldMap &fields)
{
    const auto &allFields = fields.GetAllFields();

    const uint32_t cid = static_cast<uint32_t>(componentId);
    const uint32_t sver = static_cast<uint32_t>(schemaVersion);
    const uint32_t numFields = static_cast<uint32_t>(allFields.size());

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(cid));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(sver));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(numFields));

    for (const auto &entry : allFields)
    {
        const std::string &tag = entry.first;
        const std::vector<unsigned char> &data = entry.second;

        const uint32_t dataLen = static_cast<uint32_t>(data.size());

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTagReference(tag));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(dataLen));
        m_fileStream.write(reinterpret_cast<const char *>(data.data()), dataLen);

        if (!m_fileStream.good())
            return STATUS_CODE_FAILURE;
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(COMPONENT_END_MARKER));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteGlobalHeader()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteHeader(HEADER_CONTAINER));

    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteMetadata());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSchemaRegistry());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteFooter());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteMetadata()
{
    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    FieldMap fields;
    fields.Set("producerName", m_metadata.m_producerName);
    fields.Set("producerVersion", m_metadata.m_producerVersion);
    fields.Set("creationTimestamp", m_metadata.m_creationTimestamp);
    fields.Set("description", m_metadata.m_description);

    for (const auto &kv : m_metadata.m_userParameters)
        fields.Set(std::string("userParam:") + kv.first, kv.second);

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteComponent(METADATA_COMPONENT, 1u, fields));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteSchemaRegistry()
{
    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    FieldMap fields;

    for (const ComponentSchemaVersion &entry : m_schemaRegistry)
    {
        const std::string tag = std::to_string(static_cast<uint32_t>(entry.m_componentId));
        fields.Set(tag, entry.m_schemaVersion);
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteComponent(SCHEMA_REGISTRY_COMPONENT, 0u, fields));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteSubDetector(const SubDetector *const pSubDetector)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    FieldMap fields;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pSubDetectorFactory->Write(pSubDetector, fields));

    fields.Set("subDetectorName", pSubDetector->GetSubDetectorName());
    fields.Set("subDetectorType", pSubDetector->GetSubDetectorType());
    fields.Set("innerRCoordinate", pSubDetector->GetInnerRCoordinate());
    fields.Set("innerZCoordinate", pSubDetector->GetInnerZCoordinate());
    fields.Set("innerPhiCoordinate", pSubDetector->GetInnerPhiCoordinate());
    fields.Set("innerSymmetryOrder", pSubDetector->GetInnerSymmetryOrder());
    fields.Set("outerRCoordinate", pSubDetector->GetOuterRCoordinate());
    fields.Set("outerZCoordinate", pSubDetector->GetOuterZCoordinate());
    fields.Set("outerPhiCoordinate", pSubDetector->GetOuterPhiCoordinate());
    fields.Set("outerSymmetryOrder", pSubDetector->GetOuterSymmetryOrder());
    fields.Set("isMirroredInZ", pSubDetector->IsMirroredInZ());

    const SubDetector::SubDetectorLayerVector &layers(pSubDetector->GetSubDetectorLayerVector());
    const unsigned int nLayers = static_cast<unsigned int>(layers.size());

    if (pSubDetector->GetNLayers() != nLayers)
        return STATUS_CODE_FAILURE;

    fields.Set("nLayers", nLayers);

    // Each layer is stored under indexed tags so they are individually addressable.
    for (unsigned int i = 0; i < nLayers; ++i)
    {
        const std::string prefix("layer" + std::to_string(i) + "_");
        fields.Set(prefix + "closestDistanceToIp", layers[i].GetClosestDistanceToIp());
        fields.Set(prefix + "nRadiationLengths", layers[i].GetNRadiationLengths());
        fields.Set(prefix + "nInteractionLengths", layers[i].GetNInteractionLengths());
    }

    return this->WriteComponent(SUB_DETECTOR_COMPONENT, GetSchemaVersion(SUB_DETECTOR_COMPONENT), fields);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteLArTPC(const LArTPC *const pLArTPC)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    FieldMap fields;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pLArTPCFactory->Write(pLArTPC, fields));

    fields.Set("larTPCVolumeId", pLArTPC->GetLArTPCVolumeId());
    fields.Set("centerX", pLArTPC->GetCenterX());
    fields.Set("centerY", pLArTPC->GetCenterY());
    fields.Set("centerZ", pLArTPC->GetCenterZ());
    fields.Set("widthX", pLArTPC->GetWidthX());
    fields.Set("widthY", pLArTPC->GetWidthY());
    fields.Set("widthZ", pLArTPC->GetWidthZ());
    fields.Set("wirePitchU", pLArTPC->GetWirePitchU());
    fields.Set("wirePitchV", pLArTPC->GetWirePitchV());
    fields.Set("wirePitchW", pLArTPC->GetWirePitchW());
    fields.Set("wireAngleU", pLArTPC->GetWireAngleU());
    fields.Set("wireAngleV", pLArTPC->GetWireAngleV());
    fields.Set("wireAngleW", pLArTPC->GetWireAngleW());
    fields.Set("sigmaUVW", pLArTPC->GetSigmaUVW());
    fields.Set("isDriftInPositiveX", pLArTPC->IsDriftInPositiveX());

    return this->WriteComponent(LAR_TPC_COMPONENT, GetSchemaVersion(LAR_TPC_COMPONENT), fields);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteDetectorGap(const DetectorGap *const pDetectorGap)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    const LineGap *const pLineGap = dynamic_cast<const LineGap *>(pDetectorGap);
    const BoxGap *const pBoxGap = dynamic_cast<const BoxGap *>(pDetectorGap);
    const ConcentricGap *const pConcentricGap = dynamic_cast<const ConcentricGap *>(pDetectorGap);

    if (nullptr != pLineGap)
    {
        FieldMap fields;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pLineGapFactory->Write(pLineGap, fields));

        fields.Set("lineGapType", pLineGap->GetLineGapType());
        fields.Set("lineStartX", pLineGap->GetLineStartX());
        fields.Set("lineEndX", pLineGap->GetLineEndX());
        fields.Set("lineStartZ", pLineGap->GetLineStartZ());
        fields.Set("lineEndZ", pLineGap->GetLineEndZ());

        return this->WriteComponent(LINE_GAP_COMPONENT, GetSchemaVersion(LINE_GAP_COMPONENT), fields);
    }
    else if (nullptr != pBoxGap)
    {
        FieldMap fields;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pBoxGapFactory->Write(pBoxGap, fields));

        fields.Set("vertex", pBoxGap->GetVertex());
        fields.Set("side1", pBoxGap->GetSide1());
        fields.Set("side2", pBoxGap->GetSide2());
        fields.Set("side3", pBoxGap->GetSide3());

        return this->WriteComponent(BOX_GAP_COMPONENT, GetSchemaVersion(BOX_GAP_COMPONENT), fields);
    }
    else if (nullptr != pConcentricGap)
    {
        FieldMap fields;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pConcentricGapFactory->Write(pConcentricGap, fields));

        fields.Set("minZCoordinate", pConcentricGap->GetMinZCoordinate());
        fields.Set("maxZCoordinate", pConcentricGap->GetMaxZCoordinate());
        fields.Set("innerRCoordinate", pConcentricGap->GetInnerRCoordinate());
        fields.Set("innerPhiCoordinate", pConcentricGap->GetInnerPhiCoordinate());
        fields.Set("innerSymmetryOrder", pConcentricGap->GetInnerSymmetryOrder());
        fields.Set("outerRCoordinate", pConcentricGap->GetOuterRCoordinate());
        fields.Set("outerPhiCoordinate", pConcentricGap->GetOuterPhiCoordinate());
        fields.Set("outerSymmetryOrder", pConcentricGap->GetOuterSymmetryOrder());

        return this->WriteComponent(CONCENTRIC_GAP_COMPONENT, GetSchemaVersion(CONCENTRIC_GAP_COMPONENT), fields);
    }

    return STATUS_CODE_FAILURE;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteCaloHit(const CaloHit *const pCaloHit)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    FieldMap fields;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCaloHitFactory->Write(pCaloHit, fields));

    fields.Set("cellGeometry", pCaloHit->GetCellGeometry());
    fields.Set("positionVector", pCaloHit->GetPositionVector());
    fields.Set("expectedDirection", pCaloHit->GetExpectedDirection());
    fields.Set("cellNormalVector", pCaloHit->GetCellNormalVector());
    fields.Set("cellThickness", pCaloHit->GetCellThickness());
    fields.Set("nCellRadiationLengths", pCaloHit->GetNCellRadiationLengths());
    fields.Set("nCellInteractionLengths", pCaloHit->GetNCellInteractionLengths());
    fields.Set("time", pCaloHit->GetTime());
    fields.Set("inputEnergy", pCaloHit->GetInputEnergy());
    fields.Set("mipEquivalentEnergy", pCaloHit->GetMipEquivalentEnergy());
    fields.Set("electromagneticEnergy", pCaloHit->GetElectromagneticEnergy());
    fields.Set("hadronicEnergy", pCaloHit->GetHadronicEnergy());
    fields.Set("isDigital", pCaloHit->IsDigital());
    fields.Set("hitType", pCaloHit->GetHitType());
    fields.Set("hitRegion", pCaloHit->GetHitRegion());
    fields.Set("layer", pCaloHit->GetLayer());
    fields.Set("isInOuterSamplingLayer", pCaloHit->IsInOuterSamplingLayer());
    fields.Set("parentAddress", pCaloHit->GetParentAddress());
    fields.Set("cellSize0", pCaloHit->GetCellSize0());
    fields.Set("cellSize1", pCaloHit->GetCellSize1());

    return this->WriteComponent(CALO_HIT_COMPONENT, GetSchemaVersion(CALO_HIT_COMPONENT), fields);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteTrack(const Track *const pTrack)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    FieldMap fields;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pTrackFactory->Write(pTrack, fields));

    fields.Set("d0", pTrack->GetD0());
    fields.Set("z0", pTrack->GetZ0());
    fields.Set("particleId", pTrack->GetParticleId());
    fields.Set("charge", pTrack->GetCharge());
    fields.Set("mass", pTrack->GetMass());
    fields.Set("momentumAtDca", pTrack->GetMomentumAtDca());
    fields.Set("trackStateAtStart", pTrack->GetTrackStateAtStart());
    fields.Set("trackStateAtEnd", pTrack->GetTrackStateAtEnd());
    fields.Set("trackStateAtCalorimeter", pTrack->GetTrackStateAtCalorimeter());
    fields.Set("timeAtCalorimeter", pTrack->GetTimeAtCalorimeter());
    fields.Set("reachesCalorimeter", pTrack->ReachesCalorimeter());
    fields.Set("isProjectedToEndCap", pTrack->IsProjectedToEndCap());
    fields.Set("canFormPfo", pTrack->CanFormPfo());
    fields.Set("canFormClusterlessPfo", pTrack->CanFormClusterlessPfo());
    fields.Set("parentAddress", pTrack->GetParentAddress());

    return this->WriteComponent(TRACK_COMPONENT, GetSchemaVersion(TRACK_COMPONENT), fields);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteMCParticle(const MCParticle *const pMCParticle)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    FieldMap fields;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pMCParticleFactory->Write(pMCParticle, fields));

    fields.Set("energy", pMCParticle->GetEnergy());
    fields.Set("momentum", pMCParticle->GetMomentum());
    fields.Set("vertex", pMCParticle->GetVertex());
    fields.Set("endpoint", pMCParticle->GetEndpoint());
    fields.Set("particleId", pMCParticle->GetParticleId());
    fields.Set("mcParticleType", pMCParticle->GetMCParticleType());
    fields.Set("uid", pMCParticle->GetUid());

    return this->WriteComponent(MC_PARTICLE_COMPONENT, GetSchemaVersion(MC_PARTICLE_COMPONENT), fields);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2,
    const float weight)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    // Relationships are small and fixed-schema; a FieldMap is still used for
    // consistency — the reader path is then uniform across all component types.
    FieldMap fields;
    fields.Set("relationshipId", static_cast<uint32_t>(relationshipId));
    fields.Set("address1", reinterpret_cast<uintptr_t>(address1));
    fields.Set("address2", reinterpret_cast<uintptr_t>(address2));
    fields.Set("weight", weight);

    return this->WriteComponent(RELATIONSHIP_COMPONENT, GetSchemaVersion(RELATIONSHIP_COMPONENT), fields);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteEventInformation()
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    FieldMap fields;
    fields.Set("run", m_pPandora->GetRun());
    fields.Set("subrun", m_pPandora->GetSubrun());
    fields.Set("event",  m_pPandora->GetEvent());

    return this->WriteComponent(EVENT_INFO_COMPONENT, GetSchemaVersion(EVENT_INFO_COMPONENT), fields);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileWriter::WriteTagReference(const std::string &tag)
{
    const auto it = m_tagDictionary.find(tag);

    if (it != m_tagDictionary.end())
        return this->WriteVariable(it->second);

    if (tag.size() > std::numeric_limits<uint16_t>::max())
        return STATUS_CODE_INVALID_PARAMETER;

    if (m_tagDictionary.size() >= NEW_TAG_MARKER)
        return STATUS_CODE_OUT_OF_RANGE;

    const uint16_t tagId(static_cast<uint16_t>(m_tagDictionary.size()));
    const uint16_t tagLen(static_cast<uint16_t>(tag.size()));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(NEW_TAG_MARKER));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(tagLen));
    m_fileStream.write(tag.data(), tagLen);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    m_tagDictionary[tag] = tagId;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
