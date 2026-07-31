/**
 *  @file   PandoraSDK/include/Persistency/BinaryFileReader.h
 *
 *  @brief  Header file for the binary file reader class.
 *
 *  $Log: $
 */
#ifndef PANDORA_BINARY_FILE_READER_H
#define PANDORA_BINARY_FILE_READER_H 1

#include "Pandora/Pandora.h"

#include "Persistency/FileReader.h"
#include "Persistency/Persistency.h"

#include <fstream>
#include <functional>
#include <unordered_map>

namespace pandora
{

/**
 *  @brief  BinaryFileReader
 *
 *  Reads Pandora objects from a binary file written by BinaryFileWriter.
 *
 *  Migration registration
 *  ----------------------
 *  Call RegisterMigration() to install a schema upgrade function for a given
 *  component type and version transition. Migrations are chained automatically.
 *
 *  Example:
 *  @code
 *      reader.RegisterMigration(CALO_HIT_COMPONENT, 1, 2,
 *          [](FieldMap &fields) {
 *              float v;
 *              if (STATUS_CODE_SUCCESS == fields.Get("mipEnergy", v))
 *              {
 *                  fields.Set("mipEquivalentEnergy", v);
 *                  fields.Remove("mipEnergy");
 *              }
 *          });
 *  @endcode
 */
class BinaryFileReader : public FileReader
{
public:
    /**
     *  @brief  Constructor
     *
     *  @param  pandora the pandora instance to be used alongside the file reader
     *  @param  fileName the name of the file containing the pandora objects
     */
    BinaryFileReader(const pandora::Pandora &pandora, const std::string &fileName);

    /**
     *  @brief  Destructor
     */
    ~BinaryFileReader();

    typedef std::function<void(FieldMap &)> MigrationFn;

    /**
     *  @brief  Register a migration function for a given component type and version transition
     *
     *  @param  componentId the component type identifier
     *  @param  fromVersion the schema version to migrate from
     *  @param  toVersion the schema version to migrate to
     *  @param  fn the migration function to apply to the FieldMap
     */
    void RegisterMigration(const ComponentId componentId, const unsigned int fromVersion,
        const unsigned int toVersion, MigrationFn fn);

private:
    /**
     *  @brief  Read the header of the file.
     */
    StatusCode ReadHeader();

    /**
     *  @brief  Go to the next container in the file.
     */
    StatusCode GoToNextContainer();

    /**
     *  @brief  Get the next container ID in the file.
     *
     *  @return The next container ID
     */
    ContainerId GetNextContainerId();

    /**
     *  @brief  Go to the specified geometry number in the file.
     *
     *  @param  geometryNumber the geometry number to go to
     */
    StatusCode GoToGeometry(const unsigned int geometryNumber);

    /**
     *  @brief  Go to the specified event number in the file.
     *
     *  @param  eventNumber the event number to go to
     */
    StatusCode GoToEvent(const unsigned int eventNumber);

    /**
     *  @brief  Read the next global header component from the file.
     */
    StatusCode ReadNextGlobalHeaderComponent();

    /**
     *  @brief  Read the next geometry component from the file.
     */
    StatusCode ReadNextGeometryComponent();

    /**
     *  @brief  Read the next event component from the file.
     */
    StatusCode ReadNextEventComponent();

    /**
     *  @brief  Read the next component from the file.
     *
     *  @param  expectedContainer the expected container ID
     *  @param  endComponentId the component ID that indicates the end of the container
     */
    StatusCode ReadNextComponent([[maybe_unused]] const ContainerId expectedContainer, const ComponentId endComponentId);

    /**
     *  @brief  Read the component fields from the file.
     *
     *  @param  componentId the component ID to read
     *  @param  schemaVersion the schema version to read
     *  @param  fields the field map to populate with the component fields
     */
    StatusCode ReadComponentFields(ComponentId &componentId, unsigned int &schemaVersion, FieldMap &fields);

    /**
     *  @brief  Apply any registered migrations to the component fields.
     *
     *  @param  componentId the component ID to apply migrations for
     *  @param  fileSchemaVersion the schema version of the component in the file
     *  @param  fields the field map to apply migrations to
     */
    void ApplyMigrations(const ComponentId componentId, const unsigned int fileSchemaVersion, FieldMap &fields) const;

    /**
     *  @brief  Read the metadata from the file.
     *
     *  @param  fields the field map containing the metadata fields
     */
    StatusCode ReadMetadata(const FieldMap &fields);

