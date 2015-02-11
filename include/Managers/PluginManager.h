/**
 *  @file   PandoraSDK/include/Managers/PluginManager.h
 * 
 *  @brief  Header file for the pandora plugin manager class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_PLUGIN_MANAGER_H
#define PANDORA_PLUGIN_MANAGER_H 1

#include "Pandora/StatusCodes.h"

namespace pandora
{

class BFieldPlugin;
class PseudoLayerPlugin;
class ShowerProfilePlugin;

class EnergyCorrections;
class ParticleId;

class Pandora;
class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief PluginManager class
 */
class PluginManager
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the associated pandora object
     */
    PluginManager(const Pandora *const pPandora);

    /**
     *  @brief  Destructor
     */
    ~PluginManager();

    /**
     *  @brief  Get the address of the b field plugin
     * 
     *  @return the address of the b field plugin
     */
    const BFieldPlugin *GetBFieldPlugin() const;

    /**
     *  @brief  Get the address of the pseudo layer plugin
     * 
     *  @return the address of the pseudo layer plugin
     */
    const PseudoLayerPlugin *GetPseudoLayerPlugin() const;

    /**
     *  @brief  Get the shower profile plugin
     * 
     *  @return address of the shower profile plugin
     */
    const ShowerProfilePlugin *GetShowerProfilePlugin() const;

    /**
     *  @brief  Get the pandora energy corrections instance
     * 
     *  @return the address of the pandora energy corrections instance
     */
    const EnergyCorrections *GetEnergyCorrections() const;

    /**
     *  @brief  Get the address of the pandora particle id instance
     * 
     *  @return the address of the pandora particle id instance
     */
    const ParticleId *GetParticleId() const;

private:
    /**
     *  @brief  Set the bfield plugin
     * 
     *  @param  pBFieldPlugin address of the bfield plugin
     */
    StatusCode SetBFieldPlugin(BFieldPlugin *const pBFieldPlugin);

    /**
     *  @brief  Set the pseudo layer plugin
     * 
     *  @param  pPseudoLayerPlugin address of the pseudo layer plugin
     */
    StatusCode SetPseudoLayerPlugin(PseudoLayerPlugin *const pPseudoLayerPlugin);

    /**
     *  @brief  Set the shower profile plugin
     * 
     *  @param  pPseudoLayerPlugin address of the shower profile plugin
     */
    StatusCode SetShowerProfilePlugin(ShowerProfilePlugin *const pShowerProfilePlugin);

    /**
     *  @brief  Initialize plugins
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode InitializePlugins(const TiXmlHandle *const pXmlHandle);

    BFieldPlugin                   *m_pBFieldPlugin;                    ///< Address of the bfield plugin
    PseudoLayerPlugin              *m_pPseudoLayerPlugin;               ///< Address of the pseudolayer plugin
    ShowerProfilePlugin            *m_pShowerProfilePlugin;             ///< The shower profile plugin

    EnergyCorrections              *m_pEnergyCorrections;               ///< The energy corrections
    ParticleId                     *m_pParticleId;                      ///< The particle id

    const Pandora *const            m_pPandora;                         ///< The associated pandora object

    friend class PandoraApiImpl;
    friend class PandoraImpl;
};

} // namespace pandora

#endif // #ifndef PANDORA_PLUGIN_MANAGER_H
