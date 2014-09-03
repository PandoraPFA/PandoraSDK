/**
 *  @file   PandoraSDK/include/Objects/Cluster.h
 * 
 *  @brief  Header file for the cluster class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_CLUSTER_H
#define PANDORA_CLUSTER_H 1

#include "Api/PandoraContentApi.h"

#include "Helpers/ClusterFitHelper.h"

#include "Objects/OrderedCaloHitList.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

class Pandora;
template<typename T> class AlgorithmObjectManager;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Cluster class
 */
class Cluster
{
public:
    /**
     *  @brief  Get the ordered calo hit list
     * 
     *  @return The ordered calo hit list by reference
     */
    const OrderedCaloHitList &GetOrderedCaloHitList() const;

    /**
     *  @brief  Get the isolated calo hit list
     * 
     *  @return The isolated calo hit list by reference
     */
    const CaloHitList &GetIsolatedCaloHitList() const;

    /**
     *  @brief  Get the number of calo hits in the cluster
     * 
     *  @return The number of calo hits
     */
    unsigned int GetNCaloHits() const;

    /**
     *  @brief  Get the number of isolated calo hits in the cluster
     * 
     *  @return The number of isolated calo hits
     */
    unsigned int GetNIsolatedCaloHits() const;

    /**
     *  @brief  Get the number of calo hits in the cluster that have been flagged as possible mip hits
     * 
     *  @return The number of possible mip hits
     */
    unsigned int GetNPossibleMipHits() const;

    /**
     *  @brief  Get fraction of constituent calo hits that have been flagged as possible mip hits
     * 
     *  @return The mip fraction
     */
    float GetMipFraction() const;

    /**
     *  @brief  Get the sum of electromagnetic energy measures of all constituent calo hits, units GeV
     * 
     *  @return The electromagnetic energy measure
     */
    float GetElectromagneticEnergy() const;

    /**
     *  @brief  Get the sum of hadronic energy measures of all constituent calo hits, units GeV
     * 
     *  @return The hadronic energy measure
     */
    float GetHadronicEnergy() const;

    /**
     *  @brief  Get the sum of electromagnetic energy measures of isolated constituent calo hits, units GeV
     * 
     *  @return The electromagnetic energy measure
     */
    float GetIsolatedElectromagneticEnergy() const;

    /**
     *  @brief  Get the sum of hadronic energy measures of isolated constituent calo hits, units GeV
     * 
     *  @return The hadronic energy measure
     */
    float GetIsolatedHadronicEnergy() const;

    /**
     *  @brief  Whether the cluster has been flagged as a fixed photon cluster
     * 
     *  @return boolean
     */
    bool IsFixedPhoton() const;

    /**
     *  @brief  Whether the cluster has been flagged as a fixed electron cluster
     * 
     *  @return boolean
     */
    bool IsFixedElectron() const;

    /**
     *  @brief  Whether the cluster has been flagged as a fixed muon cluster
     * 
     *  @return boolean
     */
    bool IsFixedMuon() const;

    /**
     *  @brief  Whether the cluster is track seeded
     * 
     *  @return boolean 
     */
    bool IsTrackSeeded() const;

    /**
     *  @brief  Get the address of the track with which the cluster is seeded
     * 
     *  @return address of the track seed
     */
    const Track *GetTrackSeed() const;

    /**
     *  @brief  Get the innermost pseudo layer in the cluster
     * 
     *  @return The innermost pseudo layer in the cluster
     */
    unsigned int GetInnerPseudoLayer() const;

    /**
     *  @brief  Get the outermost pseudo layer in the cluster
     * 
     *  @return The outermost pseudo layer in the cluster
     */
    unsigned int GetOuterPseudoLayer() const;

    /**
     *  @brief  Get unweighted centroid for cluster at a particular pseudo layer, calculated using cached values of hit coordinate sums
     * 
     *  @param  pseudoLayer the pseudo layer of interest
     * 
     *  @return The unweighted centroid, returned by value
     */
    const CartesianVector GetCentroid(const unsigned int pseudoLayer) const;

    /**
     *  @brief  Get the initial direction of the cluster
     * 
     *  @return The initial direction of the cluster
     */
    const CartesianVector &GetInitialDirection() const;

