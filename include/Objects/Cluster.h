/**
 *  @file   PandoraSDK/include/Objects/Cluster.h
 * 
 *  @brief  Header file for the cluster class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_CLUSTER_H
#define PANDORA_CLUSTER_H 1

#include "Helpers/ClusterFitHelper.h"

#include "Objects/OrderedCaloHitList.h"

#include "Pandora/ObjectCreation.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

class Pandora;
template<typename T> class AlgorithmObjectManager;
template<typename T, typename S> class PandoraObjectFactory;

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
     *  @brief  Get the number of hits in the outer sampling layers
     * 
     *  @return The number of hits in this cluster in the outermost sampling layer
     */
    unsigned int GetNHitsInOuterLayer() const;

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
     *  @brief  Get the particle id flag
     * 
     *  @return The particle id flag
     */
    int GetParticleId() const;

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
     *  @brief  Whether the cluster passes the photon id
     * 
     *  @param  pandora the associated pandora instance
     * 
     *  @return boolean
     */
    bool PassPhotonId(const Pandora &pandora) const;

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

    /**
     *  @brief  Get minimum and maximum X positions of the calo hits in this cluster
     *
     *  @param  the minimum position of x
     *  @param  the maximum position of x
     */
    void GetClusterSpanX(float &xmin, float &xmax) const;

    /**
     *  @brief  Get upper and lower Z positions of the calo hits in a cluster in range xmin to xmax
     *
     *  @param  xmin for range in x
     *  @param  xmax for range in x
     *  @param  zmin the lower z for this range of x
     *  @param  zmax the upper z for this range in x
     */
    void GetClusterSpanZ(const float xmin, const float xmax, float &zmin, float &zmax) const;

