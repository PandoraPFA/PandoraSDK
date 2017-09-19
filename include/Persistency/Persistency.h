/**
 *  @file   PandoraSDK/include/Persistency/Persistency.h
 * 
 *  @brief  Header file for persistency class
 * 
 *  $Log: $
 */
#ifndef PANDORA_PERSISTENCY_H
#define PANDORA_PERSISTENCY_H 1

#include "Pandora/ObjectCreation.h"
#include "Pandora/ObjectFactory.h"
#include "Pandora/PandoraObjectFactories.h"

#include "Persistency/PandoraIO.h"

#include <string>

namespace pandora
{

/**
 *  @brief  Persistency class
 */
class Persistency
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pandora the pandora instance to be used alongside the file writer
     *  @param  fileName the name of the output file
     *  @param  fileMode the mode for file writing
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
     *  @brief  Set the factory to use for all instantiations and parameter persistence
     * 
     *  @param  pFactory address of the factory
     */
    template <typename PARAMETERS, typename OBJECT>
    StatusCode SetFactory(ObjectFactory<PARAMETERS, OBJECT> *const pFactory);

protected:
    /**
     *  @brief  Replace the current factory with the provided instance
     * 
     *  @param  pFactory address of the factory
     */
    template <typename PARAMETERS, typename OBJECT>
    void ReplaceCurrentFactory(ObjectFactory<PARAMETERS, OBJECT> *const pFactory);

    const Pandora *const        m_pPandora;             ///< Address of pandora instance to be used alongside the file writer
    std::string                 m_fileName;             ///< The file name
    FileType                    m_fileType;             ///< The file type
    ContainerId                 m_containerId;          ///< The type of container currently being written to file

    ObjectFactory<object_creation::CaloHit::Parameters, object_creation::CaloHit::Object>                                  *m_pCaloHitFactory;       ///< Address of the calo hit factory
    ObjectFactory<object_creation::Track::Parameters, object_creation::Track::Object>                                      *m_pTrackFactory;         ///< Address of the track factory
    ObjectFactory<object_creation::MCParticle::Parameters, object_creation::MCParticle::Object>                            *m_pMCParticleFactory;    ///< Address of the mc particle factory
    ObjectFactory<object_creation::Geometry::SubDetector::Parameters, object_creation::Geometry::SubDetector::Object>      *m_pSubDetectorFactory;   ///< Address of the sub detector factory
    ObjectFactory<object_creation::Geometry::LArTPC::Parameters, object_creation::Geometry::LArTPC::Object>                *m_pLArTPCFactory;        ///< Address of the lar tpc factory
    ObjectFactory<object_creation::Geometry::LineGap::Parameters, object_creation::Geometry::LineGap::Object>              *m_pLineGapFactory;       ///< Address of the line gap factory
    ObjectFactory<object_creation::Geometry::BoxGap::Parameters, object_creation::Geometry::BoxGap::Object>                *m_pBoxGapFactory;        ///< Address of the box gap factory
    ObjectFactory<object_creation::Geometry::ConcentricGap::Parameters, object_creation::Geometry::ConcentricGap::Object>  *m_pConcentricGapFactory; ///< Address of the concentric gap factory
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

} // namespace pandora

#endif // #ifndef PANDORA_PERSISTENCY_H
