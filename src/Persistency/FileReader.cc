/**
 *  @file   PandoraPFANew/Framework/src/Persistency/FileReader.cc
 * 
 *  @brief  Implementation of the file reader class.
 * 
 *  $Log: $
 */

#include "Api/PandoraApi.h"

#include "Objects/CaloHit.h"
#include "Objects/Track.h"

#include "Persistency/FileReader.h"

namespace pandora
{

FileReader::FileReader(const pandora::Pandora &pandora, const std::string &fileName) :
    m_pPandora(&pandora),
    m_containerId(UNKNOWN_CONTAINER),
    m_containerPosition(0),
    m_containerSize(0)
{
    m_fileStream.open(fileName.c_str(), std::ios::in | std::ios::binary);

    if (!m_fileStream.is_open() || !m_fileStream.good())
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

FileReader::~FileReader()
{
    m_fileStream.close();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadGeometry()
{
    if (GEOMETRY != this->GetNextContainerId())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextGeometry());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadHeader());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadGeometryParameters());

    try
    {
        while (STATUS_CODE_SUCCESS == this->ReadNextGeometryComponent())
            continue;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << " FileReader::ReadGeometry() encountered unrecognized object in file: " << statusCodeException.ToString() << std::endl;
    }

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadEvent()
{
    if (EVENT != this->GetNextContainerId())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextEvent());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadHeader());

    try
    {
        while (STATUS_CODE_SUCCESS == this->ReadNextEventComponent())
            continue;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << " FileReader::ReadEvent() encountered unrecognized object in file: " << statusCodeException.ToString() << std::endl;
    }

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::GoToNextGeometry()
{
    do
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextContainer());
    }
    while (GEOMETRY != this->GetNextContainerId());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::GoToNextEvent()
{
    do
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextContainer());
    }
    while (EVENT != this->GetNextContainerId());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::GoToGeometry(const unsigned int geometryNumber)
{
    int nGeometriesRead(0);
    m_fileStream.seekg(0, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

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

StatusCode FileReader::GoToEvent(const unsigned int eventNumber)
{
    int nEventsRead(0);
    m_fileStream.seekg(0, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

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

StatusCode FileReader::GoToNextContainer()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadHeader());
    m_fileStream.seekg(m_containerPosition + m_containerSize, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadHeader()
{
    std::string fileHash;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(fileHash));

    if (PANDORA_FILE_HASH != fileHash)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(m_containerId));

    if ((EVENT != m_containerId) && (GEOMETRY != m_containerId))
        return STATUS_CODE_FAILURE;

    m_containerPosition = m_fileStream.tellg();
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(m_containerSize));

    if (0 == m_containerSize)
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

ContainerId FileReader::GetNextContainerId()
{
    const std::ifstream::pos_type initialPosition(m_fileStream.tellg());

    std::string fileHash;
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(fileHash));

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

StatusCode FileReader::ReadNextGeometryComponent()
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
    case BOX_GAP:
        return this->ReadBoxGap(false);
    case CONCENTRIC_GAP:
        return this->ReadConcentricGap(false);
    case GEOMETRY_END:
        m_containerId = UNKNOWN_CONTAINER;
        return STATUS_CODE_NOT_FOUND;
    default:
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadNextEventComponent()
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
    case CALO_HIT:
        return this->ReadCaloHit(false);
    case TRACK:
        return this->ReadTrack(false);
    case MC_PARTICLE:
        return this->ReadMCParticle(false);
    case RELATIONSHIP:
        return this->ReadRelationship(false);
    case EVENT_END:
        m_containerId = UNKNOWN_CONTAINER;
        return STATUS_CODE_NOT_FOUND;
    default:
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadGeometryParameters()
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    // Default subdetectors
    std::string detectorName;
    PandoraApi::Geometry::Parameters parameters;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(detectorName, &(parameters.m_inDetBarrelParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(detectorName, &(parameters.m_inDetEndCapParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(detectorName, &(parameters.m_eCalBarrelParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(detectorName, &(parameters.m_eCalEndCapParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(detectorName, &(parameters.m_hCalBarrelParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(detectorName, &(parameters.m_hCalEndCapParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(detectorName, &(parameters.m_muonBarrelParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(detectorName, &(parameters.m_muonEndCapParameters)));

    bool readMainTrackerDetails(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(readMainTrackerDetails));

    if (readMainTrackerDetails)
    {
        float mainTrackerInnerRadius(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mainTrackerInnerRadius));
        float mainTrackerOuterRadius(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mainTrackerOuterRadius));
        float mainTrackerZExtent(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mainTrackerZExtent));
        parameters.m_mainTrackerInnerRadius = mainTrackerInnerRadius;
        parameters.m_mainTrackerOuterRadius = mainTrackerOuterRadius;
        parameters.m_mainTrackerZExtent = mainTrackerZExtent;
    }

    bool readCoilDetails(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(readCoilDetails));

    if (readCoilDetails)
    {
        float coilInnerRadius(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(coilInnerRadius));
        float coilOuterRadius(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(coilOuterRadius));
        float coilZExtent(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(coilZExtent));
        parameters.m_coilInnerRadius = coilInnerRadius;
        parameters.m_coilOuterRadius = coilOuterRadius;
        parameters.m_coilZExtent = coilZExtent;
    }

    // Additional subdetectors
    unsigned int nAdditionalSubDetectors(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nAdditionalSubDetectors));

    for (unsigned int iSubDetector = 0; iSubDetector < nAdditionalSubDetectors; ++iSubDetector)
    {
        std::string subDetectorName;
        PandoraApi::Geometry::Parameters::SubDetectorParameters subDetectorParameters;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(subDetectorName, &subDetectorParameters));
        parameters.m_additionalSubDetectors.insert(std::make_pair(subDetectorName, subDetectorParameters));
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::Create(*m_pPandora, parameters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadSubDetector(std::string &subDetectorName, PandoraApi::GeometryParameters::SubDetectorParameters *pSubDetectorParameters,
    bool checkComponentId)
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (SUB_DETECTOR != componentId)
            return STATUS_CODE_FAILURE;
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(subDetectorName));

    bool isInitialized(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isInitialized));

    if (!isInitialized)
        return STATUS_CODE_SUCCESS;

    float innerRCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerRCoordinate));
    float innerZCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerZCoordinate));
    float innerPhiCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerPhiCoordinate));
    unsigned int innerSymmetryOrder(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerSymmetryOrder));
    float outerRCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerRCoordinate));
    float outerZCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerZCoordinate));
    float outerPhiCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerPhiCoordinate));
    unsigned int outerSymmetryOrder(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerSymmetryOrder));
    bool isMirroredInZ(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isMirroredInZ));
    unsigned int nLayers(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nLayers));

    pSubDetectorParameters->m_innerRCoordinate = innerRCoordinate;
    pSubDetectorParameters->m_innerZCoordinate = innerZCoordinate;
    pSubDetectorParameters->m_innerPhiCoordinate = innerPhiCoordinate;
    pSubDetectorParameters->m_innerSymmetryOrder = innerSymmetryOrder;
    pSubDetectorParameters->m_outerRCoordinate = outerRCoordinate;
    pSubDetectorParameters->m_outerZCoordinate = outerZCoordinate;
    pSubDetectorParameters->m_outerPhiCoordinate = outerPhiCoordinate;
    pSubDetectorParameters->m_outerSymmetryOrder = outerSymmetryOrder;
    pSubDetectorParameters->m_isMirroredInZ = isMirroredInZ;
    pSubDetectorParameters->m_nLayers = nLayers;

    for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
    {
        float closestDistanceToIp(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(closestDistanceToIp));
        float nRadiationLengths(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nRadiationLengths));
        float nInteractionLengths(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nInteractionLengths));

        PandoraApi::Geometry::Parameters::LayerParameters layerParameters;
        layerParameters.m_closestDistanceToIp = closestDistanceToIp;
        layerParameters.m_nRadiationLengths = nRadiationLengths;
        layerParameters.m_nInteractionLengths = nInteractionLengths;
        pSubDetectorParameters->m_layerParametersList.push_back(layerParameters);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadBoxGap(bool checkComponentId)
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (BOX_GAP != componentId)
            return STATUS_CODE_FAILURE;
    }

    CartesianVector vertex(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(vertex));
    CartesianVector side1(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(side1));
    CartesianVector side2(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(side2));
    CartesianVector side3(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(side3));

    PandoraApi::BoxGap::Parameters parameters;
    parameters.m_vertex = vertex;
    parameters.m_side1 = side1;
    parameters.m_side2 = side2;
    parameters.m_side3 = side3;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::BoxGap::Create(*m_pPandora, parameters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadConcentricGap(bool checkComponentId)
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (CONCENTRIC_GAP != componentId)
            return STATUS_CODE_FAILURE;
    }

    float minZCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(minZCoordinate));
    float maxZCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(maxZCoordinate));
    float innerRCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerRCoordinate));
    float innerPhiCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerPhiCoordinate));
    unsigned int innerSymmetryOrder(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerSymmetryOrder));
    float outerRCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerRCoordinate));
    float outerPhiCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerPhiCoordinate));
    unsigned int outerSymmetryOrder(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerSymmetryOrder));

    PandoraApi::ConcentricGap::Parameters parameters;
    parameters.m_minZCoordinate = minZCoordinate;
    parameters.m_maxZCoordinate = maxZCoordinate;
    parameters.m_innerRCoordinate = innerRCoordinate;
    parameters.m_innerPhiCoordinate = innerPhiCoordinate;
    parameters.m_innerSymmetryOrder = innerSymmetryOrder;
    parameters.m_outerRCoordinate = outerRCoordinate;
    parameters.m_outerPhiCoordinate = outerPhiCoordinate;
    parameters.m_outerSymmetryOrder = outerSymmetryOrder;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::ConcentricGap::Create(*m_pPandora, parameters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadCaloHit(bool checkComponentId)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (CALO_HIT != componentId)
            return STATUS_CODE_FAILURE;
    }

    CellGeometry cellGeometry(UNKNOWN_CELL_GEOMETRY);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellGeometry));
    CartesianVector positionVector(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(positionVector));
    CartesianVector expectedDirection(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(expectedDirection));
    CartesianVector cellNormalVector(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellNormalVector));
    float cellThickness(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellThickness));
    float nCellRadiationLengths(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nCellRadiationLengths));
    float nCellInteractionLengths(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nCellInteractionLengths));
    float time(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(time));
    float inputEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(inputEnergy));
    float mipEquivalentEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mipEquivalentEnergy));
    float electromagneticEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(electromagneticEnergy));
    float hadronicEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(hadronicEnergy));
    bool isDigital(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isDigital));
    HitType hitType(ECAL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(hitType));
    DetectorRegion detectorRegion(BARREL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(detectorRegion));
    unsigned int layer(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(layer));
    bool isInOuterSamplingLayer(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isInOuterSamplingLayer));
    void *pParentAddress(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(pParentAddress));

    PandoraApi::RectangularCaloHit::Parameters rectangularParameters;
    PandoraApi::PointingCaloHit::Parameters pointingParameters;

    if (RECTANGULAR == cellGeometry)
    {
        float cellSizeU(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellSizeU));
        float cellSizeV(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellSizeV));

        rectangularParameters.m_cellSizeU = cellSizeU;
        rectangularParameters.m_cellSizeV = cellSizeV;
    }
    else if (POINTING == cellGeometry)
    {
        float cellSizeEta(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellSizeEta));
        float cellSizePhi(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellSizePhi));

        pointingParameters.m_cellSizeEta = cellSizeEta;
        pointingParameters.m_cellSizePhi = cellSizePhi;
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    PandoraApi::CaloHitBaseParameters *pBaseParameters((RECTANGULAR == cellGeometry) ?
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
    pBaseParameters->m_detectorRegion = detectorRegion;
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

StatusCode FileReader::ReadTrack(bool checkComponentId)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (TRACK != componentId)
            return STATUS_CODE_FAILURE;
    }

    float d0(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(d0));
    float z0(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(z0));
    int particleId(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(particleId));
    int charge(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(charge));
    float mass(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mass));
    CartesianVector momentumAtDca(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(momentumAtDca));
    TrackState trackStateAtStart(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(trackStateAtStart));
    TrackState trackStateAtEnd(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(trackStateAtEnd));
    TrackState trackStateAtCalorimeter(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(trackStateAtCalorimeter));
    float timeAtCalorimeter(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(timeAtCalorimeter));
    bool reachesCalorimeter(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(reachesCalorimeter));
    bool isProjectedToEndCap(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isProjectedToEndCap));
    bool canFormPfo(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(canFormPfo));
    bool canFormClusterlessPfo(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(canFormClusterlessPfo));
    void *pParentAddress(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(pParentAddress));

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

StatusCode FileReader::ReadMCParticle(bool checkComponentId)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (MC_PARTICLE != componentId)
            return STATUS_CODE_FAILURE;
    }

    float energy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(energy));
    CartesianVector momentum(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(momentum));
    CartesianVector vertex(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(vertex));
    CartesianVector endpoint(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(endpoint));
    int particleId(-std::numeric_limits<int>::max());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(particleId));
    MCParticleType mcParticleType(MC_STANDARD);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mcParticleType));
    void *pParentAddress(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(pParentAddress));

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

StatusCode FileReader::ReadRelationship(bool checkComponentId)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (RELATIONSHIP != componentId)
            return STATUS_CODE_FAILURE;
    }

    RelationshipId relationshipId(UNKNOWN_RELATIONSHIP);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(relationshipId));
    void *address1(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(address1));
    void *address2(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(address2));

    switch (relationshipId)
    {
    case CALO_HIT_TO_MC:
        return PandoraApi::SetCaloHitToMCParticleRelationship(*m_pPandora, address1, address2);
    case TRACK_TO_MC:
        return PandoraApi::SetTrackToMCParticleRelationship(*m_pPandora, address1, address2);
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
