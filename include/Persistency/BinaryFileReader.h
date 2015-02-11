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

#include "Objects/CartesianVector.h"
#include "Objects/TrackState.h"

#include "Persistency/FileReader.h"

#include <fstream>

namespace pandora
{

/**
 *  @brief  BinaryFileReader class
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
    virtual ~BinaryFileReader();

private:
    StatusCode ReadHeader();
    StatusCode GoToNextContainer();
    ContainerId GetNextContainerId();
    StatusCode GoToGeometry(const unsigned int geometryNumber);
    StatusCode GoToEvent(const unsigned int eventNumber);
    StatusCode ReadNextGeometryComponent();
    StatusCode ReadNextEventComponent();

    /**
     *  @brief  Read a sub detector from the current position in the file
     * 
     *  @param  checkComponentId whether to check the component id before deserializing
     */
    StatusCode ReadSubDetector(bool checkComponentId = true);

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
     *  @param  checkComponentId whether to check the component id before deserializing
     */
    StatusCode ReadMCParticle(bool checkComponentId = true);

    /**
     *  @brief  Read a relationship from the current position in the file, recreating the stored relationship
     * 
     *  @param  checkComponentId whether to check the component id before deserializing
     */
    StatusCode ReadRelationship(bool checkComponentId = true);

    /**
     *  @brief  Read a variable from the file
     */
    template<typename T>
    StatusCode ReadVariable(T &t);

    std::ifstream::pos_type         m_containerPosition;    ///< Position of start of the current event/geometry container object in file
    std::ifstream::pos_type         m_containerSize;        ///< Size of the current event/geometry container object in the file
    std::ifstream                   m_fileStream;           ///< The stream class to read from the file
};

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode BinaryFileReader::ReadVariable(T &t)
{
    char *const pMemBlock = new char[sizeof(T)];
    m_fileStream.read(pMemBlock, sizeof(T));

    t = *(reinterpret_cast<T*>(pMemBlock));
    delete[] pMemBlock;

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode BinaryFileReader::ReadVariable(std::string &t)
{
    unsigned int stringSize;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(stringSize));

    char *const pMemBlock = new char[stringSize];
    m_fileStream.read(pMemBlock, stringSize);

    t = std::string(pMemBlock, stringSize);
    delete[] pMemBlock;

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode BinaryFileReader::ReadVariable(CartesianVector &t)
{
    float x, y, z;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(x));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(y));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(z));
    t = CartesianVector(x, y, z);

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode BinaryFileReader::ReadVariable(TrackState &t)
{
    CartesianVector position(0.f, 0.f, 0.f), momentum(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(position));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(momentum));
    t = TrackState(position, momentum);

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef PANDORA_BINARY_FILE_READER_H
