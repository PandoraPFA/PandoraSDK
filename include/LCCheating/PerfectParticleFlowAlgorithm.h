/**
 *  @file   LCContent/include/LCCheating/PerfectParticleFlowAlgorithm.h
 * 
 *  @brief  Header file for the perfect particle flow algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_PERFECT_PARTICLE_FLOW_ALGORITHM_H
#define LC_PERFECT_PARTICLE_FLOW_ALGORITHM_H 1

#include "Api/PandoraContentApi.h"

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief PerfectParticleFlowAlgorithm class
 */
class PerfectParticleFlowAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

    /**
     *  @brief Default constructor
     */
    PerfectParticleFlowAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    typedef PandoraContentApi::ParticleFlowObject::Parameters PfoParameters;

    /**
     *  @brief  Collection of calo hits
     * 
     *  @param  pPfoTarget address of the pfo target
     *  @param  pfoParameters the pfo parameters
     */
    void CaloHitCollection(const pandora::MCParticle *const pPfoTarget, PfoParameters &pfoParameters) const;

    /**
     *  @brief  Simple collection of calo hits, using only hits for which pfo target is the main mc particle
     * 
     *  @param  pPfoTarget address of the pfo target
     *  @param  pCaloHit address of the calo hit to consider
     *  @param  caloHitList to receive selected calo hits
     */
    void SimpleCaloHitCollection(const pandora::MCParticle *const pPfoTarget, pandora::CaloHit *pCaloHit, pandora::CaloHitList &caloHitList) const;

    /**
     *  @brief  Full collection of calo hits, using map of mc particles to hit weights; fragment calo hits where necessary
     * 
     *  @param  pPfoTarget address of the pfo target
     *  @param  pCaloHit address of the calo hit to consider
     *  @param  caloHitList to receive selected calo hits, which may include a list of fragments of the original input hit
     */
    void FullCaloHitCollection(const pandora::MCParticle *const pPfoTarget, pandora::CaloHit *pCaloHit, pandora::CaloHitList &caloHitList) const;

    /**
     *  @brief  Collection of tracks, using only tracks for which pfo target is the main mc particle
     * 
     *  @param  pPfoTarget address of the pfo target
     *  @param  pfoParameters the pfo parameters
     */
    void TrackCollection(const pandora::MCParticle *const pPfoTarget, PfoParameters &pfoParameters) const;

    /**
     *  @brief  Set the pfo parameters using tracks that have been collected together
     * 
     *  @param  pPfoTarget address of the pfo target
     *  @param  nTracksUsed to receive the number of tracks used for setting pfo properties
     *  @param  pfoParameters the pfo parameters
     */
    void SetPfoParametersFromTracks(const pandora::MCParticle *const pPfoTarget, int &nTracksUsed, PfoParameters &pfoParameters) const;

    /**
     *  @brief  Set the pfo parameters using clusters that have been collected together
     * 
     *  @param  pPfoTarget address of the pfo target
     *  @param  nTracksUsed the number of tracks used for setting pfo properties
     *  @param  pfoParameters the pfo parameters
     */
    void SetPfoParametersFromClusters(const pandora::MCParticle *const pPfoTarget, const int nTracksUsed, PfoParameters &pfoParameters) const;

    /**
     *  @brief  Get the energy-weighted centroid for a specified cluster, calculated over a particular pseudo layer range
     * 
     *  @param  pCluster address of the cluster to consider
     *  @param  pseudoLayer the inner pseudo layer of interest
     *  @param  pseudoLayer the outer pseudo layer of interest
     * 
     *  @return The energy-weighted centroid, returned by value
     */
    const pandora::CartesianVector GetEnergyWeightedCentroid(const pandora::Cluster *const pCluster, const unsigned int innerPseudoLayer,
        const unsigned int outerPseudoLayer) const;

    /**
     *  @brief  Print relevant pfo parameter debug information, if required
     * 
     *  @param  pPfoTarget address of the pfo target
     *  @param  nTracksUsed the number of tracks used for setting pfo properties
     *  @param  pfoParameters the pfo parameters
     */
    void PfoParameterDebugInformation(const pandora::MCParticle *const pPfoTarget, const int nTracksUsed, PfoParameters &pfoParameters) const;

    std::string     m_outputPfoListName;                    ///< The output pfo list name
    std::string     m_outputClusterListName;                ///< The output cluster list name
    bool            m_simpleCaloHitCollection;              ///< Whether to use simple calo hit collection mechanism, or full mechanism
    float           m_minWeightFraction;                    ///< The minimum mc particle calo hit weight for hit fragmentation
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PerfectParticleFlowAlgorithm::Factory::CreateAlgorithm() const
{
    return new PerfectParticleFlowAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_PERFECT_PARTICLE_FLOW_ALGORITHM_H