    /**
     *  @brief  Get the result of a linear fit to all calo hits in the cluster
     * 
     *  @return The cluster fit result
     */
    const ClusterFitResult &GetFitToAllHitsResult() const;

    /**
     *  @brief  Get the typical inner layer hit type
     * 
     *  @return The typical inner layer hit type
     */
    HitType GetInnerLayerHitType() const;

    /**
     *  @brief  Get the typical outer layer hit type
     * 
     *  @return The typical outer layer hit type
     */
    HitType GetOuterLayerHitType() const;

    /**
     *  @brief  Get the list of tracks associated with the cluster
     * 
     *  @return Address of the list of associated tracks
     */
    const TrackList &GetAssociatedTrackList() const;

    /**
     *  @brief  Set the is fixed photon flag for the cluster
     * 
     *  @param  isFixedPhotonFlag the is fixed photon flag
     */
    void SetIsFixedPhotonFlag(bool isFixedPhotonFlag);

    /**
     *  @brief  Set the is fixed electron flag for the cluster
     * 
     *  @param  isFixedElectronFlag the is fixed electron flag
     */
    void SetIsFixedElectronFlag(bool isFixedElectronFlag);

    /**
     *  @brief  Set the is fixed muon flag for the cluster
     * 
     *  @param  isFixedMuonFlag the is fixed muon flag
     */
    void SetIsFixedMuonFlag(bool isFixedMuonFlag);

    /**
     *  @brief  Whether the cluster is available to be added to a particle flow object
     * 
     *  @return boolean
     */
    bool IsAvailable() const;

    /**
     *  @brief  Get the corrected electromagnetic estimate of the cluster energy, units GeV
     * 
     *  @param  pandora the associated pandora instance
     * 
     *  @return The corrected electromagnetic energy estimate
     */
    float GetCorrectedElectromagneticEnergy(const Pandora &pandora) const;

    /**
     *  @brief  Get the corrected hadronic estimate of the cluster energy, units GeV
     * 
     *  @param  pandora the associated pandora instance
     * 
     *  @return The corrected hadronic energy estimate
     */
    float GetCorrectedHadronicEnergy(const Pandora &pandora) const;

    /**
     *  @brief  Get the best energy estimate to use when comparing cluster energy to associated track momentum, units GeV.
     *          For clusters identified as electromagnetic showers, the corrected electromagnetic energy will be returned.
     *          For all other clusters, the corrected hadronic energy will be returned.
     * 
     *  @param  pandora the associated pandora instance
     * 
     *  @return The track comparison energy estimate
     */
    float GetTrackComparisonEnergy(const Pandora &pandora) const;

    /**
     *  @brief  Whether the cluster has been flagged as a photon by photon id function
     * 
     *  @param  pandora the associated pandora instance
     * 
     *  @return boolean
     */
    bool IsPhotonFast(const Pandora &pandora) const;

    /**
     *  @brief  Get the pseudo layer at which shower commences
     * 
     *  @param  pandora the associated pandora instance
     * 
     *  @return The pseudo layer at which shower commences
     */
    unsigned int GetShowerStartLayer(const Pandora &pandora) const;

    /**
     *  @brief  Get the cluster shower profile start, units radiation lengths
     * 
     *  @param  pandora the associated pandora instance
     * 
     *  @return The cluster shower profile start
     */
    float GetShowerProfileStart(const Pandora &pandora) const;

    /**
     *  @brief  Get the cluster shower profile discrepancy
     * 
     *  @param  pandora the associated pandora instance
     * 
     *  @return The cluster shower profile discrepancy
     */
    float GetShowerProfileDiscrepancy(const Pandora &pandora) const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the cluster parameters
     */
    Cluster(const PandoraContentApi::Cluster::Parameters &parameters);

    /**
     *  @brief  Destructor
     */
    ~Cluster();

    /**
     *  @brief  Add a calo hit to the cluster
     * 
     *  @param  pCaloHit the address of the calo hit
     */
    StatusCode AddCaloHit(CaloHit *const pCaloHit);

    /**
     *  @brief  Remove a calo hit from the cluster
     * 
     *  @param  pCaloHit the address of the calo hit
     */
    StatusCode RemoveCaloHit(CaloHit *const pCaloHit);

