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

#include "Pandora/Pandora.h"

#include "Persistency/FileReader.h"
#include "Persistency/Persistency.h"

namespace pandora
{

/**
 *  @brief  XmlFileReader
 *
 *  Reads Pandora objects from an XML file written by XmlFileWriter.
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

private:
    /**
     *  @brief  Read the header of the file.
     */
    StatusCode ReadHeader();

    /**
     *  @brief  Go to the next container in the file.
     */
    StatusCode GoToNextContainer();

    /**
     *  @brief  Get the ID of the next container in the file.
     *
     *  @return the ID of the next container
     */
    ContainerId GetNextContainerId();

    /**
     *  @brief  Go to the specified geometry container in the file.
     *
     *  @param  geometryNumber the number of the geometry container to go to
     */
    StatusCode GoToGeometry(const unsigned int geometryNumber);

    /**
     *  @brief  Go to the specified event container in the file.
     *
     *  @param  eventNumber the number of the event container to go to
     */
    StatusCode GoToEvent(const unsigned int eventNumber);

    /**
     *  @brief  Read the next global header component from the file.
     */
    StatusCode ReadNextGlobalHeaderComponent();

    /**
     *  @brief  Read the next geometry component from the file.
     */
    StatusCode ReadNextGeometryComponent();

    /**
     *  @brief  Read the next event component from the file.
     */
    StatusCode ReadNextEventComponent();

    /**
     *  @brief  Read the next component from the file.
     *
     *  @param  expectedContainer the expected container ID
     */
    StatusCode ReadNextComponent([[maybe_unused]] const ContainerId expectedContainer);

    /**
     *  @brief  Read the fields of the current component from the file.
     *
     *  @param  schemaVersion the schema version of the current component
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadComponentFields(unsigned int &schemaVersion, FieldMap &fields) const;

    /**
     *  @brief  Read the metadata of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadMetadata(const FieldMap &fields);

    /**
     *  @brief  Read the schema registry of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadSchemaRegistry(const FieldMap &fields);

    /**
     *  @brief  Read the sub-detector of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadSubDetector(const FieldMap &fields);

    /**
     *  @breif  Read the LArTPC of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadLArTPC(const FieldMap &fields);

    /**
     *  @brief  Read the line gap of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadLineGap(const FieldMap &fields);

    /**
     *  @brief  Read the box gap of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadBoxGap(const FieldMap &fields);

    /**
     *  @brief  Read the concentric gap of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadConcentricGap(const FieldMap &fields);

    /**
     *  @brief  Read the calo hit of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadCaloHit(const FieldMap &fields);

    /**
     *  @brief  Read the track of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadTrack(const FieldMap &fields);

    /**
     *  @brief  Read the MC particle of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadMCParticle(const FieldMap &fields);

    /**
     *  @brief  Read the relationship of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadRelationship(const FieldMap &fields);

    /**
     *  @brief  Read the event information of the current component from the file.
     *
     *  @param  fields the field map to be populated with the fields of the current component
     */
    StatusCode ReadEventInformation(const FieldMap &fields);

    TiXmlDocument *m_pXmlDocument;
    TiXmlNode     *m_pContainerXmlNode;
    TiXmlElement  *m_pCurrentXmlElement;
    bool           m_isAtFileStart;
};

} // namespace pandora

#endif // #ifndef PANDORA_XML_FILE_READER_H
