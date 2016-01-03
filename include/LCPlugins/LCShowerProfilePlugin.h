/**
 *  @file   LCContent/include/LCPlugins/LCShowerProfilePlugin.h
 * 
 *  @brief  Header file for the lc shower profile plugin class.
 * 
 *  $Log: $
 */
#ifndef LC_SHOWER_PROFILE_PLUGIN_H
#define LC_SHOWER_PROFILE_PLUGIN_H 1

#include "Plugins/ShowerProfilePlugin.h"

namespace lc_content
{

/**
 *  @brief  LCShowerProfilePlugin class
 */
class LCShowerProfilePlugin : public pandora::ShowerProfilePlugin
{
public:
    /**
     *  @brief  Default constructor
     */
    LCShowerProfilePlugin();

    void CalculateShowerStartLayer(const pandora::Cluster *const pCluster, unsigned int &showerStartLayer) const;
    void CalculateLongitudinalProfile(const pandora::Cluster *const pCluster, float &profileStart, float &profileDiscrepancy) const;    
    void CalculateTransverseProfile(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, ShowerPeakList &showerPeakList) const;
    void CalculateTracklessTransverseProfile(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, ShowerPeakList &showerPeakList) const;
    void CalculateTrackNearbyTransverseProfile(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, const pandora::Track *const pMinTrack, 
        const pandora::TrackVector &trackVector, ShowerPeakList &showerPeakListPhoton, ShowerPeakList &showerPeakListCharge) const;

private:
    /**
     *  @brief  ShowerProfileEntry class
     */
    class ShowerProfileEntry
    {
    public:
        /**
         *  @brief  Default constructor
         */
        ShowerProfileEntry();

        bool                        m_isAvailable;          ///< Whether shower profile entry is available (prevent double counting)
        float                       m_energy;               ///< The energy associated with the shower profile entry
        pandora::CaloHitList        m_caloHitList;          ///< The list of calo hits associated with the shower profile entry
        bool                        m_potentialPeak;        ///< Whether the shower profile is a potential peak (to speed up looping)
    };

    typedef std::pair<int,int>  TwoDBin;                    ///< The two dimentional bins typedef
    typedef std::vector<TwoDBin > TwoDBinVector;            ///< The two dimentional bins forming a grid typedef
    
    class ShowerPeakObject
    {
    public:
        /**
         *  @brief  Default constructor
         */
        ShowerPeakObject(const float energy, const int uBin, const int vBin);
        
        /**
         *  @brief  Get peak energy
         * 
         *  @return the peak energy
         */
        float GetPeakEnergy() const;
        
        /**
         *  @brief  Get peak u bin
         * 
         *  @return the peak u bin
         */
        int GetPeakUBin() const;
        
        /**
         *  @brief  Get peak v bin
         * 
         *  @return the peak v bin
         */
        int GetPeakVBin() const;        
        
        TwoDBinVector               m_associatedBins;       ///< The bins associated to this peak
        bool                        m_isAvailable;          ///< Whether this is avaiable
        bool                        m_isPhotonCandidate;    ///< Whether this is photon candidate
    private:
        float                       m_energy;               ///< The energy associated with the shower profile entry
        int                         m_uBin;                 ///< The u bin of the peak
        int                         m_vBin;                 ///< The u bin of the peak
        
    };


    
    typedef std::vector<ShowerProfileEntry> ShowerProfile;  ///< The shower profile typedef
    typedef std::vector<ShowerProfile> TwoDShowerProfile;   ///< The two dimensional shower profile typedef

    typedef std::vector<ShowerPeakObject> ShowerPeakObjectVector;   ///< Careful, it is descending.

    /**
     *  @brief  Calculate transverse shower peak objects for a cluster and get the list of peaks identified in the profile, for clusters without tracks
     * 
     *  @param  pCluster the address of the cluster
     *  @param  maxPseudoLayer the maximum pseudo layer to consider
     *  @param  showerProfile two dimensional shower profile to consider
     *  @param  showerPeakObjectVector to receive the shower peak objects
     */
    void CalculateTracklessTransverseShowers(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, TwoDShowerProfile &showerProfile, 
        ShowerPeakObjectVector &showerPeakObjectVector) const;

    /**
     *  @brief  Calculate transverse shower peak objects for a cluster and get the list of peaks identified in the profile, for clusters close to tracks
     * 
     *  @param  pCluster the address of the cluster
     *  @param  maxPseudoLayer the maximum pseudo layer to consider
     *  @param  pMinTrack the closest track to the shower profile to speed up calculation
     *  @param  trackVector the vector of tracks to speed up calculation
     *  @param  showerProfile two dimensional shower profile to consider
     *  @param  showerPeakObjectVector to receive the shower peak objects
     *  @param  trackProjectionVector the 2D positions of track projections to receive and speed up subsequent calculation
     */
    void CalculateTrackNearbyTransverseShowers(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, const pandora::Track *const pMinTrack, 
        const pandora::TrackVector &trackVector, TwoDShowerProfile &showerProfile, ShowerPeakObjectVector &showerPeakObjectVector, TwoDBinVector &trackProjectionVector) const;
        