    /**
     *  @brief  Add an isolated calo hit to the cluster.
     * 
     *  @param  pCaloHit the address of the isolated calo hit
     */
    StatusCode AddIsolatedCaloHit(CaloHit *const pCaloHit);

    /**
     *  @brief  Remove an isolated calo hit from the cluster
     * 
     *  @param  pCaloHit the address of the isolated calo hit
     */
    StatusCode RemoveIsolatedCaloHit(CaloHit *const pCaloHit);

    /**
     *  @brief  Calculate result of a linear fit to all calo hits in the cluster
     */
    void CalculateFitToAllHitsResult() const;

    /**
     *  @brief  Calculate cluster initial direction
     */
    void CalculateInitialDirection() const;

    /**
     *  @brief  Calculate the typical hit type for a specified layer
     * 
     *  @param  pseudoLayer the pseudo layer
     *  @param  layerHitType to receive the typical layer hit type
     */
    void CalculateLayerHitType(const unsigned int pseudoLayer, InputHitType &layerHitType) const;

    /**
     *  @brief  PerformClusterEnergyCorrections
     * 
     *  @param  pandora the associated pandora instance
     */
    void PerformEnergyCorrections(const Pandora &pandora) const;

    /**
     *  @brief  Calculate the fast photon flag
     * 
     *  @param  pandora the associated pandora instance
     */
    void CalculateFastPhotonFlag(const Pandora &pandora) const;

    /**
     *  @brief  Calculate the pseudo layer at which shower commences
     * 
     *  @param  pandora the associated pandora instance
     */
    void CalculateShowerStartLayer(const Pandora &pandora) const;

    /**
     *  @brief  Calculate shower profile and compare it with the expected profile for a photon
     * 
     *  @param  pandora the associated pandora instance
     */
    void CalculateShowerProfile(const Pandora &pandora) const;

    /**
     *  @brief  Reset all cluster properties
     */
    StatusCode ResetProperties();

    /**
     *  @brief  Reset those cluster properties that must be recalculated upon addition/removal of a calo hit
     */
    void ResetOutdatedProperties();

    /**
     *  @brief  Add the calo hits from a second cluster to this
     * 
     *  @param  pCluster the address of the second cluster
     */
    StatusCode AddHitsFromSecondCluster(Cluster *const pCluster);

    /**
     *  @brief  Add an association between the cluster and a track
     * 
     *  @param  pTrack the address of the track with which the cluster is associated
     */
    StatusCode AddTrackAssociation(Track *const pTrack);

    /**
     *  @brief  Remove an association between the cluster and a track
     * 
     *  @param  pTrack the address of the track with which the cluster is no longer associated
     */
    StatusCode RemoveTrackAssociation(Track *const pTrack);

    /**
     *  @brief  Remove the track seed, changing the initial direction measurement.
     */
    void RemoveTrackSeed();

    /**
     *  @brief  Set availability of cluster to be added to a particle flow object
     * 
     *  @param  isAvailable the cluster availability
     */
    void SetAvailability(bool isAvailable);

    typedef std::map<unsigned int, double> ValueByPseudoLayerMap;///< The value by pseudo layer typedef
    typedef std::map<HitType, float> HitTypeToEnergyMap;        ///< The hit type to energy map typedef

    OrderedCaloHitList          m_orderedCaloHitList;           ///< The ordered calo hit list
    CaloHitList                 m_isolatedCaloHitList;          ///< The list of isolated hits, which contribute only towards cluster energy

    unsigned int                m_nCaloHits;                    ///< The number of calo hits
    unsigned int                m_nPossibleMipHits;             ///< The number of calo hits that have been flagged as possible mip hits

    double                      m_electromagneticEnergy;        ///< The sum of electromagnetic energy measures of constituent calo hits, units GeV
    double                      m_hadronicEnergy;               ///< The sum of hadronic energy measures of constituent calo hits, units GeV
    double                      m_isolatedElectromagneticEnergy;///< Sum of electromagnetic energy measures of isolated calo hits, units GeV
    double                      m_isolatedHadronicEnergy;       ///< Sum of hadronic energy measures of isolated calo hits, units GeV

