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

#include "Persistency/FileWriter.h"
#include "Persistency/Persistency.h"

#include <fstream>

namespace pandora
{

/**
 *  @brief  BinaryFileWriter
 *
 *  Writes Pandora objects to a compact binary file using tagged-field component records. Layout per component:
 *
 *      [ComponentId  : uint32]
 *      [SchemaVersion: uint32]
 *      [NumFields    : uint32]
 *      repeated NumFields times:
 *          [TagLength : uint16]
 *          [Tag       : char * TagLength]
 *          [DataLength: uint32]
 *          [Data      : byte * DataLength]
 *      [END_MARKER   : uint32 = 0xDEADBEEF]
 */
class BinaryFileWriter : public FileWriter
{
public:
    /**
     *  @brief  Constructor
     *
     *  @param  pandora   the pandora instance to be used alongside the file writer
     *  @param  fileName  the name of the output file
     *  @param  fileMode  APPEND (default) or OVERWRITE
     */
    BinaryFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode = APPEND);

    /**
     *  @brief  Destructor
     */
    ~BinaryFileWriter();

    /**
     *  @brief  Write the global header to the file
     */
    StatusCode WriteGlobalHeader();

private:
    /**
     *  @brief  Write the header for a new container to the file
     *
     *  @param  containerId the container id
     */
    StatusCode WriteHeader(const ContainerId containerId);

    /**
     *  @brief  Write the footer for a container to the file
     */
    StatusCode WriteFooter();

    /**
     *  @brief  Write the metadata to the file
     */
    StatusCode WriteMetadata();

    /**
     *  @brief  Write the schema registry to the file
     */
    StatusCode WriteSchemaRegistry();

    /**
     *  @brief  Write the sub-detector to the file
     *
     *  @param  pSubDetector the sub-detector to write
     */
    StatusCode WriteSubDetector(const SubDetector *const pSubDetector);

    /**
     *  @brief Write the LArTPC to the file
     *
     *  @param  pLArTPC the LArTPC to write
     */
    StatusCode WriteLArTPC(const LArTPC *const pLArTPC);

    /**
     *  @brief  Write the detector gap to the file
     *
     *  @param  pDetectorGap the detector gap to write
     */
    StatusCode WriteDetectorGap(const DetectorGap *const pDetectorGap);

    /**
     *  @brief  Write the calo hit to the file
     *
     *  @param  pCaloHit the calo hit to write
     */
    StatusCode WriteCaloHit(const CaloHit *const pCaloHit);

    /**
     *  @brief  Write the track to the file
     *
     *  @param  pTrack the track to write
     */
    StatusCode WriteTrack(const Track *const pTrack);

    /**
     *  @brief  Write the MC particle to the file
     *
     *  @param  pMCParticle the MC particle to write
     */
    StatusCode WriteMCParticle(const MCParticle *const pMCParticle);

    /**
     *  @brief  Write the relationship to the file
     *
     *  @param  relationshipId the relationship id
     *  @param  address1 the address of the first object in the relationship
     *  @param  address2 the address of the second object in the relationship
     *  @param  weight the weight of the relationship
     */
    StatusCode WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2, const float weight);

    /**
     *  @brief  Write the event information to the file
     */
    StatusCode WriteEventInformation();

    /**
     *  @brief  Write a component to the file
     *
     *  @param  componentId the component id
     *  @param  schemaVersion the schema version of the component
     *  @param  fields the field map containing the component fields
     */
    StatusCode WriteComponent(const ComponentId componentId, const unsigned int schemaVersion, const FieldMap &fields);

    /**
     *  @brief  Write a variable of type T to the file stream. Low-level stream primitive.
     *
     *  @param  t the variable to write
     */
    template <typename T>
    StatusCode WriteVariable(const T &t);

    static constexpr uint32_t COMPONENT_END_MARKER = 0xDEADBEEFu;

    std::ofstream::pos_type m_containerPosition;
    std::ofstream           m_fileStream;
};

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline StatusCode BinaryFileWriter::WriteVariable(const T &t)
{
    m_fileStream.write(reinterpret_cast<const char *>(&t), sizeof(T));

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template <>
inline StatusCode BinaryFileWriter::WriteVariable(const std::string &t)
{
    const uint32_t stringSize(static_cast<uint32_t>(t.size()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(stringSize));
    m_fileStream.write(t.c_str(), stringSize);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef PANDORA_BINARY_FILE_WRITER_H
