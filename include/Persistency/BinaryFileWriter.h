/**
 *  @file   PandoraSDK/include/Persistency/BinaryFileWriter.h
 * 
 *  @brief  Header file for the binary file writer class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_BINARY_FILE_WRITER_H
#define PANDORA_BINARY_FILE_WRITER_H 1

#include "Pandora/Pandora.h"

#include "Objects/CartesianVector.h"
#include "Objects/TrackState.h"

#include "Persistency/FileWriter.h"

#include <fstream>

namespace pandora
{

/**
 *  @brief  BinaryFileWriter class
 */
class BinaryFileWriter : public FileWriter
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  algorithm the pandora instance to be used alongside the file writer
     *  @param  fileName the name of the output file
     *  @param  fileMode the mode for file writing
     */
    BinaryFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode = APPEND);

    /**
     *  @brief  Destructor
     */
    ~BinaryFileWriter();

private:
    StatusCode WriteHeader(const ContainerId containerId);
    StatusCode WriteFooter();
    StatusCode WriteSubDetector(const SubDetector *const pSubDetector);
    StatusCode WriteDetectorGap(const DetectorGap *const pDetectorGap);
    StatusCode WriteCaloHit(const CaloHit *const pCaloHit);
    StatusCode WriteTrack(const Track *const pTrack);
    StatusCode WriteMCParticle(const MCParticle *const pMCParticle);
    StatusCode WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2, const float weight);

    /**
     *  @brief  Write a variable to the file
     */
    template<typename T>
    StatusCode WriteVariable(const T &t);

    std::ofstream::pos_type         m_containerPosition;    ///< Position of start of the current event/geometry container object in file
    std::ofstream                   m_fileStream;           ///< The stream class to write to the file
};

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode BinaryFileWriter::WriteVariable(const T &t)
{
    m_fileStream.write(reinterpret_cast<const char*>(&t), sizeof(T));

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode BinaryFileWriter::WriteVariable(const std::string &t)
{
    const unsigned int stringSize(t.size());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(stringSize));
    m_fileStream.write(reinterpret_cast<const char*>(t.c_str()), stringSize);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode BinaryFileWriter::WriteVariable(const CartesianVector &t)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetX()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetY()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetZ()));
    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode BinaryFileWriter::WriteVariable(const TrackState &t)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetPosition()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetMomentum()));
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef PANDORA_BINARY_FILE_WRITER_H
