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
    Persistency(pandora, fileName)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

FileReader::~FileReader()
{
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

    return STATUS_CODE_SUCCESS;
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