    bool                        m_isFixedPhoton;                ///< Whether the cluster has been flagged as a fixed photon cluster
    bool                        m_isFixedElectron;              ///< Whether the cluster has been flagged as a fixed electron cluster
    bool                        m_isFixedMuon;                  ///< Whether the cluster has been flagged as a fixed muon cluster

    bool                        m_isMipTrack;                   ///< Whether the cluster has been flagged as a section of mip track
    const Track                *m_pTrackSeed;                   ///< Address of the track with which the cluster is seeded

    ValueByPseudoLayerMap       m_sumXByPseudoLayer;            ///< The sum of the x coordinates of the calo hits, stored by pseudo layer
    ValueByPseudoLayerMap       m_sumYByPseudoLayer;            ///< The sum of the y coordinates of the calo hits, stored by pseudo layer
    ValueByPseudoLayerMap       m_sumZByPseudoLayer;            ///< The sum of the z coordinates of the calo hits, stored by pseudo layer

    InputUInt                   m_innerPseudoLayer;             ///< The innermost pseudo layer in the cluster
    InputUInt                   m_outerPseudoLayer;             ///< The outermost pseudo layer in the cluster

    mutable CartesianVector     m_initialDirection;             ///< The initial direction of the cluster
    mutable bool                m_isDirectionUpToDate;          ///< Whether the initial direction of the cluster is up to date

    mutable ClusterFitResult    m_fitToAllHitsResult;           ///< The result of a linear fit to all calo hits in the cluster
    mutable bool                m_isFitUpToDate;                ///< Whether the fit to all calo hits is up to date

    mutable InputFloat          m_correctedElectromagneticEnergy;///< The corrected electromagnetic estimate of the cluster energy, units GeV
    mutable InputFloat          m_correctedHadronicEnergy;      ///< The corrected hadronic estimate of the cluster energy, units GeV
    mutable InputFloat          m_trackComparisonEnergy;        ///< The appropriate corrected energy to use in comparisons with track momentum, units GeV

    mutable InputBool           m_isPhotonFast;                 ///< Whether the cluster is flagged as a photon by fast photon id function
    mutable InputUInt           m_showerStartLayer;             ///< The pseudo layer at which shower commences
    mutable InputFloat          m_showerProfileStart;           ///< The cluster shower profile start, units radiation lengths
    mutable InputFloat          m_showerProfileDiscrepancy;     ///< The cluster shower profile discrepancy

    mutable InputHitType        m_innerLayerHitType;            ///< The typical inner layer hit type
    mutable InputHitType        m_outerLayerHitType;            ///< The typical outer layer hit type

    TrackList                   m_associatedTrackList;          ///< The list of tracks associated with the cluster

    bool                        m_isAvailable;                  ///< Whether the cluster is available to be added to a particle flow object

