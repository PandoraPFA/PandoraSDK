/**
 *  @file   PandoraSDK/include/Persistency/XmlFileReader.h
 * 
 *  @brief  Header file for the xml file reader class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_XML_FILE_READER_H
#define PANDORA_XML_FILE_READER_H 1

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
    virtual ~XmlFileReader();

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

} // namespace pandora

#endif // #ifndef PANDORA_XML_FILE_READER_H
