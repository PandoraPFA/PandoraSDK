/**
 *  @file   PandoraSDK/src/Persistency/BinaryFileReader.cc
 *
 *  @brief  Implementation of the binary file reader class.
 *
 *  $Log: $
 */

#include "Api/PandoraApi.h"

#include "Objects/CaloHit.h"
#include "Objects/Track.h"

#include "Persistency/BinaryFileReader.h"

#include <cstdint>
#include <iostream>
#include <limits>

namespace pandora
{

BinaryFileReader::BinaryFileReader(const pandora::Pandora &pandora, const std::string &fileName) :
    FileReader(pandora, fileName),
    m_containerPosition(0),
    m_containerSize(0)
{
    m_fileType = BINARY;
    m_fileStream.open(fileName.c_str(), std::ios::in | std::ios::binary);

    if (!m_fileStream.is_open() || !m_fileStream.good())
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

BinaryFileReader::~BinaryFileReader()
{
    m_fileStream.close();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadHeader()
{
    std::string fileHash;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(fileHash));

    if (PANDORA_FILE_HASH != fileHash)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(m_containerId));

    if ((HEADER_CONTAINER != m_containerId) && (EVENT_CONTAINER != m_containerId) && (GEOMETRY_CONTAINER != m_containerId))
        return STATUS_CODE_FAILURE;

    m_containerPosition = m_fileStream.tellg();
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(m_containerSize));