    friend class PandoraContentApiImpl;
    friend class ClusterManager;
    friend class AlgorithmObjectManager<Cluster>;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const OrderedCaloHitList &Cluster::GetOrderedCaloHitList() const
{
    return m_orderedCaloHitList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CaloHitList &Cluster::GetIsolatedCaloHitList() const
{
    return m_isolatedCaloHitList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int Cluster::GetNCaloHits() const
{
    return m_nCaloHits;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int Cluster::GetNIsolatedCaloHits() const
{
    return m_isolatedCaloHitList.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int Cluster::GetNPossibleMipHits() const
{
    return m_nPossibleMipHits;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetMipFraction() const
{
    float mipFraction = 0;

    if (0 != m_nCaloHits)
        mipFraction = static_cast<float> (m_nPossibleMipHits) / static_cast<float> (m_nCaloHits);

    return mipFraction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetElectromagneticEnergy() const
{
    return static_cast<float>(m_electromagneticEnergy);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetHadronicEnergy() const
{
    return static_cast<float>(m_hadronicEnergy);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetIsolatedElectromagneticEnergy() const
{
    return static_cast<float>(m_isolatedElectromagneticEnergy);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetIsolatedHadronicEnergy() const
{
    return static_cast<float>(m_isolatedHadronicEnergy);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsFixedPhoton() const
{
    return m_isFixedPhoton;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsFixedElectron() const
{
    return m_isFixedElectron;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsFixedMuon() const
{
    return m_isFixedMuon;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsTrackSeeded() const
{
    return (NULL != m_pTrackSeed);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const Track *Cluster::GetTrackSeed() const
{
    if (NULL == m_pTrackSeed)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pTrackSeed;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int Cluster::GetInnerPseudoLayer() const
{
    return m_innerPseudoLayer.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int Cluster::GetOuterPseudoLayer() const
{
    return m_outerPseudoLayer.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &Cluster::GetInitialDirection() const
{
    if (!m_isDirectionUpToDate)
        this->CalculateInitialDirection();

    return m_initialDirection;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const ClusterFitResult &Cluster::GetFitToAllHitsResult() const
{
    if (!m_isFitUpToDate)
        this->CalculateFitToAllHitsResult();

    return m_fitToAllHitsResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline HitType Cluster::GetInnerLayerHitType() const
{
    if (!m_innerLayerHitType.IsInitialized())
        this->CalculateLayerHitType(m_innerPseudoLayer.Get(), m_innerLayerHitType);

    return m_innerLayerHitType.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline HitType Cluster::GetOuterLayerHitType() const
{
    if (!m_outerLayerHitType.IsInitialized())
        this->CalculateLayerHitType(m_outerPseudoLayer.Get(), m_outerLayerHitType);

    return m_outerLayerHitType.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const TrackList &Cluster::GetAssociatedTrackList() const
{
    return m_associatedTrackList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::SetIsFixedPhotonFlag(bool isFixedPhotonFlag)
{
    m_isPhotonFast.Reset();
    m_isFixedPhoton = isFixedPhotonFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::SetIsFixedElectronFlag(bool isFixedElectronFlag)
{
    m_isFixedElectron = isFixedElectronFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::SetIsFixedMuonFlag(bool isFixedMuonFlag)
{
    m_isFixedMuon = isFixedMuonFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetCorrectedElectromagneticEnergy(const Pandora &pandora) const
{
    if (!m_correctedElectromagneticEnergy.IsInitialized())
        this->PerformEnergyCorrections(pandora);

    return m_correctedElectromagneticEnergy.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetCorrectedHadronicEnergy(const Pandora &pandora) const
{
    if (!m_correctedHadronicEnergy.IsInitialized())
        this->PerformEnergyCorrections(pandora);

    return m_correctedHadronicEnergy.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetTrackComparisonEnergy(const Pandora &pandora) const
{
    if (!m_trackComparisonEnergy.IsInitialized())
        this->PerformEnergyCorrections(pandora);

    return m_trackComparisonEnergy.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsPhotonFast(const Pandora &pandora) const
{
    if (!m_isPhotonFast.IsInitialized())
        this->CalculateFastPhotonFlag(pandora);

    return m_isPhotonFast.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int Cluster::GetShowerStartLayer(const Pandora &pandora) const
{
    if (!m_showerStartLayer.IsInitialized())
        this->CalculateShowerStartLayer(pandora);

    return m_showerStartLayer.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetShowerProfileStart(const Pandora &pandora) const
{
    if (!m_showerProfileStart.IsInitialized())
        this->CalculateShowerProfile(pandora);

    return m_showerProfileStart.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetShowerProfileDiscrepancy(const Pandora &pandora) const
{
    if (!m_showerProfileDiscrepancy.IsInitialized())
        this->CalculateShowerProfile(pandora);

    return m_showerProfileDiscrepancy.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster::~Cluster()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::ResetOutdatedProperties()
{
    m_isFitUpToDate = false;
    m_isDirectionUpToDate = false;
    m_initialDirection.SetValues(0.f, 0.f, 0.f);
    m_fitToAllHitsResult.Reset();
    m_showerStartLayer.Reset();
    m_isPhotonFast.Reset();
    m_showerProfileStart.Reset();
    m_showerProfileDiscrepancy.Reset();
    m_correctedElectromagneticEnergy.Reset();
    m_correctedHadronicEnergy.Reset();
    m_trackComparisonEnergy.Reset();
    m_innerLayerHitType.Reset();
    m_outerLayerHitType.Reset();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsAvailable() const
{
    return m_isAvailable;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::RemoveTrackSeed()
{
    m_pTrackSeed = NULL;
    this->CalculateInitialDirection();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::SetAvailability(bool isAvailable)
{
    m_isAvailable = isAvailable;
}

} // namespace pandora

#endif // #ifndef PANDORA_CLUSTER_H
