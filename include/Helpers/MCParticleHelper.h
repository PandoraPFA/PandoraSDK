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
     *  @brief  Find the mc particle making the largest contribution to a specified calo hit, track or cluster
     * 
     *  @param  pT address of the calo hit, track or cluster to examine
     * 
     *  @return address of the main mc particle
     */
    template <typename T>
    static const MCParticle *GetMainMCParticle(const T *const pT);

    /**
     *  @brief  Find the main mc particle making the largest contribution to a specified calo hit list 
     *
     *  @param caloHitList to recieve the address of the calo hit list under consideration
     *  @param pBestMCParticle address of mc particle making the largest contribution to calo hit list
     */
    static StatusCode FindMainMCParticle(const OrderedCaloHitList &caloHitList, const MCParticle *&pBestMCParticle);
};

} // namespace pandora

#endif // #ifndef PANDORA_MC_PARTICLE_HELPER_H
