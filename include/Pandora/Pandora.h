/**
 *  @file   PandoraPFANew/Framework/include/Pandora/Pandora.h
 * 
 *  @brief  Header file for the pandora class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_MAIN_H
#define PANDORA_MAIN_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraInputTypes.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

class AlgorithmManager;
class CaloHitManager;
class ClusterManager;
class MCManager;
class PandoraApiImpl;
class PandoraContentApiImpl;
class PandoraImpl;
class ParticleFlowObjectManager;
class PluginManager;
class TrackManager;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *    @brief Pandora class
 */
class Pandora
{
public:
    /**
     *  @brief  Default constructor
     */
    Pandora();

    /**
     *  @brief  Destructor
     */
    ~Pandora();

    /**
     *  @brief  Get the pandora api impl
     * 
     *  @return Address of the pandora api impl
     */
    const PandoraApiImpl *GetPandoraApiImpl() const;

    /**
     *  @brief  Get the pandora content api impl
     * 
     *  @return Address of the pandora content api impl
     */
    const PandoraContentApiImpl *GetPandoraContentApiImpl() const;

private:
    /**
     *  @brief  Prepare event, calculating properties of input objects for later use in algorithms
     */
    StatusCode PrepareEvent();

    /**
     *  @brief  Prepare mc particles: select mc pfo targets, match tracks and calo hits to the correct mc
     *          particles for particle flow
     */
    StatusCode PrepareMCParticles();

    /**
     *  @brief  Process event, calling event prepare event function, then running the algorithms
     */
    StatusCode ProcessEvent();

    /**
     *  @brief  Reset event, calling manager reset functions and any registered reset functions
     */
    StatusCode ResetEvent();

    /**
     *  @brief  Register a reset function, called whenever client application resets pandora to process another event
     * 
     *  @param  pResetFunction pointer to the reset function
     */
    StatusCode RegisterResetFunction(ResetFunction *pResetFunction);

    /**
     *  @brief  Read pandora settings
     * 
     *  @param  xmlFileName the name of the xml file containing the settings
     */
    StatusCode ReadSettings(const std::string &xmlFileName);

    AlgorithmManager            *m_pAlgorithmManager;           ///< The algorithm manager
    CaloHitManager              *m_pCaloHitManager;             ///< The hit manager
    ClusterManager              *m_pClusterManager;             ///< The cluster manager
    MCManager                   *m_pMCManager;                  ///< The MC manager
    ParticleFlowObjectManager   *m_pPfoManager;                 ///< The particle flow object manager
    PluginManager               *m_pPluginManager;              ///< The pandora plugin manager
    TrackManager                *m_pTrackManager;               ///< The track manager

    PandoraApiImpl              *m_pPandoraApiImpl;             ///< The pandora api implementation
    PandoraContentApiImpl       *m_pPandoraContentApiImpl;      ///< The pandora content api implementation
    PandoraImpl                 *m_pPandoraImpl;                ///< The pandora implementation

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

} // namespace pandora

#endif // #ifndef PANDORA_MAIN_H
