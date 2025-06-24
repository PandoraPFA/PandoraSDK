/**
 *  @file   PandoraSDK/include/Pandora/Pandora.h
 * 
 *  @brief  Header file for the pandora class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_MAIN_H
#define PANDORA_MAIN_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/StatusCodes.h"

#include <string>

namespace pandora
{

class AlgorithmManager;
class CaloHitManager;
class ClusterManager;
class EnergyCorrectionsPlugin;
class GeometryManager;
class MCManager;
class PandoraApiImpl;
class PandoraContentApiImpl;
class PandoraImpl;
class PandoraSettings;
class ParticleFlowObjectManager;
class ParticleIdPlugin;
class PluginManager;
class TrackManager;
class VertexManager;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *    @brief Pandora class
 */
class Pandora
{
public:
    /**
     *  @brief  Default constructor
     *
     *  @param  name descriptive name or label for the pandora instance
     */
    Pandora(const std::string &name = ""); 

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

    /**
     *  @brief  Get the pandora settings instance
     * 
     *  @return the address of the pandora settings instance
     */
    const PandoraSettings *GetSettings() const;

    /**
     *  @brief  Get the pandora geometry instance
     * 
     *  @return the address of the pandora geometry instance
     */
    const GeometryManager *GetGeometry() const;

    /**
     *  @brief  Get the pandora plugin instance, providing access to user registered functions and calculators
     * 
     *  @return the address of the pandora plugin instance
     */
    const PluginManager *GetPlugins() const;

    /**
     *  @brief  Get the descriptive name or label for the pandora instance
     * 
     *  @return the descriptive name or label for the pandora instance
     */
    const std::string &GetName() const;

    /**
     *  @brief  Get run number of the input data
     * 
     *  @return the run number of the input data 
     */
    unsigned int GetRun() const;

    /**
     *  @brief  Get subrun number of the input data
     * 
     *  @return the subrun number of the input data 
     */
    unsigned int GetSubrun() const;

    /**
     *  @brief  Get event number of the input data
     * 
     *  @return the event number of the input data 
     */
    unsigned int GetEvent() const;

private:
    /**
     *  @brief  Prepare event, calculating properties of input objects for later use in algorithms
     */
    StatusCode PrepareEvent();

    /**
     *  @brief  Process event, calling event prepare event function, then running the algorithms
     */
    StatusCode ProcessEvent();

    /**
     *  @brief  Reset event, calling manager reset functions and any registered reset functions
     */
    StatusCode ResetEvent();

    /**
     *  @brief  Get event number of the input data
     * 
     *  @param  the run number of the input data 
     *  @param  the subrun number of the input data 
     *  @param  the event number of the input data 
     */
    StatusCode SetEventInformation(const unsigned int run, const unsigned int subrun, const unsigned int event);

    /**
     *  @brief  Read pandora settings
     * 
     *  @param  xmlFileName the name of the xml file containing the settings
     */
    StatusCode ReadSettings(const std::string &xmlFileName);

    AlgorithmManager            *m_pAlgorithmManager;           ///< The algorithm manager
    CaloHitManager              *m_pCaloHitManager;             ///< The hit manager
    ClusterManager              *m_pClusterManager;             ///< The cluster manager
    GeometryManager             *m_pGeometryManager;            ///< The geometry manager
    MCManager                   *m_pMCManager;                  ///< The MC manager
    ParticleFlowObjectManager   *m_pPfoManager;                 ///< The particle flow object manager
    PluginManager               *m_pPluginManager;              ///< The pandora plugin manager
    TrackManager                *m_pTrackManager;               ///< The track manager
    VertexManager               *m_pVertexManager;              ///< The vertex manager

    PandoraSettings             *m_pPandoraSettings;            ///< The pandora settings instance
    PandoraApiImpl              *m_pPandoraApiImpl;             ///< The pandora api implementation
    PandoraContentApiImpl       *m_pPandoraContentApiImpl;      ///< The pandora content api implementation
    PandoraImpl                 *m_pPandoraImpl;                ///< The pandora implementation

    std::string                  m_name;                        ///< The descriptive name or label for the pandora instance
    InputUInt                    m_run;                         ///< the run number of the input data 
    InputUInt                    m_subrun;                      ///< the subrun number of the input data 
    InputUInt                    m_event;                       ///< the event number of the input data 

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

} // namespace pandora

#endif // #ifndef PANDORA_MAIN_H