    if (0 == m_containerSize)
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::GoToNextContainer()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadHeader());
    m_fileStream.seekg(m_containerPosition + m_containerSize, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

ContainerId BinaryFileReader::GetNextContainerId()
{
    const std::ifstream::pos_type initialPosition(m_fileStream.tellg());

    std::string fileHash;
    const StatusCode hashSc(this->ReadVariable(fileHash));

    if (STATUS_CODE_SUCCESS != hashSc)
        throw StatusCodeException(hashSc);

    if (PANDORA_FILE_HASH != fileHash)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    ContainerId containerId(UNKNOWN_CONTAINER);
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(containerId));

    m_fileStream.seekg(initialPosition, std::ios::beg);

    if (!m_fileStream.good())
        throw StatusCodeException(STATUS_CODE_FAILURE);

    return containerId;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::GoToGeometry(const unsigned int geometryNumber)
{
    int nGeometriesRead(0);
    m_fileStream.clear();
    m_fileStream.seekg(0, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    if (GEOMETRY_CONTAINER != this->GetNextContainerId())
        --nGeometriesRead;

    while (nGeometriesRead < static_cast<int>(geometryNumber))
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextGeometry());
        ++nGeometriesRead;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::GoToEvent(const unsigned int eventNumber)
{
    int nEventsRead(0);
    m_fileStream.clear();
    m_fileStream.seekg(0, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    if (EVENT_CONTAINER != this->GetNextContainerId())
        --nEventsRead;

    while (nEventsRead < static_cast<int>(eventNumber))
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextEvent());
        ++nEventsRead;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadComponentFields(ComponentId &componentId,
    unsigned int &schemaVersion, FieldMap &fields)
{
    // Read component header
    uint32_t cid(0), sver(0), numFields(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cid));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(sver));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(numFields));

    componentId   = static_cast<ComponentId>(cid);
    schemaVersion = static_cast<unsigned int>(sver);

    // Read all tagged fields
    for (uint32_t i = 0; i < numFields; ++i)
    {
        uint16_t tagLen(0);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(tagLen));

        std::string tag(tagLen, '\0');
        m_fileStream.read(&tag[0], tagLen);

        if (!m_fileStream.good())
            return STATUS_CODE_FAILURE;

        uint32_t dataLen(0);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(dataLen));

        std::vector<unsigned char> data(dataLen);
        m_fileStream.read(reinterpret_cast<char *>(data.data()), dataLen);

        if (!m_fileStream.good())
            return STATUS_CODE_FAILURE;

        fields.SetRawBytes(tag, std::move(data));
    }

    // Verify end marker
    uint32_t marker(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(marker));

    if (COMPONENT_END_MARKER != marker)
    {
        std::cout << "BinaryFileReader: missing component end marker — file may be corrupt" << std::endl;
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadNextComponent([[maybe_unused]] const ContainerId expectedContainer,
    const ComponentId endComponentId)
{
    // Peek at the stream — if at EOF return NOT_FOUND so the loop terminates cleanly.
    if (m_fileStream.peek() == std::ifstream::traits_type::eof())
        return STATUS_CODE_NOT_FOUND;

    ComponentId componentId(UNKNOWN_COMPONENT);
    unsigned int schemaVersion(0);
    FieldMap fields;

    const StatusCode sc = this->ReadComponentFields(componentId, schemaVersion, fields);

    if (STATUS_CODE_SUCCESS != sc)
    {
        if (STATUS_CODE_NOT_FOUND != sc)
            throw StatusCodeException(sc);

        return STATUS_CODE_NOT_FOUND;
    }

    // End-of-container sentinel
    if (endComponentId == componentId)
    {
        m_containerId = UNKNOWN_CONTAINER;
        return STATUS_CODE_NOT_FOUND;
    }

    this->ApplyMigrations(componentId, schemaVersion, fields);

    // Dispatch. Unknown component IDs are silently skipped: the FieldMap was already read and consumed; we simply don't dispatch and return
    // success so the outer loop continues to the next component.
    switch (componentId)
    {
        // Global header components
        case METADATA_COMPONENT:       return this->ReadMetadata(fields);
        case SCHEMA_REGISTRY_COMPONENT:return this->ReadSchemaRegistry(fields);

        // Geometry components
        case SUB_DETECTOR_COMPONENT:  return this->ReadSubDetector(fields);
        case LAR_TPC_COMPONENT:       return this->ReadLArTPC(fields);
        case LINE_GAP_COMPONENT:      return this->ReadLineGap(fields);
        case BOX_GAP_COMPONENT:       return this->ReadBoxGap(fields);
        case CONCENTRIC_GAP_COMPONENT:return this->ReadConcentricGap(fields);

        // Event components
        case CALO_HIT_COMPONENT:      return this->ReadCaloHit(fields);
        case TRACK_COMPONENT:         return this->ReadTrack(fields);
        case MC_PARTICLE_COMPONENT:   return this->ReadMCParticle(fields);
        case RELATIONSHIP_COMPONENT:  return this->ReadRelationship(fields);
        case EVENT_INFO_COMPONENT:    return this->ReadEventInformation(fields);

        default:
            // Unknown component — already consumed from stream, just continue.
            std::cout << "BinaryFileReader: skipping unknown component id "
                      << static_cast<unsigned int>(componentId) << std::endl;
            return STATUS_CODE_SUCCESS;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadNextGlobalHeaderComponent()
{
    return this->ReadNextComponent(HEADER_CONTAINER, HEADER_END_COMPONENT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadNextGeometryComponent()
{
    return this->ReadNextComponent(GEOMETRY_CONTAINER, GEOMETRY_END_COMPONENT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadNextEventComponent()
{
    return this->ReadNextComponent(EVENT_CONTAINER, EVENT_END_COMPONENT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadMetadata(const FieldMap &fields)
{
    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_metadata.m_producerName      = fields.GetOrDefault<std::string>("producerName",      std::string());
    m_metadata.m_producerVersion   = fields.GetOrDefault<std::string>("producerVersion",   std::string());
    m_metadata.m_creationTimestamp = fields.GetOrDefault<std::string>("creationTimestamp", std::string());
    m_metadata.m_description       = fields.GetOrDefault<std::string>("description",       std::string());

    // Recover user parameters: any tag beginning with "userParam:" is a user parameter.
    for (const auto &entry : fields.GetAllFields())
    {
        const std::string &tag = entry.first;
        const std::string prefix("userParam:");

        if (tag.substr(0, prefix.size()) == prefix)
        {
            std::string value;
            if (STATUS_CODE_SUCCESS == fields.Get(tag, value))
                m_metadata.m_userParameters[tag.substr(prefix.size())] = value;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadSchemaRegistry(const FieldMap &fields)
{
    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_schemaRegistry.clear();

    for (const auto &entry : fields.GetAllFields())
    {
        const std::string &tag = entry.first;

        try
        {
            const unsigned int componentIdVal = static_cast<unsigned int>(std::stoul(tag));
            unsigned int schemaVersion(0);

            if (STATUS_CODE_SUCCESS == fields.Get(tag, schemaVersion))
            {
                ComponentSchemaVersion csv;
                csv.m_componentId   = static_cast<ComponentId>(componentIdVal);
                csv.m_schemaVersion = schemaVersion;
                m_schemaRegistry.push_back(csv);
            }
        }
        catch (const std::invalid_argument &)
        {
            // Tag was not a numeric component id — skip
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadSubDetector(const FieldMap &fields)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::SubDetector::Parameters *pParameters = m_pSubDetectorFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pSubDetectorFactory->Read(*pParameters, fields));

        pParameters->m_subDetectorName = fields.GetOrDefault<std::string>("subDetectorName", std::string());
        pParameters->m_subDetectorType = fields.GetOrDefault<SubDetectorType>("subDetectorType", SUB_DETECTOR_OTHER);
        pParameters->m_innerRCoordinate = fields.GetOrDefault<float>("innerRCoordinate", 0.f);
        pParameters->m_innerZCoordinate = fields.GetOrDefault<float>("innerZCoordinate", 0.f);
        pParameters->m_innerPhiCoordinate = fields.GetOrDefault<float>("innerPhiCoordinate", 0.f);
        pParameters->m_innerSymmetryOrder = fields.GetOrDefault<unsigned int>("innerSymmetryOrder", 0u);
        pParameters->m_outerRCoordinate = fields.GetOrDefault<float>("outerRCoordinate", 0.f);
        pParameters->m_outerZCoordinate = fields.GetOrDefault<float>("outerZCoordinate", 0.f);
        pParameters->m_outerPhiCoordinate = fields.GetOrDefault<float>("outerPhiCoordinate", 0.f);
        pParameters->m_outerSymmetryOrder = fields.GetOrDefault<unsigned int>("outerSymmetryOrder", 0u);
        pParameters->m_isMirroredInZ = fields.GetOrDefault<bool>("isMirroredInZ", false);

        const unsigned int nLayers = fields.GetOrDefault<unsigned int>("nLayers", 0u);
        pParameters->m_nLayers = nLayers;

        for (unsigned int i = 0; i < nLayers; ++i)
        {
            const std::string prefix("layer" + std::to_string(i) + "_");
            PandoraApi::Geometry::LayerParameters layerParameters;
            layerParameters.m_closestDistanceToIp = fields.GetOrDefault<float>(prefix + "closestDistanceToIp", 0.f);
            layerParameters.m_nRadiationLengths = fields.GetOrDefault<float>(prefix + "nRadiationLengths", 0.f);
            layerParameters.m_nInteractionLengths = fields.GetOrDefault<float>(prefix + "nInteractionLengths", 0.f);
            pParameters->m_layerParametersVector.push_back(layerParameters);
        }

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::SubDetector::Create(*m_pPandora, *pParameters, *m_pSubDetectorFactory));
        delete pParameters;
    }
    catch (StatusCodeException &e)
    {
        delete pParameters;
        return e.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadLArTPC(const FieldMap &fields)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::LArTPC::Parameters *pParameters = m_pLArTPCFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pLArTPCFactory->Read(*pParameters, fields));

        pParameters->m_larTPCVolumeId = fields.GetOrDefault<unsigned int>("larTPCVolumeId", 0u);
        pParameters->m_centerX = fields.GetOrDefault<float>("centerX", 0.f);
        pParameters->m_centerY = fields.GetOrDefault<float>("centerY", 0.f);
        pParameters->m_centerZ = fields.GetOrDefault<float>("centerZ", 0.f);
        pParameters->m_widthX = fields.GetOrDefault<float>("widthX", 0.f);
        pParameters->m_widthY = fields.GetOrDefault<float>("widthY", 0.f);
        pParameters->m_widthZ = fields.GetOrDefault<float>("widthZ", 0.f);
        pParameters->m_wirePitchU = fields.GetOrDefault<float>("wirePitchU", 0.f);
        pParameters->m_wirePitchV = fields.GetOrDefault<float>("wirePitchV", 0.f);
        pParameters->m_wirePitchW = fields.GetOrDefault<float>("wirePitchW", 0.f);
        pParameters->m_wireAngleU = fields.GetOrDefault<float>("wireAngleU", 0.f);
        pParameters->m_wireAngleV = fields.GetOrDefault<float>("wireAngleV", 0.f);
        pParameters->m_wireAngleW = fields.GetOrDefault<float>("wireAngleW", 0.f);
        pParameters->m_sigmaUVW = fields.GetOrDefault<float>("sigmaUVW", 0.f);
        pParameters->m_isDriftInPositiveX = fields.GetOrDefault<bool>("isDriftInPositiveX", false);

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::LArTPC::Create(*m_pPandora, *pParameters, *m_pLArTPCFactory));
        delete pParameters;
    }
    catch (StatusCodeException &e)
    {
        delete pParameters;
        return e.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadLineGap(const FieldMap &fields)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::LineGap::Parameters *pParameters = m_pLineGapFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pLineGapFactory->Read(*pParameters, fields));

        pParameters->m_lineGapType = fields.GetOrDefault<LineGapType>("lineGapType", TPC_WIRE_GAP_VIEW_U);
        pParameters->m_lineStartX = fields.GetOrDefault<float>("lineStartX", 0.f);
        pParameters->m_lineEndX = fields.GetOrDefault<float>("lineEndX", 0.f);
        pParameters->m_lineStartZ = fields.GetOrDefault<float>("lineStartZ", 0.f);
        pParameters->m_lineEndZ = fields.GetOrDefault<float>("lineEndZ", 0.f);

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::LineGap::Create(*m_pPandora, *pParameters, *m_pLineGapFactory));
        delete pParameters;
    }
    catch (StatusCodeException &e)
    {
        delete pParameters;
        return e.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadBoxGap(const FieldMap &fields)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::BoxGap::Parameters *pParameters = m_pBoxGapFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pBoxGapFactory->Read(*pParameters, fields));

        pParameters->m_vertex = fields.GetOrDefault<CartesianVector>("vertex", CartesianVector(0.f, 0.f, 0.f));
        pParameters->m_side1 = fields.GetOrDefault<CartesianVector>("side1", CartesianVector(0.f, 0.f, 0.f));
        pParameters->m_side2 = fields.GetOrDefault<CartesianVector>("side2", CartesianVector(0.f, 0.f, 0.f));
        pParameters->m_side3 = fields.GetOrDefault<CartesianVector>("side3", CartesianVector(0.f, 0.f, 0.f));

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::BoxGap::Create(*m_pPandora, *pParameters, *m_pBoxGapFactory));
        delete pParameters;
    }
    catch (StatusCodeException &e)
    {
        delete pParameters;
        return e.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadConcentricGap(const FieldMap &fields)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::ConcentricGap::Parameters *pParameters = m_pConcentricGapFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pConcentricGapFactory->Read(*pParameters, fields));

        pParameters->m_minZCoordinate = fields.GetOrDefault<float>("minZCoordinate", 0.f);
        pParameters->m_maxZCoordinate = fields.GetOrDefault<float>("maxZCoordinate", 0.f);
        pParameters->m_innerRCoordinate = fields.GetOrDefault<float>("innerRCoordinate", 0.f);
        pParameters->m_innerPhiCoordinate = fields.GetOrDefault<float>("innerPhiCoordinate", 0.f);
        pParameters->m_innerSymmetryOrder = fields.GetOrDefault<unsigned int>("innerSymmetryOrder", 0u);
        pParameters->m_outerRCoordinate = fields.GetOrDefault<float>("outerRCoordinate", 0.f);
        pParameters->m_outerPhiCoordinate = fields.GetOrDefault<float>("outerPhiCoordinate", 0.f);
        pParameters->m_outerSymmetryOrder = fields.GetOrDefault<unsigned int>("outerSymmetryOrder", 0u);

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::ConcentricGap::Create(*m_pPandora, *pParameters, *m_pConcentricGapFactory));
        delete pParameters;
    }
    catch (StatusCodeException &e)
    {
        delete pParameters;
        return e.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadCaloHit(const FieldMap &fields)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::CaloHit::Parameters *pParameters = m_pCaloHitFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCaloHitFactory->Read(*pParameters, fields));

        pParameters->m_cellGeometry = fields.GetOrDefault<CellGeometry>("cellGeometry", RECTANGULAR);
        pParameters->m_positionVector = fields.GetOrDefault<CartesianVector>("positionVector", CartesianVector(0.f, 0.f, 0.f));
        pParameters->m_expectedDirection = fields.GetOrDefault<CartesianVector>("expectedDirection", CartesianVector(0.f, 0.f, 0.f));
        pParameters->m_cellNormalVector = fields.GetOrDefault<CartesianVector>("cellNormalVector", CartesianVector(0.f, 0.f, 0.f));
        pParameters->m_cellThickness = fields.GetOrDefault<float>("cellThickness", 0.f);
        pParameters->m_nCellRadiationLengths = fields.GetOrDefault<float>("nCellRadiationLengths", 0.f);
        pParameters->m_nCellInteractionLengths = fields.GetOrDefault<float>("nCellInteractionLengths", 0.f);
        pParameters->m_time = fields.GetOrDefault<float>("time", 0.f);
        pParameters->m_inputEnergy = fields.GetOrDefault<float>("inputEnergy", 0.f);
        pParameters->m_mipEquivalentEnergy = fields.GetOrDefault<float>("mipEquivalentEnergy", 0.f);
        pParameters->m_electromagneticEnergy = fields.GetOrDefault<float>("electromagneticEnergy", 0.f);
        pParameters->m_hadronicEnergy = fields.GetOrDefault<float>("hadronicEnergy", 0.f);
        pParameters->m_isDigital = fields.GetOrDefault<bool>("isDigital", false);
        pParameters->m_hitType = fields.GetOrDefault<HitType>("hitType", ECAL);
        pParameters->m_hitRegion = fields.GetOrDefault<HitRegion>("hitRegion", BARREL);
        pParameters->m_layer = fields.GetOrDefault<unsigned int>("layer", 0u);
        pParameters->m_isInOuterSamplingLayer = fields.GetOrDefault<bool>("isInOuterSamplingLayer", false);
        pParameters->m_pParentAddress = fields.GetOrDefault<const void *>("parentAddress", nullptr);
        pParameters->m_cellSize0 = fields.GetOrDefault<float>("cellSize0", 0.f);
        pParameters->m_cellSize1 = fields.GetOrDefault<float>("cellSize1", 0.f);

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::CaloHit::Create(*m_pPandora, *pParameters, *m_pCaloHitFactory));
        delete pParameters;
    }
    catch (StatusCodeException &e)
    {
        delete pParameters;
        return e.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadTrack(const FieldMap &fields)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Track::Parameters *pParameters = m_pTrackFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pTrackFactory->Read(*pParameters, fields));

        pParameters->m_d0 = fields.GetOrDefault<float>("d0", 0.f);
        pParameters->m_z0 = fields.GetOrDefault<float>("z0", 0.f);
        pParameters->m_particleId = fields.GetOrDefault<int>("particleId", 0);
        pParameters->m_charge = fields.GetOrDefault<int>("charge", 0);
        pParameters->m_mass = fields.GetOrDefault<float>("mass", 0.f);
        pParameters->m_momentumAtDca = fields.GetOrDefault<CartesianVector>("momentumAtDca", CartesianVector(0.f, 0.f, 0.f));
        pParameters->m_trackStateAtStart = fields.GetOrDefault<TrackState>("trackStateAtStart", TrackState(0.f, 0.f, 0.f, 0.f, 0.f, 0.f));
        pParameters->m_trackStateAtEnd = fields.GetOrDefault<TrackState>("trackStateAtEnd", TrackState(0.f, 0.f, 0.f, 0.f, 0.f, 0.f));
        pParameters->m_trackStateAtCalorimeter = fields.GetOrDefault<TrackState>("trackStateAtCalorimeter", TrackState(0.f, 0.f, 0.f, 0.f, 0.f, 0.f));
        pParameters->m_timeAtCalorimeter = fields.GetOrDefault<float>("timeAtCalorimeter", 0.f);
        pParameters->m_reachesCalorimeter = fields.GetOrDefault<bool>("reachesCalorimeter", false);
        pParameters->m_isProjectedToEndCap = fields.GetOrDefault<bool>("isProjectedToEndCap", false);
        pParameters->m_canFormPfo = fields.GetOrDefault<bool>("canFormPfo", false);
        pParameters->m_canFormClusterlessPfo = fields.GetOrDefault<bool>("canFormClusterlessPfo", false);
        pParameters->m_pParentAddress = fields.GetOrDefault<const void *>("parentAddress", nullptr);

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Track::Create(*m_pPandora, *pParameters, *m_pTrackFactory));
        delete pParameters;
    }
    catch (StatusCodeException &e)
    {
        delete pParameters;
        return e.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadMCParticle(const FieldMap &fields)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::MCParticle::Parameters *pParameters = m_pMCParticleFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pMCParticleFactory->Read(*pParameters, fields));

        pParameters->m_energy = fields.GetOrDefault<float>("energy", 0.f);
        pParameters->m_momentum = fields.GetOrDefault<CartesianVector>("momentum", CartesianVector(0.f, 0.f, 0.f));
        pParameters->m_vertex = fields.GetOrDefault<CartesianVector>("vertex", CartesianVector(0.f, 0.f, 0.f));
        pParameters->m_endpoint = fields.GetOrDefault<CartesianVector>("endpoint", CartesianVector(0.f, 0.f, 0.f));
        pParameters->m_particleId = fields.GetOrDefault<int>("particleId", -std::numeric_limits<int>::max());
        pParameters->m_mcParticleType = fields.GetOrDefault<MCParticleType>("mcParticleType", MC_3D);
        pParameters->m_pParentAddress = fields.GetOrDefault<const void *>("uid", nullptr);

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::MCParticle::Create(*m_pPandora, *pParameters, *m_pMCParticleFactory));
        delete pParameters;
    }
    catch (StatusCodeException &e)
    {
        delete pParameters;
        return e.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadRelationship(const FieldMap &fields)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    const RelationshipId relationshipId = fields.GetOrDefault<RelationshipId>("relationshipId", UNKNOWN_RELATIONSHIP);

    const uintptr_t addr1 = fields.GetOrDefault<uintptr_t>("address1", 0u);
    const uintptr_t addr2 = fields.GetOrDefault<uintptr_t>("address2", 0u);
    const float weight = fields.GetOrDefault<float>("weight", 1.f);

    const void *const address1 = reinterpret_cast<const void *>(addr1);
    const void *const address2 = reinterpret_cast<const void *>(addr2);

    switch (relationshipId)
    {
        case CALO_HIT_TO_MC_RELATIONSHIP:
            return PandoraApi::SetCaloHitToMCParticleRelationship(*m_pPandora, address1, address2, weight);
        case TRACK_TO_MC_RELATIONSHIP:
            return PandoraApi::SetTrackToMCParticleRelationship(*m_pPandora, address1, address2, weight);
        case MC_PARENT_DAUGHTER_RELATIONSHIP:
            return PandoraApi::SetMCParentDaughterRelationship(*m_pPandora, address1, address2);
        case TRACK_PARENT_DAUGHTER_RELATIONSHIP:
            return PandoraApi::SetTrackParentDaughterRelationship(*m_pPandora, address1, address2);
        case TRACK_SIBLING_RELATIONSHIP:
            return PandoraApi::SetTrackSiblingRelationship(*m_pPandora, address1, address2);
        default:
            return STATUS_CODE_FAILURE;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadEventInformation(const FieldMap &fields)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    const unsigned int run = fields.GetOrDefault<unsigned int>("run", 0u);
    const unsigned int subrun = fields.GetOrDefault<unsigned int>("subrun", 0u);
    const unsigned int event  = fields.GetOrDefault<unsigned int>("event",  0u);

    return PandoraApi::SetEventInformation(*m_pPandora, run, subrun, event);
}

} // namespace pandora
