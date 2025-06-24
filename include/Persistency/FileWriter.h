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
     *  @param  algorithm the pandora instance to be used alongside the file writer
     *  @param  fileName the name of the output file
     *  @param  majorVersion the major version of the output file
     *  @param  minorVersion the minor version of the output file
     */
    FileWriter(const pandora::Pandora &pandora, const std::string &fileName, const unsigned int majorVersion = 0, 
        const unsigned int minorVersion = 0);

    /**
     *  @brief  Destructor
     */
    virtual ~FileWriter();

    /**
     *  @brief  Write the global header to the file
     */
    StatusCode WriteGlobalHeader();
  
    /**
     *  @brief  Write the current geometry information to the file
     */
    StatusCode WriteGeometry();

    /**
     *  @brief  Write the specified event components to the file
     * 
     *  @param  caloHitList the list of calo hits to write to the file
     *  @param  trackList the list of tracks to write to the file
     *  @param  mcParticleList the list of mc particles to write to the file
     *  @param  writeMCRelationships whether to write mc relationship information to the file
     *  @param  writeTrackRelationships whether to write track relationship information to the file
     */
    StatusCode WriteEvent(const CaloHitList &caloHitList, const TrackList &trackList, const MCParticleList &mcParticleList,
        const bool writeMCRelationships = true, const bool writeTrackRelationships = true);

protected:

   /**
     *  @brief  Write the container header to the file
     * 
     *  @param  containerId the container id
     */
    virtual StatusCode WriteHeader(const ContainerId containerId) = 0;

    /**
     *  @brief  Write the container footer to the file
     */
    virtual StatusCode WriteFooter() = 0;

    /**
     *  @brief  Write file version information to the file
     * 
     */  
    virtual StatusCode WriteVersion() = 0;  
  
    /**
     *  @brief  Write a sub detector to the file
     * 
     *  @param  pSubDetector address of the sub detector
     */
    virtual StatusCode WriteSubDetector(const SubDetector *const pSubDetector) = 0;

    /**
     *  @brief  Write a lar tpc to the file
     * 
     *  @param  pLArTPC address of the lar tpc
     */
    virtual StatusCode WriteLArTPC(const LArTPC *const pLArTPC) = 0;

    /**
     *  @brief  Write the detector gap parameters to the file
     * 
     *  @param  pDetectorGap address of the detector gap
     */
    virtual StatusCode WriteDetectorGap(const DetectorGap *const pDetectorGap) = 0;

    /**
     *  @brief  Write a calo hit to the current position in the file
     * 
     *  @param  pCaloHit address of the calo hit
     */
    virtual StatusCode WriteCaloHit(const CaloHit *const pCaloHit) = 0;

    /**
     *  @brief  Write a track to the current position in the file
     * 
     *  @param  pTrack address of the track
     */
    virtual StatusCode WriteTrack(const Track *const pTrack) = 0;

    /**
     *  @brief  Write a mc particle to the current position in the file
     * 
     *  @param  pMCParticle address of the mc particle
     */
    virtual StatusCode WriteMCParticle(const MCParticle *const pMCParticle) = 0;

    /**
     *  @brief  Write a relationship between two objects with specified addresses
     * 
     *  @param  relationshipId the relationship id
     *  @param  address1 first address to write
     *  @param  address2 second address to write
     */
    virtual StatusCode WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2, const float weight = 1.f) = 0;

    /**
     *  @brief  Write event-level information
     * 
     */
    virtual StatusCode WriteEventInformation() = 0;

    unsigned int m_fileMajorVersion; ///< Major version of the output file 
    unsigned int m_fileMinorVersion; ///< Minor version of the output file 

private:
    /**
     *  @brief  Write the sub detector parameters to the file
     */
    StatusCode WriteSubDetectorList();

    /**
     *  @brief  Write the lar tpc parameters to the file
     */
    StatusCode WriteLArTPCList();

    /**
     *  @brief  Write the detector gap parameters to the file
     */
    StatusCode WriteDetectorGapList();

    /**
     *  @brief  Write a track list to the current position in the file
     * 
     *  @param  trackList the track list
     */
    StatusCode WriteTrackList(const TrackList &trackList);

    /**
     *  @brief  Write a calo hit list to the current position in the file
     * 
     *  @param  caloHitList the calo hit list
     */
    StatusCode WriteCaloHitList(const CaloHitList &caloHitList);

    /**
     *  @brief  Write a mc particle list to the current position in the file
     * 
     *  @param  mcParticleList the mc particle list
     */
    StatusCode WriteMCParticleList(const MCParticleList &mcParticleList);

    /**
     *  @brief  Write calo hit to mc particle relationships for a specified calo hit list
     * 
     *  @param  caloHitList the calo hit list
     */
    StatusCode WriteCaloHitToMCParticleRelationships(const CaloHitList &caloHitList);

    /**
     *  @brief  Write track to mc particle relationships for a specified track list
     * 
     *  @param  trackList the track list
     */
    StatusCode WriteTrackToMCParticleRelationships(const TrackList &trackList);

    /**
     *  @brief  Write mc particle relationships for a specified mc particle list
     * 
     *  @param  mcParticleList the mc particle list
     */
    StatusCode WriteMCParticleRelationships(const MCParticleList &mcParticleList);

    /**
     *  @brief  Write track relationships for a specified list of tracks
     * 
     *  @param  trackList the track list
     */
    StatusCode WriteTrackRelationships(const TrackList &trackList);

    /**
     *  @brief  Write a calo hit to mc particle relationship to the current position in the file
     * 
     *  @param  pCaloHit address of the calo hit
     */
    StatusCode WriteCaloHitToMCParticleRelationship(const CaloHit *const pCaloHit);

    /**
     *  @brief  Write a track to mc particle relationship to the current position in the file
     * 
     *  @param  pTrack address of the track
     */
    StatusCode WriteTrackToMCParticleRelationship(const Track *const pTrack);

    /**
     *  @brief  Write a mc particle parent/daughter relationship to the current position in the file
     * 
     *  @param  pMCParticle address of the mc particle
     */
    StatusCode WriteMCParticleRelationships(const MCParticle *const pMCParticle);

    /**
     *  @brief  Write a track sibling or parent/daughter relationship to the current position in the file
     * 
     *  @param  pTrack address of the track
     */
    StatusCode WriteTrackRelationships(const Track *const pTrack);
};

} // namespace pandora

#endif // #ifndef PANDORA_FILE_WRITER_H
