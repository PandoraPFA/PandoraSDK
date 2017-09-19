/**
 *  @file   PandoraSDK/src/Persistency/FileWriter.cc
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

#include "Managers/GeometryManager.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"
#include "Objects/Track.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraInternal.h"

#include "Persistency/FileWriter.h"

#include <algorithm>

namespace pandora
{

FileWriter::FileWriter(const pandora::Pandora &pandora, const std::string &fileName) :
    Persistency(pandora, fileName)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

FileWriter::~FileWriter()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteGeometry()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteHeader(GEOMETRY_CONTAINER));

    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetectorList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteLArTPCList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteDetectorGapList());

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteFooter());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteEvent(const CaloHitList &caloHitList, const TrackList &trackList, const MCParticleList &mcParticleList,
    const bool writeMCRelationships, const bool writeTrackRelationships)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteHeader(EVENT_CONTAINER));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteCaloHitList(caloHitList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrackList(trackList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteMCParticleList(mcParticleList));

    if (writeMCRelationships)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteCaloHitToMCParticleRelationships(caloHitList));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrackToMCParticleRelationships(trackList));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteMCParticleRelationships(mcParticleList));
    }

    if (writeTrackRelationships)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrackRelationships(trackList));
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteFooter());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteSubDetectorList()
{
    for (const SubDetectorMap::value_type &mapEntry : m_pPandora->GetGeometry()->GetSubDetectorMap())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteSubDetector(mapEntry.second));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteLArTPCList()
{
    for (const LArTPCMap::value_type &mapEntry : m_pPandora->GetGeometry()->GetLArTPCMap())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteLArTPC(mapEntry.second));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteDetectorGapList()
{
    for (const DetectorGap *const pDetectorGap : m_pPandora->GetGeometry()->GetDetectorGapList())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteDetectorGap(pDetectorGap));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrackList(const TrackList &trackList)
{
    for (const Track *const pTrack : trackList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrack(pTrack));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteCaloHitList(const CaloHitList &caloHitList)
{
    for (const CaloHit *const pCaloHit : caloHitList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteCaloHit(pCaloHit));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteMCParticleList(const MCParticleList &mcParticleList)
{
    for (const MCParticle *const pMCParticle : mcParticleList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteMCParticle(pMCParticle));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteCaloHitToMCParticleRelationships(const CaloHitList &caloHitList)
{
    for (const CaloHit *const pCaloHit : caloHitList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteCaloHitToMCParticleRelationship(pCaloHit));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrackToMCParticleRelationships(const TrackList &trackList)
{
    for (const Track *const pTrack : trackList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrackToMCParticleRelationship(pTrack));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteMCParticleRelationships(const MCParticleList &mcParticleList)
{
    for (const MCParticle *const pMCParticle : mcParticleList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteMCParticleRelationships(pMCParticle));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrackRelationships(const TrackList &trackList)
{
    for (const Track *const pTrack : trackList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteTrackRelationships(pTrack));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteCaloHitToMCParticleRelationship(const CaloHit *const pCaloHit)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    const MCParticleWeightMap &mcParticleWeightMap(pCaloHit->GetMCParticleWeightMap());

    MCParticleVector mcParticleVector;
    for (const MCParticleWeightMap::value_type &mapEntry : mcParticleWeightMap) mcParticleVector.push_back(mapEntry.first);
    std::sort(mcParticleVector.begin(), mcParticleVector.end(), PointerLessThan<MCParticle>());

    for (const MCParticle *const pMCParticle : mcParticleVector)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(CALO_HIT_TO_MC_RELATIONSHIP, pCaloHit->GetParentAddress(),
            pMCParticle->GetUid(), mcParticleWeightMap.at(pMCParticle)));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrackToMCParticleRelationship(const Track *const pTrack)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    const MCParticleWeightMap &mcParticleWeightMap(pTrack->GetMCParticleWeightMap());

    MCParticleVector mcParticleVector;
    for (const MCParticleWeightMap::value_type &mapEntry : mcParticleWeightMap) mcParticleVector.push_back(mapEntry.first);
    std::sort(mcParticleVector.begin(), mcParticleVector.end(), PointerLessThan<MCParticle>());

    for (const MCParticle *const pMCParticle : mcParticleVector)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(TRACK_TO_MC_RELATIONSHIP, pTrack->GetParentAddress(),
            pMCParticle->GetUid(), mcParticleWeightMap.at(pMCParticle)));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteMCParticleRelationships(const MCParticle *const pMCParticle)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    const Uid uid(pMCParticle->GetUid());
    const MCParticleList &parentList(pMCParticle->GetParentList());
    const MCParticleList &daughterList(pMCParticle->GetDaughterList());

    for (const MCParticle *const pParentMCParticle : parentList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(MC_PARENT_DAUGHTER_RELATIONSHIP, pParentMCParticle->GetUid(), uid));
    }

    for (const MCParticle *const pDaughterMCParticle : daughterList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(MC_PARENT_DAUGHTER_RELATIONSHIP, uid, pDaughterMCParticle->GetUid()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileWriter::WriteTrackRelationships(const Track *const pTrack)
{
    if (EVENT_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    const void *address(pTrack->GetParentAddress());
    const TrackList &parentList(pTrack->GetParentList());
    const TrackList &daughterList(pTrack->GetDaughterList());
    const TrackList &siblingList(pTrack->GetSiblingList());

    for (const Track *const pParentTrack : parentList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(TRACK_PARENT_DAUGHTER_RELATIONSHIP, pParentTrack->GetParentAddress(), address));
    }

    for (const Track *const pDaughterTrack : daughterList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(TRACK_PARENT_DAUGHTER_RELATIONSHIP, address, pDaughterTrack->GetParentAddress()));
    }

    for (const Track *const pSiblingTrack : siblingList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteRelationship(TRACK_SIBLING_RELATIONSHIP, address, pSiblingTrack->GetParentAddress()));
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
