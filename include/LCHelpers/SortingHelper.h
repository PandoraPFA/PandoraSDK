/**
 *  @file   LCContent/include/LCHelpers/SortingHelper.h
 * 
 *  @brief  Header file for the sorting helper class.
 * 
 *  $Log: $
 */
#ifndef LC_SORTING_HELPER_H
#define LC_SORTING_HELPER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

namespace lc_content
{

/**
 *  @brief  SortingHelper class
 */
class SortingHelper
{
public:
    /**
     *  @brief  Sort clusters by descending hadronic energy
     * 
     *  @param  pLhs address of first cluster
     *  @param  pRhs address of second cluster
     */
    static bool SortClustersByHadronicEnergy(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs);

    /**
     *  @brief  Sort clusters by descending electromagnetic energy
     * 
     *  @param  pLhs address of first cluster
     *  @param  pRhs address of second cluster
     */
    static bool SortClustersByElectromagneticEnergy(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs);

    /**
     *  @brief  Sort clusters by ascending inner layer, and by hadronic energy within a layer
     * 
     *  @param  pLhs address of first cluster
     *  @param  pRhs address of second cluster
     */
    static bool SortClustersByInnerLayer(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs);

    /**
     *  @brief  Sort MCParticles by descending energy
     * 
     *  @param  pLhs address of first MCParticle
     *  @param  pRhs address of second MCParticle
     */
    static bool SortMCParticlesByEnergy(const pandora::MCParticle *const pLhs, const pandora::MCParticle *const pRhs);

    /**
     *  @brief  Sort pfos by descending energy 
     * 
     *  @param  pLhs address of first pfo
     *  @param  pRhs address of second pfo
     */
    static bool SortPfosByEnergy(const pandora::ParticleFlowObject *const pLhs, const pandora::ParticleFlowObject *const pRhs);

    /**
     *  @brief  Sort tracks by descending momentum
     * 
     *  @param  pLhs address of first track
     *  @param  pRhs address of second track
     */
    static bool SortTracksByMomentum(const pandora::Track *const pLhs, const pandora::Track *const pRhs);

    /**
     *  @brief  Sort tracks by descending energy at dca
     * 
     *  @param  pLhs address of first track
     *  @param  pRhs address of second track
     */
    static bool SortTracksByEnergy(const pandora::Track *const pLhs, const pandora::Track *const pRhs);
};

} // namespace lc_content

#endif // #ifndef LC_SORTING_HELPER_H
