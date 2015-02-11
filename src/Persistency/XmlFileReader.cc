/**
 *  @file   PandoraSDK/src/Persistency/XmlFileReader.cc
 * 
 *  @brief  Implementation of the xml file reader class.
 * 
 *  $Log: $
 */

#include "Helpers/XmlHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/Track.h"

#include "Persistency/XmlFileReader.h"

namespace pandora
{

XmlFileReader::XmlFileReader(const pandora::Pandora &pandora, const std::string &fileName) :
    FileReader(pandora, fileName),
    m_pContainerXmlNode(NULL),
    m_pCurrentXmlElement(NULL),
    m_isAtFileStart(true)
{
    m_pXmlDocument = new TiXmlDocument(fileName);

    if (!m_pXmlDocument->LoadFile())
    {
        std::cout << "XmlFileReader - Invalid xml file." << std::endl;
        delete m_pXmlDocument;
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

XmlFileReader::~XmlFileReader()
{
    delete m_pXmlDocument;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadHeader()
{
    m_pCurrentXmlElement = NULL;
    m_containerId = this->GetNextContainerId();

    if ((EVENT != m_containerId) && (GEOMETRY != m_containerId))
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::GoToNextContainer()
{
    m_pCurrentXmlElement = NULL;

    if (m_isAtFileStart)
    {
        if (NULL == m_pContainerXmlNode)
            m_pContainerXmlNode = TiXmlHandle(m_pXmlDocument).FirstChildElement().Element();

        m_isAtFileStart = false;
    }
    else
    {
        if (NULL == m_pContainerXmlNode)
            throw StatusCodeException(STATUS_CODE_NOT_FOUND);

        m_pContainerXmlNode = m_pContainerXmlNode->NextSibling();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

ContainerId XmlFileReader::GetNextContainerId()
{
    const std::string containerId((NULL != m_pContainerXmlNode) ? m_pContainerXmlNode->ValueStr() : "");

    if (std::string("Event") == containerId)
    {
        return EVENT;
    }
    else if (std::string("Geometry") == containerId)
    {
        return GEOMETRY;
    }
    else
    {
        return UNKNOWN_CONTAINER;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::GoToGeometry(const unsigned int geometryNumber)
{
    int nGeometriesRead(0);
    m_isAtFileStart = true;
    m_pContainerXmlNode = NULL;
    m_pCurrentXmlElement = NULL;

    if (GEOMETRY != this->GetNextContainerId())
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
    m_pContainerXmlNode = NULL;
    m_pCurrentXmlElement = NULL;

    if (EVENT != this->GetNextContainerId())
        --nEventsRead;

    while (nEventsRead < static_cast<int>(eventNumber))
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextEvent());
        ++nEventsRead;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadNextGeometryComponent()
{
    if (NULL == m_pCurrentXmlElement)
    {
        TiXmlHandle localHandle(m_pContainerXmlNode);
        m_pCurrentXmlElement = localHandle.FirstChild().Element();
    }
    else
    {
        m_pCurrentXmlElement = m_pCurrentXmlElement->NextSiblingElement();
    }

    if (NULL == m_pCurrentXmlElement)
    {
        this->GoToNextContainer();
        return STATUS_CODE_NOT_FOUND;
    }

    const std::string componentName(m_pCurrentXmlElement->ValueStr());

    if (std::string("SubDetector") == componentName)
    {
        return this->ReadSubDetector();
    }
    if (std::string("BoxGap") == componentName)
    {
        return this->ReadBoxGap();
    }
    else if (std::string("ConcentricGap") == componentName)
    {
        return this->ReadConcentricGap();
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadNextEventComponent()
{
    if (NULL == m_pCurrentXmlElement)
    {
        TiXmlHandle localHandle(m_pContainerXmlNode);
        m_pCurrentXmlElement = localHandle.FirstChild().Element();
    }
    else
    {
        m_pCurrentXmlElement = m_pCurrentXmlElement->NextSiblingElement();
    }

    if (NULL == m_pCurrentXmlElement)
    {
        this->GoToNextContainer();
        return STATUS_CODE_NOT_FOUND;
    }

    const std::string componentName(m_pCurrentXmlElement->ValueStr());

    if (std::string("CaloHit") == componentName)
    {
        return this->ReadCaloHit();
    }
    else if (std::string("Track") == componentName)
    {
        return this->ReadTrack();
    }
    else if (std::string("MCParticle") == componentName)
    {
        return this->ReadMCParticle();
    }
    else if (std::string("Relationship") == componentName)
    {
        return this->ReadRelationship();
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadSubDetector()
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    const TiXmlHandle xmlHandle(m_pCurrentXmlElement);

    std::string subDetectorName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "SubDetectorName", subDetectorName));
    unsigned int subDetectorTypeInput(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "SubDetectorType", subDetectorTypeInput));
    const SubDetectorType subDetectorType(static_cast<SubDetectorType>(subDetectorTypeInput));
    float innerRCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "InnerRCoordinate", innerRCoordinate));
    float innerZCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "InnerZCoordinate", innerZCoordinate));
    float innerPhiCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "InnerPhiCoordinate", innerPhiCoordinate));
    unsigned int innerSymmetryOrder(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "InnerSymmetryOrder", innerSymmetryOrder));
    float outerRCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "OuterRCoordinate", outerRCoordinate));
    float outerZCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "OuterZCoordinate", outerZCoordinate));
    float outerPhiCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "OuterPhiCoordinate", outerPhiCoordinate));
    unsigned int outerSymmetryOrder(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "OuterSymmetryOrder", outerSymmetryOrder));
    bool isMirroredInZ(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "IsMirroredInZ", isMirroredInZ));
    unsigned int nLayers(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "NLayers", nLayers));

    PandoraApi::Geometry::SubDetector::Parameters parameters;
    parameters.m_subDetectorName = subDetectorName;
    parameters.m_subDetectorType = subDetectorType;
    parameters.m_innerRCoordinate = innerRCoordinate;
    parameters.m_innerZCoordinate = innerZCoordinate;
    parameters.m_innerPhiCoordinate = innerPhiCoordinate;
    parameters.m_innerSymmetryOrder = innerSymmetryOrder;
    parameters.m_outerRCoordinate = outerRCoordinate;
    parameters.m_outerZCoordinate = outerZCoordinate;
    parameters.m_outerPhiCoordinate = outerPhiCoordinate;
    parameters.m_outerSymmetryOrder = outerSymmetryOrder;
    parameters.m_isMirroredInZ = isMirroredInZ;
    parameters.m_nLayers = nLayers;

    if (nLayers > 0)
    {
        FloatVector closestDistanceToIp, nRadiationLengths, nInteractionLengths;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle, "ClosestDistanceToIp", closestDistanceToIp));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle, "NRadiationLengths", nRadiationLengths));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle, "NInteractionLengths", nInteractionLengths));

        if ((closestDistanceToIp.size() != nLayers) || (nRadiationLengths.size() != nLayers) || (nInteractionLengths.size() != nLayers))
            return STATUS_CODE_FAILURE;

        for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
        {
            PandoraApi::Geometry::LayerParameters layerParameters;
            layerParameters.m_closestDistanceToIp = closestDistanceToIp[iLayer];
            layerParameters.m_nRadiationLengths = nRadiationLengths[iLayer];
            layerParameters.m_nInteractionLengths = nInteractionLengths[iLayer];
            parameters.m_layerParametersList.push_back(layerParameters);
        }
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::SubDetector::Create(*m_pPandora, parameters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadBoxGap()
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    const TiXmlHandle xmlHandle(m_pCurrentXmlElement);

    CartesianVector vertex(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Vertex", vertex));
    CartesianVector side1(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Side1", side1));
    CartesianVector side2(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Side2", side2));
    CartesianVector side3(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Side3", side3));

    PandoraApi::Geometry::BoxGap::Parameters parameters;
    parameters.m_vertex = vertex;
    parameters.m_side1 = side1;
    parameters.m_side2 = side2;
    parameters.m_side3 = side3;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::BoxGap::Create(*m_pPandora, parameters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadConcentricGap()
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    const TiXmlHandle xmlHandle(m_pCurrentXmlElement);

    float minZCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "MinZCoordinate", minZCoordinate));
    float maxZCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "MaxZCoordinate", maxZCoordinate));
    float innerRCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "InnerRCoordinate", innerRCoordinate));
    float innerPhiCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "InnerPhiCoordinate", innerPhiCoordinate));
    unsigned int innerSymmetryOrder(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "InnerSymmetryOrder", innerSymmetryOrder));
    float outerRCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "OuterRCoordinate", outerRCoordinate));
    float outerPhiCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "OuterPhiCoordinate", outerPhiCoordinate));
    unsigned int outerSymmetryOrder(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "OuterSymmetryOrder", outerSymmetryOrder));

    PandoraApi::Geometry::ConcentricGap::Parameters parameters;
    parameters.m_minZCoordinate = minZCoordinate;
    parameters.m_maxZCoordinate = maxZCoordinate;
    parameters.m_innerRCoordinate = innerRCoordinate;
    parameters.m_innerPhiCoordinate = innerPhiCoordinate;
    parameters.m_innerSymmetryOrder = innerSymmetryOrder;
    parameters.m_outerRCoordinate = outerRCoordinate;
    parameters.m_outerPhiCoordinate = outerPhiCoordinate;
    parameters.m_outerSymmetryOrder = outerSymmetryOrder;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::ConcentricGap::Create(*m_pPandora, parameters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadCaloHit()
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const TiXmlHandle xmlHandle(m_pCurrentXmlElement);

    unsigned int cellGeometryInput(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "CellGeometry", cellGeometryInput));
    const CellGeometry cellGeometry(static_cast<CellGeometry>(cellGeometryInput));
    CartesianVector positionVector(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "PositionVector", positionVector));
    CartesianVector expectedDirection(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "ExpectedDirection", expectedDirection));
    CartesianVector cellNormalVector(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "CellNormalVector", cellNormalVector));
    float cellThickness(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "CellThickness", cellThickness));
    float nCellRadiationLengths(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "NCellRadiationLengths", nCellRadiationLengths));
    float nCellInteractionLengths(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "NCellInteractionLengths", nCellInteractionLengths));
    float time(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Time", time));
    float inputEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "InputEnergy", inputEnergy));
    float mipEquivalentEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "MipEquivalentEnergy", mipEquivalentEnergy));
    float electromagneticEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "ElectromagneticEnergy", electromagneticEnergy));
    float hadronicEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "HadronicEnergy", hadronicEnergy));
    bool isDigital(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "IsDigital", isDigital));
    unsigned int hitTypeInput(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "HitType", hitTypeInput));
    const HitType hitType(static_cast<HitType>(hitTypeInput));
    unsigned int hitRegionInput(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "HitRegion", hitRegionInput));
    const HitRegion hitRegion(static_cast<HitRegion>(hitRegionInput));
    unsigned int layer(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Layer", layer));
    bool isInOuterSamplingLayer(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "IsInOuterSamplingLayer", isInOuterSamplingLayer));
    const void *pParentAddress(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "ParentCaloHitAddress", pParentAddress));

    PandoraApi::RectangularCaloHit::Parameters rectangularParameters;
    PandoraApi::PointingCaloHit::Parameters pointingParameters;

    if (RECTANGULAR == cellGeometry)
    {
        float cellSizeU(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "CellSizeU", cellSizeU));
        float cellSizeV(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "CellSizeV", cellSizeV));

        rectangularParameters.m_cellSizeU = cellSizeU;
        rectangularParameters.m_cellSizeV = cellSizeV;
    }
    else if (POINTING == cellGeometry)
    {
        float cellSizeEta(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "CellSizeEta", cellSizeEta));
        float cellSizePhi(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "CellSizePhi", cellSizePhi));

        pointingParameters.m_cellSizeEta = cellSizeEta;
        pointingParameters.m_cellSizePhi = cellSizePhi;
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    PandoraApi::CaloHitBaseParameters *const pBaseParameters((RECTANGULAR == cellGeometry) ?
        static_cast<PandoraApi::CaloHitBaseParameters*>(&rectangularParameters) :
        static_cast<PandoraApi::CaloHitBaseParameters*>(&pointingParameters));

    pBaseParameters->m_positionVector = positionVector;
    pBaseParameters->m_expectedDirection = expectedDirection;
    pBaseParameters->m_cellNormalVector = cellNormalVector;
    pBaseParameters->m_cellThickness = cellThickness;
    pBaseParameters->m_nCellRadiationLengths = nCellRadiationLengths;
    pBaseParameters->m_nCellInteractionLengths = nCellInteractionLengths;
    pBaseParameters->m_time = time;
    pBaseParameters->m_inputEnergy = inputEnergy;
    pBaseParameters->m_mipEquivalentEnergy = mipEquivalentEnergy;
    pBaseParameters->m_electromagneticEnergy = electromagneticEnergy;
    pBaseParameters->m_hadronicEnergy = hadronicEnergy;
    pBaseParameters->m_isDigital = isDigital;
    pBaseParameters->m_hitType = hitType;
    pBaseParameters->m_hitRegion = hitRegion;
    pBaseParameters->m_layer = layer;
    pBaseParameters->m_isInOuterSamplingLayer = isInOuterSamplingLayer;
    pBaseParameters->m_pParentAddress = pParentAddress;

    if (RECTANGULAR == cellGeometry)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::RectangularCaloHit::Create(*m_pPandora, rectangularParameters));
    }
    else
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::PointingCaloHit::Create(*m_pPandora, pointingParameters));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadTrack()
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const TiXmlHandle xmlHandle(m_pCurrentXmlElement);

    float d0(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "D0", d0));
    float z0(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Z0", z0));
    int particleId(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "ParticleId", particleId));
    int charge(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Charge", charge));
    float mass(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Mass", mass));
    CartesianVector momentumAtDca(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "MomentumAtDca", momentumAtDca));
    TrackState trackStateAtStart(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "TrackStateAtStart", trackStateAtStart));
    TrackState trackStateAtEnd(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "TrackStateAtEnd", trackStateAtEnd));
    TrackState trackStateAtCalorimeter(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "TrackStateAtCalorimeter", trackStateAtCalorimeter));
    float timeAtCalorimeter(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "TimeAtCalorimeter", timeAtCalorimeter));
    bool reachesCalorimeter(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "ReachesCalorimeter", reachesCalorimeter));
    bool isProjectedToEndCap(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "IsProjectedToEndCap", isProjectedToEndCap));
    bool canFormPfo(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "CanFormPfo", canFormPfo));
    bool canFormClusterlessPfo(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "CanFormClusterlessPfo", canFormClusterlessPfo));
    const void *pParentAddress(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "ParentTrackAddress", pParentAddress));

    PandoraApi::Track::Parameters parameters;
    parameters.m_d0 = d0;
    parameters.m_z0 = z0;
    parameters.m_particleId = particleId;
    parameters.m_charge = charge;
    parameters.m_mass = mass;
    parameters.m_momentumAtDca = momentumAtDca;
    parameters.m_trackStateAtStart = trackStateAtStart;
    parameters.m_trackStateAtEnd = trackStateAtEnd;
    parameters.m_trackStateAtCalorimeter = trackStateAtCalorimeter;
    parameters.m_timeAtCalorimeter = timeAtCalorimeter;
    parameters.m_reachesCalorimeter = reachesCalorimeter;
    parameters.m_isProjectedToEndCap = isProjectedToEndCap;
    parameters.m_canFormPfo = canFormPfo;
    parameters.m_canFormClusterlessPfo = canFormClusterlessPfo;
    parameters.m_pParentAddress = pParentAddress;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Track::Create(*m_pPandora, parameters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadMCParticle()
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const TiXmlHandle xmlHandle(m_pCurrentXmlElement);

    float energy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Energy", energy));
    CartesianVector momentum(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Momentum", momentum));
    CartesianVector vertex(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Vertex", vertex));
    CartesianVector endpoint(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Endpoint", endpoint));
    int particleId(-std::numeric_limits<int>::max());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "ParticleId", particleId));
    unsigned int mcParticleTypeInput(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "MCParticleType", mcParticleTypeInput));
    const MCParticleType mcParticleType(static_cast<MCParticleType>(mcParticleTypeInput));
    const void *pParentAddress(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Uid", pParentAddress));

    PandoraApi::MCParticle::Parameters parameters;
    parameters.m_energy = energy;
    parameters.m_momentum = momentum;
    parameters.m_vertex = vertex;
    parameters.m_endpoint = endpoint;
    parameters.m_particleId = particleId;
    parameters.m_mcParticleType = mcParticleType;
    parameters.m_pParentAddress = pParentAddress;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::MCParticle::Create(*m_pPandora, parameters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadRelationship()
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const TiXmlHandle xmlHandle(m_pCurrentXmlElement);

    unsigned int relationshipIdInput(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "RelationshipId", relationshipIdInput));
    const RelationshipId relationshipId(static_cast<RelationshipId>(relationshipIdInput));
    const void *address1(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Address1", address1));
    const void *address2(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Address2", address2));
    float weight(1.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "Weight", weight));

    switch (relationshipId)
    {
    case CALO_HIT_TO_MC:
        return PandoraApi::SetCaloHitToMCParticleRelationship(*m_pPandora, address1, address2, weight);
    case TRACK_TO_MC:
        return PandoraApi::SetTrackToMCParticleRelationship(*m_pPandora, address1, address2, weight);
    case MC_PARENT_DAUGHTER:
        return PandoraApi::SetMCParentDaughterRelationship(*m_pPandora, address1, address2);
    case TRACK_PARENT_DAUGHTER:
        return PandoraApi::SetTrackParentDaughterRelationship(*m_pPandora, address1, address2);
    case TRACK_SIBLING:
        return PandoraApi::SetTrackSiblingRelationship(*m_pPandora, address1, address2);
    default:
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