protected:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the cluster parameters
     */
    Cluster(const object_creation::Cluster::Parameters &parameters);

    /**
     *  @brief  Destructor
     */
    virtual ~Cluster();

    /**
     *  @brief  Alter the metadata information stored in a cluster
     * 
     *  @param  metaData the metadata (only populated metadata fields will be propagated to the object)
     */
    StatusCode AlterMetadata(const object_creation::Cluster::Metadata &metadata);

    /**
     *  @brief  Add a calo hit to the cluster
     * 
     *  @param  pCaloHit the address of the calo hit
     */
    StatusCode AddCaloHit(const CaloHit *const pCaloHit);

    /**
     *  @brief  Remove a calo hit from the cluster
     * 
     *  @param  pCaloHit the address of the calo hit
     */
    StatusCode RemoveCaloHit(const CaloHit *const pCaloHit);

    /**
     *  @brief  Add an isolated calo hit to the cluster.
     * 
     *  @param  pCaloHit the address of the isolated calo hit
     */
    StatusCode AddIsolatedCaloHit(const CaloHit *const pCaloHit);

    /**
     *  @brief  Remove an isolated calo hit from the cluster
     * 
     *  @param  pCaloHit the address of the isolated calo hit
     */
    StatusCode RemoveIsolatedCaloHit(const CaloHit *const pCaloHit);

    /**
     *  @brief  Update result of linear fit to all calo hits in cluster
     */
    void UpdateFitToAllHitsCache() const;

    /**
     *  @brief  Update cluster initial direction
     */
    void UpdateInitialDirectionCache() const;

    /**
     *  @brief  Update typical hit type for specified layer
     * 
     *  @param  pseudoLayer the pseudo layer
     *  @param  layerHitType to receive the typical layer hit type
     */
    void UpdateLayerHitTypeCache(const unsigned int pseudoLayer, InputHitType &layerHitType) const;

    /**
     *  @brief  Update cluster corrected energy values
     * 
     *  @param  pandora the associated pandora instance
     */
    void UpdateEnergyCorrectionsCache(const Pandora &pandora) const;

    /**
     *  @brief  Update photon if flag
     * 
     *  @param  pandora the associated pandora instance
     */
    void UpdatePhotonIdCache(const Pandora &pandora) const;

    /**
     *  @brief  Update the pseudo layer at which shower commences
     * 
     *  @param  pandora the associated pandora instance
     */
    void UpdateShowerLayerCache(const Pandora &pandora) const;

    /**
     *  @brief  Update shower profile and comparison with expectation for a photon
     * 
     *  @param  pandora the associated pandora instance
     */
    void UpdateShowerProfileCache(const Pandora &pandora) const;

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
    StatusCode AddHitsFromSecondCluster(const Cluster *const pCluster);

    /**
     *  @brief  Add an association between the cluster and a track
     * 
     *  @param  pTrack the address of the track with which the cluster is associated
     */
    StatusCode AddTrackAssociation(const Track *const pTrack);

    /**
     *  @brief  Remove an association between the cluster and a track
     * 
     *  @param  pTrack the address of the track with which the cluster is no longer associated
     */
    StatusCode RemoveTrackAssociation(const Track *const pTrack);

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

    /**
     *  @brief  SimplePoint class
     */
    class SimplePoint
    {
    public:
        double                  m_xyzPositionSums[3];           ///< The sum of the x, y and z hit positions in the pseudo layer
        unsigned int            m_nHits;                        ///< The number of hits in the pseudo layer
    };

    typedef std::map<unsigned int, SimplePoint> PointByPseudoLayerMap;///< The point by pseudo layer typedef
    typedef std::map<HitType, float> HitTypeToEnergyMap;        ///< The hit type to energy map typedef

    OrderedCaloHitList          m_orderedCaloHitList;           ///< The ordered calo hit list
    CaloHitList                 m_isolatedCaloHitList;          ///< The list of isolated hits, which contribute only towards cluster energy
    unsigned int                m_nCaloHits;                    ///< The number of calo hits
    unsigned int                m_nPossibleMipHits;             ///< The number of calo hits that have been flagged as possible mip hits
    unsigned int                m_nCaloHitsInOuterLayer;        ///< Keep track of the number of calo hits in the outermost layers
    double                      m_electromagneticEnergy;        ///< The sum of electromagnetic energy measures of constituent calo hits, units GeV
    double                      m_hadronicEnergy;               ///< The sum of hadronic energy measures of constituent calo hits, units GeV
    double                      m_isolatedElectromagneticEnergy;///< Sum of electromagnetic energy measures of isolated calo hits, units GeV
    double                      m_isolatedHadronicEnergy;       ///< Sum of hadronic energy measures of isolated calo hits, units GeV
    int                         m_particleId;                   ///< The particle id flag
    const Track                *m_pTrackSeed;                   ///< Address of the track with which the cluster is seeded
    PointByPseudoLayerMap       m_sumXYZByPseudoLayer;          ///< Construct to allow rapid calculation of centroid in each pseudolayer
    InputUInt                   m_innerPseudoLayer;             ///< The innermost pseudo layer in the cluster
    InputUInt                   m_outerPseudoLayer;             ///< The outermost pseudo layer in the cluster

    mutable CartesianVector     m_initialDirection;             ///< The initial direction of the cluster
    mutable bool                m_isDirectionUpToDate;          ///< Whether the initial direction of the cluster is up to date
    mutable ClusterFitResult    m_fitToAllHitsResult;           ///< The result of a linear fit to all calo hits in the cluster
    mutable bool                m_isFitUpToDate;                ///< Whether the fit to all calo hits is up to date
    mutable InputFloat          m_correctedElectromagneticEnergy;///< The corrected electromagnetic estimate of the cluster energy, units GeV
    mutable InputFloat          m_correctedHadronicEnergy;      ///< The corrected hadronic estimate of the cluster energy, units GeV
    mutable InputFloat          m_trackComparisonEnergy;        ///< The appropriate corrected energy to use in comparisons with track momentum, units GeV
    mutable InputBool           m_passPhotonId;                 ///< Whether the cluster passes the photon id
    mutable InputUInt           m_showerStartLayer;             ///< The pseudo layer at which shower commences
    mutable InputFloat          m_showerProfileStart;           ///< The cluster shower profile start, units radiation lengths
    mutable InputFloat          m_showerProfileDiscrepancy;     ///< The cluster shower profile discrepancy
    mutable InputHitType        m_innerLayerHitType;            ///< The typical inner layer hit type
    mutable InputHitType        m_outerLayerHitType;            ///< The typical outer layer hit type
    mutable InputFloat          m_xMin;                         ///< Cached cluster minimum in x
    mutable InputFloat          m_xMax;                         ///< Cached cluster maximum in x

    TrackList                   m_associatedTrackList;          ///< The list of tracks associated with the cluster
    bool                        m_isAvailable;                  ///< Whether the cluster is available to be added to a particle flow object

    friend class ClusterManager;
    friend class AlgorithmObjectManager<Cluster>;
    friend class PandoraObjectFactory<object_creation::Cluster::Parameters, object_creation::Cluster::Object>;
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
    return ((0 != m_nCaloHits) ? static_cast<float> (m_nPossibleMipHits) / static_cast<float> (m_nCaloHits) : 0);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int Cluster::GetNHitsInOuterLayer() const
{
    return m_nCaloHitsInOuterLayer;
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

inline int Cluster::GetParticleId() const
{
    return m_particleId;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsTrackSeeded() const
{
    return (nullptr != m_pTrackSeed);
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

inline const TrackList &Cluster::GetAssociatedTrackList() const
{
    return m_associatedTrackList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsAvailable() const
{
    return m_isAvailable;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::SetAvailability(bool isAvailable)
{
    m_isAvailable = isAvailable;
}

} // namespace pandora

#endif // #ifndef PANDORA_CLUSTER_H
