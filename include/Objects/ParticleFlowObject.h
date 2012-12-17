/**
 *  @file   PandoraPFANew/Framework/include/Objects/ParticleFlowObject.h
 * 
 *  @brief  Header file for the particle flow object class.
 * 
 *  $Log: $
 */
#ifndef PARTICLE_FLOW_OBJECT_H
#define PARTICLE_FLOW_OBJECT_H 1

#include "Api/PandoraContentApi.h"

namespace pandora
{

template<typename T> class AlgorithmObjectManager;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ParticleFlowObject class
 */
class ParticleFlowObject
{
public:
    /**
     *  @brief  Sort pfos by descending energy 
     * 
     *  @param  pLhs address of first pfo
     *  @param  pRhs address of second pfo
     */
    static bool SortByEnergy(const ParticleFlowObject *const pLhs, const ParticleFlowObject *const pRhs);

    /**
     *  @brief  Get the particle flow object id (PDG code)
     * 
     *  @return The particle flow object id
     */
    int GetParticleId() const;

    /**
     *  @brief  Get particle flow object charge
     * 
     *  @return The particle flow object charge
     */
    int GetCharge() const;

    /**
     *  @brief  Get particle flow object mass
     * 
     *  @return The particle flow object mass
     */
    float GetMass() const;

    /**
     *  @brief  Get the particle flow object energy
     * 
     *  @return The particle flow object energy
     */
    float GetEnergy() const;

    /**
     *  @brief  Get particle flow object momentum vector
     * 
     *  @return The particle flow object momentum vector
     */
    const CartesianVector &GetMomentum() const;

    /**
     *  @brief  Get the track list
     * 
     *  @return The track list
     */
    const TrackList &GetTrackList() const;

    /**
     *  @brief  Get the cluster list
     * 
     *  @return The cluster list
     */
    const ClusterList &GetClusterList() const;

    /**
     *  @brief  Get track address list
     * 
     *  @return The track address list
     */
    TrackAddressList GetTrackAddressList() const;

    /**
     *  @brief  Get the cluster address list
     * 
     *  @return The cluster address list
     */
    ClusterAddressList GetClusterAddressList() const;

    /**
     *  @brief  Get the number of tracks in the particle flow object
     * 
     *  @return The number of tracks in the particle flow object
     */
    unsigned int GetNTracks() const;

    /**
     *  @brief  Get the number of clusters in the particle flow object
     * 
     *  @return The number of clusters in the particle flow object
     */
    unsigned int GetNClusters() const;

    /**
     *  @brief  Set the particle flow object id (PDG code)
     * 
     *  @param  particleId the particle flow object id
     */
    void SetParticleId(const int particleId);

    /**
     *  @brief  Set particle flow object charge
     * 
     *  @param  charge particle flow object charge
     */
    void SetCharge(const int charge);

    /**
     *  @brief  Set particle flow object mass
     * 
     *  @param  mass particle flow object mass
     */
    void SetMass(const float mass);

    /**
     *  @brief  Set the particle flow object energy
     * 
     *  @param  energy particle flow object energy
     */
    void SetEnergy(const float energy);

    /**
     *  @brief  Set particle flow object momentum vector
     * 
     *  @param  momentum the particle flow object momentum vector
     */
    void SetMomentum(const CartesianVector &momentum);

private:
    /**
     *  @brief  Constructor
     */
    ParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters);

    /**
     *  @brief  Destructor
     */
    ~ParticleFlowObject();

    int                     m_particleId;               ///< The particle flow object id (PDG code)
    int                     m_charge;                   ///< The particle flow object charge
    float                   m_mass;                     ///< The particle flow object mass
    float                   m_energy;                   ///< The particle flow object energy
    CartesianVector         m_momentum;                 ///< The particle flow object momentum

    TrackList               m_trackList;                ///< The track list
    ClusterList             m_clusterList;              ///< The cluster list

    friend class ParticleFlowObjectManager;
    friend class AlgorithmObjectManager<ParticleFlowObject>;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ParticleFlowObject::SortByEnergy(const ParticleFlowObject *const pLhs, const ParticleFlowObject *const pRhs)
{
    return (pLhs->GetEnergy() > pRhs->GetEnergy());
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int ParticleFlowObject::GetParticleId() const
{
    return m_particleId;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int ParticleFlowObject::GetCharge() const
{
    return m_charge;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ParticleFlowObject::GetMass() const
{
    return m_mass;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ParticleFlowObject::GetEnergy() const
{
    return m_energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ParticleFlowObject::GetMomentum() const
{
    return m_momentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const TrackList &ParticleFlowObject::GetTrackList() const
{
    return m_trackList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const ClusterList &ParticleFlowObject::GetClusterList() const
{
    return m_clusterList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int ParticleFlowObject::GetNTracks() const
{
    return m_trackList.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int ParticleFlowObject::GetNClusters() const
{
    return m_clusterList.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ParticleFlowObject::SetParticleId(const int particleId)
{
    m_particleId = particleId;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ParticleFlowObject::SetCharge(const int charge)
{
    m_charge = charge;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ParticleFlowObject::SetMass(const float mass)
{
    m_mass = mass;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ParticleFlowObject::SetEnergy(const float energy)
{
    m_energy = energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ParticleFlowObject::SetMomentum(const CartesianVector &momentum)
{
    m_momentum = momentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline ParticleFlowObject::~ParticleFlowObject()
{
}

} // namespace pandora

#endif // #ifndef PARTICLE_FLOW_OBJECT_H
