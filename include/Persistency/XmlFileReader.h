/**
 *  @file   PandoraSDK/include/Persistency/XmlFileReader.h
 * 
 *  @brief  Header file for the xml file reader class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_XML_FILE_READER_H
#define PANDORA_XML_FILE_READER_H 1

#include "Helpers/XmlHelper.h"

#include "Objects/CartesianVector.h"
#include "Objects/TrackState.h"

#include "Pandora/Pandora.h"

#include "Persistency/FileReader.h"

namespace pandora
{

/**
 *  @brief  XmlFileReader class
 */
class XmlFileReader : public FileReader
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pandora the pandora instance to be used alongside the file reader
     *  @param  fileName the name of the file containing the pandora objects
     */
    XmlFileReader(const pandora::Pandora &pandora, const std::string &fileName);

    /**
     *  @brief  Destructor
     */
    ~XmlFileReader();

    /**
     *  @brief  Read a variable from the file
     * 
     *  @param  xmlKey the xml key
     */
    template<typename T>
    StatusCode ReadVariable(const std::string &xmlKey, T &t);

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
     */
    StatusCode ReadSubDetector();

    /**
     *  @brief  Read a lar tpc from the current position in the file
     */
    StatusCode ReadLArTPC();

    /**
     *  @brief  Read a line gap from the current position in the file
     */
    StatusCode ReadLineGap();

    /**
     *  @brief  Read a box gap from the current position in the file
     */
    StatusCode ReadBoxGap();

    /**
     *  @brief  Read a concentric gap from the current position in the file
     */
    StatusCode ReadConcentricGap();

    /**
     *  @brief  Read a calo hit from the current position in the file, recreating the stored object
     */
    StatusCode ReadCaloHit();

    /**
     *  @brief  Read a track from the current position in the file, recreating the stored object
     */
    StatusCode ReadTrack();

    /**
     *  @brief  Read a mc particle from the current position in the file, recreating the stored object
     */
    StatusCode ReadMCParticle();

    /**
     *  @brief  Read a relationship from the current position in the file, recreating the stored relationship
     */
    StatusCode ReadRelationship();

    TiXmlDocument                  *m_pXmlDocument;         ///< The xml document
    TiXmlNode                      *m_pContainerXmlNode;    ///< The document xml node
    TiXmlElement                   *m_pCurrentXmlElement;   ///< The current xml element
    bool                            m_isAtFileStart;        ///< Whether reader is at file start
};

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode XmlFileReader::ReadVariable(const std::string &xmlKey, T &t)
{
    if (!m_pCurrentXmlElement)
        return STATUS_CODE_FAILURE;

    return XmlHelper::ReadValue(TiXmlHandle(m_pCurrentXmlElement), xmlKey, t);
}

template<>
inline StatusCode XmlFileReader::ReadVariable(const std::string &xmlKey, IntVector &t)
{
    if (!m_pCurrentXmlElement)
        return STATUS_CODE_FAILURE;

    return XmlHelper::ReadVectorOfValues(TiXmlHandle(m_pCurrentXmlElement), xmlKey, t);
}

template<>
inline StatusCode XmlFileReader::ReadVariable(const std::string &xmlKey, FloatVector &t)
{
    if (!m_pCurrentXmlElement)
        return STATUS_CODE_FAILURE;

    return XmlHelper::ReadVectorOfValues(TiXmlHandle(m_pCurrentXmlElement), xmlKey, t);
}

} // namespace pandora

#endif // #ifndef PANDORA_XML_FILE_READER_H
