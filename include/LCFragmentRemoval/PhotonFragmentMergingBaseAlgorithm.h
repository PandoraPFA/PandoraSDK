/**
 *  @file   LCContent/include/LCFragmentRemoval/PhotonFragmentMergingBaseAlgorithm.h
 * 
 *  @brief  Header file for the photon fragment merging algorithm base class.
 * 
 *  $Log: $
 */
#ifndef LC_PHOTON_FRAGMENT_MERGING_BASE_ALGORITHM_H
#define LC_PHOTON_FRAGMENT_MERGING_BASE_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "Plugins/ShowerProfilePlugin.h"

namespace lc_content
{

/**
 *  @brief  PhotonFragmentMergingBaseAlgorithm class
 */
class PhotonFragmentMergingBaseAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief Default constructor
     */
    PhotonFragmentMergingBaseAlgorithm();

protected:
    /**
     *  @brief Parameters class
     */
    class Parameters
    {
    public:
        /**
        *  @brief  Default constructor
        */
        Parameters();

        float           m_weightedLayerSeparation;              ///< Energy-weighted mean common layer separation
        float           m_energyOfMainCluster;                  ///< Energy of the main cluster
        float           m_energyOfCandidateCluster;             ///< Energy of the candidate cluster
        float           m_energyOfMainPeak;                     ///< Energy of the main peak from transverse shower profile calculator
        float           m_energyOfCandidatePeak;                ///< Energy of the second peak from transverse shower profile calculator
        float           m_hitSeparation;                        ///< Closest distance separation between clusters at the calo hit level
        float           m_centroidSeparation;                   ///< Closest distance separation between centroids of the two clusters
        unsigned int    m_nCaloHitsMain;                        ///< Number of calo hits of the main cluster
        unsigned int    m_nCaloHitsCandidate;                   ///< Number of calo hits of the candidate cluster
        float           m_cosineMain;                           ///< Cosine of main cluster using fitted results
        float           m_cosineCandidate;                      ///< Cosine of candidate cluster using fitted results
        bool            m_hasCrossedGap;                        ///< Whether the two clusters have crossed a gap between subdetectors
    };

    pandora::StatusCode Run();

    /**
     *  @brief  Get list of affected clusters (potentially combining separate lists of photon and non-photon clusters)
     * 
     *  @param  pClusterList the cluster list to receive photon clusters and other clusters
     */
    virtual pandora::StatusCode GetAffectedClusterList(const pandora::ClusterList *&pClusterList) const = 0;

    /**
     *  @brief  Get merging flags for photon - photon pair
     * 
     *  @param  parameters quantities for the cuts
     * 
     *  @return true for should merging
     */
    virtual bool GetPhotonPhotonMergingFlag(const Parameters &parameters) const = 0;

    /**
     *  @brief  Get merging flags for photon - neutral hadron pair
     * 
     *  @param  parameters quantities for the cuts    
     * 
     *  @return true for should merging
     */
    virtual bool GetPhotonNeutralMergingFlag(const Parameters &parameters) const = 0;

