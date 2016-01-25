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

namespace pandora { class TiXmlDocument; }

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
    /**
     *  @brief  pdf variables
     */
    enum PDFVar
    {
        PEAKRMS,
        RMSXYRATIO,
        LONGPROFILESTART,
        LONGPROFILEDISCREPANCY,
        PEAKENERGYFRACTION,
        MINDISTANCETOTRACK
    };

    /**
     *  @brief  likelihood pdf obejct class
     */
    class LikelihoodPDFObject
    {
    public:
        LikelihoodPDFObject(const std::string &pdfVarName);

        std::string             m_pdfVarName;           /// pdf variable name
        int                     m_nBins;                /// number of bins
        float                   m_lowValue;             /// The min value
        float                   m_highValue;            /// The max value
        pandora::Histogram    **m_pSignalPDF;           /// The signal pdf
        pandora::Histogram    **m_pBackgroundPDF;       /// The background pdf
    };

    typedef std::map<PDFVar, LikelihoodPDFObject>   PDFVarLikelihoodPDFMap;   /// The pdf variable to pdf object map
    typedef std::map<PDFVar, float>                 PDFVarFloatMap;           /// The pdf variable to float object map

    pandora::StatusCode Initialize();
    pandora::StatusCode Run();

    /**
     *  @brief  Set input cluster list name
     *
     *  @param  inputClusterListName input cluster list name
     */
    pandora::StatusCode InitialiseInputClusterListName(std::string &inputClusterListName) const;

    /**
     *  @brief  Create clusters of interest
     *
     *  @param  clusterVector to receive clusters of interests
     */
    pandora::StatusCode CreateClustersOfInterest(pandora::ClusterVector &clusterVector) const;

    /**
     *  @brief  Get all tracks from event track list and put into a vector
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
    pandora::StatusCode GetTracklessClusterShowerList(const pandora::Cluster *const pCluster, pandora::ShowerProfilePlugin::ShowerPeakList &showersPhoton) const;

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
        pandora::ShowerProfilePlugin::ShowerPeakList &showersPhoton, pandora::ShowerProfilePlugin::ShowerPeakList &showersCharged) const;

    /**
     *  @brief  Create photons by checking and setting photon id
     *
     *  @param  pCluster address of the cluster
     *  @param  showersPhoton shower peak list of photon candidates from the big cluster
     *  @param  isFromTrack true for a cluster close to a track
     */
    pandora::StatusCode CreatePhotons(const pandora::Cluster *const pCluster, const pandora::ShowerProfilePlugin::ShowerPeakList &showersPhoton, const bool isFromTrack) const;

    /**
     *  @brief  Initialise fragmentation
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
     *  @brief  Create a photon by checking and setting photon id
     *
     *  @param  showersPhoton shower peak list for photon candidate
     *  @param  wholeClusuterEnergy the total energy of the big cluster where the shower peak comes from
     *  @param  usedCluster true for the cluster is a photon to receive
     *  @param  isFromTrack true for the cluster close to a track
     */
    pandora::StatusCode CreateClustersAndSetPhotonID(const pandora::ShowerProfilePlugin::ShowerPeakList &showersPhoton, const float wholeClusuterEnergy, bool &usedCluster, const bool isFromTrack) const;

    /**
     *  @brief  Create a photon and modify the particle id to photon
     *
     *  @param  showerPeak shower peak list for photon candidate
     *  @param  pPeakCluster address of the photon to form
     */
    pandora::StatusCode CreateCluster(const pandora::ShowerProfilePlugin::ShowerPeak &showerPeak, const pandora::Cluster *&pPeakCluster) const;

    /**
     *  @brief  Check and set photon id for a cluster
     *
     *  @param  showerPeak shower peak list for photon candidate
     *  @param  pPeakCluster address of the photon candidate
     *  @param  wholeClusuterEnergy the total energy of the big cluster where the shower peak comes from
     *  @param  isPhoton true for the cluster is a photon to receive
     *  @param  isFromTrack true for the cluster close to a track
     */
    pandora::StatusCode CheckAndSetPhotonID(const pandora::ShowerProfilePlugin::ShowerPeak &showerPeak, const pandora::Cluster *const pPeakCluster,
        const float wholeClusuterEnergy, bool &isPhoton, const bool isFromTrack) const;

    /**
     *  @brief  Calculate quantities for photon id pdf test
     *
     *  @param  showerPeak shower peak list for photon candidate
     *  @param  pPeakCluster address of the photon candidate
     *  @param  wholeClusuterEnergy the total energy of the big cluster where the shower peak comes from
     *  @param  pdfVarFloatMap a varible to value map to store quantities for checking photon id
     */
    pandora::StatusCode CalculateForPhotonID(const pandora::ShowerProfilePlugin::ShowerPeak &showerPeak, const pandora::Cluster *const pPeakCluster,
        const float wholeClusuterEnergy, PDFVarFloatMap &pdfVarFloatMap) const;

    /**
     *  @brief  Use likelihood pdf to check photon id
     *
     *  @param  pPeakCluster address of the photon candidate
     *  @param  pdfVarFloatMap a varible to value map to store quantities for checking photon id
     *  @param  isFromTrack true for the cluster close to a track
     */
    bool IsPhoton(const pandora::Cluster *const pPeakCluster, const PDFVarFloatMap &pdfVarFloatMap, const bool isFromTrack) const;

    /**
     *  @brief  Set particle id to photon
     *
     *  @param  pPeakCluster address of the photon candidate
     */
    pandora::StatusCode SetPhotonID(const pandora::Cluster *const pPeakCluster) const;

    /**
     *  @brief  True for passing quality cuut
     *
     *  @param  clusterEnergy energy of the photon candidate
     *  @param  pdfVarFloatMap a varible to value map to store quantities for checking photon id
     * 
     *  @return True for passing quality cuut
     */
    bool PassPhotonQualityCut(const float clusterEnergy, const PDFVarFloatMap &pdfVarFloatMap) const;

    /**
     *  @brief  Get the pid of photon id
     *
     *  @param  clusterEnergy energy of the photon candidate
     *  @param  pdfVarFloatMap a varible to value map to store quantities for checking photon id
     *
     *  @return The pid of photon id
     */
    float GetPIDForPhotonID(const float clusterEnergy, const PDFVarFloatMap &pdfVarFloatMap) const;

    /**
     *  @brief  True for the pid of photon passing the cut
     *
     *  @param  pid The pid of photon id
     *  @param  clusterEnergy energy of the photon candidate
     *  @param  isFromTrack true for the cluster close to a track
     *
     *  @return True for the pid of photon passing the cut
     */
    bool PassPhotonPIDCut(const float pid, const float clusterEnergy, const bool isFromTrack) const;

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
     *
     *  @param  inputClusterListName input cluster list name
     */
    pandora::StatusCode ReplaceInputClusterList(const std::string  &inputClusterListName) const;

    /**
     *  @brief  Get minimum distance to the closest track to a cluster, use the ClusterHelper::GetTrackClusterDistance
     *
     *  @param  pCluster the address of the cluster
     *  @param  trackVector the vector that holds addresses of all tracks
     *  @param  minDistance to receive the minimum distance to closest track to a cluster
     *  @param  pMinTrack to receive the address of the closest track to a cluster
     */
    pandora::StatusCode GetMinDistanceToTrack(const pandora::Cluster *const pCluster, const pandora::TrackVector &trackVector,
        float &minDistance, const pandora::Track *&pMinTrack) const;

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
     *  @brief  Initialise pdf varible to likelihood pdf object map
     */
    pandora::StatusCode InitialisePDFVarLikelihoodPDFObjectMap();

    /**
     *  @brief  Get the number of energy bins
     *
     *  @param  xmlHandle xml handler
     *  @param  nEnergyBinsStr the string of the n energy bin
     *  @param  nEnergyBins number of energy bin to receive
     */
    pandora::StatusCode GetNEnergyBins(const pandora::TiXmlHandle xmlHandle, const std::string &nEnergyBinsStr, unsigned int &nEnergyBins) const;

    /**
     *  @brief  Get the energy bin lower edges
     *
     *  @param  xmlHandle xml handler
     *  @param  energyBinLowerEdgesStr the string of the energy blow edge in
     *  @param  energyBinLowerEdges the energy bin lower edges to receive
     */
    pandora::StatusCode GetEnergyBinLowerEdges(const pandora::TiXmlHandle xmlHandle, const std::string &energyBinLowerEdgesStr, pandora::FloatVector &energyBinLowerEdges) const;

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
     *  @param  nSignalEvents signal events number to receive
     *  @param  nBackgroundEvents background events number to receive
     */
    pandora::StatusCode GetNSignalBackgroundEvts(const pandora::TiXmlHandle xmlHandle, const std::string &nSignalEventsStr, const std::string &nBackgroundEventsStr,
        pandora::IntVector &nSignalEvents, pandora::IntVector &nBackgroundEvents) const;

    /**
     *  @brief  Fill pdf varible to likelihood pdf object map parameters
     *
     *  @param  xmlHandle xml handler
     *  @param  PDFVar the varible to fill
     *  @param  nBinStr the name of the varible
     *  @param  nBinDefault the value to fill
     *  @param  lowValueStr the string of the lower edge
     *  @param  lowValueDefault the value to the lower edge
     *  @param  highValueStr the string of the upper edge
     *  @param  highValueDefault the value to the upper edge
     */
    pandora::StatusCode FillPDFVarLikelihoodPDFMapParameters(const pandora::TiXmlHandle xmlHandle, const PDFVar histVar, const std::string &nBinStr,
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
     *  @brief  Create a photon for training
     * 
     *  @param  pCluster address of the cluster
     *  @param  showersPhoton shower peak list for photon candidate
     */
    pandora::StatusCode CreatePhotonsForTraining(const pandora::Cluster *const pCluster, const pandora::ShowerProfilePlugin::ShowerPeakList &showersPhoton);

    /**
     *  @brief  Create a photon and train photon likelihood id
     *
     *  @param  showersPhoton shower peak list for photon candidate
     *  @param  wholeClusuterEnergy the energy of the whole cluster
     */
    pandora::StatusCode CreateClustersAndTrainPhotonID(const pandora::ShowerProfilePlugin::ShowerPeakList &showersPhoton, const float wholeClusuterEnergy);

    /**
     *  @brief  Train photon likelihood id
     *
     *  @param  showerPeak shower peak list for photon candidate
     *  @param  pCluster the address of the photon candidate
     *  @param  wholeClusuterEnergy the energy of the whole cluster
     */
    pandora::StatusCode TrainPhotonID(const pandora::ShowerProfilePlugin::ShowerPeak &showerPeak, const pandora::Cluster *const pCluster, const float wholeClusuterEnergy);

    /**
     *  @brief  Fill histogram
     *
     *  @param  pCluster the address of the photon candidate
     *  @param  pdfVarFloatMap a varible to value map to store quantities for checking photon id
     */
    pandora::StatusCode FillPdfHistograms(const pandora::Cluster *const pCluster, const PDFVarFloatMap &pdfVarFloatMap);

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

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    std::string             m_photonClusteringAlgName;      ///< The name of the photon clustering algorithm to run
    std::string             m_fragmentMergingAlgName;       ///< The name of the photon fragment merging algorithm to run

    std::string             m_clusterListName;              ///< The name of the output cluster list 
    bool                    m_replaceCurrentClusterList;    ///< Whether to subsequently use the new cluster list as the "current" list
    bool                    m_shouldDeleteNonPhotonClusters;///< Whether to delete clusters that are not reconstructed photons

    float                   m_minClusterEnergy;             ///< The minimum energy to consider a cluster
    float                   m_minPeakEnergy;                ///< The minimum energy to consider a transverse profile peak
    float                   m_maxPeakRms;                   ///< The maximum rms value to consider a transverse profile peak
    float                   m_maxRmsRatio;                  ///< The max rms ratio
    float                   m_maxLongProfileStart;          ///< The maximum longitudinal shower profile start
    float                   m_maxLongProfileDiscrepancy;    ///< The maximum longitudinal shower profile discrepancy
    unsigned int            m_maxSearchLayer;               ///< Max pseudo layer to examine when calculating track-cluster distance
    float                   m_parallelDistanceCut;          ///< Max allowed projection of track-hit separation along track direction
    float                   m_minTrackClusterCosAngle;      ///< Min cos(angle) between track and cluster initial direction
    float                   m_minDistanceToTrackDivisionCut;///< Minimum distance to track to separate clusters close to track or not
    bool                    m_transProfileEcalOnly;         ///< Transverse profile shower calculator uses EcalOnly. Can be overridden by the m_transProfileMaxLayer
    unsigned int            m_transProfileMaxLayer;         ///< Maximum layer to consider in calculation of shower transverse profiles
    float                   m_minDistanceToTrackCutLow;     ///< Minimum distance to track to consider
    float                   m_minDistanceToTrackCutHigh;    ///< Maximum distance to track to consider
    float                   m_energyCutForPid1;             ///< The energy cut for pid test range 1
    float                   m_pidCut1;                      ///< The pid cut to apply for photon cluster identification for energy in range 1
    float                   m_energyCutForPid2;             ///< The energy cut for pid test range 2
    float                   m_pidCut2;                      ///< The pid cut to apply for photon cluster identification for energy in range 2
    float                   m_pidCut3;                      ///< The pid cut to apply for photon cluster identification for energy in range 3

    // histogram settings
    std::string             m_histogramFile;                ///< The name of the file containing (or to contain) pdf histograms
    bool                    m_shouldMakePdfHistograms;      ///< Whether to create pdf histograms, rather than perform photon reconstruction
    bool                    m_shouldDrawPdfHistograms;      ///< Whether to draw pdf histograms at end of reconstruction (requires monitoring)

    unsigned int            m_nEnergyBins;                  ///< Number of pdf energy bins
    pandora::FloatVector    m_energyBinLowerEdges;          ///< List of lower edges of the pdf energy bins
    pandora::IntVector      m_nSignalEvents;                ///< Number of signal(photons) pfos in training
    pandora::IntVector      m_nBackgroundEvents;            ///< Number of background pfos in training
    PDFVarLikelihoodPDFMap  m_pdfVarLikelihoodPDFMap;       ///< Histogram varible to signal background map
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonReconstructionAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonReconstructionAlgorithm();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline PhotonReconstructionAlgorithm::LikelihoodPDFObject::LikelihoodPDFObject(const std::string &pdfVarName):
    m_pdfVarName(pdfVarName),
    m_nBins(0),
    m_lowValue(0.f),
    m_highValue(0.f),
    m_pSignalPDF(NULL),
    m_pBackgroundPDF(NULL)
{
}

} // namespace lc_content

#endif // #ifndef LC_PHOTON_RECONSTRUCTION_ALGORITHM_H
