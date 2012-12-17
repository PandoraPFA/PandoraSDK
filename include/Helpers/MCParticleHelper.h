/**
 *  @file   PandoraPFANew/Framework/include/Helpers/MCParticleHelper.h
 * 
 *  @brief  Header file for the mc particle helper class.
 * 
 *  $Log: $
 */
#ifndef MC_PARTICLE_HELPER_H
#define MC_PARTICLE_HELPER_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  MCParticleHelper class
 */
class MCParticleHelper
{
public:
    /**
     *  @brief  Find the mc particle making the largest electromagnetic energy contribution to a specified cluster
     * 
     *  @param  pCluster address of the cluster to examine
     * 
     *  @return address of the main mc particle
     */
    static const MCParticle *GetMainMCParticle(const pandora::Cluster *const pCluster);

private:
    /**
     *  @brief  Read the cluster helper settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle *const pXmlHandle);

    friend class PandoraSettings;
};

} // namespace pandora

#endif // #ifndef MC_PARTICLE_HELPER_H
