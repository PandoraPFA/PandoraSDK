/**
 *  @file   PandoraPFANew/Framework/include/Utilities/ShowerProfileCalculator.h
 * 
 *  @brief  Header file for the shower profile calculator interface class.
 * 
 *  $Log: $
 */
#ifndef SHOWER_PROFILE_CALCULATOR_H
#define SHOWER_PROFILE_CALCULATOR_H 1

#include "Helpers/ParticleIdHelper.h"
#include "Helpers/XmlHelper.h"

namespace pandora
{

/**
 *  @brief  ShowerProfileCalculator class
 */
class ShowerProfileCalculator
{
protected:
    /**
     *  @brief  Destructor
     */
    virtual ~ShowerProfileCalculator();

    /**
     *  @brief  Read shower profile calculator settings from xml. This function will receive a callback if the top level xml tags
     *          <ShowerProfileCalculator></ShowerProfileCalculator> are present.
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    virtual void ReadSettings(const TiXmlHandle *const pXmlHandle);

    /**
     *  @brief  Calculate longitudinal shower profile for a cluster and compare it with the expected profile for a photon
     * 
     *  @param  pCluster address of the cluster to investigate
     *  @param  profileStart to receive the profile start, in radiation lengths
     *  @param  profileDiscrepancy to receive the profile discrepancy
     */
    virtual void CalculateLongitudinalProfile(const Cluster *const pCluster, float &profileStart, float &profileDiscrepancy) const = 0;

    /**
     *  @brief  Calculate transverse shower profile for a cluster and get the list of peaks identified in the profile
     * 
     *  @param  pCluster the address of the cluster
     *  @param  maxPseudoLayer the maximum pseudo layer to consider
     *  @param  showerPeakList to receive the shower peak list
     */
    virtual void CalculateTransverseProfile(const Cluster *const pCluster, const PseudoLayer maxPseudoLayer, ParticleIdHelper::ShowerPeakList &showerPeakList) const = 0;

    friend class ParticleIdHelper;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline ShowerProfileCalculator::~ShowerProfileCalculator()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ShowerProfileCalculator::ReadSettings(const TiXmlHandle *const /*pXmlHandle*/)
{
}

} // namespace pandora

#endif // #ifndef SHOWER_PROFILE_CALCULATOR_H
