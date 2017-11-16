/**
 *  @file   PandoraSDK/include/Plugins/ParticleIdPlugin.h
 * 
 *  @brief  Header file for the particle id plugin class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_PARTICLE_ID_PLUGIN_H
#define PANDORA_PARTICLE_ID_PLUGIN_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/Process.h"

namespace pandora
{

/**
 *  @brief  ParticleIdPlugin class
 */
class ParticleIdPlugin : public Process
{
public:
    /**
     *  @brief  Whether the cluster matches the specific particle hypothesis
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    virtual bool IsMatch(const Cluster *const pCluster) const = 0;

    /**
     *  @brief  Whether the pfo matches the specific particle hypothesis
     * 
     *  @param  pPfo address of the cluster
     * 
     *  @return boolean
     */
    virtual bool IsMatch(const ParticleFlowObject *const pPfo) const = 0;

protected:
    friend class ParticleId;
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ParticleId class
 */
class ParticleId
{
public:
    /**
     *  @brief  Provide identification of whether a cluster or pfo is an electromagnetic shower
     * 
     *  @param  pT address of the cluster or pfo
     * 
     *  @return boolean
     */
    template <typename T>
    bool IsEmShower(const T *const pT) const;

    /**
     *  @brief  Provide identification of whether a cluster or pfo is a photon
     * 
     *  @param  pT address of the cluster or pfo
     * 
     *  @return boolean
     */
    template <typename T>
    bool IsPhoton(const T *const pT) const;

    /**
     *  @brief  Provide identification of whether a cluster or pfo is an electron
     * 
     *  @param  pT address of the cluster or pfo
     * 
     *  @return boolean
     */
    template <typename T>
    bool IsElectron(const T *const pT) const;

    /**
     *  @brief  Provide identification of whether a cluster or pfo is a muon
     * 
     *  @param  pT address of the cluster or pfo
     * 
     *  @return boolean
     */
    template <typename T>
    bool IsMuon(const T *const pT) const;

private:
    /**
     *  @brief  Default constructor
     * 
     *  @param  pPandora address of the associated pandora instance
     */
    ParticleId(const Pandora *const pPandora);

    /**
     *  @brief  Destructor
     */
    ~ParticleId();

    /**
     *  @brief  Register a particle id plugin
     * 
     *  @param  pluginName the name/label associated with the particle id plugin
     *  @param  particleIdPlugin pointer to a particle id plugin
     */
    StatusCode RegisterPlugin(const std::string &pluginName, ParticleIdPlugin *const pParticleIdPlugin);

    /**
     *  @brief  Initialize plugins
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode InitializePlugins(const TiXmlHandle *const pXmlHandle);

    /**
     *  @brief  Read requested plugin name/label from a specified xml tag and attempt to assign the plugin pointer as requested
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     *  @param  xmlTagName the xml tag name for a given particle id "slot"
     *  @param  pParticleIdPlugin to receive the address of the particle id plugin
     */
    StatusCode InitializePlugin(const TiXmlHandle *const pXmlHandle, const std::string &xmlTagName, ParticleIdPlugin *&pParticleIdPlugin);

    /**
     *  @brief  Call the reset callback in all managed plugins
     */
    StatusCode ResetForNextEvent();

    const Pandora *const        m_pPandora;                   ///< Address of the associated pandora instance
    ParticleIdPlugin           *m_pEmShowerPlugin;            ///< The electromagnetic shower id plugin pointer
    ParticleIdPlugin           *m_pPhotonPlugin;              ///< The photon id plugin pointer
    ParticleIdPlugin           *m_pElectronPlugin;            ///< The electron id plugin pointer
    ParticleIdPlugin           *m_pMuonPlugin;                ///< The muon id plugin pointer

    typedef std::map<std::string, ParticleIdPlugin *> ParticleIdPluginMap;
    ParticleIdPluginMap         m_particleIdPluginMap;        ///< The particle id plugin map

    friend class PandoraApiImpl;
    friend class PluginManager;
};

} // namespace pandora

#endif // #ifndef PANDORA_PARTICLE_ID_HELPER_H
