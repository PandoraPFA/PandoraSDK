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

namespace pandora
{

XmlFileReader::XmlFileReader(const pandora::Pandora &pandora, const std::string &fileName) :
    FileReader(pandora, fileName),
    m_pContainerXmlNode(nullptr),
    m_pCurrentXmlElement(nullptr),
    m_isAtFileStart(true)
{
    m_fileType = XML;
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
    m_pCurrentXmlElement = nullptr;
    m_containerId = this->GetNextContainerId();

    if ((EVENT_CONTAINER != m_containerId) && (GEOMETRY_CONTAINER != m_containerId))
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::GoToNextContainer()
{
    m_pCurrentXmlElement = nullptr;

    if (m_isAtFileStart)
    {
        if (!m_pContainerXmlNode)
            m_pContainerXmlNode = TiXmlHandle(m_pXmlDocument).FirstChildElement().Element();

        m_isAtFileStart = false;
    }
    else
    {
        if (!m_pContainerXmlNode)
            throw StatusCodeException(STATUS_CODE_NOT_FOUND);

        m_pContainerXmlNode = m_pContainerXmlNode->NextSibling();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

ContainerId XmlFileReader::GetNextContainerId()
{
    const std::string containerId((nullptr != m_pContainerXmlNode) ? m_pContainerXmlNode->ValueStr() : "");

    if (std::string("Event") == containerId)
    {
        return EVENT_CONTAINER;
    }
    else if (std::string("Geometry") == containerId)
    {
        return GEOMETRY_CONTAINER;
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

StatusCode XmlFileReader::ReadNextGeometryComponent()
{
    if (!m_pCurrentXmlElement)
    {
        TiXmlHandle localHandle(m_pContainerXmlNode);
        m_pCurrentXmlElement = localHandle.FirstChild().Element();
    }
    else
    {
        m_pCurrentXmlElement = m_pCurrentXmlElement->NextSiblingElement();
    }

    if (!m_pCurrentXmlElement)
    {
        this->GoToNextContainer();
        return STATUS_CODE_NOT_FOUND;
    }

    const std::string componentName(m_pCurrentXmlElement->ValueStr());

    if (std::string("SubDetector") == componentName)
    {
        return this->ReadSubDetector();
    }
    if (std::string("LArTPC") == componentName)
    {
        return this->ReadLArTPC();
    }
    if (std::string("LineGap") == componentName)
    {
        return this->ReadLineGap();
    }
    else if (std::string("BoxGap") == componentName)
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
    if (!m_pCurrentXmlElement)
    {
        TiXmlHandle localHandle(m_pContainerXmlNode);
        m_pCurrentXmlElement = localHandle.FirstChild().Element();
    }
    else
    {
        m_pCurrentXmlElement = m_pCurrentXmlElement->NextSiblingElement();
    }

    if (!m_pCurrentXmlElement)
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
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::SubDetector::Parameters *pParameters = m_pSubDetectorFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pSubDetectorFactory->Read(*pParameters, *this));

        std::string subDetectorName;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("SubDetectorName", subDetectorName));
        unsigned int subDetectorTypeInput(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("SubDetectorType", subDetectorTypeInput));
        const SubDetectorType subDetectorType(static_cast<SubDetectorType>(subDetectorTypeInput));
        float innerRCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("InnerRCoordinate", innerRCoordinate));
        float innerZCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("InnerZCoordinate", innerZCoordinate));
        float innerPhiCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("InnerPhiCoordinate", innerPhiCoordinate));
        unsigned int innerSymmetryOrder(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("InnerSymmetryOrder", innerSymmetryOrder));
        float outerRCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("OuterRCoordinate", outerRCoordinate));
        float outerZCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("OuterZCoordinate", outerZCoordinate));
        float outerPhiCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("OuterPhiCoordinate", outerPhiCoordinate));
        unsigned int outerSymmetryOrder(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("OuterSymmetryOrder", outerSymmetryOrder));
        bool isMirroredInZ(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("IsMirroredInZ", isMirroredInZ));
        unsigned int nLayers(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("NLayers", nLayers));

        pParameters->m_subDetectorName = subDetectorName;
        pParameters->m_subDetectorType = subDetectorType;
        pParameters->m_innerRCoordinate = innerRCoordinate;
        pParameters->m_innerZCoordinate = innerZCoordinate;
        pParameters->m_innerPhiCoordinate = innerPhiCoordinate;
        pParameters->m_innerSymmetryOrder = innerSymmetryOrder;
        pParameters->m_outerRCoordinate = outerRCoordinate;
        pParameters->m_outerZCoordinate = outerZCoordinate;
        pParameters->m_outerPhiCoordinate = outerPhiCoordinate;
        pParameters->m_outerSymmetryOrder = outerSymmetryOrder;
        pParameters->m_isMirroredInZ = isMirroredInZ;
        pParameters->m_nLayers = nLayers;

        if (nLayers > 0)
        {
            FloatVector closestDistanceToIp, nRadiationLengths, nInteractionLengths;
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("ClosestDistanceToIp", closestDistanceToIp));
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("NRadiationLengths", nRadiationLengths));
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("NInteractionLengths", nInteractionLengths));

            if ((closestDistanceToIp.size() != nLayers) || (nRadiationLengths.size() != nLayers) || (nInteractionLengths.size() != nLayers))
                throw StatusCodeException(STATUS_CODE_FAILURE);

            for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
            {
                PandoraApi::Geometry::LayerParameters layerParameters;
                layerParameters.m_closestDistanceToIp = closestDistanceToIp[iLayer];
                layerParameters.m_nRadiationLengths = nRadiationLengths[iLayer];
                layerParameters.m_nInteractionLengths = nInteractionLengths[iLayer];
                pParameters->m_layerParametersVector.push_back(layerParameters);
            }
        }

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::SubDetector::Create(*m_pPandora, *pParameters, *m_pSubDetectorFactory));
        delete pParameters;
    }
    catch (StatusCodeException &statusCodeException)
    {
        delete pParameters;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadLArTPC()
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::LArTPC::Parameters *pParameters = m_pLArTPCFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pLArTPCFactory->Read(*pParameters, *this));

        unsigned int larTPCVolumeId;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("LArTPCVolumeId", larTPCVolumeId));
        float centerX(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("CenterX", centerX));
        float centerY(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("CenterY", centerY));
        float centerZ(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("CenterZ", centerZ));
        float widthX(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("WidthX", widthX));
        float widthY(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("WidthY", widthY));
        float widthZ(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("WidthZ", widthZ));
        float wirePitchU(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("WirePitchU", wirePitchU));
        float wirePitchV(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("WirePitchV", wirePitchV));
        float wirePitchW(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("WirePitchW", wirePitchW));
        float wireAngleU(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("WireAngleU", wireAngleU));
        float wireAngleV(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("WireAngleV", wireAngleV));
        float wireAngleW(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("WireAngleW", wireAngleW));
        float sigmaUVW(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("SigmaUVW", sigmaUVW));
        bool isDriftInPositiveX(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("IsDriftInPositiveX", isDriftInPositiveX));

        pParameters->m_larTPCVolumeId = larTPCVolumeId;
        pParameters->m_centerX = centerX;
        pParameters->m_centerY = centerY;
        pParameters->m_centerZ = centerZ;
        pParameters->m_widthX = widthX;
        pParameters->m_widthY = widthY;
        pParameters->m_widthZ = widthZ;
        pParameters->m_wirePitchU = wirePitchU;
        pParameters->m_wirePitchV = wirePitchV;
        pParameters->m_wirePitchW = wirePitchW;
        pParameters->m_wireAngleU = wireAngleU;
        pParameters->m_wireAngleV = wireAngleV;
        pParameters->m_wireAngleW = wireAngleW;
        pParameters->m_sigmaUVW = sigmaUVW;
        pParameters->m_isDriftInPositiveX = isDriftInPositiveX;

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::LArTPC::Create(*m_pPandora, *pParameters, *m_pLArTPCFactory));
        delete pParameters;
    }
    catch (StatusCodeException &statusCodeException)
    {
        delete pParameters;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadLineGap()
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::LineGap::Parameters *pParameters = m_pLineGapFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pLineGapFactory->Read(*pParameters, *this));

        unsigned int lineGapTypeInput(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("LineGapType", lineGapTypeInput));
        const LineGapType lineGapType(static_cast<LineGapType>(lineGapTypeInput));
        float lineStartX(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("LineStartX", lineStartX));
        float lineEndX(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("LineEndX", lineEndX));
        float lineStartZ(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("LineStartZ", lineStartZ));
        float lineEndZ(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("LineEndZ", lineEndZ));

        pParameters->m_lineGapType = lineGapType;
        pParameters->m_lineStartX = lineStartX;
        pParameters->m_lineEndX = lineEndX;
        pParameters->m_lineStartZ = lineStartZ;
        pParameters->m_lineEndZ = lineEndZ;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::LineGap::Create(*m_pPandora, *pParameters, *m_pLineGapFactory));
        delete pParameters;
    }
    catch (StatusCodeException &statusCodeException)
    {
        delete pParameters;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadBoxGap()
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::BoxGap::Parameters *pParameters = m_pBoxGapFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pBoxGapFactory->Read(*pParameters, *this));

        CartesianVector vertex(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Vertex", vertex));
        CartesianVector side1(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Side1", side1));
        CartesianVector side2(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Side2", side2));
        CartesianVector side3(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Side3", side3));

        pParameters->m_vertex = vertex;
        pParameters->m_side1 = side1;
        pParameters->m_side2 = side2;
        pParameters->m_side3 = side3;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::BoxGap::Create(*m_pPandora, *pParameters, *m_pBoxGapFactory));
        delete pParameters;
    }
    catch (StatusCodeException &statusCodeException)
    {
        delete pParameters;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadConcentricGap()
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::ConcentricGap::Parameters *pParameters = m_pConcentricGapFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pConcentricGapFactory->Read(*pParameters, *this));

        float minZCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("MinZCoordinate", minZCoordinate));
        float maxZCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("MaxZCoordinate", maxZCoordinate));
        float innerRCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("InnerRCoordinate", innerRCoordinate));
        float innerPhiCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("InnerPhiCoordinate", innerPhiCoordinate));
        unsigned int innerSymmetryOrder(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("InnerSymmetryOrder", innerSymmetryOrder));
        float outerRCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("OuterRCoordinate", outerRCoordinate));
        float outerPhiCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("OuterPhiCoordinate", outerPhiCoordinate));
        unsigned int outerSymmetryOrder(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("OuterSymmetryOrder", outerSymmetryOrder));

        pParameters->m_minZCoordinate = minZCoordinate;
        pParameters->m_maxZCoordinate = maxZCoordinate;
        pParameters->m_innerRCoordinate = innerRCoordinate;
        pParameters->m_innerPhiCoordinate = innerPhiCoordinate;
        pParameters->m_innerSymmetryOrder = innerSymmetryOrder;
        pParameters->m_outerRCoordinate = outerRCoordinate;
        pParameters->m_outerPhiCoordinate = outerPhiCoordinate;
        pParameters->m_outerSymmetryOrder = outerSymmetryOrder;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::ConcentricGap::Create(*m_pPandora, *pParameters, *m_pConcentricGapFactory));
        delete pParameters;
    }
    catch (StatusCodeException &statusCodeException)
    {
        delete pParameters;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadCaloHit()
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::CaloHit::Parameters *pParameters = m_pCaloHitFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCaloHitFactory->Read(*pParameters, *this));

        unsigned int cellGeometryInput(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("CellGeometry", cellGeometryInput));
        const CellGeometry cellGeometry(static_cast<CellGeometry>(cellGeometryInput));
        CartesianVector positionVector(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("PositionVector", positionVector));
        CartesianVector expectedDirection(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("ExpectedDirection", expectedDirection));
        CartesianVector cellNormalVector(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("CellNormalVector", cellNormalVector));
        float cellThickness(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("CellThickness", cellThickness));
        float nCellRadiationLengths(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("NCellRadiationLengths", nCellRadiationLengths));
        float nCellInteractionLengths(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("NCellInteractionLengths", nCellInteractionLengths));
        float time(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Time", time));
        float inputEnergy(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("InputEnergy", inputEnergy));
        float mipEquivalentEnergy(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("MipEquivalentEnergy", mipEquivalentEnergy));
        float electromagneticEnergy(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("ElectromagneticEnergy", electromagneticEnergy));
        float hadronicEnergy(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("HadronicEnergy", hadronicEnergy));
        bool isDigital(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("IsDigital", isDigital));
        unsigned int hitTypeInput(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("HitType", hitTypeInput));
        const HitType hitType(static_cast<HitType>(hitTypeInput));
        unsigned int hitRegionInput(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("HitRegion", hitRegionInput));
        const HitRegion hitRegion(static_cast<HitRegion>(hitRegionInput));
        unsigned int layer(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Layer", layer));
        bool isInOuterSamplingLayer(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("IsInOuterSamplingLayer", isInOuterSamplingLayer));
        const void *pParentAddress(nullptr);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("ParentCaloHitAddress", pParentAddress));
        float cellSize0(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("CellSize0", cellSize0));
        float cellSize1(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("CellSize1", cellSize1));

        pParameters->m_positionVector = positionVector;
        pParameters->m_expectedDirection = expectedDirection;
        pParameters->m_cellNormalVector = cellNormalVector;
        pParameters->m_cellGeometry = cellGeometry;
        pParameters->m_cellSize0 = cellSize0;
        pParameters->m_cellSize1 = cellSize1;
        pParameters->m_cellThickness = cellThickness;
        pParameters->m_nCellRadiationLengths = nCellRadiationLengths;
        pParameters->m_nCellInteractionLengths = nCellInteractionLengths;
        pParameters->m_time = time;
        pParameters->m_inputEnergy = inputEnergy;
        pParameters->m_mipEquivalentEnergy = mipEquivalentEnergy;
        pParameters->m_electromagneticEnergy = electromagneticEnergy;
        pParameters->m_hadronicEnergy = hadronicEnergy;
        pParameters->m_isDigital = isDigital;
        pParameters->m_hitType = hitType;
        pParameters->m_hitRegion = hitRegion;
        pParameters->m_layer = layer;
        pParameters->m_isInOuterSamplingLayer = isInOuterSamplingLayer;
        pParameters->m_pParentAddress = pParentAddress;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::CaloHit::Create(*m_pPandora, *pParameters, *m_pCaloHitFactory));
        delete pParameters;
    }
    catch (StatusCodeException &statusCodeException)
    {
        delete pParameters;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadTrack()
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Track::Parameters *pParameters = m_pTrackFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pTrackFactory->Read(*pParameters, *this));

        float d0(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("D0", d0));
        float z0(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Z0", z0));
        int particleId(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("ParticleId", particleId));
        int charge(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Charge", charge));
        float mass(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Mass", mass));
        CartesianVector momentumAtDca(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("MomentumAtDca", momentumAtDca));
        TrackState trackStateAtStart(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("TrackStateAtStart", trackStateAtStart));
        TrackState trackStateAtEnd(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("TrackStateAtEnd", trackStateAtEnd));
        TrackState trackStateAtCalorimeter(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("TrackStateAtCalorimeter", trackStateAtCalorimeter));
        float timeAtCalorimeter(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("TimeAtCalorimeter", timeAtCalorimeter));
        bool reachesCalorimeter(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("ReachesCalorimeter", reachesCalorimeter));
        bool isProjectedToEndCap(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("IsProjectedToEndCap", isProjectedToEndCap));
        bool canFormPfo(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("CanFormPfo", canFormPfo));
        bool canFormClusterlessPfo(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("CanFormClusterlessPfo", canFormClusterlessPfo));
        const void *pParentAddress(nullptr);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("ParentTrackAddress", pParentAddress));

        pParameters->m_d0 = d0;
        pParameters->m_z0 = z0;
        pParameters->m_particleId = particleId;
        pParameters->m_charge = charge;
        pParameters->m_mass = mass;
        pParameters->m_momentumAtDca = momentumAtDca;
        pParameters->m_trackStateAtStart = trackStateAtStart;
        pParameters->m_trackStateAtEnd = trackStateAtEnd;
        pParameters->m_trackStateAtCalorimeter = trackStateAtCalorimeter;
        pParameters->m_timeAtCalorimeter = timeAtCalorimeter;
        pParameters->m_reachesCalorimeter = reachesCalorimeter;
        pParameters->m_isProjectedToEndCap = isProjectedToEndCap;
        pParameters->m_canFormPfo = canFormPfo;
        pParameters->m_canFormClusterlessPfo = canFormClusterlessPfo;
        pParameters->m_pParentAddress = pParentAddress;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Track::Create(*m_pPandora, *pParameters, *m_pTrackFactory));
        delete pParameters;
    }
    catch (StatusCodeException &statusCodeException)
    {
        delete pParameters;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadMCParticle()
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::MCParticle::Parameters *pParameters = m_pMCParticleFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pMCParticleFactory->Read(*pParameters, *this));

        float energy(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Energy", energy));
        CartesianVector momentum(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Momentum", momentum));
        CartesianVector vertex(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Vertex", vertex));
        CartesianVector endpoint(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Endpoint", endpoint));
        int particleId(-std::numeric_limits<int>::max());
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("ParticleId", particleId));
        unsigned int mcParticleTypeInput(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("MCParticleType", mcParticleTypeInput));
        const MCParticleType mcParticleType(static_cast<MCParticleType>(mcParticleTypeInput));
        const void *pParentAddress(nullptr);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Uid", pParentAddress));

        pParameters->m_energy = energy;
        pParameters->m_momentum = momentum;
        pParameters->m_vertex = vertex;
        pParameters->m_endpoint = endpoint;
        pParameters->m_particleId = particleId;
        pParameters->m_mcParticleType = mcParticleType;
        pParameters->m_pParentAddress = pParentAddress;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::MCParticle::Create(*m_pPandora, *pParameters, *m_pMCParticleFactory));
        delete pParameters;
    }
    catch (StatusCodeException &statusCodeException)
    {
        delete pParameters;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode XmlFileReader::ReadRelationship()
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    unsigned int relationshipIdInput(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("RelationshipId", relationshipIdInput));
    const RelationshipId relationshipId(static_cast<RelationshipId>(relationshipIdInput));
    const void *address1(nullptr);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Address1", address1));
    const void *address2(nullptr);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Address2", address2));
    float weight(1.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable("Weight", weight));

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

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
