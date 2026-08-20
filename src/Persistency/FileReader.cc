/**
 *  @file   PandoraSDK/src/Persistency/FileReader.cc
 *
 *  @brief  Implementation of the file reader class.
 *
 *  $Log: $
 */

#include "Api/PandoraApi.h"

#include "Persistency/FileReader.h"

namespace pandora
{

FileReader::FileReader(const pandora::Pandora &pandora, const std::string &fileName) :
    Persistency(pandora, fileName),
    m_fileMajorVersion(1),
    m_fileMinorVersion(0)
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
        RETURN_ON_ERROR(this->GoToGlobalHeader());
    }

    RETURN_ON_ERROR(this->ReadHeader());

    if (HEADER_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    try
    {
        while (STATUS_CODE_SUCCESS == this->ReadNextGlobalHeaderComponent())
            continue;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << " FileReader::ReadGlobalHeader() encountered unrecognized object in file: " << statusCodeException.ToString() << std::endl;
    }

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadGeometry()
{
    if (GEOMETRY_CONTAINER != this->GetNextContainerId())
    {
        RETURN_ON_ERROR(this->GoToNextGeometry());
    }

    RETURN_ON_ERROR(this->ReadHeader());

    if (GEOMETRY_CONTAINER != m_containerId)
        return STATUS_CODE_FAILURE;

    try
    {
        while (STATUS_CODE_SUCCESS == this->ReadNextGeometryComponent())
            continue;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << " FileReader::ReadGeometry() encountered unrecognized object in file: " << statusCodeException.ToString() << std::endl;
    }

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadEvent()
{
    if (EVENT_CONTAINER != this->GetNextContainerId())
    {
        RETURN_ON_ERROR(this->GoToNextEvent());
    }

    RETURN_ON_ERROR(this->ReadHeader());

    try
    {
        while (STATUS_CODE_SUCCESS == this->ReadNextEventComponent())
            continue;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << " FileReader::ReadEvent() encountered unrecognized object in file: " << statusCodeException.ToString() << std::endl;
    }

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::GoToGlobalHeader()
{
    do
    {
        RETURN_ON_ERROR(this->GoToNextContainer());
    } while (HEADER_CONTAINER != this->GetNextContainerId());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::GoToNextGeometry()
{
    do
    {
        RETURN_ON_ERROR(this->GoToNextContainer());
    } while (GEOMETRY_CONTAINER != this->GetNextContainerId());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::GoToNextEvent()
{
    do
    {
        RETURN_ON_ERROR(this->GoToNextContainer());
    } while (EVENT_CONTAINER != this->GetNextContainerId());

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
