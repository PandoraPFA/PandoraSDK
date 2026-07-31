/**
 *  @file   PandoraSDK/include/Persistency/FileWriter.h
 *
 *  @brief  Header file for the file writer class.
 *
 *  $Log: $
 */
#ifndef PANDORA_FILE_WRITER_H
#define PANDORA_FILE_WRITER_H 1

#include "Pandora/StatusCodes.h"

#include "Persistency/PandoraIO.h"
#include "Persistency/Persistency.h"

#include <string>

namespace pandora
{

class Pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  FileWriter class
 */
class FileWriter : public Persistency
{
public:
    /**
     *  @brief  Constructor
     *
     *  @param  pandora   the pandora instance to be used alongside the file writer
     *  @param  fileName  the name of the output file
     */
    FileWriter(const pandora::Pandora &pandora, const std::string &fileName);

    /**
     *  @brief  Destructor
     */
    virtual ~FileWriter();

    /**
     *  @brief  Write the global header to the file.
     */
    virtual StatusCode WriteGlobalHeader() = 0;

    /**
     *  @brief  Write the current geometry information to the file
     */
    StatusCode WriteGeometry();

    /**
     *  @brief  Write the specified event components to the file.
     *
     *  @param  caloHitList the list of calo hits to write
     *  @param  trackList the list of tracks to write
     *  @param  mcParticleList the list of MC particles to write
     *  @param  writeMCRelationships whether to write MC relationship information
     *  @param  writeTrackRelationships whether to write track relationship information
     */
    StatusCode WriteEvent(const CaloHitList &caloHitList, const TrackList &trackList, const MCParticleList &mcParticleList,
        const bool writeMCRelationships = true, const bool writeTrackRelationships = true);

protected:
    /**
     *  @brief  Write the header for a new container to the file
     *
     *  @param  containerId the container id
     */
    virtual StatusCode WriteHeader(const ContainerId containerId) = 0;

    /**
     *  @brief  Write the footer for a container to the file
     */
    virtual StatusCode WriteFooter() = 0;

    /**
     *  @brief  Write the sub-detector to the file
     *
     *  @param  pSubDetector the sub-detector to write
     */
    virtual StatusCode WriteSubDetector(const SubDetector *const pSubDetector) = 0;

    /**
     *  @brief Write the LArTPC to the file
     *
     *  @param  pLArTPC the LArTPC to write
     */
    virtual StatusCode WriteLArTPC(const LArTPC *const pLArTPC) = 0;

    /**
     *  @brief  Write the detector gap to the file
     *
     *  @param  pDetectorGap the detector gap to write
     */
    virtual StatusCode WriteDetectorGap(const DetectorGap *const pDetectorGap) = 0;

    /**
     *  @brief  Write the calo hit to the file
     *
     *  @param  pCaloHit the calo hit to write
     */
    virtual StatusCode WriteCaloHit(const CaloHit *const pCaloHit) = 0;

    /**
     *  @brief  Write the track to the file
     *
     *  @param  pTrack the track to write
     */
    virtual StatusCode WriteTrack(const Track *const pTrack) = 0;

    /**
     *  @brief  Write the MC particle to the file
     *
     *  @param  pMCParticle the MC particle to write
     */
    virtual StatusCode WriteMCParticle(const MCParticle *const pMCParticle) = 0;

    /**
     *  @brief  Write a relationship between two objects to the file
     *
     *  @param  relationshipId the relationship id
     *  @param  address1 the address of the first object in the relationship
     *  @param  address2 the address of the second object in the relationship
     *  @param  weight the weight of the relationship
     */
    virtual StatusCode WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2,
        const float weight = 1.f) = 0;

    /**
     *  @brief  Write the event information to the file
     */
    virtual StatusCode WriteEventInformation() = 0;

    /**
     *  @brief  Return the current schema version for a given component type.
     *
     *  Single source of truth shared by every concrete FileWriter (binary, XML, and any future format), so schema versions cannot drift
     *  between formats. Increment a value here (and register a corresponding reader migration in BinaryFileReader/XmlFileReader) when a
     *  field is removed or its semantics change. Adding a new optional field does not need a bump.
     */
    static unsigned int GetSchemaVersion(const ComponentId componentId);

    /**
     *  @brief  Populate m_schemaRegistry from GetSchemaVersion() for every persisted component type. Called once by each concrete
     *          writer's constructor, after the output stream/document is ready.
     */
    void PopulateSchemaRegistry();

private:
    /**
     *  @brief  Write the list of sub-detectors to the file
     */
    StatusCode WriteSubDetectorList();

    /**
     *  @brief Write the list of LArTPCs to the file
     */
    StatusCode WriteLArTPCList();

    /**
     *  @brief  Write the list of detector gaps to the file
     */
    StatusCode WriteDetectorGapList();

    /**
     *  @brief  Write the list of tracks to the file
     *
     *  @param  trackList the list of tracks to write
     */
    StatusCode WriteTrackList(const TrackList &trackList);

    /**
     *  @brief  Write the list of calo hits to the file
     *
     *  @param  caloHitList the list of calo hits to write
     */
    StatusCode WriteCaloHitList(const CaloHitList &caloHitList);

    /**
     *  @brief  Write the list of MC particles to the file
     *
     *  @param  mcParticleList the list of MC particles to write
     */
    StatusCode WriteMCParticleList(const MCParticleList &mcParticleList);

    /**
     *  @brief  Write the list of calo hit to MC particle relationships to the file
     *
     *  @param  caloHitList the list of calo hits whose relationships are to be written
     */
    StatusCode WriteCaloHitToMCParticleRelationships(const CaloHitList &caloHitList);

    /**
     *  @brief  Write the list of track to MC particle relationships to the file
     *
     *  @param  trackList the list of tracks whose relationships are to be written
     */
    StatusCode WriteTrackToMCParticleRelationships(const TrackList &trackList);

    /**
     *  @brief  Write the list of MC particle relationships to the file
     *
     *  @param  mcParticleList the list of MC particles whose relationships are to be written
     */
    StatusCode WriteMCParticleRelationships(const MCParticleList &mcParticleList);

    /**
     *  @brief  Write the list of track relationships to the file
     *
     *  @param  trackList the list of tracks whose relationships are to be written
     */
    StatusCode WriteTrackRelationships(const TrackList &trackList);

    /**
     *  @brief  Write the calo hit to MC particle relationships for a single calo hit to the file
     *
     *  @param  pCaloHit the calo hit whose relationships are to be written
     */
    StatusCode WriteCaloHitToMCParticleRelationship(const CaloHit *const pCaloHit);

    /**
     *  @brief  Write the track to MC particle relationships for a single track to the file
     *
     *  @param  pTrack the track whose relationships are to be written
     */
    StatusCode WriteTrackToMCParticleRelationship(const Track *const pTrack);

    /**
     *  @brief  Write the MC particle relationships for a single MC particle to the file
     *
     *  @param  pMCParticle the MC particle whose relationships are to be written
     */
    StatusCode WriteMCParticleRelationships(const MCParticle *const pMCParticle);

    /**
     *  @brief  Write the track relationships for a single track to the file
     *
     *  @param  pTrack the track whose relationships are to be written
     */
    StatusCode WriteTrackRelationships(const Track *const pTrack);
};

} // namespace pandora

#endif // #ifndef PANDORA_FILE_WRITER_H
