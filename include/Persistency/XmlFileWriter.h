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

#include "Objects/CartesianVector.h"
#include "Objects/TrackState.h"

#include "Persistency/FileWriter.h"

namespace pandora
{

/**
 *  @brief  XmlFileWriter class
 */
class XmlFileWriter : public FileWriter
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  algorithm the pandora instance to be used alongside the file writer
     *  @param  fileName the name of the output file
     *  @param  fileMode the mode for file writing
     */
    XmlFileWriter(const pandora::Pandora &pandora, const std::string &fileName, const FileMode fileMode = APPEND);

    /**
     *  @brief  Destructor
     */
    ~XmlFileWriter();

private:
    StatusCode WriteHeader(const ContainerId containerId);
    StatusCode WriteFooter();
    StatusCode WriteSubDetector(const SubDetector *const pSubDetector);
    StatusCode WriteDetectorGap(const DetectorGap *const pDetectorGap);
    StatusCode WriteCaloHit(const CaloHit *const pCaloHit);
    StatusCode WriteTrack(const Track *const pTrack);
    StatusCode WriteMCParticle(const MCParticle *const pMCParticle);
    StatusCode WriteRelationship(const RelationshipId relationshipId, const void *address1, const void *address2, const float weight);

    /**
     *  @brief  Write a variable to the file
     * 
     *  @param  xmlKey the xml key
     */
    template<typename T>
    StatusCode WriteVariable(const std::string &xmlKey, const T &t);

    TiXmlDocument                  *m_pXmlDocument;         ///< The xml document
    TiXmlElement                   *m_pContainerXmlElement; ///< The container xml element
    TiXmlElement                   *m_pCurrentXmlElement;   ///< The current xml element
};

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode XmlFileWriter::WriteVariable(const std::string &xmlKey, const T &t)
{
    TiXmlElement *pTiXmlElement = new TiXmlElement(xmlKey);
    pTiXmlElement->LinkEndChild(new TiXmlText(TypeToString(t)));
    m_pCurrentXmlElement->LinkEndChild(pTiXmlElement);

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode XmlFileWriter::WriteVariable(const std::string &xmlKey, const CartesianVector &t)
{
    return this->WriteVariable(xmlKey, TypeToString(t.GetX()) + " " + TypeToString(t.GetY()) + " " + TypeToString(t.GetZ()));
}

template<>
inline StatusCode XmlFileWriter::WriteVariable(const std::string &xmlKey, const TrackState &t)
{
    return this->WriteVariable(xmlKey, TypeToString(t.GetPosition().GetX()) + " " + TypeToString(t.GetPosition().GetY()) + " " + TypeToString(t.GetPosition().GetZ()) +
        " " + TypeToString(t.GetMomentum().GetX()) + " " + TypeToString(t.GetMomentum().GetY()) + " " + TypeToString(t.GetMomentum().GetZ()));
}

} // namespace pandora

#endif // #ifndef PANDORA_XML_FILE_WRITER_H
