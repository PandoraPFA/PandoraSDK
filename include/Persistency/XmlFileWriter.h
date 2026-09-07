/**
 *  @file   PandoraSDK/include/Persistency/XmlFileWriter.h
 *
 *  @brief  Header file for the xml file writer class.
 *
 *  $Log: $
 */
#ifndef PANDORA_XML_FILE_WRITER_H
#define PANDORA_XML_FILE_WRITER_H 1

#include "Pandora/Pandora.h"

#include "Persistency/FileWriter.h"
#include "Persistency/Persistency.h"

#include "Xml/tinyxml.h"

namespace pandora
{

/**
 *  @brief  XmlFileWriter
 *
 *  Writes Pandora objects to a self-describing XML file. Document structure:
 *
 *  <PandoraFile>
 *    <Header>
 *      <Metadata schemaVersion="1">...</Metadata>
 *      <SchemaRegistry schemaVersion="0">...</SchemaRegistry>
 *    </Header>
 *    <Geometry><LArTPC schemaVersion="1">...</LArTPC></Geometry>
 *    <Event><CaloHit schemaVersion="1">...</CaloHit></Event>
 *  </PandoraFile>
 */
class XmlFileWriter : public FileWriter
{
public:
    /**
     *  @brief  Constructor
     *
     *  @param  pandora   the pandora instance
     *  @param  fileName  the name of the output file
     *  @param  fileMode  APPEND (default) or OVERWRITE
     */
    XmlFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode = APPEND);

    /**
     *  @brief  Destructor — saves the XML document to disk
     */
    ~XmlFileWriter();

    /**
     *  @brief  Write the global header information to the file
     */
    StatusCode WriteGlobalHeader();

private:
    /**
     *  @brief  Write the header of the file.
     *
     *  @param  containerId the ID of the container to be written
     */
    StatusCode WriteHeader(const ContainerId containerId);

    /**
     *  @brief  Write the footer of the file.
     */
    StatusCode WriteFooter();

    /**
     *  @brief  Write the metadata of the current component to the file.
     */
    StatusCode WriteMetadata();

    /**
     *  @brief  Write the schema registry of the current component to the file.
     */
    StatusCode WriteSchemaRegistry();

    /**
     *  @brief  Write the fields of the current component to the file.
     *
     *  @param  elementName the name of the XML element to be written
     *  @param  schemaVersion the schema version of the current component
     *  @param  fields the field map containing the fields of the current component
     */
    StatusCode WriteComponent(const std::string &elementName, const unsigned int schemaVersion, const FieldMap &fields);

    /**
     *  @brief  Write the sub-detector of the current component to the file.
     *
     *  @param  pSubDetector the address of the sub-detector to be written
     */
    StatusCode WriteSubDetector(const SubDetector *const pSubDetector);

    /**
     *  @brief  Write the LArTPC of the current component to the file.
     *
     *  @param  pLArTPC the address of the LArTPC to be written
     */
    StatusCode WriteLArTPC(const LArTPC *const pLArTPC);

    /**
     *  @brief  Write the detector gap of the current component to the file.
     *
     *  @param  pDetectorGap the address of the detector gap to be written
     */
    StatusCode WriteDetectorGap(const DetectorGap *const pDetectorGap);

    /**
     *  @brief  Write the calo hit of the current component to the file.
     *
     *  @param  pCaloHit the address of the calo hit to be written
     */
    StatusCode WriteCaloHit(const CaloHit *const pCaloHit);

    /**
     *  @brief  Write the track of the current component to the file.
     *
     *  @param  pTrack the address of the track to be written
     */
    StatusCode WriteTrack(const Track *const pTrack);

    /**
     *  @brief  Write the MC particle of the current component to the file.
     *
     *  @param  pMCParticle the address of the MC particle to be written
     */
    StatusCode WriteMCParticle(const MCParticle *const pMCParticle);

    /**
     *  @brief  Write the relationship between objects of the current component to the file.
     *
     *  @param  relationshipId the ID of the relationship to be written
     *  @param  address1 the address of the first object in the relationship
     *  @param  address2 the address of the second object in the relationship
     *  @param  weight the weight of the relationship
     */
    StatusCode WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2, const float weight);

    /**
     *  @brief  Write the event information of the current component to the file.
     */
    StatusCode WriteEventInformation();

    TiXmlDocument *m_pXmlDocument;
    TiXmlElement *m_pContainerXmlElement;
    TiXmlElement *m_pCurrentXmlElement;
};

} // namespace pandora

#endif // #ifndef PANDORA_XML_FILE_WRITER_H
