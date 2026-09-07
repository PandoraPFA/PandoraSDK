/**
 *  @file   PandoraSDK/src/Persistency/XmlFileReader.cc
 *
 *  @brief  Implementation of the xml file reader class.
 *
 *  $Log: $
 */

#include "Api/PandoraApi.h"

#include "Objects/CaloHit.h"
#include "Objects/Track.h"

#include "Persistency/XmlFileReader.h"

#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <limits>
#include <sstream>

namespace pandora
{

namespace
{

void WarnUnparseable(const std::string &typeName, const std::string &text)
{
    std::cout << "XmlFileReader: could not parse \"" << text << "\" as " << typeName << " — substituting zero" << std::endl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
std::vector<unsigned char> BytesFromValue(const T &value)
{
    std::vector<unsigned char> bytes(sizeof(T));
    std::memcpy(bytes.data(), &value, sizeof(T));
    return bytes;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
std::vector<unsigned char> SignedIntegerFromString(const std::string &s, const std::string &typeName)
{
    long long v(0);

    if (!s.empty())
    {
        try
        {
            v = std::stoll(s);
        }
        catch (const std::exception &)
        {
            WarnUnparseable(typeName, s);
            v = 0;
        }
    }

    return BytesFromValue(static_cast<T>(v));
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
std::vector<unsigned char> UnsignedIntegerFromString(const std::string &s, const std::string &typeName)
{
    unsigned long long v(0);

    if (!s.empty())
    {
        try
        {
            v = std::stoull(s);
        }
        catch (const std::exception &)
        {
            WarnUnparseable(typeName, s);
            v = 0;
        }
    }

    return BytesFromValue(static_cast<T>(v));
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
std::vector<unsigned char> FloatingPointFromString(const std::string &s, const std::string &typeName)
{
    double v(0.0);

    if (!s.empty())
    {
        try
        {
            v = std::stod(s);
        }
        catch (const std::exception &)
        {
            WarnUnparseable(typeName, s);
            v = 0.0;
        }
    }

    return BytesFromValue(static_cast<T>(v));
}

//------------------------------------------------------------------------------------------------------------------------------------------

std::vector<unsigned char> CartesianVectorFromString(const std::string &s)
{
    std::istringstream iss(s);
    float x = 0.f, y = 0.f, z = 0.f;
    iss >> x >> y >> z;
    std::vector<unsigned char> bytes(3 * sizeof(float));
    std::memcpy(bytes.data() + 0 * sizeof(float), &x, sizeof(float));
    std::memcpy(bytes.data() + 1 * sizeof(float), &y, sizeof(float));
    std::memcpy(bytes.data() + 2 * sizeof(float), &z, sizeof(float));
    return bytes;
}

//------------------------------------------------------------------------------------------------------------------------------------------

std::vector<unsigned char> TrackStateFromString(const std::string &s)
{
    std::istringstream iss(s);
    float f[6] = {};
    for (int i = 0; i < 6; ++i)
        iss >> f[i];
    std::vector<unsigned char> bytes(6 * sizeof(float));
    for (int i = 0; i < 6; ++i)
        std::memcpy(bytes.data() + i * sizeof(float), &f[i], sizeof(float));
    return bytes;
}

//------------------------------------------------------------------------------------------------------------------------------------------

std::vector<unsigned char> StringToBytes(const std::string &s)
{
    uint32_t len = static_cast<uint32_t>(s.size());
    std::vector<unsigned char> bytes(sizeof(uint32_t) + len);
    std::memcpy(bytes.data(), &len, sizeof(uint32_t));
    if (len > 0)
        std::memcpy(bytes.data() + sizeof(uint32_t), s.data(), len);
    return bytes;
}

//------------------------------------------------------------------------------------------------------------------------------------------

FieldValueType FieldTypeFromAttributeString(const char *const pAttr)
{
    if (nullptr == pAttr)
        return FieldValueType::UNKNOWN;

    const std::string type(pAttr);

    if (type == "float")
        return FieldValueType::FLOAT;
    if (type == "double")
        return FieldValueType::DOUBLE;
    if (type == "int8")
        return FieldValueType::INT8;
    if (type == "int16")
        return FieldValueType::INT16;
    if (type == "int32")
        return FieldValueType::INT32;
    if (type == "int64")
        return FieldValueType::INT64;
    if (type == "uint8")
        return FieldValueType::UINT8;
    if (type == "uint16")
        return FieldValueType::UINT16;
    if (type == "uint32")
        return FieldValueType::UINT32;
    if (type == "uint64")
        return FieldValueType::UINT64;
    if (type == "bool")
        return FieldValueType::BOOL;
    if (type == "string")
        return FieldValueType::STRING;
    if (type == "cvec")
        return FieldValueType::CARTESIAN_VECTOR;
    if (type == "tstate")
        return FieldValueType::TRACK_STATE;

    return FieldValueType::UNKNOWN;
}

//------------------------------------------------------------------------------------------------------------------------------------------

std::vector<unsigned char> FieldTextToBytes(const FieldValueType type, const std::string &text)
{
    switch (type)
    {
        case FieldValueType::FLOAT:
            return FloatingPointFromString<float>(text, "float");
        case FieldValueType::DOUBLE:
            return FloatingPointFromString<double>(text, "double");
        case FieldValueType::INT8:
            return SignedIntegerFromString<int8_t>(text, "int8");
        case FieldValueType::INT16:
            return SignedIntegerFromString<int16_t>(text, "int16");
        case FieldValueType::INT32:
            return SignedIntegerFromString<int32_t>(text, "int32");
        case FieldValueType::INT64:
            return SignedIntegerFromString<int64_t>(text, "int64");
        case FieldValueType::UINT8:
            return UnsignedIntegerFromString<uint8_t>(text, "uint8");
        case FieldValueType::UINT16:
            return UnsignedIntegerFromString<uint16_t>(text, "uint16");
        case FieldValueType::UINT32:
            return UnsignedIntegerFromString<uint32_t>(text, "uint32");
        case FieldValueType::UINT64:
            return UnsignedIntegerFromString<uint64_t>(text, "uint64");
        case FieldValueType::BOOL:
            return UnsignedIntegerFromString<uint8_t>(text, "bool");
        case FieldValueType::STRING:
            return StringToBytes(text);
        case FieldValueType::CARTESIAN_VECTOR:
            return CartesianVectorFromString(text);
        case FieldValueType::TRACK_STATE:
            return TrackStateFromString(text);
        case FieldValueType::UNKNOWN:
        default:
            // No usable type attribute: preserve the text verbatim. A subsequent typed Get<T> will then report
            // STATUS_CODE_INVALID_PARAMETER on the size mismatch.
            return StringToBytes(text);
    }
}

} // anonymous namespace

XmlFileReader::XmlFileReader(const pandora::Pandora &pandora, const std::string &fileName) :
    FileReader(pandora, fileName),
    m_pXmlDocument(nullptr),
    m_pContainerXmlNode(nullptr),
    m_pCurrentXmlElement(nullptr),
    m_isAtFileStart(true)
{
    m_fileType = XML;
    m_pXmlDocument = new TiXmlDocument(fileName);

    if (!m_pXmlDocument->LoadFile())
    {
        std::cout << "XmlFileReader — invalid or missing XML file: " << fileName << std::endl;
        delete m_pXmlDocument;
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }
    std::cout << "Reading XML file: " << fileName << std::endl;

    // Seed the container cursor at the root element's first child so that GetNextContainerId() works correctly before the first
    // GoToNextContainer.
    m_pContainerXmlNode = TiXmlHandle(m_pXmlDocument).FirstChildElement().FirstChildElement().Node();
}

//------------------------------------------------------------------------------------------------------------------------------------------

XmlFileReader::~XmlFileReader()
{
    delete m_pXmlDocument;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadHeader()
{
    m_pCurrentXmlElement = nullptr;
    m_containerId = this->GetNextContainerId();

    if ((HEADER_CONTAINER != m_containerId) && (EVENT_CONTAINER != m_containerId) && (GEOMETRY_CONTAINER != m_containerId))
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::GoToNextContainer()
{
    m_pCurrentXmlElement = nullptr;

    if (m_isAtFileStart)
    {
        // First call: position at the first child of the root element.
        if (!m_pContainerXmlNode)
            m_pContainerXmlNode = TiXmlHandle(m_pXmlDocument).FirstChildElement().FirstChildElement().Node();

        m_isAtFileStart = false;
    }
    else
    {
        if (!m_pContainerXmlNode)
            throw StatusCodeException(STATUS_CODE_NOT_FOUND);

        m_pContainerXmlNode = m_pContainerXmlNode->NextSiblingElement();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

ContainerId XmlFileReader::GetNextContainerId()
{
    const std::string name((nullptr != m_pContainerXmlNode) ? m_pContainerXmlNode->ValueStr() : "");

    if ("Header" == name)
        return HEADER_CONTAINER;
    else if ("Event" == name)
        return EVENT_CONTAINER;
    else if ("Geometry" == name)
        return GEOMETRY_CONTAINER;
    else
        return UNKNOWN_CONTAINER;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::GoToGeometry(const unsigned int geometryNumber)
{
    int nGeometriesRead(0);
    m_isAtFileStart = true;
    m_pContainerXmlNode = nullptr;
    m_pCurrentXmlElement = nullptr;

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

StatusCode XmlFileReader::GoToEvent(const unsigned int eventNumber)
{
    int nEventsRead(0);
    m_isAtFileStart = true;
    m_pContainerXmlNode = nullptr;
    m_pCurrentXmlElement = nullptr;

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

StatusCode XmlFileReader::ReadComponentFields(unsigned int &schemaVersion, FieldMap &fields) const
{
    if (!m_pCurrentXmlElement)
        return STATUS_CODE_FAILURE;

    schemaVersion = 0;
    const char *const pAttr = m_pCurrentXmlElement->Attribute("schemaVersion");

    if (nullptr != pAttr)
    {
        try
        {
            schemaVersion = static_cast<unsigned int>(std::stoul(pAttr));
        }
        catch (...)
        {
            schemaVersion = 0;
        }
    }

    for (TiXmlElement *pChild = m_pCurrentXmlElement->FirstChildElement(); nullptr != pChild; pChild = pChild->NextSiblingElement())
    {
        const std::string tag(pChild->ValueStr());
        const std::string text(pChild->GetText() ? pChild->GetText() : "");
        const FieldValueType type = FieldTypeFromAttributeString(pChild->Attribute("type"));

        fields.SetRawBytes(tag, FieldTextToBytes(type, text));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadNextComponent([[maybe_unused]] const ContainerId expectedContainer)
{
    // Advance to the next sibling element within the current container.
    if (!m_pCurrentXmlElement)
    {
        TiXmlHandle localHandle(m_pContainerXmlNode);
        m_pCurrentXmlElement = localHandle.FirstChildElement().Element();
    }
    else
    {
        m_pCurrentXmlElement = m_pCurrentXmlElement->NextSiblingElement();
    }

    // No more siblings — end of container.
    if (!m_pCurrentXmlElement)
    {
        if (m_pContainerXmlNode)
            m_pContainerXmlNode = m_pContainerXmlNode->NextSiblingElement();

        m_containerId = UNKNOWN_CONTAINER;
        return STATUS_CODE_NOT_FOUND;
    }

    const std::string elementName(m_pCurrentXmlElement->ValueStr());

    unsigned int schemaVersion = 0;
    FieldMap fields;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadComponentFields(schemaVersion, fields));

    if ("Metadata" == elementName)
        return this->ReadMetadata(fields);

    if ("SchemaRegistry" == elementName)
        return this->ReadSchemaRegistry(fields);

    if ("SubDetector" == elementName)
    {
        this->ApplyMigrations(SUB_DETECTOR_COMPONENT, schemaVersion, fields);
        return this->ReadSubDetector(fields);
    }
    if ("LArTPC" == elementName)
    {
        this->ApplyMigrations(LAR_TPC_COMPONENT, schemaVersion, fields);
        return this->ReadLArTPC(fields);
    }
    if ("LineGap" == elementName)
    {
        this->ApplyMigrations(LINE_GAP_COMPONENT, schemaVersion, fields);
        return this->ReadLineGap(fields);
    }
    if ("BoxGap" == elementName)
    {
        this->ApplyMigrations(BOX_GAP_COMPONENT, schemaVersion, fields);
        return this->ReadBoxGap(fields);
    }
    if ("ConcentricGap" == elementName)
    {
        this->ApplyMigrations(CONCENTRIC_GAP_COMPONENT, schemaVersion, fields);
        return this->ReadConcentricGap(fields);
    }

    if ("CaloHit" == elementName)
    {
        this->ApplyMigrations(CALO_HIT_COMPONENT, schemaVersion, fields);
        return this->ReadCaloHit(fields);
    }
    if ("Track" == elementName)
    {
        this->ApplyMigrations(TRACK_COMPONENT, schemaVersion, fields);
        return this->ReadTrack(fields);
    }
    if ("MCParticle" == elementName)
    {
        this->ApplyMigrations(MC_PARTICLE_COMPONENT, schemaVersion, fields);
        return this->ReadMCParticle(fields);
    }
    if ("Relationship" == elementName)
    {
        this->ApplyMigrations(RELATIONSHIP_COMPONENT, schemaVersion, fields);
        return this->ReadRelationship(fields);
    }
    if ("EventInfo" == elementName)
    {
        this->ApplyMigrations(EVENT_INFO_COMPONENT, schemaVersion, fields);
        return this->ReadEventInformation(fields);
    }

    std::cout << "XmlFileReader: skipping unknown element <" << elementName << ">" << std::endl;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadNextGlobalHeaderComponent()
{
    // If we've moved past the header container, signal end.
    if (HEADER_CONTAINER != this->GetNextContainerId() && HEADER_CONTAINER == m_containerId)
    {
        m_containerId = UNKNOWN_CONTAINER;
        return STATUS_CODE_NOT_FOUND;
    }

    return this->ReadNextComponent(HEADER_CONTAINER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadNextGeometryComponent()
{
    return this->ReadNextComponent(GEOMETRY_CONTAINER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadNextEventComponent()
{
    return this->ReadNextComponent(EVENT_CONTAINER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadMetadata(const FieldMap &fields)
{
    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_metadata.m_producerName = fields.GetOrDefault<std::string>("producerName", std::string());
    m_metadata.m_producerVersion = fields.GetOrDefault<std::string>("producerVersion", std::string());
    m_metadata.m_creationTimestamp = fields.GetOrDefault<std::string>("creationTimestamp", std::string());
    m_metadata.m_description = fields.GetOrDefault<std::string>("description", std::string());

    for (const auto &entry : fields.GetAllFields())
    {
        const std::string &tag = entry.first;
        const std::string prefix = "userParam:";

        if (tag.size() > prefix.size() && tag.substr(0, prefix.size()) == prefix)
        {
            std::string value;
            if (STATUS_CODE_SUCCESS == fields.Get(tag, value))
                m_metadata.m_userParameters[tag.substr(prefix.size())] = value;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadSchemaRegistry(const FieldMap &fields)
{
    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    m_schemaRegistry.clear();
    const std::string prefix("component_");

    for (const auto &entry : fields.GetAllFields())
    {
        if (entry.first.size() <= prefix.size() || entry.first.substr(0, prefix.size()) != prefix)
            continue;

        try
        {
            const unsigned int componentIdVal = static_cast<unsigned int>(std::stoul(entry.first.substr(prefix.size())));
            unsigned int schemaVersion = 0;

            if (STATUS_CODE_SUCCESS == fields.Get(entry.first, schemaVersion))
            {
                ComponentSchemaVersion csv;
                csv.m_componentId = static_cast<ComponentId>(componentIdVal);
                csv.m_schemaVersion = schemaVersion;
                m_schemaRegistry.push_back(csv);
            }
        }
        catch (const std::exception &)
        {
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadSubDetector(const FieldMap &fields)
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

StatusCode XmlFileReader::ReadLArTPC(const FieldMap &fields)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::LArTPC::Parameters *pParameters = m_pLArTPCFactory->NewParameters();

    try
    {
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
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pLArTPCFactory->Read(*pParameters, fields));

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

StatusCode XmlFileReader::ReadLineGap(const FieldMap &fields)
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

StatusCode XmlFileReader::ReadBoxGap(const FieldMap &fields)
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

StatusCode XmlFileReader::ReadConcentricGap(const FieldMap &fields)
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

StatusCode XmlFileReader::ReadCaloHit(const FieldMap &fields)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::CaloHit::Parameters *pParameters = m_pCaloHitFactory->NewParameters();

    try
    {
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
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCaloHitFactory->Read(*pParameters, fields));

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

StatusCode XmlFileReader::ReadTrack(const FieldMap &fields)
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
        pParameters->m_trackStateAtCalorimeter =
            fields.GetOrDefault<TrackState>("trackStateAtCalorimeter", TrackState(0.f, 0.f, 0.f, 0.f, 0.f, 0.f));
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

StatusCode XmlFileReader::ReadMCParticle(const FieldMap &fields)
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

StatusCode XmlFileReader::ReadRelationship(const FieldMap &fields)
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

StatusCode XmlFileReader::ReadEventInformation(const FieldMap &fields)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    const unsigned int run = fields.GetOrDefault<unsigned int>("run", 0u);
    const unsigned int subrun = fields.GetOrDefault<unsigned int>("subrun", 0u);
    const unsigned int event = fields.GetOrDefault<unsigned int>("event", 0u);

    return PandoraApi::SetEventInformation(*m_pPandora, run, subrun, event);
}

} // namespace pandora
