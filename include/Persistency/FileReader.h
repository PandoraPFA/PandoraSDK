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

#include <string>

namespace pandora
{

class Pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  FileReader class
 */
class FileReader : public Persistency
{
public:
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
};

} // namespace pandora

#endif // #ifndef PANDORA_FILE_READER_H