    /**
     *  @brief  Delete non fixed photons in the cluster vectors
     * 
     *  @param  photonClusterVec photon cluster vector
     *  @param  neutralClusterVec neutral cluster vector
     *  @param  unusedClusterVec unused cluster vector
     */
    virtual pandora::StatusCode DeleteClusters(const pandora::ClusterVector &photonClusterVec, const pandora::ClusterVector &neutralClusterVec,
        const pandora::ClusterVector &unusedClusterVec) const = 0;

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    int             m_transProfileMaxLayer;                                                 ///< Transverse profile shower calculator max layer 
    float           m_minWeightedLayerSeparation;                                           ///< Minimum distance weighted over layer between candidate and main to consider
    float           m_maxWeightedLayerSeparation;                                           ///< Max distance weighted over layer between candidate and main to consider
    float           m_lowEnergyOfCandidateClusterThreshold;                                 ///< Threshold for the low energy of the candidate cluster 
    float           m_minRatioTotalShowerPeakEnergyToTotalEnergyThreshold;                  ///< Threshold the minimum ratio of sum of energy of 1st peak and 2nd peak divided by the sum of energy of candidate and main cluster
    float           m_weightedLayerSeparationPhotonNeutralThresholdLow1;                    ///< Distance weighted over layer between candidate and main threshold for low energy neutral 1
    float           m_weightedLayerSeparationPhotonNeutralThresholdLow2;                    ///< Distance weighted over layer between candidate and main threshold for low energy neutral 2
    float           m_hitSeparationPhotonNeutralThresholdLow2;                              ///< Closest distance separation between two clusters at calo hits level threshold for low energy neutral 2
    unsigned int    m_nCaloHitsCandidatePhotonNeutralThresholdLow2;                         ///< Number of calo hits of the candidate cluster neutral 2
    float           m_hitSeparationPhotonNeutralThresholdLow3;                              ///< Closest distance separation between two clusters at calo hits level threshold for low energy neutral 3
    float           m_energyRatioCandidateToMainNeutralThresholdLow3;                       ///< Ratio of energy of candidate culster to main cluster for low energy neutral 3
    float           m_weightedLayerSeparationPhotonNeutralThresholdLow3;                    ///< Distance weighted over layer between candidate and main threshold for low energy neutral 3
    float           m_weightedLayerSeparationPhotonNeutralThresholdHigh1;                   ///< Distance weighted over layer between candidate and main threshold for high energy neutral 1
    float           m_energyRatioCandidatePeakToClusterNeutralThresholdHigh1;               ///< Ratio of energy of candidate peak to culster high energy neutral 1
    float           m_energyRatioMainPeakToClusterNeutralThresholdHigh1;                    ///< Ratio of energy of main peak to culster high energy neutral 1
    float           m_triangularEnergyRatioMainPeakToClusterNeutralThresholdHigh1;          ///< Triangular cut ratio of energy of main peak to culster high energy neutral 1
    float           m_triangularSumEnergyRatioMainPeakToClusterNeutralThresholdHigh1;       ///< Triangular cut ratio of energy of main peak to culster high energy neutral 1
    float           m_squareEnergyRatioCandidatePeakToClusterNeutralThresholdHigh1;         ///< Ratio of energy of candidate peak to culster high energy neutral 1
    float           m_squareEnergyRatioMainPeakToClusterNeutralThresholdHigh1;              ///< Ratio of energy of main peak to culster high energy neutral 1
    float           m_energyRatioCandidateToMainNeutralThresholdHigh1;                      ///< Ratio of energy of candidate culster to main cluster for high energy neutral 1
    float           m_hitSeparationPhotonNeutralThresholdHigh2;                             ///< Closest distance separation between two clusters at calo hits level threshold for high energy neutral 2
    float           m_energyRatioCandidateToMainNeutralThresholdHigh2;                      ///< Ratio of energy of candidate culster to main cluster for high energy neutral 2
    float           m_weightedLayerSeparationPhotonNeutralThresholdHigh2;                   ///< Distance weighted over layer between candidate and main threshold for high energy neutral 2
    float           m_weightedLayerSeparationPhotonPhotonThresholdLow1;                     ///< Distance weighted over layer between candidate and main threshold for low energy photon 1
    float           m_energyRatioCandidatePeakToClusterPhotonThresholdLow1;                 ///< Ratio of energy of candidate peak to culster low energy photon 1
    float           m_weightedLayerSeparationPhotonPhotonThresholdLow2;                     ///< Distance weighted over layer between candidate and main threshold for low energy photon 2
    float           m_energyOfCandidateClusterPhotonPhotonThresholdLow2;                    ///< Energy of candidate cluster threshold for low energy photon 2
    float           m_weightedLayerSeparationPhotonPhotonThresholdLow3;                     ///< Distance weighted over layer between candidate and main threshold for low energy photon 3
    unsigned int    m_nCaloHitsCandidatePhotonPhotonThresholdLow3;                          ///< Number of calo hits of candidate cluster for low energy photon 3
    float           m_centroidSeparationPhotonPhotonThresholdLow4;                          ///< Distance separation between two centroids of two clusters threshold for low energy photon 4
    unsigned int    m_nCaloHitsCandidatePhotonPhotonThresholdLow4;                          ///< Number of calo hits of candidate cluster for low energy photon 4
    float           m_weightedLayerSeparationPhotonPhotonThresholdHigh1;                    ///< Distance weighted over layer between candidate and main threshold for high energy photon 1
    float           m_energyRatioCandidatePeakToClusterPhotonThresholdHigh1;                ///< Ratio of energy of candidate peak to culster high energy photon 1
    float           m_energyRatioMainPeakToClusterPhotonThresholdHigh1;                     ///< Ratio of energy of main peak to culster high energy photon 1
    float           m_triangularEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1;      ///< Trangular coefficient ratio of energy of candidate peak to culster high energy photon 1
    float           m_triangularSumEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1;   ///< Trangular sum ratio of energy of candidate peak to culster high energy photon 1
    float           m_linearEnergyRatioCandidatePeakToClusterPhotonThresholdHigh1;          ///< Linear coefficient ratio of energy of candidate peak to culster high energy photon 1
    float           m_linearEnergyRatioMainPeakToClusterPhotonThresholdHigh1;               ///< Linear coefficient ratio of energy of main peak to culster high energy photon 1

private:
    /**
     *  @brief  Get the photon clusters and neutral clusters from the cluster list, clusters with tracks go to the unusedClusterVec
     * 
     *  @param  pClusterList the cluster list for photon clusters and neutral clusters
     *  @param  photonClusterVec to receive the photon cluster vector 
     *  @param  neutralClusterVec to receive the neutral cluster vector
     *  @param  unusedClusterVec to receive the unused cluster vector
     */
    pandora::StatusCode GetAffectedClusterVec(const pandora::ClusterList *const pClusterList, pandora::ClusterVector &photonClusterVec,
        pandora::ClusterVector &neutralClusterVec, pandora::ClusterVector &unusedClusterVec) const;

