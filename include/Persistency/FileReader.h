/**
 *  @file   PandoraPFANew/Framework/include/Persistency/FileReader.h
 * 
 *  @brief  Header file for the file reader class.
 * 
 *  $Log: $
 */
#ifndef FILE_READER_H
#define FILE_READER_H 1

#include "Api/PandoraApi.h"

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
    ~FileReader();

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
    StatusCode GoToGeometry(const unsigned int geometryNumber);

    /**
     *  @brief  Skip to a specified event number in the file
     * 
     *  @param  eventNumber the event number
     */
    StatusCode GoToEvent(const unsigned int eventNumber);

private:
    /**
     *  @brief  Skip to next container in the file
     */
    StatusCode GoToNextContainer();

    /**
     *  @brief  Read the container header from the current position in the file, checking for properly written container
     */
    StatusCode ReadHeader();

    /**
     *  @brief  Get the id of the next container in the file without changing the current position in the file
     * 
     *  @return The id of the next container in the file
     */
    ContainerId GetNextContainerId();

    /**
     *  @brief  Read the next pandora geometry component from the current position in the file, recreating the stored component
     */
    StatusCode ReadNextGeometryComponent();

    /**
     *  @brief  Read the next pandora event component from the current position in the file, recreating the stored component
     */
    StatusCode ReadNextEventComponent();

    /**
     *  @brief  Read the geometry parameters from the current position in the file
     */
    StatusCode ReadGeometryParameters();

    /**
     *  @brief  Read a sub detector from the current position in the file
     * 
     *  @param  subDetectorName to receive the sub detector name
     *  @param  pSubDetectorParameters address of the sub detector parameters to populate
     *  @param  checkComponentId whether to check the component id before deserializing
     */
    StatusCode ReadSubDetector(std::string &subDetectorName, PandoraApi::GeometryParameters::SubDetectorParameters *pSubDetectorParameters,
        bool checkComponentId = true);

    /**
     *  @brief  Read a box gap from the current position in the file
     * 
     *  @param  checkComponentId whether to check the component id before deserializing
     */
    StatusCode ReadBoxGap(bool checkComponentId = true);

    /**
     *  @brief  Read a concentric gap from the current position in the file
     * 
     *  @param  checkComponentId whether to check the component id before deserializing
     */
    StatusCode ReadConcentricGap(bool checkComponentId = true);

    /**
     *  @brief  Read a calo hit from the current position in the file, recreating the stored object
     * 
     *  @param  checkComponentId whether to check the component id before deserializing
     */
    StatusCode ReadCaloHit(bool checkComponentId = true);

    /**
     *  @brief  Read a track from the current position in the file, recreating the stored object
     * 
     *  @param  checkComponentId whether to check the component id before deserializing
     */
    StatusCode ReadTrack(bool checkComponentId = true);

    /**
     *  @brief  Read a mc particle from the current position in the file, recreating the stored object
     * 
     *  @param  pMCParticle address of the mc particle
     */
    StatusCode ReadMCParticle(bool checkComponentId = true);

    /**
     *  @brief  Read a relationship from the current position in the file, recreating the stored relationship
     * 
     *  @param  pCaloHit address of the calo hit
     */
    StatusCode ReadRelationship(bool checkComponentId = true);

    /**
     *  @brief  Read a variable from the file
     */
    template<typename T>
    StatusCode ReadVariable(T &t);

    const pandora::Pandora *const   m_pPandora;             ///< Address of pandora instance to be used alongside the file reader
    ContainerId                     m_containerId;          ///< The type of container currently being read from file
    std::ifstream::pos_type         m_containerPosition;    ///< Position of start of the current event/geometry container object in file
    std::ifstream::pos_type         m_containerSize;        ///< Size of the current event/geometry container object in the file
    std::ifstream                   m_fileStream;           ///< The stream class to read from the file
};

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode FileReader::ReadVariable(T &t)
{
    char *pMemBlock = new char[sizeof(T)];
    m_fileStream.read(pMemBlock, sizeof(T));

    t = *(reinterpret_cast<T*>(pMemBlock));
    delete[] pMemBlock;

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode FileReader::ReadVariable(std::string &t)
{
    unsigned int stringSize;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(stringSize));

    char *pMemBlock = new char[stringSize];
    m_fileStream.read(pMemBlock, stringSize);

    t = std::string(pMemBlock, stringSize);
    delete[] pMemBlock;

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode FileReader::ReadVariable(CartesianVector &t)
{
    float x, y, z;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(x));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(y));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(z));
    t = CartesianVector(x, y, z);

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode FileReader::ReadVariable(TrackState &t)
{
    CartesianVector position(0.f, 0.f, 0.f), momentum(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(position));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(momentum));
    t = TrackState(position, momentum);

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef FILE_READER_H
