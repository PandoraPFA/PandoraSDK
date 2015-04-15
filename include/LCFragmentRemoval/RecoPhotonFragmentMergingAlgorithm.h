/**
 *  @file   LCContent/include/LCFragmentRemoval/RecoPhotonFragmentMergingAlgorithm.h
 * 
 *  @brief  Header file for the photon fragment removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_RECO_PHOTON_FRAGMENT_MERGING_ALGORITHM_H
#define LC_RECO_PHOTON_FRAGMENT_MERGING_ALGORITHM_H 1

#include "LCFragmentRemoval/PhotonFragmentMergingBaseAlgorithm.h"

namespace lc_content
{
/**
 *  @brief  RecoPhotonFragmentMergingAlgorithm class
 */
class RecoPhotonFragmentMergingAlgorithm : public PhotonFragmentMergingBaseAlgorithm
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
    RecoPhotonFragmentMergingAlgorithm();

private:
    pandora::StatusCode GetAffectedClusterList(const pandora::ClusterList *&pClusterList) const;
    bool GetPhotonPhotonMergingFlag(const Parameters &parameters) const;
    bool GetPhotonNeutralMergingFlag(const Parameters &parameters) const;
    pandora::StatusCode DeleteClusters(const pandora::ClusterVector &photonClusterVec, const pandora::ClusterVector &neutralClusterVec,
        const pandora::ClusterVector &unusedClusterVec) const;

    /**
     *  @brief  Delete clusters in the provided cluster vector
     * 
     *  @param  clusterVec the cluster vector
     *  @param  nonFixedPhotonsOnly whether to delete all clusters or only those not flagged as fixed photons
     */
    pandora::StatusCode DeleteClusters(const pandora::ClusterVector &clusterVec, const bool nonFixedPhotonsOnly) const;

    /**
     *  @brief  A photon is a photon fragment if it looks like part of the main cluster from the 2D shower profile
     *          and is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsPhotonFragmentInShowerProfile(const Parameters &parameters) const;

    /**
     *  @brief  A photon is a photon fragment if it has absolute low energy and is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsAbsoluteLowEnergyPhotonFragment(const Parameters &parameters) const;

    /**
     *  @brief  A photon is a photon fragment if it has few calo hits and is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsSmallPhotonFragment1(const Parameters &parameters) const;

    /**
     *  @brief  A photon is a photon fragment if it has few calo hits and is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsSmallPhotonFragment2(const Parameters &parameters) const;

    /**
     *  @brief  A high energy photon is a photon fragment if it looks like part of the main cluster from the 2D shower
     *          profile (a triangular cut on ratio of energy of first peak to main cluster and ratio of energy of second
     *          peak to candidate cluster) and is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsHighEnergyPhotonFragmentInShowerProfile(const Parameters &parameters) const;

    /**
     *  @brief  A neutral hadron is a neutral hadron fragment if it looks like part of the main cluster from the 2D shower
     *          profile and is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsNeutralFragmentInShowerProfile(const Parameters &parameters) const;

    /**
     *  @brief  A neutral hadron is a neutral hadron fragment if it has few calo hits and is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsSmallNeutralFragment(const Parameters &parameters) const;

    /**
     *  @brief  A neutral hadron is a neutral hadron fragment if it has relative low energy comparing to the main cluster
     *          and is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsRelativeLowEnergyNeutralFragment(const Parameters &parameters) const;

    /**
     *  @brief  A high energy neutral hadron is a neutral hadron fragment if it looks like part of the main cluster from
     *          the 2D shower profile and is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsHighEnergyNeutralFragmentInShowerProfile(const Parameters &parameters) const;

    /**
     *  @brief  A high energy neutral hadron is a fragment if it has relative low energy comparing to the main cluster and
     *          is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsHighEnergyRelativeLowEnergyNeutralFragment(const Parameters &parameters) const;

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    pandora::StringVector   m_associationAlgorithms;                                    ///< Ordered list of topological association algorithms to be used
    std::string             m_nonPhotonClusterListName;                                 ///< Name for the cluster list processed by topological association algs

    float                   m_energyRatioCandidatePeakToClusterNeutralThresholdLow1;    ///< Ratio of energy of candidate peak to cluster low energy neutral 1
    float                   m_hitSeparationPhotonPhotonThresholdLow3;                   ///< Closest distance separation threshold for low energy photon 3
    float                   m_hitSeparationPhotonPhotonThresholdLow4;                   ///< Closest distance separation threshold for low energy photon 4
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *RecoPhotonFragmentMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new RecoPhotonFragmentMergingAlgorithm();
}

} // end namespace lc_content

#endif // #ifndef LC_RECO_PHOTON_FRAGMENT_MERGING_ALGORITHM_H
