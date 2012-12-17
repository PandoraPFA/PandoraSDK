/**
 *  @file   PandoraPFANew/Framework/include/Persistency/FileWriter.h
 * 
 *  @brief  Header file for the file writer class.
 * 
 *  $Log: $
 */
#ifndef FILE_WRITER_H
#define FILE_WRITER_H 1

#include "Helpers/GeometryHelper.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraIO.h"
#include "Pandora/StatusCodes.h"

#include "Objects/CartesianVector.h"
#include "Objects/TrackState.h"

#include <fstream>
#include <string>

namespace pandora
{

/**
 *  @brief  FileWriter class
 */
class FileWriter
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  algorithm the pandora instance to be used alongside the file writer
     *  @param  fileName the name of the output file
     *  @param  fileMode the mode for file writing
     */
    FileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode = APPEND);

    /**
     *  @brief  Destructor
     */
    ~FileWriter();

    /**
     *  @brief  Write the current geometry information to the file
     */
    StatusCode WriteGeometry();

    /**
     *  @brief  Write the current event to the file
     * 
     *  @param  writeMCRelationships whether to write mc relationship information to the file
     *  @param  writeTrackRelationships whether to write track relationship information to the file
     */
    StatusCode WriteEvent(const bool writeMCRelationships = true, const bool writeTrackRelationships = true);

    /**
     *  @brief  Write the specified event components to the file
     * 
     *  @param  caloHitList the list of calo hits to write to the file
     *  @param  trackList the list of tracks to write to the file
     *  @param  writeMCRelationships whether to write mc relationship information to the file
     *  @param  writeTrackRelationships whether to write track relationship information to the file
     */
    StatusCode WriteEvent(const CaloHitList &caloHitList, const TrackList &trackList, const bool writeMCRelationships = true,
        const bool writeTrackRelationships = true);

private:
   /**
     *  @brief  Write the container header to the file
     * 
     *  @param  containerId the container id
     */
    StatusCode WriteHeader(const ContainerId containerId);

    /**
     *  @brief  Write the container footer to the file
     */
    StatusCode WriteFooter();

    /**
     *  @brief  Write the geometry parameters to the file
     */
    StatusCode WriteGeometryParameters();

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
     *  @brief  Write a sub detector to the current position in the file
     * 
     *  @param  subDetectorName the sub detector name
     *  @param  pSubDetectorParameters address of the sub detector parameters
     */
    StatusCode WriteSubDetector(const std::string &subDetectorName, const GeometryHelper::SubDetectorParameters *const pSubDetectorParameters);

    /**
     *  @brief  Write the detector gap parameters to the file
     */
    StatusCode WriteDetectorGaps();

    /**
     *  @brief  Write a calo hit to the current position in the file
     * 
     *  @param  pCaloHit address of the calo hit
     */
    StatusCode WriteCaloHit(const CaloHit *const pCaloHit);

    /**
     *  @brief  Write a track to the current position in the file
     * 
     *  @param  pTrack address of the track
     */
    StatusCode WriteTrack(const Track *const pTrack);

    /**
     *  @brief  Write a mc particle to the current position in the file
     * 
     *  @param  pMCParticle address of the mc particle
     */
    StatusCode WriteMCParticle(const MCParticle *const pMCParticle);

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

    /**
     *  @brief  Write a relationship between two objects with specified addresses
     * 
     *  @param  relationshipId the relationship id
     *  @param  address1 first address to write
     *  @param  address2 second address to write
     */
    StatusCode WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2);

    /**
     *  @brief  Write a variable to the file
     */
    template<typename T>
    StatusCode WriteVariable(const T &t);

    const pandora::Pandora *const   m_pPandora;             ///< Address of pandora instance to be used alongside the file writer
    ContainerId                     m_containerId;          ///< The type of container currently being written to file
    std::ofstream::pos_type         m_containerPosition;    ///< Position of start of the current event/geometry container object in file
    std::ofstream                   m_fileStream;           ///< The stream class to write to the file
};

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode FileWriter::WriteVariable(const T &t)
{
    m_fileStream.write(reinterpret_cast<const char*>(&t), sizeof(T));

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode FileWriter::WriteVariable(const std::string &t)
{
    const unsigned int stringSize(t.size());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(stringSize));
    m_fileStream.write(reinterpret_cast<const char*>(t.c_str()), stringSize);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode FileWriter::WriteVariable(const CartesianVector &t)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetX()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetY()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetZ()));
    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode FileWriter::WriteVariable(const TrackState &t)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetPosition()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetMomentum()));
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef FILE_WRITER_H