    /**
     *  @brief  Calculate empty 2D shower profile 
     * 
     *  @param  showerProfile two dimensional shower profile to receive
     */
    void CreateEmptyTwoDShowerProfile(TwoDShowerProfile &showerProfile) const;
    
    /**
     *  @brief  Initialise 2D shower profile for clusters not close to tracks
     * 
     *  @param  pCluster the address of the cluster
     *  @param  maxPseudoLayer the maximum pseudo layer to consider
     *  @param  showerProfile two dimensional shower profile to consider
     */
    void InitialiseTransverseProfile(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, TwoDShowerProfile &showerProfile) const;

    /**
     *  @brief  Initialise 2D shower profile for clusters close to tracks
     * 
     *  @param  pCluster the address of the cluster
     *  @param  maxPseudoLayer the maximum pseudo layer to consider
     *  @param  pMinTrack the closest track to the shower profile to speed up calculation
     *  @param  trackVector the vector of tracks to speed up calculation
     *  @param  showerProfile two dimensional shower profile to consider
     *  @param  trackProjectionVector the 2D positions of track projections to receive and speed up subsequent calculation
     */
    void InitialiseTransverseProfileWithTracks(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, const pandora::Track *const pMinTrack,
        const pandora::TrackVector &trackVector, TwoDShowerProfile &showerProfile, TwoDBinVector &trackProjectionVector) const;

    /**
     *  @brief  Find axes of projection
     * 
     *  @param  pCluster the address of the cluster
     *  @param  innerLayerCentroid the inner layer centroid to receive
     *  @param  uAxis u axis to receive
     *  @param  vAxis v axis to receive
     *  @param  pMinTrack the closest track to the shower profile to speed up calculation
     */
    void CalculateProjectionAxes(const pandora::Cluster *const pCluster, pandora::CartesianVector &innerLayerCentroid, pandora::CartesianVector &uAxis, 
        pandora::CartesianVector &vAxis, const pandora::Track *const pMinTrack = NULL) const;   
    
    /**
     *  @brief  Project cluster to 2D shower profile implementation
     * 
     *  @param  pCluster the address of the cluster
     *  @param  maxPseudoLayer the maximum pseudo layer to consider
     *  @param  innerLayerCentroid the inner layer centroid to consider
     *  @param  uAxis u axis to consider
     *  @param  vAxis v axis to consider
     *  @param  showerProfile two dimensional shower profile to receive
     */
    void InitialiseTwoDShowerProfile(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, const pandora::CartesianVector &innerLayerCentroid, 
        const pandora::CartesianVector &uAxis, const pandora::CartesianVector &vAxis, TwoDShowerProfile &showerProfile) const;
    
    /**
     *  @brief  Find all tracks projections on 2D shower profile
     * 
     *  @param  pCluster the address of the cluster
     *  @param  trackVector the vector of address of all tracks
     *  @param  innerLayerCentroid the inner layer centroid to consider
     *  @param  uAxis u axis to consider
     *  @param  vAxis v axis to consider
     *  @param  trackProjectionVector the 2D positions of track projections to receive and speed up subsequent calculation
     */
    void FindTracksProjection(const pandora::Cluster *const pCluster, const pandora::TrackVector &trackVector, const pandora::CartesianVector &innerLayerCentroid, 
        const pandora::CartesianVector &uAxis, const pandora::CartesianVector &vAxis, TwoDBinVector &trackProjectionVector) const;

    /**
     *  @brief  Mark region with low height unavailable
     * 
     *  @param  showerProfile two dimensional shower profile to consider
     */
    void MaskLowHeightRegions(TwoDShowerProfile &showerProfile) const;
    
    /**
     *  @brief  Find raw peaks in 2D profile, based on local maxima
     * 
     *  @param  showerProfile two dimensional shower profile to consider
     *  @param  showerPeakObjectVector the 2D peak object to receive
     */
    void FindRawPeaksInTwoDShowerProfile(TwoDShowerProfile &showerProfile, ShowerPeakObjectVector &showerPeakObjectVector) const;
    
    /**
     *  @brief  Associate bins to peaks, using TwoDShowerProfile
     * 
     *  @param  showerProfile two dimensional shower profile to consider
     *  @param  showerPeakObjectVector the 2D peak object to receive
     */
    void AssociateBinsToPeaks(const TwoDShowerProfile &showerProfile, ShowerPeakObjectVector &showerPeakObjectVector) const;
    
