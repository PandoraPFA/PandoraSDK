/**
 *  @file   PandoraSDK/include/Pandora/PandoraSettings.h
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

class Pandora;
class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PandoraSettings class
 */
class PandoraSettings
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the associated pandora object
     */
    PandoraSettings(const Pandora *const pPandora);

    /**
     *  @brief  Destructor
     */
    ~PandoraSettings();

    /**
     *  @brief  Whether monitoring is enabled
     * 
     *  @return boolean
     */
    bool IsMonitoringEnabled() const;

    /**
     *  @brief  Whether to display algorithm information during processing
     * 
     *  @return boolean
     */
    bool ShouldDisplayAlgorithmInfo() const;

    /**
     *  @brief  Whether to allow only single hit types in individual clusters
     * 
     *  @return boolean
     */
    bool SingleHitTypeClusteringMode() const;

    /**
     *  @brief  Whether to collapse mc particle decay chains down to just the pfo target
     * 
     *  @return boolean
     */
    bool ShouldCollapseMCParticlesToPfoTarget() const;

    /**
     *  @brief  Whether to allow only single mc particle association to objects (largest weight)
     * 
     *  @return boolean
     */
    bool UseSingleMCParticleAssociation() const;

    /**
     *  @brief  Get the electromagnetic energy resolution as a fraction, X, such that sigmaE = ( X * E / sqrt(E) )
     * 
     *  @return The electromagnetic energy resolution
     */
    float GetElectromagneticEnergyResolution() const;

    /**
     *  @brief  Get the hadronic energy resolution as a fraction, X, such that sigmaE = ( X * E / sqrt(E) )
     * 
     *  @return The hadronic energy resolution
     */
    float GetHadronicEnergyResolution() const;

    /**
     *  @brief  Get the radius used to select the pfo target from a mc particle decay chain, units mm
     * 
     *  @return The pfo selection radius
     */
    float GetMCPfoSelectionRadius() const;

    /**
     *  @brief  Get the momentum magnitude used to select the pfo target from a mc particle decay chain, units GeV
     * 
     *  @return The pfo selection momentum magnitude
     */
    float GetMCPfoSelectionMomentum() const;

    /**
     *  @brief  Get the low energy cut-off for selection of protons/neutrons as mc pfos
     * 
     *  @return The low energy cut-off for selection of protons/neutrons as mc pfos
     */
    float GetMCPfoSelectionLowEnergyNeutronProtonCutOff() const;

    /**
     *  @brief  Get the tolerance allowed when declaring a point to be "in" a gap region, units mm
     * 
     *  @return the tolerance allowed when declaring a point to be "in" a gap region, units mm
     */
    float GetGapTolerance() const;

private:
    /**
     *  @brief  Initialize pandora settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode Initialize(const TiXmlHandle *const pXmlHandle);

    /**
     *  @brief  Read global pandora settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode ReadGlobalSettings(const TiXmlHandle *const pXmlHandle);

    bool     m_isMonitoringEnabled;                         ///< Whether monitoring is enabled
    bool     m_shouldDisplayAlgorithmInfo;                  ///< Whether to display algorithm information during processing
    bool     m_singleHitTypeClusteringMode;                 ///< Whether to allow only single hit types in individual clusters
    bool     m_shouldCollapseMCParticlesToPfoTarget;        ///< Whether to collapse mc particle decay chains down to just the pfo target
    bool     m_useSingleMCParticleAssociation;              ///< Whether to allow only single mc particle association to objects (largest weight)

    float    m_electromagneticEnergyResolution;             ///< Electromagnetic energy resolution, X, such that sigmaE = ( X * E / sqrt(E) )
    float    m_hadronicEnergyResolution;                    ///< Hadronic energy resolution, X, such that sigmaE = ( X * E / sqrt(E) )
    float    m_mcPfoSelectionRadius;                        ///< Radius used to select pfo target from a mc decay chain, units mm
    float    m_mcPfoSelectionMomentum;                      ///< Momentum magnitude used to select pfo target from a mc decay chain, units GeV/c
    float    m_mcPfoSelectionLowEnergyNPCutOff;             ///< Low energy cut-off for selection of protons/neutrons as MCPFOs

    float    m_gapTolerance;                                ///< Tolerance allowed when declaring a point to be "in" a gap region, units mm

    const Pandora *const m_pPandora;                        ///< The associated pandora object

    friend class PandoraApiImpl;
    friend class PandoraImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::IsMonitoringEnabled() const
{
    return m_isMonitoringEnabled;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::ShouldDisplayAlgorithmInfo() const
{
    return m_shouldDisplayAlgorithmInfo;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::SingleHitTypeClusteringMode() const
{
    return m_singleHitTypeClusteringMode;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::ShouldCollapseMCParticlesToPfoTarget() const
{
    return m_shouldCollapseMCParticlesToPfoTarget;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::UseSingleMCParticleAssociation() const
{
    return m_useSingleMCParticleAssociation;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetElectromagneticEnergyResolution() const
{
    return m_electromagneticEnergyResolution;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetHadronicEnergyResolution() const
{
    return m_hadronicEnergyResolution;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetMCPfoSelectionRadius() const
{
    return m_mcPfoSelectionRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetMCPfoSelectionMomentum() const
{
    return m_mcPfoSelectionMomentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetMCPfoSelectionLowEnergyNeutronProtonCutOff() const
{
    return m_mcPfoSelectionLowEnergyNPCutOff;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetGapTolerance() const
{
    return m_gapTolerance;
}

} // namespace pandora

#endif // #ifndef PANDORA_SETTINGS_H
