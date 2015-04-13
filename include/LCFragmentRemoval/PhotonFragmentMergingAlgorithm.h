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