    /**
     *  @brief  Merge photon fragments with photons, based on the merge flag
     * 
     *  @param  photonClusterVec the photon cluster vector
     */
    pandora::StatusCode MergePhotonFragmentWithPhotons(pandora::ClusterVector &photonClusterVec) const;

    /**
     *  @brief  Merge neutral fragments with photons, based on the merge flag
     * 
     *  @param  photonClusterVec the photon cluster vector
     *  @param  neutralClusterVec the neutral cluster vector
     */
    pandora::StatusCode MergeNeutralFragmentWithPhotons(pandora::ClusterVector &photonClusterVec, pandora::ClusterVector &neutralClusterVec) const;

    /**
     *  @brief  Calculate quantities for cluster merging
     * 
     *  @param  pParentCluster address of the parent cluster
     *  @param  pDaughterCluster address of the daughter cluster
     *  @param  clusterSeparation the cluster separation
     *  @param  parameters to receive the populated merging parameters
     */
    pandora::StatusCode GetEvidenceForMerging(const pandora::Cluster *const pParentCluster, const pandora::Cluster *const pDaughterCluster,
        const float clusterSeparation, Parameters &parameters) const;

    /**
     *  @brief  Get the shower peak list for a provided cobination of parent and daughter clusters
     * 
     *  @param  pParentCluster address of the parent cluster
     *  @param  pDaughterCluster address of the daughter cluster
     *  @param  showerPeakList to receive the populated shower peak list
     */
    pandora::StatusCode GetShowerPeakList(const pandora::Cluster *const pParentCluster, const pandora::Cluster *const pDaughterCluster,
        pandora::ShowerProfilePlugin::ShowerPeakList &showerPeakList) const;
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline PhotonFragmentMergingBaseAlgorithm::Parameters::Parameters() :
    m_weightedLayerSeparation(-std::numeric_limits<float>::max()),
    m_energyOfMainCluster(-std::numeric_limits<float>::max()),
    m_energyOfCandidateCluster(-std::numeric_limits<float>::max()),
    m_energyOfMainPeak(-std::numeric_limits<float>::max()),
    m_energyOfCandidatePeak(-std::numeric_limits<float>::max()),
    m_hitSeparation(-std::numeric_limits<float>::max()),
    m_centroidSeparation(-std::numeric_limits<float>::max()),
    m_nCaloHitsMain(0),
    m_nCaloHitsCandidate(0),
    m_cosineMain(-std::numeric_limits<float>::max()),
    m_cosineCandidate(-std::numeric_limits<float>::max()),
    m_hasCrossedGap(false)
{
}

} // end namespace lc_content

#endif // #ifndef LC_PHOTON_FRAGMENT_MERGING_BASE_ALGORITHM_H