    /**
     *  @brief  Read the schema registry from the file.
     *
     *  @param  fields the field map containing the schema registry fields
     */
    StatusCode ReadSchemaRegistry(const FieldMap &fields);

    /**
     *  @brief  Read the sub-detector from the file.
     *
     *  @param  fields the field map containing the sub-detector fields
     */
    StatusCode ReadSubDetector(const FieldMap &fields);

    /**
     *  @brief  Read the LArTPC from the file.
     *
     *  @param  fields the field map containing the LArTPC fields
     */
    StatusCode ReadLArTPC(const FieldMap &fields);

    /**
     *  @brief  Read the line gap from the file.
     *
     *  @param  fields the field map containing the line gap fields
     */
    StatusCode ReadLineGap(const FieldMap &fields);

    /**
     *  @brief  Read the box gap from the file.
     *
     *  @param  fields the field map containing the box gap fields
     */
    StatusCode ReadBoxGap(const FieldMap &fields);

    /**
     *  @brief  Read the concentric gap from the file.
     *
     *  @param  fields the field map containing the concentric gap fields
     */
    StatusCode ReadConcentricGap(const FieldMap &fields);

    /**
     *  @brief  Read the calo hit from the file.
     *
     *  @param  fields the field map containing the calo hit fields
     */
    StatusCode ReadCaloHit(const FieldMap &fields);

    /**
     *  @brief  Read the track from the file.
     *
     *  @param  fields the field map containing the track fields
     */
    StatusCode ReadTrack(const FieldMap &fields);

    /**
     *  @brief  Read the MC particle from the file.
     *
     *  @param  fields the field map containing the MC particle fields
     */
    StatusCode ReadMCParticle(const FieldMap &fields);

    /**
     *  @brief  Read the relationship from the file.
     *
     *  @param  fields the field map containing the relationship fields
     */
    StatusCode ReadRelationship(const FieldMap &fields);

    /**
     *  @brief  Read the event information from the file.
     *
     *  @param  fields the field map containing the event information fields
     */
    StatusCode ReadEventInformation(const FieldMap &fields);

    /**
     *  @brief  Read a variable of type T from the file stream. Low-level stream primitive.
     *
     *  @param  t the variable to read into
     */
    template <typename T>
    StatusCode ReadVariable(T &t);

    struct MigrationKey
    {
        ComponentId  m_componentId;
        unsigned int m_fromVersion;

        /**
         *  @brief  Equality operator for MigrationKey
         *
         *  @param  rhs the right-hand side MigrationKey to compare with
         *
         *  @return True if the component ID and from version are equal, false otherwise
         */
        bool operator==(const MigrationKey &rhs) const
        {
            return m_componentId == rhs.m_componentId && m_fromVersion == rhs.m_fromVersion;
        }
    };

    struct MigrationKeyHash
    {
        /**
         *  @brief  Hash function for MigrationKey
         *
         *  @param  k the MigrationKey to hash
         *
         *  @return The hash value of the MigrationKey
         */
        std::size_t operator()(const MigrationKey &k) const
        {
            return std::hash<unsigned int>()(static_cast<unsigned int>(k.m_componentId))
                ^ (std::hash<unsigned int>()(k.m_fromVersion) << 16);
        }
    };

    static constexpr uint32_t COMPONENT_END_MARKER = 0xDEADBEEFu;

    std::ifstream::pos_type m_containerPosition;
    std::ifstream::pos_type m_containerSize;
    std::ifstream           m_fileStream;

    std::unordered_map<MigrationKey, MigrationFn, MigrationKeyHash> m_migrations;
};

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
inline StatusCode BinaryFileReader::ReadVariable(T &t)
{
    char *const pMemBlock = new char[sizeof(T)];
    m_fileStream.read(pMemBlock, sizeof(T));

    t = *(reinterpret_cast<T *>(pMemBlock));
    delete[] pMemBlock;

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
inline StatusCode BinaryFileReader::ReadVariable(std::string &t)
{
    uint32_t stringSize = 0;
    const StatusCode statusCode(this->ReadVariable(stringSize));

    if (STATUS_CODE_SUCCESS != statusCode)
        return statusCode;

    char *const pMemBlock = new char[stringSize];
    m_fileStream.read(pMemBlock, stringSize);

    t = std::string(pMemBlock, stringSize);
    delete[] pMemBlock;

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef PANDORA_BINARY_FILE_READER_H
