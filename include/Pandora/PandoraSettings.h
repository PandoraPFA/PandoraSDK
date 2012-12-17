/**
 *  @file   PandoraPFANew/Framework/include/Pandora/PandoraSettings.h
 * 
 *  @brief  Header file for the pandora settings class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_SETTINGS_H
#define PANDORA_SETTINGS_H 1

#include "Pandora/StatusCodes.h"

namespace pandora
{

class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PandoraSettings class
 */
class PandoraSettings
{
public:
    /**
     *  @brief  Whether monitoring is enabled
     * 
     *  @return boolean
     */
    static bool IsMonitoringEnabled();

    /**
     *  @brief  Whether to display algorithm information during processing
     * 
     *  @return boolean
     */
    static bool ShouldDisplayAlgorithmInfo();

    /**
     *  @brief  Get the electromagnetic energy resolution as a fraction, X, such that sigmaE = ( X * E / sqrt(E) )
     * 
     *  @return The electromagnetic energy resolution
     */
    static float GetElectromagneticEnergyResolution();

    /**
     *  @brief  Get the hadronic energy resolution as a fraction, X, such that sigmaE = ( X * E / sqrt(E) )
     * 
     *  @return The hadronic energy resolution
     */
    static float GetHadronicEnergyResolution();

    /**
     *  @brief  Get the radius used to select the pfo target from a mc particle decay chain, units mm
     * 
     *  @return The pfo selection radius
     */
    static float GetMCPfoSelectionRadius();

    /**
     *  @brief  Get the momentum magnitude used to select the pfo target from a mc particle decay chain, units GeV
     * 
     *  @return The pfo selection momentum magnitude
     */
    static float GetMCPfoSelectionMomentum();

    /**
     *  @brief  Get the low energy cut-off for selection of protons/neutrons as mc pfos
     * 
     *  @return The low energy cut-off for selection of protons/neutrons as mc pfos
     */
    static float GetMCPfoSelectionLowEnergyNeutronProtonCutOff();

    /**
     *  @brief  Whether to collapse mc particle decay chains down to just the pfo target
     * 
     *  @return boolean
     */
    static bool ShouldCollapseMCParticlesToPfoTarget();

private:
    /**
     *  @brief  Register a pandora settings function to e.g. read settings for a registered particle id or energy correction function
     * 
     *  @param  xmlTagName the name of the xml tag (within the <pandora></pandora> tags) containing the settings
     *  @param  pSettingsFunction pointer to the pandora settings function
     */
    static StatusCode RegisterSettingsFunction(const std::string &xmlTagName, SettingsFunction *pSettingsFunction);

    /**
     *  @brief  Initialize pandora settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    static StatusCode Initialize(const TiXmlHandle *const pXmlHandle);

    /**
     *  @brief  Run all registered settings functions
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    static StatusCode RunRegisteredSettingsFunctions(const TiXmlHandle *const pXmlHandle);

    /**
     *  @brief  Read global pandora settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    static StatusCode ReadGlobalSettings(const TiXmlHandle *const pXmlHandle);

    typedef std::map<SettingsFunction *, std::string> SettingsFunctionToNameMap;

    static bool     m_isInitialized;                        ///< Whether the pandora settings have been initialized

    static bool     m_isMonitoringEnabled;                  ///< Whether monitoring is enabled
    static bool     m_shouldDisplayAlgorithmInfo;           ///< Whether to display algorithm information during processing
    static bool     m_shouldCollapseMCParticlesToPfoTarget; ///< Whether to collapse mc particle decay chains down to just the pfo target

    static float    m_electromagneticEnergyResolution;      ///< Electromagnetic energy resolution, X, such that sigmaE = ( X * E / sqrt(E) )
    static float    m_hadronicEnergyResolution;             ///< Hadronic energy resolution, X, such that sigmaE = ( X * E / sqrt(E) )
    static float    m_mcPfoSelectionRadius;                 ///< Radius used to select pfo target from a mc decay chain, units mm
    static float    m_mcPfoSelectionMomentum;               ///< Momentum magnitude used to select pfo target from a mc decay chain, units GeV/c
    static float    m_mcPfoSelectionLowEnergyNPCutOff;      ///< Low energy cut-off for selection of protons/neutrons as MCPFOs

    static SettingsFunctionToNameMap m_settingsFunctionToNameMap;   ///< The settings function to xml tag name map

    friend class Pandora;
    friend class PandoraApiImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::IsMonitoringEnabled()
{
    return m_isMonitoringEnabled;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::ShouldDisplayAlgorithmInfo()
{
    return m_shouldDisplayAlgorithmInfo;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::ShouldCollapseMCParticlesToPfoTarget()
{
    return m_shouldCollapseMCParticlesToPfoTarget;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetElectromagneticEnergyResolution()
{
    return m_electromagneticEnergyResolution;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetHadronicEnergyResolution()
{
    return m_hadronicEnergyResolution;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetMCPfoSelectionRadius()
{
    return m_mcPfoSelectionRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetMCPfoSelectionMomentum()
{
    return m_mcPfoSelectionMomentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetMCPfoSelectionLowEnergyNeutronProtonCutOff()
{
    return m_mcPfoSelectionLowEnergyNPCutOff;
}

} // namespace pandora

#endif // #ifndef PANDORA_SETTINGS_H
