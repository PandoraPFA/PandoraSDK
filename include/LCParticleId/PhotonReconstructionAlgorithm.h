/**
 *  @file   LCContent/include/LCParticleId/PhotonReconstructionAlgorithm.h
 * 
 *  @brief  Header file for the photon reconstruction algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_PHOTON_RECONSTRUCTION_ALGORITHM_H
#define LC_PHOTON_RECONSTRUCTION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  PhotonReconstructionAlgorithm class
 */
class PhotonReconstructionAlgorithm : public pandora::Algorithm
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
    PhotonReconstructionAlgorithm();

    /**
     *  @param  Destructor
     */
    ~PhotonReconstructionAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Get the minimum distance between a cluster and a track in a specified track list
     * 
     *  @param  pPeakCluster address of the cluster
     *  @param  trackVector the specified track list
     * 
     *  @return the minimum track-cluster distance
     */
    float GetMinDistanceToTrack(const pandora::Cluster *const pPeakCluster, const pandora::TrackVector &trackVector) const;

    /**
     *  @brief  Get the particle identification (Pid) value for a given set of parameters. Values near unity are deemed photon-like,
     *          values near zero are identified as background.
     * 
     *  @param  pPeakCluster the address of the cluster with the specified properties
     *  @param  peakRms the rms of the shower peak
     *  @param  longProfileStart the longitudinal shower profile start
     *  @param  longProfileDiscrepancy the longitudinal shower profile discrepancy
     *  @param  peakEnergyFraction the ratio of shower peak energy to original cluster energy
     *  @param  minDistanceToTrack the minimum distance to a track
     * 
     *  @return the pid value
     */
    float GetPid(const pandora::Cluster *const pPeakCluster, const float peakRms, const float longProfileStart,
        const float longProfileDiscrepancy, const float peakEnergyFraction, const float minDistanceToTrack) const;

    /**
     *  @brief  Fill the probability density functions used in the calculation of Pid values.
     * 
     *  @param  pPeakCluster the address of the cluster with the specified properties
     *  @param  peakRms the rms of the shower peak
     *  @param  longProfileStart the longitudinal shower profile start
     *  @param  longProfileDiscrepancy the longitudinal shower profile discrepancy
     *  @param  peakEnergyFraction the ratio of shower peak energy to original cluster energy
     *  @param  minDistanceToTrack the minimum distance to a track
     */
    void FillPdfHistograms(const pandora::Cluster *const pPeakCluster, const float peakRms, const float longProfileStart,
        const float longProfileDiscrepancy, const float peakEnergyFraction, const float minDistanceToTrack) const;

    /**
     *  @brief  Normalizing member variable histograms and write them to xml
     */
    void NormalizeAndWriteHistograms();

    /**
     *  @brief  Draw member variable histograms if pandora monitoring functionality is enabled
     */
    void DrawHistograms() const;

    /**
     *  @brief  Delete member variable histograms
     */
    void DeleteHistograms();

    /**
     *  @brief  Get the relevant energy bin number for a specified energy value
     * 
     *  @param  energy the specified energy value
     */
    unsigned int GetEnergyBin(const float energy) const;

    /**
     *  @brief  Get the relevant histogram bin content for a specified parameter value, avoiding overflow bins
     * 
     *  @param  pHistogram address of the histogram
     *  @param  value the parameter value to look-up in the histogram
     * 
     *  @return the relevant histogram bin content
     */
    float GetHistogramContent(const pandora::Histogram *const pHistogram, const float value) const;

    /**
     *  @brief  Scale contents of histogram so that its cumulative sum is unity, avoiding overflow bins
     * 
     *  @param  pHistogram address of the histogram
     */
    void NormalizeHistogram(pandora::Histogram *const pHistogram) const;

    std::string             m_photonClusteringAlgName;      ///< The name of the photon clustering algorithm to run
    std::string             m_clusterListName;              ///< The name of the output cluster list 
    bool                    m_replaceCurrentClusterList;    ///< Whether to subsequently use the new cluster list as the "current" list

    std::string             m_histogramFile;                ///< The name of the file containing (or to contain) pdf histograms
    bool                    m_shouldMakePdfHistograms;      ///< Whether to create pdf histograms, rather than perform photon reconstruction
    bool                    m_shouldDrawPdfHistograms;      ///< Whether to draw pdf histograms at end of reconstruction (requires monitoring)

    unsigned int            m_nEnergyBins;                  ///< Number of pdf energy bins
    pandora::FloatVector    m_energyBinLowerEdges;          ///< List of lower edges of the pdf energy bins

    pandora::Histogram    **m_pSigPeakRms;                  ///< PDF histogram, signal peak rms
    pandora::Histogram    **m_pBkgPeakRms;                  ///< PDF histogram, background peak rms
    pandora::Histogram    **m_pSigLongProfileStart;         ///< PDF histogram, signal peak longitudinal profile start
    pandora::Histogram    **m_pBkgLongProfileStart;         ///< PDF histogram, background peak longitudinal profile start
    pandora::Histogram    **m_pSigLongProfileDiscrepancy;   ///< PDF histogram, signal peak longitudinal profile discrepancy
    pandora::Histogram    **m_pBkgLongProfileDiscrepancy;   ///< PDF histogram, background peak longitudinal profile discrepancy
    pandora::Histogram    **m_pSigPeakEnergyFraction;       ///< PDF histogram, signal peak energy fraction
    pandora::Histogram    **m_pBkgPeakEnergyFraction;       ///< PDF histogram, background peak energy fraction
    pandora::Histogram    **m_pSigMinDistanceToTrack;       ///< PDF histogram, signal peak min distance to track
    pandora::Histogram    **m_pBkgMinDistanceToTrack;       ///< PDF histogram, background peak min distance to track

    float                   m_pidCut;                       ///< The pid cut to apply for photon cluster identification

    float                   m_minClusterEnergy;             ///< The minimum energy to consider a cluster
    unsigned int            m_transProfileMaxLayer;         ///< Maximum layer to consider in calculation of shower transverse profiles
    float                   m_minPeakEnergy;                ///< The minimum energy to consider a transverse profile peak
    float                   m_maxPeakRms;                   ///< The maximum rms value to consider a transverse profile peak
    unsigned int            m_minPeakCaloHits;              ///< The minimum number of calo hits associated with a transverse profile peak
    float                   m_maxLongProfileStart;          ///< The maximum longitudinal shower profile start
    float                   m_maxLongProfileDiscrepancy;    ///< The maximum longitudinal shower profile discrepancy
    unsigned int            m_maxSearchLayer;               ///< Max pseudo layer to examine when calculating track-cluster distance
    float                   m_parallelDistanceCut;          ///< Max allowed projection of track-hit separation along track direction
    float                   m_minTrackClusterCosAngle;      ///< Min cos(angle) between track and cluster initial direction
    float                   m_minDistanceToTrackCut;        ///< The minimum value of the distance to the nearest track

    float                   m_oldClusterEnergyFraction0;    ///< The cluster energy fraction above which original cluster will be used
    float                   m_oldClusterEnergyFraction1;    ///< Decision to use original cluster: energy fraction 1
    float                   m_oldClusterEnergyDifference1;  ///< Decision to use original cluster: energy difference 1
    float                   m_oldClusterEnergyFraction2;    ///< Decision to use original cluster: energy fraction 2
    float                   m_oldClusterEnergyDifference2;  ///< Decision to use original cluster: energy difference 2
    float                   m_oldClusterEnergyFraction3;    ///< Decision to use original cluster: energy fraction 3
    float                   m_oldClusterEnergyDifference3;  ///< Decision to use original cluster: energy difference 3
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonReconstructionAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonReconstructionAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_PHOTON_RECONSTRUCTION_ALGORITHM_H
