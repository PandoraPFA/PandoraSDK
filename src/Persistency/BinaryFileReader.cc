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

    if ((EVENT_CONTAINER != m_containerId) && (GEOMETRY_CONTAINER != m_containerId))
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
    const StatusCode fileHashStatusCode(this->ReadVariable(fileHash));

    if (STATUS_CODE_SUCCESS != fileHashStatusCode)
        throw StatusCodeException(fileHashStatusCode);

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

StatusCode BinaryFileReader::ReadNextGeometryComponent()
{
    ComponentId componentId(UNKNOWN_COMPONENT);
    const StatusCode statusCode(this->ReadVariable(componentId));

    if (STATUS_CODE_SUCCESS != statusCode)
    {
        if (STATUS_CODE_NOT_FOUND != statusCode)
            throw StatusCodeException(statusCode);

        return STATUS_CODE_NOT_FOUND;
    }

    switch (componentId)
    {
    case SUB_DETECTOR_COMPONENT:
        return this->ReadSubDetector(false);
    case LAR_TPC_COMPONENT:
        return this->ReadLArTPC(false);
    case LINE_GAP_COMPONENT:
        return this->ReadLineGap(false);
    case BOX_GAP_COMPONENT:
        return this->ReadBoxGap(false);
    case CONCENTRIC_GAP_COMPONENT:
        return this->ReadConcentricGap(false);
    case GEOMETRY_END_COMPONENT:
        m_containerId = UNKNOWN_CONTAINER;
        return STATUS_CODE_NOT_FOUND;
    default:
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadNextEventComponent()
{
    ComponentId componentId(UNKNOWN_COMPONENT);
    const StatusCode statusCode(this->ReadVariable(componentId));

    if (STATUS_CODE_SUCCESS != statusCode)
    {
        if (STATUS_CODE_NOT_FOUND != statusCode)
            throw StatusCodeException(statusCode);

        return STATUS_CODE_NOT_FOUND;
    }

    switch (componentId)
    {
    case CALO_HIT_COMPONENT:
        return this->ReadCaloHit(false);
    case TRACK_COMPONENT:
        return this->ReadTrack(false);
    case MC_PARTICLE_COMPONENT:
        return this->ReadMCParticle(false);
    case RELATIONSHIP_COMPONENT:
        return this->ReadRelationship(false);
    case EVENT_END_COMPONENT:
        m_containerId = UNKNOWN_CONTAINER;
        return STATUS_CODE_NOT_FOUND;
    default:
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BinaryFileReader::ReadSubDetector(bool checkComponentId)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (SUB_DETECTOR_COMPONENT != componentId)
            return STATUS_CODE_FAILURE;
    }

    PandoraApi::Geometry::SubDetector::Parameters *pParameters = m_pSubDetectorFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pSubDetectorFactory->Read(*pParameters, *this));

        std::string subDetectorName;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(subDetectorName));
        SubDetectorType subDetectorType(SUB_DETECTOR_OTHER);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(subDetectorType));
        float innerRCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerRCoordinate));
        float innerZCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerZCoordinate));
        float innerPhiCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerPhiCoordinate));
        unsigned int innerSymmetryOrder(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerSymmetryOrder));
        float outerRCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerRCoordinate));
        float outerZCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerZCoordinate));
        float outerPhiCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerPhiCoordinate));
        unsigned int outerSymmetryOrder(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerSymmetryOrder));
        bool isMirroredInZ(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isMirroredInZ));
        unsigned int nLayers(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nLayers));

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

        for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
        {
            float closestDistanceToIp(0.f);
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(closestDistanceToIp));
            float nRadiationLengths(0.f);
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nRadiationLengths));
            float nInteractionLengths(0.f);
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nInteractionLengths));

            PandoraApi::Geometry::LayerParameters layerParameters;
            layerParameters.m_closestDistanceToIp = closestDistanceToIp;
            layerParameters.m_nRadiationLengths = nRadiationLengths;
            layerParameters.m_nInteractionLengths = nInteractionLengths;
            pParameters->m_layerParametersVector.push_back(layerParameters);
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

StatusCode BinaryFileReader::ReadLArTPC(bool checkComponentId)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (LAR_TPC_COMPONENT != componentId)
            return STATUS_CODE_FAILURE;
    }

    PandoraApi::Geometry::LArTPC::Parameters *pParameters = m_pLArTPCFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pLArTPCFactory->Read(*pParameters, *this));

        unsigned int larTPCVolumeId;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(larTPCVolumeId));
        float centerX(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(centerX));
        float centerY(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(centerY));
        float centerZ(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(centerZ));
        float widthX(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(widthX));
        float widthY(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(widthY));
        float widthZ(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(widthZ));
        float wirePitchU(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(wirePitchU));
        float wirePitchV(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(wirePitchV));
        float wirePitchW(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(wirePitchW));
        float wireAngleU(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(wireAngleU));
        float wireAngleV(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(wireAngleV));
        float wireAngleW(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(wireAngleW));
        float sigmaUVW(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(sigmaUVW));
        bool isDriftInPositiveX(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isDriftInPositiveX));

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