    /**
     *  @brief  Associate bins to peaks, using TwoDBinVector
     * 
     *  @param  twoDBinVector two dimensional bins
     *  @param  showerPeakObjectVector the 2D peak object to receive
     */
    void AssociateBinsToPeaks(const TwoDBinVector &twoDBinVector, ShowerPeakObjectVector &showerPeakObjectVector) const;
    
    /**
     *  @brief  Apply quality cuts tp peaks
     * 
     *  @param  showerPeakObjectVector the 2D peak object to modify
     *  @param  twoDBinVector the discared two dimensional bins to receive
     */
    void ApplyQualityCutPeakNBin(ShowerPeakObjectVector &showerPeakObjectVector, TwoDBinVector &twoDBinVector) const;
    
    /**
     *  @brief  True for passing the quality cuts for minimum number of bins
     * 
     *  @param  showerPeakObject the 2D peak object to consider
     * 
     *  @return True for passing the quality cuts for minimum number of bins
     */
    bool PassQualityCutPeakNBin(const ShowerPeakObject &showerPeakObject) const;
    
    /**
     *  @brief  Mark bins close to tracks not photon candidate
     * 
     *  @param  trackProjectionVector the all track projections in the 2D plane
     *  @param  showerPeakObjectVector the 2D peak object vector to modify
     */
    void MarkPeaksCloseToTracks(const TwoDBinVector &trackProjectionVector, ShowerPeakObjectVector &showerPeakObjectVector) const;
    
    /**
     *  @brief  Mark bins shifted too much between slices not photon candidate
     * 
     *  @param  showerPeakObjectVectorFirst the 2D peak object vector from previous slice
     *  @param  showerPeakObjectVectorNext the 2D peak object vector from next slice to modify
     */
    void MatchPeaksInTwoSlices(const ShowerPeakObjectVector &showerPeakObjectVectorFirst, ShowerPeakObjectVector &showerPeakObjectVectorNext) const;
    
    /**
     *  @brief  Process the quality cuts and association of bins to peaks 
     * 
     *  @param  showerProfile the 2D shower profile to modify
     *  @param  showerPeakObjectVector the 2D peak object vector to modify
     */
    void ProcessShowerProfile(TwoDShowerProfile &showerProfile, ShowerPeakObjectVector &showerPeakObjectVector) const;

    /**
     *  @brief  Convert 2D bins to shower lists
     * 
     *  @param  showerProfile the 2D shower profile to consider
     *  @param  showerPeakObjectVector the 2D peak object vector to consider
     *  @param  showerPeakListPhoton shower peak list of photon candidates
     *  @param  showerPeakListCharge shower peak list of non photon candidates
     */
    void ConvertBinsToShowerLists(const TwoDShowerProfile &showerProfile, const ShowerPeakObjectVector &showerPeakObjectVector, 
        ShowerPeakList &showerPeakListPhoton, ShowerPeakList &showerPeakListCharge) const;
    
    /**
     *  @brief  Find projection of a 3D point
     * 
     *  @param  hitPosition the 3D position
     *  @param  innerLayerCentroid the inner layer centroid to consider
     *  @param  uAxis u axis to consider
     *  @param  vAxis v axis to consider
     *  @param  nOffsetBins the number of offset bins
     *  @param  cellLengthScale cell length scale
     *  @param  uBin projection in u direction
     *  @param  vBin projection in v direction
     */
    void FindHitPositionProjection(const pandora::CartesianVector &hitPosition, const pandora::CartesianVector &innerLayerCentroid, const pandora::CartesianVector &uAxis, 
        const pandora::CartesianVector &vAxis, const int nOffsetBins, const int cellLengthScale, int &uBin, int &vBin) const;
    
    /**
     *  @brief  True if the peak is a local maxima
     * 
     *  @param  showerProfile the 2D shower profile to modify
     *  @param  uBin projection in u direction
     *  @param  vBin projection in v direction
     * 
     *  @return True if the peak is a local maxima
     */
    bool IsPeak(TwoDShowerProfile &showerProfile, const int uBin, const int vBin ) const;
    
    /**
     *  @brief  True if the peak is a local maxima implementaion
     * 
     *  @param  showerProfile the 2D shower profile to modify
     *  @param  uBin projection in u direction
     *  @param  vBin projection in v direction
     * 
     *  @return True if the peak is a local maxima
     */
    bool Check8NeighbourFull(TwoDShowerProfile &showerProfile, const int uBin, const int vBin) const;
    
