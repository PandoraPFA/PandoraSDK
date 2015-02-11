/**
 *  @file   PandoraSDK/include/Plugins/EnergyCorrectionsPlugin.h
 * 
 *  @brief  Header file for the calo hit plugin class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_ENERGY_CORRECTIONS_PLUGIN_H
#define PANDORA_ENERGY_CORRECTIONS_PLUGIN_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraInputTypes.h"
#include "Pandora/Process.h"

namespace pandora
{

class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  EnergyCorrectionPlugin class
 */
class EnergyCorrectionPlugin : public Process
{
public:
    /**
     *  @brief  Make energy corrections to a cluster
     * 
     *  @param  pCluster address of the cluster
     *  @param  correctedEnergy to receive the corrected energy
     */
    virtual StatusCode MakeEnergyCorrections(const Cluster *const pCluster, float &correctedEnergy) const = 0;

protected:
    friend class EnergyCorrections;
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  EnergyCorrections class
 */
class EnergyCorrections
{
public:
    /**
     *  @brief  Make an ordered list of energy corrections to a cluster
     * 
     *  @param  pCluster address of the cluster
     *  @param  correctedElectromagneticEnergy to receive the corrected electromagnetic energy
     *  @param  correctedHadronicEnergy to receive the corrected hadronic energy
     */
    StatusCode MakeEnergyCorrections(const Cluster *const pCluster, float &correctedElectromagneticEnergy, float &correctedHadronicEnergy) const;

private:
    /**
     *  @brief  Default constructor
     * 
     *  @param  pPandora address of the associated pandora instance
     */
    EnergyCorrections(const Pandora *const pPandora);

    /**
     *  @brief  Destructor
     */
    ~EnergyCorrections();

    /**
     *  @brief  Register an energy correction plugin
     * 
     *  @param  pluginName the name/label associated with the energy correction plugin
     *  @param  energyCorrectionType the energy correction type
     *  @param  energyCorrectionPlugin pointer to an energy correction plugin
     */
    StatusCode RegisterPlugin(const std::string &pluginName, const EnergyCorrectionType energyCorrectionType,
        EnergyCorrectionPlugin *const pEnergyCorrectionPlugin);

    /**
     *  @brief  Initialize plugins
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode InitializePlugins(const TiXmlHandle *const pXmlHandle);

    typedef std::vector<EnergyCorrectionPlugin *> EnergyCorrectionPluginVector;

    /**
     *  @brief  Read requested plugin names/labels from a specified xml tag and attempt to assign the plugin pointers as requested
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     *  @param  xmlTagName the xml tag name for a given energy correction type
     *  @param  energyCorrectionType the energy correction type
     *  @param  energyCorrectionPluginVector to receive the addresses of the energy correction plugins
     */
    StatusCode InitializePlugin(const TiXmlHandle *const pXmlHandle, const std::string &xmlTagName,
        const EnergyCorrectionType energyCorrectionType, EnergyCorrectionPluginVector &energyCorrectionPluginVector);

    typedef std::map<std::string, EnergyCorrectionPlugin *> EnergyCorrectionPluginMap;

    /**
     *  @brief  Get the energy correction plugin map corresponding to the specified energy correction type
     * 
     *  @param  energyCorrectionType the energy correction type
     * 
     *  @return reference to the relevant energy correction plugin map
     */
    EnergyCorrectionPluginMap &GetEnergyCorrectionPluginMap(const EnergyCorrectionType energyCorrectionType);

    const Pandora *const            m_pPandora;                         ///< Address of the associated pandora instance
    EnergyCorrectionPluginMap       m_hadEnergyCorrectionPluginMap;     ///< The hadronic energy correction plugin map
    EnergyCorrectionPluginMap       m_emEnergyCorrectionPluginMap;      ///< The electromagnetic energy correction plugin map
    EnergyCorrectionPluginVector    m_hadEnergyCorrectionPlugins;       ///< The final hadronic energy correction plugin vector
    EnergyCorrectionPluginVector    m_emEnergyCorrectionPlugins;        ///< The final electromagnetic energy correction plugin vector

    friend class PandoraApiImpl;
    friend class PluginManager;
};

} // namespace pandora

#endif // #ifndef PANDORA_ENERGY_CORRECTIONS_PLUGIN_H