StatusCode BinaryFileReader::ReadLineGap(bool checkComponentId)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (LINE_GAP_COMPONENT != componentId)
            return STATUS_CODE_FAILURE;
    }

    PandoraApi::Geometry::LineGap::Parameters *pParameters = m_pLineGapFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pLineGapFactory->Read(*pParameters, *this));

        LineGapType lineGapType(TPC_WIRE_GAP_VIEW_U);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(lineGapType));
        float lineStartX(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(lineStartX));
        float lineEndX(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(lineEndX));
        float lineStartZ(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(lineStartZ));
        float lineEndZ(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(lineEndZ));

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

StatusCode BinaryFileReader::ReadBoxGap(bool checkComponentId)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (BOX_GAP_COMPONENT != componentId)
            return STATUS_CODE_FAILURE;
    }

    PandoraApi::Geometry::BoxGap::Parameters *pParameters = m_pBoxGapFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pBoxGapFactory->Read(*pParameters, *this));

        CartesianVector vertex(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(vertex));
        CartesianVector side1(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(side1));
        CartesianVector side2(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(side2));
        CartesianVector side3(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(side3));

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

StatusCode BinaryFileReader::ReadConcentricGap(bool checkComponentId)
{
    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (CONCENTRIC_GAP_COMPONENT != componentId)
                return STATUS_CODE_FAILURE;
    }

    PandoraApi::Geometry::ConcentricGap::Parameters *pParameters = m_pConcentricGapFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pConcentricGapFactory->Read(*pParameters, *this));

        float minZCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(minZCoordinate));
        float maxZCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(maxZCoordinate));
        float innerRCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerRCoordinate));
        float innerPhiCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerPhiCoordinate));
        unsigned int innerSymmetryOrder(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerSymmetryOrder));
        float outerRCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerRCoordinate));
        float outerPhiCoordinate(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerPhiCoordinate));
        unsigned int outerSymmetryOrder(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerSymmetryOrder));

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

StatusCode BinaryFileReader::ReadCaloHit(bool checkComponentId)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (CALO_HIT_COMPONENT != componentId)
            return STATUS_CODE_FAILURE;
    }

    PandoraApi::CaloHit::Parameters *pParameters = m_pCaloHitFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCaloHitFactory->Read(*pParameters, *this));

        CellGeometry cellGeometry(RECTANGULAR);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellGeometry));
        CartesianVector positionVector(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(positionVector));
        CartesianVector expectedDirection(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(expectedDirection));
        CartesianVector cellNormalVector(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellNormalVector));
        float cellThickness(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellThickness));
        float nCellRadiationLengths(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nCellRadiationLengths));
        float nCellInteractionLengths(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nCellInteractionLengths));
        float time(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(time));
        float inputEnergy(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(inputEnergy));
        float mipEquivalentEnergy(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mipEquivalentEnergy));
        float electromagneticEnergy(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(electromagneticEnergy));
        float hadronicEnergy(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(hadronicEnergy));
        bool isDigital(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isDigital));
        HitType hitType(ECAL);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(hitType));
        HitRegion hitRegion(BARREL);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(hitRegion));
        unsigned int layer(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(layer));
        bool isInOuterSamplingLayer(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isInOuterSamplingLayer));
        const void *pParentAddress(nullptr);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(pParentAddress));
        float cellSize0(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellSize0));
        float cellSize1(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellSize1));

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

StatusCode BinaryFileReader::ReadTrack(bool checkComponentId)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (TRACK_COMPONENT != componentId)
            return STATUS_CODE_FAILURE;
    }

    PandoraApi::Track::Parameters *pParameters = m_pTrackFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pTrackFactory->Read(*pParameters, *this));

        float d0(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(d0));
        float z0(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(z0));
        int particleId(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(particleId));
        int charge(0);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(charge));
        float mass(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mass));
        CartesianVector momentumAtDca(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(momentumAtDca));
        TrackState trackStateAtStart(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(trackStateAtStart));
        TrackState trackStateAtEnd(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(trackStateAtEnd));
        TrackState trackStateAtCalorimeter(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(trackStateAtCalorimeter));
        float timeAtCalorimeter(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(timeAtCalorimeter));
        bool reachesCalorimeter(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(reachesCalorimeter));
        bool isProjectedToEndCap(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isProjectedToEndCap));
        bool canFormPfo(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(canFormPfo));
        bool canFormClusterlessPfo(false);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(canFormClusterlessPfo));
        const void *pParentAddress(nullptr);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(pParentAddress));

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

StatusCode BinaryFileReader::ReadMCParticle(bool checkComponentId)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (MC_PARTICLE_COMPONENT != componentId)
            return STATUS_CODE_FAILURE;
    }

    PandoraApi::MCParticle::Parameters *pParameters = m_pMCParticleFactory->NewParameters();

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pMCParticleFactory->Read(*pParameters, *this));

        float energy(0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(energy));
        CartesianVector momentum(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(momentum));
        CartesianVector vertex(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(vertex));
        CartesianVector endpoint(0.f, 0.f, 0.f);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(endpoint));
        int particleId(-std::numeric_limits<int>::max());
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(particleId));
        MCParticleType mcParticleType(MC_3D);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mcParticleType));
        const void *pParentAddress(nullptr);
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(pParentAddress));

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

StatusCode BinaryFileReader::ReadRelationship(bool checkComponentId)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (RELATIONSHIP_COMPONENT != componentId)
            return STATUS_CODE_FAILURE;
    }

    RelationshipId relationshipId(UNKNOWN_RELATIONSHIP);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(relationshipId));
    const void *address1(nullptr);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(address1));
    const void *address2(nullptr);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(address2));
    float weight(1.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(weight));

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