    /**
     *  @brief  True if the peak is a local maxima implementaion, fast
     * 
     *  @param  showerProfile the 2D shower profile to modify
     *  @param  uBin projection in u direction
     *  @param  vBin projection in v direction     
     * 
     *  @return True if the peak is a local maxima
     */
    bool Check8NeighbourFast(TwoDShowerProfile &showerProfile, const int uBin, const int vBin) const;
    
    /**
     *  @brief  Calculate the metric for peak association
     * 
     *  @param  distance the distance to the peak
     *  @param  energy energy of the peak
     * 
     *  @return the metric for peak association
     */
    float CalculatePeakFindingMetric(const float distance, const float energy)  const;
    
    /**
     *  @brief  Find the best shower peak to associate bins
     * 
     *  @param  showerPeakObjectVector shower peak obejct vectors for all peaks
     *  @param  uBin position of bin
     *  @param  vBin position of bin
     *  @param  bestShowerPeakObject address of best peak object to receive
     */
    void CalculateBestPeakUsingMetric(ShowerPeakObjectVector &showerPeakObjectVector, const int uBin, const int vBin, ShowerPeakObject *&bestShowerPeakObject) const;
    
    /**
     *  @brief  True for showerPeakObjectVector contains photon candiates
     * 
     *  @param  showerPeakObjectVector shower peak obejct vectors for all peaks
     * 
     *  @return True for showerPeakObjectVector contains photon candiates
     */
    bool HasPhotonCandidate(const ShowerPeakObjectVector &showerPeakObjectVector) const;
    
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    float        m_showerStartMipFraction;                  ///< Max layer mip-fraction to declare layer as shower-like
    unsigned int m_showerStartNonMipLayers;                 ///< Number of successive shower-like layers to identify shower start

    float        m_longProfileBinWidth;                     ///< Bin width used to construct longitudinal profile, units radiation lengths
    unsigned int m_longProfileNBins;                        ///< Number of bins used to construct longitudinal profile
    float        m_longProfileMinCosAngle;                  ///< Min angular correction used to adjust radiation length measures
    float        m_longProfileCriticalEnergy;               ///< Critical energy, used to calculate argument for gamma function
    float        m_longProfileParameter0;                   ///< Parameter0, used to calculate argument for gamma function
    float        m_longProfileParameter1;                   ///< Parameter1, used to calculate argument for gamma function
    float        m_longProfileMaxDifference;                ///< Max difference between current and best longitudinal profile comparisons

    int          m_transProfileNBins;                       ///< Number of bins used to construct transverse profile
    float        m_transProfilePeakThreshold;               ///< Minimum electrogmagnetic energy for profile peak bin, units GeV
    float        m_transProfileNearbyEnergyRatio;           ///< Max ratio of bin energy to nearby bin energy; used to identify peak extent
    unsigned int m_transProfileMaxPeaksToFind;              ///< Maximum number of peaks to identify in transverse profile
    unsigned int m_transProfilePeakFindingMetric;           ///< The metric for peak association
    unsigned int m_transProfileMinNBinsCut;                 ///< The minimum number for bins of a substantial peak
    unsigned int m_transProfileTrackNearbyNSlices;          ///< The number of slices to analyse the EM shower
    float        m_transProfileMinTrackToPeakCut;           ///< The minimum 2D distance of a track to the peak postion
    float        m_transProfileMinDisTrackMatch ;           ///< The maximum allowed shift of 2D distance of the peak position through the slices
};
//------------------------------------------------------------------------------------------------------------------------------------------
inline void LCShowerProfilePlugin::CalculateTransverseProfile(const pandora::Cluster *const pCluster, const unsigned int maxPseudoLayer, ShowerPeakList &showerPeakList) const
{
    return CalculateTracklessTransverseProfile(pCluster, maxPseudoLayer, showerPeakList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline LCShowerProfilePlugin::ShowerProfileEntry::ShowerProfileEntry() :
    m_isAvailable(true),
    m_energy(0.f),
    m_potentialPeak(true)
{
}
//------------------------------------------------------------------------------------------------------------------------------------------

inline LCShowerProfilePlugin::ShowerPeakObject::ShowerPeakObject(const float energy, const int uBin, const int vBin) :
    m_isAvailable(true),
    m_isPhotonCandidate(true),
    m_energy(energy),
    m_uBin(uBin),
    m_vBin(vBin)    
{
    m_associatedBins.push_back(std::make_pair(uBin,vBin));
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LCShowerProfilePlugin::ShowerPeakObject::GetPeakEnergy() const
{
    return m_energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int LCShowerProfilePlugin::ShowerPeakObject::GetPeakUBin() const
{
    return m_uBin;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int LCShowerProfilePlugin::ShowerPeakObject::GetPeakVBin() const
{
    return m_vBin;
}

} // namespace lc_content

#endif // #ifndef LC_SHOWER_PROFILE_PLUGIN_H
