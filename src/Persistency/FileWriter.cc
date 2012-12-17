/**
 *  @file   PandoraPFANew/Framework/src/Persistency/FileWriter.cc
 * 
 *  @brief  Implementation of the file writer class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

#include "Helpers/GeometryHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/DetectorGap.h"
#include "Objects/MCParticle.h"
#include "Objects/Track.h"

#include "Persistency/FileWriter.h"

namespace pandora
{

FileWriter::FileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode) :
    m_pPandora(&pandora),
    m_containerId(UNKNOWN_CONTAINER)
{
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

FileWriter::~FileWriter()
{
    m_fileStream.close();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteGeometry()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteHeader(GEOMETRY));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteGeometryParameters());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteDetectorGaps());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteFooter());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteEvent(const bool writeMCRelationships, const bool writeTrackRelationships)
{
    std::string caloHitListName;
    const CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->GetPandoraContentApiImpl()->GetCurrentCaloHitList(pCaloHitList, caloHitListName));

    std::string trackListName;
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->GetPandoraContentApiImpl()->GetCurrentTrackList(pTrackList, trackListName));

    return this->WriteEvent(*pCaloHitList, *pTrackList, writeMCRelationships, writeTrackRelationships);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteEvent(const CaloHitList &caloHitList, const TrackList &trackList, const bool writeMCRelationships,
    const bool writeTrackRelationships)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteHeader(EVENT));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteCaloHitList(caloHitList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrackList(trackList));

    if (writeMCRelationships)
    {
        std::string mcParticleListName;
        const MCParticleList *pMCParticleList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->GetPandoraContentApiImpl()->GetCurrentMCParticleList(pMCParticleList, mcParticleListName));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteMCParticleList(*pMCParticleList));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteCaloHitToMCParticleRelationships(caloHitList));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrackToMCParticleRelationships(trackList));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteMCParticleRelationships(*pMCParticleList));
    }

    if (writeTrackRelationships)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrackRelationships(trackList));
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteFooter());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteHeader(const ContainerId containerId)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(PANDORA_FILE_HASH));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(containerId));

    m_containerPosition = m_fileStream.tellp();
    const std::ofstream::pos_type dummyContainerSize(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(dummyContainerSize));

    m_containerId = containerId;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteFooter()
{
    if ((EVENT != m_containerId) && (GEOMETRY != m_containerId))
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable((EVENT == m_containerId) ? EVENT_END : GEOMETRY_END));
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

StatusCode FileWriter::WriteGeometryParameters()
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    // Default subdetectors
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("InDetBarrel", &(GeometryHelper::GetInDetBarrelParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("InDetEndCap", &(GeometryHelper::GetInDetEndCapParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("ECalBarrel", &(GeometryHelper::GetECalBarrelParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("ECalEndCap", &(GeometryHelper::GetECalEndCapParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("HCalBarrel", &(GeometryHelper::GetHCalBarrelParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("HCalEndCap", &(GeometryHelper::GetHCalEndCapParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("MuonBarrel", &(GeometryHelper::GetMuonBarrelParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("MuonEndCap", &(GeometryHelper::GetMuonEndCapParameters())));

    try
    {
        const float mainTrackerInnerRadius(GeometryHelper::GetMainTrackerInnerRadius());
        const float mainTrackerOuterRadius(GeometryHelper::GetMainTrackerOuterRadius());
        const float mainTrackerZExtent(GeometryHelper::GetMainTrackerZExtent());
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(true));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(mainTrackerInnerRadius));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(mainTrackerOuterRadius));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(mainTrackerZExtent));
    }
    catch (StatusCodeException &)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(false));
    }

    try
    {
        const float coilInnerRadius(GeometryHelper::GetCoilInnerRadius());
        const float coilOuterRadius(GeometryHelper::GetCoilOuterRadius());
        const float coilZExtent(GeometryHelper::GetCoilZExtent());
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(true));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(coilInnerRadius));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(coilOuterRadius));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(coilZExtent));
    }
    catch (StatusCodeException &)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(false));
    }

    // Additional subdetectors
    const GeometryHelper::SubDetectorParametersMap &subDetectorParametersMap(GeometryHelper::GetAdditionalSubDetectors());

    const unsigned int nAdditionalSubDetectors(subDetectorParametersMap.size());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(nAdditionalSubDetectors));

    for (GeometryHelper::SubDetectorParametersMap::const_iterator iter = subDetectorParametersMap.begin(), iterEnd = subDetectorParametersMap.end();
        iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector(iter->first, &iter->second));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrackList(const TrackList &trackList)
{
    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrack(*iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteCaloHitList(const CaloHitList &caloHitList)
{
    for (CaloHitList::const_iterator hitIter = caloHitList.begin(), hitIterEnd = caloHitList.end(); hitIter != hitIterEnd; ++hitIter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteCaloHit(*hitIter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteMCParticleList(const MCParticleList &mcParticleList)
{
    for (MCParticleList::const_iterator iter = mcParticleList.begin(), iterEnd = mcParticleList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteMCParticle(*iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteCaloHitToMCParticleRelationships(const CaloHitList &caloHitList)
{
    for (CaloHitList::const_iterator hitIter = caloHitList.begin(), hitIterEnd = caloHitList.end(); hitIter != hitIterEnd; ++hitIter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteCaloHitToMCParticleRelationship(*hitIter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrackToMCParticleRelationships(const TrackList &trackList)
{
    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrackToMCParticleRelationship(*iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteMCParticleRelationships(const MCParticleList &mcParticleList)
{
    for (MCParticleList::const_iterator iter = mcParticleList.begin(), iterEnd = mcParticleList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteMCParticleRelationships(*iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrackRelationships(const TrackList &trackList)
{
    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrackRelationships(*iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteSubDetector(const std::string &subDetectorName, const GeometryHelper::SubDetectorParameters *const pSubDetectorParameters)
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(SUB_DETECTOR));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(subDetectorName));

    const bool isInitialized(pSubDetectorParameters->IsInitialized());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(isInitialized));

    if (!isInitialized)
        return STATUS_CODE_SUCCESS;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetectorParameters->GetInnerRCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetectorParameters->GetInnerZCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetectorParameters->GetInnerPhiCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetectorParameters->GetInnerSymmetryOrder()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetectorParameters->GetOuterRCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetectorParameters->GetOuterZCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetectorParameters->GetOuterPhiCoordinate()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetectorParameters->GetOuterSymmetryOrder()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pSubDetectorParameters->IsMirroredInZ()));

    const unsigned int nLayers(pSubDetectorParameters->GetNLayers());
    const GeometryHelper::LayerParametersList &layerParametersList(pSubDetectorParameters->GetLayerParametersList());

    if (layerParametersList.size() != nLayers)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(nLayers));

    for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(layerParametersList[iLayer].m_closestDistanceToIp));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(layerParametersList[iLayer].m_nRadiationLengths));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(layerParametersList[iLayer].m_nInteractionLengths));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteDetectorGaps()
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    const GeometryHelper::DetectorGapList &detectorGapList(GeometryHelper::GetDetectorGapList());

    for (GeometryHelper::DetectorGapList::const_iterator iter = detectorGapList.begin(), iterEnd = detectorGapList.end(); iter != iterEnd; ++iter)
    {
        BoxGap *pBoxGap = NULL;
        pBoxGap = dynamic_cast<BoxGap *>(*iter);

        if (NULL != pBoxGap)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(BOX_GAP));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pBoxGap->m_vertex));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pBoxGap->m_side1));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pBoxGap->m_side2));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pBoxGap->m_side3));
            continue;
        }

        ConcentricGap *pConcentricGap = NULL;
        pConcentricGap = dynamic_cast<ConcentricGap *>(*iter);

        if (NULL != pConcentricGap)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(CONCENTRIC_GAP));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->m_minZCoordinate));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->m_maxZCoordinate));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->m_innerRCoordinate));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->m_innerPhiCoordinate));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->m_innerSymmetryOrder));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->m_outerRCoordinate));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->m_outerPhiCoordinate));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pConcentricGap->m_outerSymmetryOrder));
            continue;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteCaloHit(const CaloHit *const pCaloHit)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const CellGeometry cellGeometry(pCaloHit->GetCellGeometry());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(CALO_HIT));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(cellGeometry));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetPositionVector()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetExpectedDirection()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetCellNormalVector()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetCellThickness()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetNCellRadiationLengths()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetNCellInteractionLengths()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetTime()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetInputEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetMipEquivalentEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetElectromagneticEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetHadronicEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->IsDigital()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetHitType()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetDetectorRegion()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetLayer()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->IsInOuterSamplingLayer()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pCaloHit->GetParentCaloHitAddress()));

    if (RECTANGULAR == cellGeometry)
    {
        const RectangularCaloHit *pRectangularCaloHit = dynamic_cast<const RectangularCaloHit *>(pCaloHit);

        if (NULL == pRectangularCaloHit)
            return STATUS_CODE_FAILURE;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pRectangularCaloHit->GetCellSizeU()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pRectangularCaloHit->GetCellSizeV()));
    }
    else if (POINTING == cellGeometry)
    {
        const PointingCaloHit *pPointingCaloHit = dynamic_cast<const PointingCaloHit *>(pCaloHit);

        if (NULL == pPointingCaloHit)
            return STATUS_CODE_FAILURE;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pPointingCaloHit->GetCellSizeEta()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pPointingCaloHit->GetCellSizePhi()));
    }
    else
    {
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrack(const Track *const pTrack)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(TRACK));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetD0()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetZ0()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetParticleId()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetCharge()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetMass()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetMomentumAtDca()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetTrackStateAtStart()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetTrackStateAtEnd()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetTrackStateAtCalorimeter()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetTimeAtCalorimeter()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->ReachesCalorimeter()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->IsProjectedToEndCap()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->CanFormPfo()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->CanFormClusterlessPfo()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pTrack->GetParentTrackAddress()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteMCParticle(const MCParticle *const pMCParticle)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(MC_PARTICLE));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetEnergy()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetMomentum()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetVertex()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetEndpoint()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetParticleId()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetMCParticleType()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(pMCParticle->GetUid()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteCaloHitToMCParticleRelationship(const CaloHit *const pCaloHit)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const MCParticle *pMCParticle = NULL;
    (void) pCaloHit->GetMCParticle(pMCParticle);

    // Allow cases where mc particle links not formed
    if (NULL == pMCParticle)
        return STATUS_CODE_SUCCESS;

    return this->WriteRelationship(CALO_HIT_TO_MC, pCaloHit->GetParentCaloHitAddress(), pMCParticle->GetUid());
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrackToMCParticleRelationship(const Track *const pTrack)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const MCParticle *pMCParticle = NULL;
    (void) pTrack->GetMCParticle(pMCParticle);

    // Allow cases where mc particle links not formed
    if (NULL == pMCParticle)
        return STATUS_CODE_SUCCESS;

    return this->WriteRelationship(TRACK_TO_MC, pTrack->GetParentTrackAddress(), pMCParticle->GetUid());
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteMCParticleRelationships(const MCParticle *const pMCParticle)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const Uid uid(pMCParticle->GetUid());
    const MCParticleList &parentList(pMCParticle->GetParentList());
    const MCParticleList &daughterList(pMCParticle->GetDaughterList());

    for (MCParticleList::const_iterator iter = parentList.begin(), iterEnd = parentList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(MC_PARENT_DAUGHTER, (*iter)->GetUid(), uid));
    }

    for (MCParticleList::const_iterator iter = daughterList.begin(), iterEnd = daughterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(MC_PARENT_DAUGHTER, uid, (*iter)->GetUid()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrackRelationships(const Track *const pTrack)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const void *address(pTrack->GetParentTrackAddress());
    const TrackList &parentList(pTrack->GetParentTrackList());
    const TrackList &daughterList(pTrack->GetDaughterTrackList());
    const TrackList &siblingList(pTrack->GetSiblingTrackList());

    for (TrackList::const_iterator iter = parentList.begin(), iterEnd = parentList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(TRACK_PARENT_DAUGHTER, (*iter)->GetParentTrackAddress(), address));
    }

    for (TrackList::const_iterator iter = daughterList.begin(), iterEnd = daughterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(TRACK_PARENT_DAUGHTER, address, (*iter)->GetParentTrackAddress()));
    }

    for (TrackList::const_iterator iter = siblingList.begin(), iterEnd = siblingList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(TRACK_SIBLING, address, (*iter)->GetParentTrackAddress()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(RELATIONSHIP));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(relationshipId));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(address1));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(address2));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
