/**
 *  @file   PandoraPFANew/Framework/include/Helpers/EnergyCorrectionsHelper.h
 * 
 *  @brief  Header file for the calo hit helper class.
 * 
 *  $Log: $
 */
#ifndef ENERGY_CORRECTIONS_HELPER_H
#define ENERGY_CORRECTIONS_HELPER_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraInputTypes.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  EnergyCorrectionsHelper class
 */
class EnergyCorrectionsHelper
{
public:
    /**
     *  @brief  Make energy corrections to a cluster
     * 
     *  @param  pCluster address of the cluster
     *  @param  correctedElectromagneticEnergy to receive the corrected electromagnetic energy
     *  @param  correctedHadronicEnergy to receive the corrected hadronic energy
     */
    static StatusCode EnergyCorrection(const Cluster *const pCluster, float &correctedElectromagneticEnergy, float &correctedHadronicEnergy);

private:
    /**
     *  @brief  Read the energy corrections helper settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle *const pXmlHandle);

    static EnergyCorrectionFunctionVector m_hadEnergyCorrectionFunctions;   ///< The hadronic energy correction function vector
    static EnergyCorrectionFunctionVector m_emEnergyCorrectionFunctions;    ///< The electromagnetic energy correction function vector

    static unsigned int     m_nEnergyCorrectionCalls;       ///< Number of calls to energy correction function, used to prevent recursion

    friend class PandoraSettings;
    friend class PluginManager;
};

} // namespace pandora

#endif // #ifndef ENERGY_CORRECTIONS_HELPER_H
