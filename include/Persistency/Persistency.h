/**
 *  @file   PandoraSDK/include/Persistency/Persistency.h
 * 
 *  @brief  Header file for persistency class
 * 
 *  $Log: $
 */
#ifndef PANDORA_PERSISTENCY_H
#define PANDORA_PERSISTENCY_H 1

#include "Api/PandoraApi.h"

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

    ObjectFactory<PandoraApi::CaloHit::Parameters, CaloHit>                        *m_pCaloHitFactory;       ///< Address of the calo hit factory
    ObjectFactory<PandoraApi::Track::Parameters, Track>                            *m_pTrackFactory;         ///< Address of the track factory
    ObjectFactory<PandoraApi::MCParticle::Parameters, MCParticle>                  *m_pMCParticleFactory;    ///< Address of the mc particle factory
    ObjectFactory<PandoraApi::Geometry::SubDetector::Parameters, SubDetector>      *m_pSubDetectorFactory;   ///< Address of the sub detector factory
    ObjectFactory<PandoraApi::Geometry::LineGap::Parameters, LineGap>              *m_pLineGapFactory;       ///< Address of the line gap factory
    ObjectFactory<PandoraApi::Geometry::BoxGap::Parameters, BoxGap>                *m_pBoxGapFactory;        ///< Address of the box gap factory
    ObjectFactory<PandoraApi::Geometry::ConcentricGap::Parameters, ConcentricGap>  *m_pConcentricGapFactory; ///< Address of the concentric gap factory
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
