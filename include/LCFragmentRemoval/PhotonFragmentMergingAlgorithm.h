/**
 *  @file   LCContent/include/LCFragmentRemoval/PhotonFragmentMergingAlgorithm.h
 *
 *  @brief  Header file for the photon fragment merging algorithm class.
 *
 *  $Log: $
 */
#ifndef LC_PHOTON_FRAGMENT_MERGING_ALGORITHM_H
#define LC_PHOTON_FRAGMENT_MERGING_ALGORITHM_H 1

#include "LCFragmentRemoval/PhotonFragmentMergingBaseAlgorithm.h"

namespace lc_content
{
/**
 *  @brief  PhotonFragmentMergingAlgorithm class
 */
class PhotonFragmentMergingAlgorithm : public PhotonFragmentMergingBaseAlgorithm
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
    PhotonFragmentMergingAlgorithm();

private:
    pandora::StatusCode GetAffectedClusterList(const pandora::ClusterList *&pClusterList) const;
    bool GetPhotonPhotonMergingFlag(const Parameters & parameters) const;
    bool GetPhotonNeutralMergingFlag(const Parameters &parameters) const;
    pandora::StatusCode DeleteClusters(const pandora::ClusterVector &photonClusterVec, const pandora::ClusterVector &neutralClusterVec,
        const pandora::ClusterVector &unusedClusterVec) const;

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
     *  @brief  A photon is a photon fragment if it has few calo hits, low energy, in the forward region and close
     *          to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsSmallForwardPhotonFragment(const Parameters &parameters) const;

    /**
     *  @brief  A photon is a photon fragment if it has relative low energy comparing to the main cluster and is close
     *          to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsRelativeLowEnergyPhotonFragment(const Parameters &parameters) const;

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
     *  @brief  A high energy photon is a photon fragment if it has relative low energy comparing to the main cluster and
     *          is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsHighEnergyRelativeLowEnergyPhotonFragment(const Parameters &parameters) const;

    /**
     *  @brief  A neutral hadron is a neutral hadron fragment if it is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsCloseByNeutralFragment(const Parameters &parameters) const;

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
     *          the 2D shower profile (a triangular cut on ratio of energy of first peak to main cluster and ratio of energy
     *          of second peak to candidate cluster) and is close to the main cluster
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
    bool IsHighEnergyRelativeLowEnergyNeutralFragment1(const Parameters &parameters) const;

    /**
     *  @brief  A high energy neutral hadron is a fragment if it has relative low energy comparing to the main cluster and
     *          is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsHighEnergyRelativeLowEnergyNeutralFragment2(const Parameters &parameters) const;

    /**
     *  @brief  A high energy neutral hadron is a fragment if it has relative low energy comparing to the main cluster and
     *          is close to the main cluster
     * 
     *  @param  parameters the cluster comparison parameters
     * 
     *  @return boolean
     */
    bool IsHighEnergyRelativeLowEnergyNeutralFragment3(const Parameters &parameters) const;

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    float           m_centroidSeparationPhotonNeutralThresholdLow1;         ///< Distance separation between two centroids of two clusters threshold for low energy neutral 1
    float           m_hitSeparationPhotonNeutralThresholdHigh3;             ///< Closest distance separation between two clusters at calo hits level threshold for high energy neutral 3
    float           m_energyRatioCandidateToMainNeutralThresholdHigh3;      ///< Ratio of energy of candidate culster to main cluster for high energy neutral 3
    float           m_weightedLayerSeparationPhotonNeutralThresholdHigh3;   ///< Distance weighted over layer between candidate and main threshold for high energy neutral 3
    float           m_energyOfCandidateClusterPhotonNeutralThresholdHigh3;  ///< Energy of candidate cluster threshold for high energy neutral 3
    float           m_hitSeparationPhotonNeutralThresholdHigh4;             ///< Closest distance separation between two clusters at calo hits level threshold for high energy neutral 4
    float           m_energyRatioCandidateToMainNeutralThresholdHigh4;      ///< Ratio of energy of candidate culster to main cluster for high energy neutral 4
    float           m_weightedLayerSeparationPhotonNeutralThresholdHigh4;   ///< Distance weighted over layer between candidate and main threshold for high energy neutral 4
    float           m_energyOfCandidateClusterPhotonNeutralThresholdHigh4;  ///< Energy of candidate cluster threshold for high energy neutral 4
    float           m_centroidSeparationPhotonPhotonThresholdLow3;          ///< Distance separation between two centroids of two clusters threshold for low energy photon 3
    float           m_energyOfCandidateClusterPhotonPhotonThresholdLow4;    ///< Energy of candidate cluster threshold for low energy photon 4
    float           m_cosineBarrelLimit;                                    ///< Cosine of the barrel limit
    unsigned int    m_nCaloHitsCandidatePhotonPhotonThresholdLow5;          ///< Number of calo hits for candidate threshold for low energy photon 5
    float           m_weightedLayerSeparationPhotonPhotonThresholdLow5;     ///< Distance weighted over layer between candidate and main threshold for low energy photon 5
    float           m_hitSeparationPhotonPhotonThresholdLow6;               ///< Closest distance separation between two clusters at calo hits level threshold for low energy photon 6
    float           m_energyRatioCandidateToMainPhotonThresholdLow6;        ///< Ratio of energy of candidate culster to main cluster for low energy photon 6
    float           m_weightedLayerSeparationPhotonPhotonThresholdLow6;     ///< Distance weighted over layer between candidate and main threshold for low energy photon 6
    float           m_hitSeparationPhotonPhotonThresholdHigh2;              ///< Closest distance separation between two clusters at calo hits level threshold for high energy photon 2
    float           m_energyRatioCandidateToMainPhotonThresholdHigh2;       ///< Ratio of energy of candidate culster to main cluster for high energy photon 2
    float           m_weightedLayerSeparationPhotonPhotonThresholdHigh2;    ///< Distance weighted over layer between candidate and main threshold for high energy photon 2
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonFragmentMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonFragmentMergingAlgorithm();
}

} // end namespace lc_content

#endif // #ifndef LC_PHOTON_FRAGMENT_MERGING_ALGORITHM_H
