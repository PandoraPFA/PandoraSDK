/**
 *  @file   PandoraSDK/include/Helpers/MCParticleHelper.h
 * 
 *  @brief  Header file for the mc particle helper class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_MC_PARTICLE_HELPER_H
#define PANDORA_MC_PARTICLE_HELPER_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

/**
 *  @brief  MCParticleHelper class
 */
class MCParticleHelper
{
public:
    /**
     *  @brief  Find the mc particle making the largest contribution to a specified calo hit or cluster
     * 
     *  @param  pT address of the calo hit or cluster to examine
     * 
     *  @return address of the main mc particle
     */
    template <typename T>
    static const MCParticle *GetMainMCParticle(const T *const pT);
};

} // namespace pandora

#endif // #ifndef PANDORA_MC_PARTICLE_HELPER_H
