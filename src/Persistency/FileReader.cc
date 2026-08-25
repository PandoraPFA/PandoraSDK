/**
 *  @file   PandoraSDK/src/Persistency/FileReader.cc
 *
 *  @brief  Implementation of the file reader class.
 *
 *  $Log: $
 */

#include "Api/PandoraApi.h"

#include "Persistency/FileReader.h"

#include <iostream>

namespace pandora
{

FileReader::FileReader(const pandora::Pandora &pandora, const std::string &fileName) :
    Persistency(pandora, fileName),
    m_schemaCheckPolicy(SCHEMA_CHECK_FAIL)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

FileReader::~FileReader()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void FileReader::RegisterMigration(const ComponentId componentId, const unsigned int fromVersion, const unsigned int toVersion,
    MigrationFn fn)
{
    if (toVersion != fromVersion + 1)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    MigrationKey key;
    key.m_componentId = componentId;
    key.m_fromVersion = fromVersion;
    m_migrations[key] = std::move(fn);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void FileReader::ApplyMigrations(const ComponentId componentId, const unsigned int fileSchemaVersion, FieldMap &fields) const
{
    unsigned int version = fileSchemaVersion;

    while (true)
    {
        MigrationKey key;
        key.m_componentId = componentId;
        key.m_fromVersion = version;

        auto it = m_migrations.find(key);

        if (it == m_migrations.end())
            break;

        it->second(fields);
        ++version;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool FileReader::HasMigrationPath(const ComponentId componentId, const unsigned int fromVersion, const unsigned int toVersion) const
{
    unsigned int version(fromVersion);

    while (version < toVersion)
    {
        MigrationKey key;
        key.m_componentId = componentId;
        key.m_fromVersion = version;

        if (m_migrations.find(key) == m_migrations.end())
            return false;

        ++version;
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::CheckSchemaCompatibility() const
{
    if (SCHEMA_CHECK_NONE == m_schemaCheckPolicy)
        return STATUS_CODE_SUCCESS;

    bool isFaithful(true);

    for (const ComponentSchemaVersion &entry : m_schemaRegistry)
    {
        const unsigned int fileVersion(entry.m_schemaVersion);
        const unsigned int buildVersion(GetSchemaVersion(entry.m_componentId));

        if (fileVersion == buildVersion)
            continue;

        std::cout << "FileReader: schema mismatch in " << m_fileName << " for component " << static_cast<unsigned int>(entry.m_componentId) <<
            " — file v" << fileVersion << ", build v" << buildVersion << ": ";

        if (fileVersion > buildVersion)
        {
            std::cout << "file written by a newer build; affected fields would read as defaults." << std::endl;
            isFaithful = false;
        }
        else if (!this->HasMigrationPath(entry.m_componentId, fileVersion, buildVersion))
        {
            std::cout << "no registered migration covers v" << fileVersion << " -> v" << buildVersion <<
                "; affected fields would read as defaults." << std::endl;
            isFaithful = false;
        }
        else
        {
            std::cout << "registered migrations will upgrade this component on read." << std::endl;
        }
    }

    if (isFaithful || (SCHEMA_CHECK_WARN == m_schemaCheckPolicy))
        return STATUS_CODE_SUCCESS;

    return STATUS_CODE_INVALID_PARAMETER;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadGlobalHeader()
{
    if (HEADER_CONTAINER != this->GetNextContainerId())
    {
        const StatusCode seekSc = this->GoToGlobalHeader();

        if (STATUS_CODE_SUCCESS != seekSc)
        {
            std::cout << "FileReader::ReadGlobalHeader() — no header container found; "
                      << "proceeding with default metadata and schema registry." << std::endl;
            return STATUS_CODE_SUCCESS;
        }
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadHeader());

    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    try
    {
        while (STATUS_CODE_SUCCESS == this->ReadNextGlobalHeaderComponent())
            continue;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "FileReader::ReadGlobalHeader() encountered unrecognized component: " << statusCodeException.ToString() << std::endl;
    }

    m_containerId = UNKNOWN_CONTAINER;

    return this->CheckSchemaCompatibility();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadGeometry()
{
    if (GEOMETRY_CONTAINER != this->GetNextContainerId())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextGeometry());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadHeader());

    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    try
    {
        while (STATUS_CODE_SUCCESS == this->ReadNextGeometryComponent())
            continue;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "FileReader::ReadGeometry() encountered unrecognized object in file: " << statusCodeException.ToString() << std::endl;
    }

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadEvent()
{
    if (EVENT_CONTAINER != this->GetNextContainerId())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextEvent());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadHeader());

    try
    {
        while (STATUS_CODE_SUCCESS == this->ReadNextEventComponent())
            continue;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "FileReader::ReadEvent() encountered unrecognized object in file: " << statusCodeException.ToString() << std::endl;
    }

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::GoToGlobalHeader()
{
    do
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextContainer());
    } while (HEADER_CONTAINER != this->GetNextContainerId());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::GoToNextGeometry()
{
    do
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextContainer());
    } while (GEOMETRY_CONTAINER != this->GetNextContainerId());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::GoToNextEvent()
{
    do
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GoToNextContainer());
    } while (EVENT_CONTAINER != this->GetNextContainerId());

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
