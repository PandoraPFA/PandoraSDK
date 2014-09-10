/**
 *  @file   LCContent/include/LCKinkPfoCreationAlgorithm.h
 * 
 *  @brief  Header file for the kink pfo creation algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_KINK_PFO_CREATION_ALGORITHM_H
#define LC_KINK_PFO_CREATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"
#include "Objects/CartesianVector.h"

namespace lc_content
{

/**
 *  @brief  KinkPfoCreationAlgorithm class
 */
class KinkPfoCreationAlgorithm : public pandora::Algorithm
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

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Kink mass calculation
     * 
     *  @param  parentMomentum cartesian vector of parent track at end
     *  @param  daughterMomentum cartesian vector of daughter track at start
     *  @param  daughterMass assumed mass of daughter track
     *  @param  daughterMass assumed mass of neutral
     *  @param  mass the resulting mass of the decay
     */
    pandora::StatusCode GetKinkMass(const pandora::CartesianVector &parentMomentum, const pandora::CartesianVector &daughterMomentum,
        const float daughterMass, const float neutralMass, float &mass) const;

    /**
     *  @brief  V0 mass calculation
     * 
     *  @param  momentum1 is cartesian vector of track1 at start
     *  @param  momentum2 is cartesian vector of track2 at start
     *  @param  mass1 is assumed mass of track1
     *  @param  mass2 is assumed mass of track2
     *  @param  V0mass is the resulting mass of the V0
     */
    pandora::StatusCode GetV0Mass(const pandora::CartesianVector &momentum1, const pandora::CartesianVector &momentum2, float mass1,
        float mass2, float &V0mass) const;

    /**
     *  @brief  Set the PFO parameters to those of the parent
     * 
     *  @param  pPfo address of the particle flow object
     */
    pandora::StatusCode SetPfoParametersFromParentTrack(pandora::ParticleFlowObject *pPfo) const;

    /**
     *  @brief  Merge the clusters from one Pfo into another and delete list
     * 
     *  @param  pfosToMerge the list of Pfos to merge with pfo
     *  @param  pPfo address of particle flow object accepting the clusters
     */
    pandora::StatusCode MergeClustersFromPfoListToPfo(pandora::PfoList &pfosToMerge, pandora::ParticleFlowObject *pPfo) const;

    /**
     *  @brief  Disply MC information for candidate kink
     * 
     *  @param  pParentTrack track assumed to be the kink parent
    */
    void DisplayMcInformation(pandora::Track *pParentTrack) const;

    typedef std::multimap<float, pandora::ParticleFlowObject*> PfosOrderedByDistanceMap; 

    bool    m_mcMonitoring;                                 ///< printout MC information
    bool    m_monitoring;                                   ///< printout reco information

    float   m_minTrackEnergyDifference;                     ///< Min difference in energy between parent and daughter tracks in pfo
    float   m_maxDcaToEndMomentumRatio;                     ///< Max ratio between parent track momentum at dca and end before correction
    float   m_minDcaToEndMomentumRatio;                     ///< Min ratio between parent track momentum at dca and end before correction
    float   m_maxProjectedDistanceToNeutral;                ///< Max distance from neutral to expected position from decay
    float   m_photonDistancePenalty;                        ///< Penalty for photon distances (scale factor)
    float   m_chi2CutForMergingPfos;                        ///< Chi2 cut to merge Pfos

    bool    m_findDecaysWithNeutrinos;                      ///< Find decays with neutrinos, k->mu nu, pi->mu nu             
    bool    m_findDecaysWithPiZeros;                        ///< Find decays with pizeros,   k->pi pi0          
    bool    m_findDecaysWithNeutrons;                       ///< Find decays with neutrons   
    bool    m_findDecaysWithLambdas;                        ///< Find decays with lambdas   
    bool    m_findDecaysMergedWithTrackClusters;            ///< Find decays where the neutral is merged in a charged cluster  

    float   m_upperCutOnPiZeroMass;                         ///< upper cut on pizero mass
    float   m_lowerCutOnPiZeroMass;                         ///< lower cut on pizero mass
    float   m_lowerCutOnPiToMuNuMass;                       ///< lower cut on pi decay mass
    float   m_upperCutOnPiToMuNuMass;                       ///< upper cut on pi decay mass
    float   m_lowerCutOnKToMuNuMass;                        ///< lower cut on K decay mass
    float   m_upperCutOnKToMuNuMass;                        ///< upper cut on K decay mass
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *KinkPfoCreationAlgorithm::Factory::CreateAlgorithm() const
{
    return new KinkPfoCreationAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_KINK_PFO_CREATION_ALGORITHM_H
