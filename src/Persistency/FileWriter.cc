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

FileWriter::FileWriter(const pandora::Pandora &pandora, const std::string &fileName) :
    m_pPandora(&pandora),
    m_containerId(UNKNOWN_CONTAINER),
    m_fileName(fileName)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

FileWriter::~FileWriter()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteGeometry()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteHeader(GEOMETRY));

    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("InDetBarrel", &(GeometryHelper::GetInDetBarrelParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("InDetEndCap", &(GeometryHelper::GetInDetEndCapParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("ECalBarrel", &(GeometryHelper::GetECalBarrelParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("ECalEndCap", &(GeometryHelper::GetECalEndCapParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("HCalBarrel", &(GeometryHelper::GetHCalBarrelParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("HCalEndCap", &(GeometryHelper::GetHCalEndCapParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("MuonBarrel", &(GeometryHelper::GetMuonBarrelParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector("MuonEndCap", &(GeometryHelper::GetMuonEndCapParameters())));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTracker());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteCoil())
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteAdditionalSubDetectors())
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteDetectorGapList());

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteFooter());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteEvent(const bool writeMCRelationships, const bool writeTrackRelationships)
{
    std::string caloHitListName;
    const CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->GetPandoraContentApiImpl()->GetCurrentList(pCaloHitList, caloHitListName));

    std::string trackListName;
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->GetPandoraContentApiImpl()->GetCurrentList(pTrackList, trackListName));

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
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->GetPandoraContentApiImpl()->GetCurrentList(pMCParticleList, mcParticleListName));
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

StatusCode FileWriter::WriteDetectorGapList()
{
    const GeometryHelper::DetectorGapList &detectorGapList(GeometryHelper::GetDetectorGapList());

    for (GeometryHelper::DetectorGapList::const_iterator iter = detectorGapList.begin(), iterEnd = detectorGapList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteDetectorGap(*iter));
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

StatusCode FileWriter::WriteCaloHitToMCParticleRelationship(const CaloHit *const pCaloHit)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const MCParticleWeightMap &mcParticleWeightMap(pCaloHit->GetMCParticleWeightMap());

    for (MCParticleWeightMap::const_iterator iter = mcParticleWeightMap.begin(), iterEnd = mcParticleWeightMap.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(CALO_HIT_TO_MC, pCaloHit->GetParentCaloHitAddress(),
            iter->first->GetUid(), iter->second));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrackToMCParticleRelationship(const Track *const pTrack)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    const MCParticleWeightMap &mcParticleWeightMap(pTrack->GetMCParticleWeightMap());

    for (MCParticleWeightMap::const_iterator iter = mcParticleWeightMap.begin(), iterEnd = mcParticleWeightMap.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(TRACK_TO_MC, pTrack->GetParentTrackAddress(),
            iter->first->GetUid(), iter->second));
    }

    return STATUS_CODE_SUCCESS;
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

} // namespace pandora
