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
    enum HistVar {PEAKRMS, RMSRATIO, LONGPROFILESTART, LONGPROFILEDISCREPANCY, PEAKENERGYFRACTION, MINDISTANCETOTRACK}; 
    
    class HistSglBkgObject
    {
    public:
        HistSglBkgObject(const std::string histName);
        
        std::string         m_histName;
        int                 m_nBins;
        float               m_lowValue;
        float               m_highValue;
        pandora::Histogram** m_pSglHistogram;
        pandora::Histogram** m_pBkgHistogram;
    };
    
    typedef std::map<HistVar, HistSglBkgObject> HistVarSglBkgHistMap;
    typedef std::map<HistVar, float> HistVarQuantityMap;
    
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Set input cluster list name
     */
    pandora::StatusCode InitialiseInputClusterListName();
    
    /**
     *  @brief  Create regions of interests 
     * 
     *  @param  clusterVector clusters of interests to receive
     */
    pandora::StatusCode CreateClustersOfInterest(pandora::ClusterVector &clusterVector) const;
    
    /**
     *  @brief  Get all tracks 
     * 
     *  @param  trackVector all tracks in vector to receive
     */
    pandora::StatusCode GetTrackVectors(pandora::TrackVector &trackVector) const;
    
    /**
     *  @brief  True for passing pre selection cut. Ideally loose cuts to rejet non interesting cluster
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return True for passing pre selection cut.
     */
    bool PassClusterQualityPreCut(const pandora::Cluster *const pCluster) const;
    
    /**
     *  @brief  Get individual showers(clusters) from the big cluster, for the cluster far from charged tracks projection. Main power horse
     * 
     *  @param  pCluster address of the cluster
     *  @param  showersPhoton shower peak list of photon candidates from the big cluster
     */
    pandora::StatusCode GetTracklessClusterShowerList(const pandora::Cluster *const pCluster, BXShowerProfilePluginNew::ShowerPeakList &showersPhoton) const;
    
    /**
     *  @brief  Get individual showers(clusters) from the big cluster, for the cluster close to charged tracks projection. Main power horse
     * 
     *  @param  pCluster address of the cluster
     *  @param  pMinTrack address of the cloeset track to the cluster
     *  @param  trackVector the vector of all tracks
     *  @param  showersPhoton shower peak list of photon candidates from the big cluster
     *  @param  showersCharged shower peak list of non photon candidates from the big cluster
     */
    pandora::StatusCode GetTrackClusterShowerList(const pandora::Cluster *const pCluster, const pandora::Track *const pMinTrack, const pandora::TrackVector trackVector,
        BXShowerProfilePluginNew::ShowerPeakList &showersPhoton, BXShowerProfilePluginNew::ShowerPeakList &showersCharged) const;
    
    /**
     *  @brief  Creat photons by checking and setting photon id.
     * 
     *  @param  pCluster address of the cluster
     *  @param  showersPhoton shower peak list of photon candidates from the big cluster
     *  @param  isFromTrack true for a cluster close to a track
     */
    pandora::StatusCode CreatePhotons(const pandora::Cluster *const pCluster, const BXShowerProfilePluginNew::ShowerPeakList &showersPhoton, const bool isFromTrack) const;
    
    /**
     *  @brief  Initialise fragmentation.  
     * 
     *  @param  pCluster address of the cluster
     *  @param  originalClusterListName original cluster list name
     *  @param  peakClusterListName new cluster list name
     */
    pandora::StatusCode InitialiseFragmentation(const pandora::Cluster *const pCluster, std::string &originalClusterListName, std::string &peakClusterListName) const;

    /**
     *  @brief  End fragmentation. Revert back to the correct cluster list depending on whether the cluster is used
     * 
     *  @param  usedCluster true for any part of cluster is a photon
     *  @param  originalClusterListName original cluster list name
     *  @param  peakClusterListName new cluster list name
     */
    pandora::StatusCode EndFragmentation(const bool usedCluster, const std::string &originalClusterListName, const std::string &peakClusterListName) const;
    
    /**
     *  @brief  Creat a photon by checking and setting photon id.
     * 
     *  @param  showersPhoton shower peak list for photon candidate
     *  @param  wholeClusuterEnergy the total energy of the big cluster where the shower peak comes from
     *  @param  usedCluster true for the cluster is a photon to receive
     *  @param  isFromTrack true for the cluster close to a track
     */
    pandora::StatusCode CreateClustersAndSetPhotonID(const BXShowerProfilePluginNew::ShowerPeakList &showersPhoton, const float wholeClusuterEnergy, bool &usedCluster, const bool isFromTrack) const;
    
    /**
     *  @brief  Creat a photon by setting photon id.
     * 
     *  @param  showerPeak shower peak list for photon candidate
     *  @param  pPeakCluster address of the photon to form
     */
    pandora::StatusCode CreateCluster(const BXShowerProfilePluginNew::ShowerPeak &showerPeak, const pandora::Cluster *&pPeakCluster) const;
    
    /**
     *  @brief  Check and set photon id for a cluster.
     * 
     *  @param  showerPeak shower peak list for photon candidate
     *  @param  pPeakCluster address of the photon candidate
     *  @param  wholeClusuterEnergy the total energy of the big cluster where the shower peak comes from
     *  @param  isPhoton true for the cluster is a photon to receive
     *  @param  isFromTrack true for the cluster close to a track
     */
    pandora::StatusCode CheckAndSetPhotonID(const BXShowerProfilePluginNew::ShowerPeak &showerPeak, const pandora::Cluster *const pPeakCluster, const float wholeClusuterEnergy, bool &isPhoton, const bool isFromTrack) const;
    
    /**
     *  @brief  Calculate quantities for checking photon id
     * 
     *  @param  showerPeak shower peak list for photon candidate
     *  @param  pPeakCluster address of the photon candidate
     *  @param  wholeClusuterEnergy the total energy of the big cluster where the shower peak comes from
     *  @param  hisVarQuantityMap a varible to value map to store quantities for checking photon id
     */
    pandora::StatusCode CalculateForPhotonID(const BXShowerProfilePluginNew::ShowerPeak &showerPeak, const pandora::Cluster *const pPeakCluster, 
        const float wholeClusuterEnergy, HistVarQuantityMap &hisVarQuantityMap) const;
        
    /**
     *  @brief  Use quantities to check photon id
     * 
     *  @param  pPeakCluster address of the photon candidate
     *  @param  hisVarQuantityMap a varible to value map to store quantities for checking photon id
     *  @param  isFromTrack true for the cluster close to a track
     */
    bool    isPhotonFromQuantities(const pandora::Cluster *const pPeakCluster, const HistVarQuantityMap &hisVarQuantityMap, const bool isFromTrack) const;
    
    /**
     *  @brief  Set photon id
     * 
     *  @param  pPeakCluster address of the photon candidate
     */
    pandora::StatusCode SetPhotonID(const pandora::Cluster *const pPeakCluster) const;
    
    /**
     *  @brief  True for passing quality cuut
     * 
     *  @param  clusterEnergy energy of the photon candidate
     *  @param  hisVarQuantityMap a varible to value map to store quantities for checking photon id
     * 
     *  @return True for passing quality cuut
     */
    bool PassPhotonQualityCut(const float clusterEnergy, const HistVarQuantityMap &hisVarQuantityMap) const;
    
    /**
     *  @brief  Get the metric of photon id
     * 
     *  @param  clusterEnergy energy of the photon candidate
     *  @param  hisVarQuantityMap a varible to value map to store quantities for checking photon id
     * 
     *  @return The metric of photon id
     */
    float   GetMetricForPhotonID(const float clusterEnergy, const HistVarQuantityMap &hisVarQuantityMap) const;
    
    /**
     *  @brief  True for the metric of photon passing the cut
     * 
     *  @param  metric The metric of photon id
     *  @param  isFromTrack true for the cluster close to a track
     * 
     *  @return True for the metric of photon passing the cut
     */
    bool    PassPhotonMetricCut(const float metric, const bool isFromTrack) const;
    
    /**
     *  @brief  Delete cluster
     * 
     *  @param  pCluster address of the cluster
     */
    pandora::StatusCode DeleteCluster(const pandora::Cluster *const pCluster) const;
    
    /**
     *  @brief  Run nested fragment removal algorithm
     */
    pandora::StatusCode RunNestedFragmentRemovalAlg() const;
    
    /**
     *  @brief  Revert to input cluster list
     */
    pandora::StatusCode ReplaceInputClusterList() const;
    
    /**
     *  @brief  Get minimum distance to the closest track to a cluster
     * 
     *  @param  pCluster the address of the cluster 
     *  @param  trackVector the vector that holds addresses of all tracks
     *  @param  minDistance the minimum distance to closest track to a cluster to receive
     *  @param  pMinTrack the address of the closest track to a cluster to receive
     */
    pandora::StatusCode GetMinDistanceToTrack(const pandora::Cluster *const pCluster, const pandora::TrackVector &trackVector,
        float &minDistance, const pandora::Track *&pMinTrack ) const;
    
    // histogram functions
    /**
     *  @brief  Read histogram settings
     * 
     *  @param  xmlHandle xml handler
     */
    pandora::StatusCode ReadHistogramSettings(const pandora::TiXmlHandle xmlHandle);
    
    /**
     *  @brief  Initialise histogram writing
     * 
     *  @param  xmlHandle xml handler
     */
    pandora::StatusCode InitialiseHistogramWriting(const pandora::TiXmlHandle xmlHandle);
    
    /**
     *  @brief  Initialise histogram reading
     * 
     *  @param  xmlHandle xml handler
     */
    pandora::StatusCode InitialiseHistogramReading();
    
    /**
     *  @brief  Initialise histogram varible object map
     */
    pandora::StatusCode InitialiseHistogramVarObjectMap();
    
    /**
     *  @brief  Get the number of energy bin 
     * 
     *  @param  xmlHandle xml handler
     *  @param  energyBinLowerEdgesStr the string of the energy bin
     */
    pandora::StatusCode GetEnergyBinLowerEdges(const pandora::TiXmlHandle xmlHandle, const std::string &energyBinLowerEdgesStr);
    
    /**
     *  @brief  Check for the correct parameter element of the histogram
     * 
     *  @param  parameter the parameter to receive
     */
    pandora::StatusCode ParameterElementNumberCheck(pandora::FloatVector &parameter) const;
    
    /**
     *  @brief  Get number of signal and background events in training
     * 
     *  @param  xmlHandle xml handler
     *  @param  nSignalEventsStr string for signal events
     *  @param  nBackgroundEventsStr string for background events
     */
    pandora::StatusCode GetNSglBkgEvts(const pandora::TiXmlHandle xmlHandle, const std::string &nSignalEventsStr, const std::string &nBackgroundEventsStr);
    
    /**
     *  @brief  Fill histogram varible object map parameters
     * 
     *  @param  xmlHandle xml handler
     *  @param  histVar the varible to fill
     *  @param  nBinStr the name of the varible
     *  @param  nBinDefault the value to fill
     *  @param  lowValueStr the string of the lower edge 
     *  @param  lowValueDefault the value to the lower edge 
     *  @param  highValueStr the string of the upper edge 
     *  @param  highValueDefault the value to the upper edge 
     */
    pandora::StatusCode FillHistogramVarObjectMapParameters(const pandora::TiXmlHandle xmlHandle, const HistVar histVar, const std::string &nBinStr, 
        const int nBinDefault, const std::string &lowValueStr, const float lowValueDefault, const std::string &highValueStr, const float highValueDefault);
        
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
     *  @brief  Creat a photon for training
     * 
     *  @param  pCluster address of the cluster
     *  @param  showersPhoton shower peak list for photon candidate
     */
    pandora::StatusCode CreatePhotonsForTraining(const pandora::Cluster *const pCluster, const BXShowerProfilePluginNew::ShowerPeakList &showersPhoton);
    
    /**
     *  @brief  Creat a photon and train photon likelihood id
     * 
     *  @param  showersPhoton shower peak list for photon candidate
     *  @param  wholeClusuterEnergy the energy of the whole cluster
     */
    pandora::StatusCode CreateClustersAndTrainPhotonID(const BXShowerProfilePluginNew::ShowerPeakList &showersPhoton, const float wholeClusuterEnergy);
    
    /**
     *  @brief  Train photon likelihood id
     * 
     *  @param  showerPeak shower peak list for photon candidate
     *  @param  pCluster the address of the photon candidate
     *  @param  wholeClusuterEnergy the energy of the whole cluster
     */
    pandora::StatusCode TrainPhotonID(const BXShowerProfilePluginNew::ShowerPeak &showerPeak, const pandora::Cluster *const pCluster, const float wholeClusuterEnergy);
    
    /**
     *  @brief  Fill histogram
     * 
     *  @param  pCluster the address of the photon candidate
     *  @param  hisVarQuantityMap a varible to value map to store quantities for checking photon id
     */
    pandora::StatusCode FillPdfHistograms(const pandora::Cluster *const pCluster, const HistVarQuantityMap &hisVarQuantityMap);
    
    /**
     *  @brief  Normalizing member variable histograms 
     * 
     *  @param  pHistogram the address of the histogram
     */
    void NormalizeHistogram(pandora::Histogram *const pHistogram) const;

    /**
     *  @brief  Write varibles to xml
     * 
     *  @param  xmlDocument xml file
     *  @param  nameStr string of the varible name
     *  @param  valueStr string of the value
     */
    void WriteString(pandora::TiXmlDocument &xmlDocument, const std::string nameStr, const std::string valueStr);
    
    /**
     *  @brief  Normalizing member variable histograms and write them to xml
     */
    void NormalizeAndWriteHistograms();

    /**
     *  @brief  Draw member variable histograms if pandora monitoring functionality is enabled
     */
    void DrawHistograms() const;
    
    std::string             m_photonClusteringAlgName;      ///< The name of the photon clustering algorithm to run
    std::string             m_fragmentMergingAlgName;       ///< The name of the photon fragment merging algorithm to run

    std::string             m_clusterListName;              ///< The name of the output cluster list 
    bool                    m_replaceCurrentClusterList;    ///< Whether to subsequently use the new cluster list as the "current" list
    bool                    m_shouldDeleteNonPhotonClusters;///< Whether to delete clusters that are not reconstructed photons

    std::string             m_inputClusterListName;   
    
    float                   m_minClusterEnergy;             ///< The minimum energy to consider a cluster
    float                   m_minPeakEnergy;                ///< The minimum energy to consider a transverse profile peak
    float                   m_maxPeakRms;                   ///< The maximum rms value to consider a transverse profile peak
    float                   m_maxRmsRatio;                   ///< The max
    float                   m_maxLongProfileStart;          ///< The maximum longitudinal shower profile start
    float                   m_maxLongProfileDiscrepancy;    ///< The maximum longitudinal shower profile discrepancy
    unsigned int            m_maxSearchLayer;               ///< Max pseudo layer to examine when calculating track-cluster distance
    float                   m_parallelDistanceCut;          ///< Max allowed projection of track-hit separation along track direction
    float                   m_minTrackClusterCosAngle;      ///< Min cos(angle) between track and cluster initial direction
    float                   m_minDistanceToTrackCut;
    unsigned int            m_transProfileMaxLayer;         ///< Maximum layer to consider in calculation of shower transverse profiles
    float                   m_pidCut;                       ///< The pid cut to apply for photon cluster identification

    // histogram settings
    std::string             m_histogramFile;                ///< The name of the file containing (or to contain) pdf histograms
    bool                    m_shouldMakePdfHistograms;      ///< Whether to create pdf histograms, rather than perform photon reconstruction
    bool                    m_shouldDrawPdfHistograms;      ///< Whether to draw pdf histograms at end of reconstruction (requires monitoring)

    unsigned int            m_nEnergyBins;                  ///< Number of pdf energy bins
    pandora::FloatVector    m_energyBinLowerEdges;          ///< List of lower edges of the pdf energy bins
    pandora::IntVector      m_nSignalEvents;                  ///< 
    pandora::IntVector      m_nBackgroundEvents;                  ///< 
    HistVarSglBkgHistMap    m_histVarSglBkgHistMap;
};
    

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonReconstructionAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonReconstructionAlgorithm();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline PhotonReconstructionAlgorithm::HistSglBkgObject::HistSglBkgObject(const std::string histName):
    m_histName(histName),
    m_nBins(0),
    m_lowValue(0.f),
    m_highValue(0.f),
    m_pSglHistogram(NULL),
    m_pBkgHistogram(NULL)
{
}

} // namespace lc_content

#endif // #ifndef LC_PHOTON_RECONSTRUCTION_ALGORITHM_H
