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

#include <string>

namespace pandora
{

class Pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  FileReader class
 */
class FileReader
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
     *  @brief  Read the current geometry information from the file
     */
    StatusCode ReadGeometry();

    /**
     *  @brief  Read an entire pandora event from the file, recreating the stored objects
     */
    StatusCode ReadEvent();

    /**
     *  @brief  Skip to next geometry container in the file
     */
    StatusCode GoToNextGeometry();

    /**
     *  @brief  Skip to next event container in the file
     */
    StatusCode GoToNextEvent();

    /**
     *  @brief  Skip to a specified geometry number in the file
     * 
     *  @param  geometryNumber the geometry number
     */
    virtual StatusCode GoToGeometry(const unsigned int geometryNumber) = 0;

    /**
     *  @brief  Skip to a specified event number in the file
     * 
     *  @param  eventNumber the event number
     */
    virtual StatusCode GoToEvent(const unsigned int eventNumber) = 0;

protected:
    /**
     *  @brief  Read the container header from the current position in the file, checking for properly written container
     */
    virtual StatusCode ReadHeader() = 0;

    /**
     *  @brief  Skip to next container in the file
     */
    virtual StatusCode GoToNextContainer() = 0;

    /**
     *  @brief  Get the id of the next container in the file without changing the current position in the file
     * 
     *  @return The id of the next container in the file
     */
    virtual ContainerId GetNextContainerId() = 0;

    /**
     *  @brief  Read the next pandora geometry component from the current position in the file, recreating the stored component
     */
    virtual StatusCode ReadNextGeometryComponent() = 0;

    /**
     *  @brief  Read the next pandora event component from the current position in the file, recreating the stored component
     */
    virtual StatusCode ReadNextEventComponent() = 0;

    const Pandora *const            m_pPandora;             ///< Address of pandora instance to be used alongside the file reader
    ContainerId                     m_containerId;          ///< The type of container currently being read from file
    std::string                     m_fileName;             ///< The file name
};

} // namespace pandora

#endif // #ifndef PANDORA_FILE_READER_H
