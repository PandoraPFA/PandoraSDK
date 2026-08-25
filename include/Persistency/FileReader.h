/**
 *  @file   PandoraSDK/include/Persistency/FileReader.h
 *
 *  @brief  Header file for the file reader class.
 *
 *  $Log: $
 */
#ifndef PANDORA_FILE_READER_H
#define PANDORA_FILE_READER_H 1

#include "Pandora/StatusCodes.h"

#include "Persistency/PandoraIO.h"
#include "Persistency/Persistency.h"

#include <functional>
#include <string>
#include <unordered_map>

namespace pandora
{

class Pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  FileReader class
 *
 *  Migration registration
 *  ----------------------
 *  Call RegisterMigration() to install a schema upgrade function for a given component type and version transition. Migrations are chained
 *  automatically and are applied to the FieldMap before it is dispatched to component code, so component readers only ever see current
 *  schema tags. Register before calling ReadGlobalHeader(), so that the compatibility check can see the registered path.
 *
 *  Example:
 *  @code
 *      reader.RegisterMigration(CALO_HIT_COMPONENT, 1, 2, [](FieldMap &fields)
 *      {
 *          float v;
 *          if (STATUS_CODE_SUCCESS == fields.Get("mipEnergy", v))
 *          {
 *              fields.Set("mipEquivalentEnergy", v);
 *              fields.Remove("mipEnergy");
 *          }
 *      });
 *  @endcode
 */
class FileReader : public Persistency
{
public:
    typedef std::function<void(FieldMap &)> MigrationFn;

    /**
     *  @brief  Constructor
     *
     *  @param  pandora the pandora instance to be used alongside the file reader
     *  @param  fileName the name of the file containing the pandora objects
     */
    FileReader(const pandora::Pandora &pandora, const std::string &fileName);

    /**
     *  @brief  Destructor
     */
    virtual ~FileReader();

    /**
     *  @brief  Register a migration function for a given component type and version transition.
     *
     *  @param  componentId the component type identifier
     *  @param  fromVersion the schema version to migrate from
     *  @param  toVersion the schema version to migrate to (must equal fromVersion + 1)
     *  @param  fn the migration function to apply to the FieldMap
     */
    void RegisterMigration(const ComponentId componentId, const unsigned int fromVersion,
        const unsigned int toVersion, MigrationFn fn);

    /**
     *  @brief  Set what happens when a file's recorded schema versions cannot be read faithfully by this build.
     *          Defaults to SCHEMA_CHECK_FAIL.
     */
    void SetSchemaCheckPolicy(const SchemaCheckPolicy policy);

    /**
     *  @brief  Read the global header information from the file
     */
    StatusCode ReadGlobalHeader();

    /**
     *  @brief  Read the geometry information from the file
     */
    StatusCode ReadGeometry();

    /**
     *  @brief  Read the event information from the file
     */
    StatusCode ReadEvent();

    /**
     *  @brief  Skip to global header container in the file
     */
    StatusCode GoToGlobalHeader();

    /**
     *  @brief  Skip to next geometry container in the file
     */
    StatusCode GoToNextGeometry();

    /**
     *  @brief  Skip to next event container in the file
     */
    StatusCode GoToNextEvent();

    /**
     *  @brief  Skip to the specified geometry number in the file
     *
     *  @param  geometryNumber the geometry number to go to
     */
    virtual StatusCode GoToGeometry(const unsigned int geometryNumber) = 0;

    /**
     *  @brief  Skip to the specified event number in the file
     *
     *  @param  eventNumber the event number to go to
     */
    virtual StatusCode GoToEvent(const unsigned int eventNumber) = 0;

protected:
    /**
     *  @brief  Apply any registered migrations to the component fields, chaining from the file's schema version upwards for as long as
     *          consecutive migrations are registered.
     *
     *  @param  componentId the component ID to apply migrations for
     *  @param  fileSchemaVersion the schema version of the component in the file
     *  @param  fields the field map to apply migrations to
     */
    void ApplyMigrations(const ComponentId componentId, const unsigned int fileSchemaVersion, FieldMap &fields) const;

    /**
     *  @brief  Read the header of the file.
     */
    virtual StatusCode ReadHeader() = 0;

    /**
     *  @brief  Go to the next container in the file.
     */
    virtual StatusCode GoToNextContainer() = 0;

    /**
     *  @brief  Get the next container ID in the file.
     *
     *  @return The next container ID
     */
    virtual ContainerId GetNextContainerId() = 0;

    /**
     *  @brief  Read the next global header component from the file.
     */
    virtual StatusCode ReadNextGlobalHeaderComponent() = 0;

    /**
     *  @brief  Read the next geometry component from the file.
     */
    virtual StatusCode ReadNextGeometryComponent() = 0;

    /**
     *  @brief  Read the next event component from the file.
     */
    virtual StatusCode ReadNextEventComponent() = 0;

private:
    /**
     *  @brief  Compare the schema registry read from the file's global header against this build's own schema
     *          versions, and report or reject any component this build cannot read faithfully.
     */
    StatusCode CheckSchemaCompatibility() const;

    /**
     *  @brief  Whether an unbroken chain of registered migrations spans the given version range.
     */
    bool HasMigrationPath(const ComponentId componentId, const unsigned int fromVersion, const unsigned int toVersion) const;

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
            return std::hash<unsigned int>()(static_cast<unsigned int>(k.m_componentId)) ^ (std::hash<unsigned int>()(k.m_fromVersion) << 16);
        }
    };

    std::unordered_map<MigrationKey, MigrationFn, MigrationKeyHash> m_migrations;
    SchemaCheckPolicy m_schemaCheckPolicy;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline void FileReader::SetSchemaCheckPolicy(const SchemaCheckPolicy policy)
{
    m_schemaCheckPolicy = policy;
}

} // namespace pandora

#endif // #ifndef PANDORA_FILE_READER_H
