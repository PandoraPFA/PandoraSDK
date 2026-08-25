/**
 *  @file   PandoraSDK/include/Persistency/Persistency.h
 *
 *  @brief  Header file for persistency class.
 *
 *  $Log: $
 */
#ifndef PANDORA_PERSISTENCY_H
#define PANDORA_PERSISTENCY_H 1

#include "Pandora/ObjectCreation.h"
#include "Pandora/ObjectFactory.h"
#include "Pandora/PandoraObjectFactories.h"
#include "Persistency/PandoraIO.h"
#include "Pandora/StatusCodes.h"

#include "Persistency/FieldMap.h"

#include <functional>
#include <string>

namespace pandora
{

/**
 *  @brief  Persistency class
 *
 *  Base class for FileReader and FileWriter. Owns the object factories and the shared file-identity state (name, type, active container).
 *  Also carries the SchemaRegistry and FileMetadata loaded from (or to be written to) the global header, making them available to both the
 *  reader and writer hierarchies.
 */
class Persistency
{
public:
    /**
     *  @brief  Constructor
     *
     *  @param  pandora   the pandora instance to be used alongside the file reader/writer
     *  @param  fileName  the name of the file
     */
    Persistency(const pandora::Pandora &pandora, const std::string &fileName);

    /**
     *  @brief  Destructor
     */
    virtual ~Persistency();

    /**
     *  @brief  Get the file name
     *
     *  @return the file name
     */
    const std::string &GetFileName() const;

    /**
     *  @brief  Get the file type
     *
     *  @return the file type
     */
    FileType GetFileType() const;

    /**
     *  @brief  Get the file metadata (populated after ReadGlobalHeader / set before WriteGlobalHeader).
     *          Returns a non-const reference so callers can populate it before writing.
     */
    FileMetadata &GetFileMetadata();

    /**
     *  @brief  Get the schema registry (populated after ReadGlobalHeader / set before WriteGlobalHeader)
     */
    const SchemaRegistry &GetSchemaRegistry() const;

    /**
     *  @brief  Return the current schema version for a given component type.
     *
     *  Single source of truth shared by every reader and writer (binary, XML, and any future format), so schema versions cannot drift
     *  between formats. Increment a value here (and register a corresponding reader migration in BinaryFileReader/XmlFileReader) when a
     *  field is removed or its semantics change. Adding a new optional field does not need a bump.
     */
    static unsigned int GetSchemaVersion(const ComponentId componentId);

    /**
     *  @brief  Set the factory to use for all instantiations and parameter persistence
     *
     *  @param  pFactory  address of the factory (ownership transferred)
     */
    template <typename PARAMETERS, typename OBJECT>
    StatusCode SetFactory(ObjectFactory<PARAMETERS, OBJECT> *const pFactory);

protected:
    /**
     *  @brief  Replace the current factory with the provided instance (takes ownership)
     */
    template <typename PARAMETERS, typename OBJECT>
    void ReplaceCurrentFactory(ObjectFactory<PARAMETERS, OBJECT> *const pFactory);

    const Pandora *const  m_pPandora;       ///< Address of pandora instance
    std::string           m_fileName;       ///< The file name
    FileType              m_fileType;       ///< The file type (BINARY / XML)
    ContainerId           m_containerId;    ///< The type of container currently being read/written
    FileMetadata          m_metadata;       ///< File-level metadata
    SchemaRegistry        m_schemaRegistry; ///< Per-component schema versions recorded in the global header

    ObjectFactory<object_creation::CaloHit::Parameters, object_creation::CaloHit::Object> *m_pCaloHitFactory; ///< Address of the calo hit factory
    ObjectFactory<object_creation::Track::Parameters, object_creation::Track::Object> *m_pTrackFactory; ///< Address of the track factory
    ObjectFactory<object_creation::MCParticle::Parameters, object_creation::MCParticle::Object> *m_pMCParticleFactory; ///< Address of the MC particle factory
    ObjectFactory<object_creation::Geometry::SubDetector::Parameters, object_creation::Geometry::SubDetector::Object> *m_pSubDetectorFactory; ///< Address of the subdetector factory
    ObjectFactory<object_creation::Geometry::LArTPC::Parameters, object_creation::Geometry::LArTPC::Object> *m_pLArTPCFactory; ///< Address of the LArTPC factory
    ObjectFactory<object_creation::Geometry::LineGap::Parameters, object_creation::Geometry::LineGap::Object> *m_pLineGapFactory; ///< Address of the line gap factory
    ObjectFactory<object_creation::Geometry::BoxGap::Parameters, object_creation::Geometry::BoxGap::Object> *m_pBoxGapFactory; ///< Address of the box gap factory
    ObjectFactory<object_creation::Geometry::ConcentricGap::Parameters, object_creation::Geometry::ConcentricGap::Object>*m_pConcentricGapFactory; ///< Address of the concentric gap factory
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const std::string &Persistency::GetFileName() const
{
    return m_fileName;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline FileType Persistency::GetFileType() const
{
    return m_fileType;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline FileMetadata &Persistency::GetFileMetadata()
{
    return m_metadata;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const SchemaRegistry &Persistency::GetSchemaRegistry() const
{
    return m_schemaRegistry;
}

} // namespace pandora

#endif // #ifndef PANDORA_PERSISTENCY_H
