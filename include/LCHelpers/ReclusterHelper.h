/**
 *  @file   LCContent/include/LCHelpers/ReclusterHelper.h
 * 
 *  @brief  Header file for the recluster helper class.
 * 
 *  $Log: $
 */
#ifndef LC_RECLUSTER_HELPER_H
#define LC_RECLUSTER_HELPER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

namespace lc_content
{

/**
 *  @brief  ReclusterResult class
 */
class ReclusterResult
{
public:
    /**
     *  @brief  Get the total chi value for the suitability of all track/cluster associations
     * 
     *  @return the total chi value
     */
    float GetChi() const;

    /**
     *  @brief  Get the total chi2 value for the suitability of all track/cluster associations
     * 
     *  @return the total chi2 value
     */
    float GetChi2() const;

    /**
     *  @brief  Get the chi per degree of freedom value for suitability of track/cluster associations
     * 
     *  @return the chi per degree of freedom value
     */
    float GetChiPerDof() const;

    /**
     *  @brief  Get the chi2 per degree of freedom value for suitability of track/cluster associations
     * 
     *  @return the chi2 per degree of freedom value
     */
    float GetChi2PerDof() const;

    /**
     *  @brief  Get the total hadronic energy in clusters that have no track associations
     * 
     *  @return the total hadronic energy in clusters that have no track associations
     */
    float GetUnassociatedEnergy() const;

    /**
     *  @brief  Get the minimum energy of a cluster associated with a track
     * 
     *  @return the minimum energy of a cluster associated with a track
     */
    float GetMinTrackAssociationEnergy() const;

    /**
     *  @brief  Get the number of excess track-cluster associations
     * 
     *  @return the number of excess track-cluster associations
     */
    unsigned int GetNExcessTrackAssociations() const;

    /**
     *  @brief  Set the total chi value for the suitability of all track/cluster associations
     * 
     *  @param  chi the total chi value
     */
    void SetChi(float chi);

    /**
     *  @brief  Set the total chi2 value for the suitability of all track/cluster associations
     * 
     *  @param  chi the total chi2 value
     */
    void SetChi2(float chi2);

    /**
     *  @brief  Set the chi per degree of freedom value for suitability of track/cluster associations
     * 
     *  @param  chiPerDof the chi per degree of freedom value
     */
    void SetChiPerDof(float chiPerDof);

    /**
     *  @brief  Set the chi2 per degree of freedom value for suitability of track/cluster associations
     * 
     *  @param  chi2PerDof the chi2 per degree of freedom value
     */
    void SetChi2PerDof(float chi2PerDof);

    /**
     *  @brief  Set the total hadronic energy in clusters that have no track associations
     * 
     *  @param  unassociatedEnergy the total hadronic energy in clusters that have no track associations
     */
    void SetUnassociatedEnergy(float unassociatedEnergy);

    /**
     *  @brief  Set the minimum energy of a cluster associated with a track
     * 
     *  @param  minTrackAssociationEnergy the minimum energy of a cluster associated with a track
     */
    void SetMinTrackAssociationEnergy(float minTrackAssociationEnergy);

    /**
     *  @brief  Set the number of excess track-cluster associations
     * 
     *  @param  nExcessTrackAssociations the number of excess track-cluster associations
     */
    void SetNExcessTrackAssociations(unsigned int nExcessTrackAssociations);

private:
    pandora::InputFloat     m_chi;                          ///< Total chi value for the suitability of all track/cluster associations
    pandora::InputFloat     m_chi2;                         ///< Total chi2 value for the suitability of all track/cluster associations
    pandora::InputFloat     m_chiPerDof;                    ///< Chi per degree of freedom value for suitability of track/cluster associations
    pandora::InputFloat     m_chi2PerDof;                   ///< Chi2 per degree of freedom value for suitability of track/cluster associations
    pandora::InputFloat     m_unassociatedEnergy;           ///< Total hadronic energy in clusters that have no track associations
    pandora::InputFloat     m_minTrackAssociationEnergy;    ///< Minimum energy of a cluster associated with a track
    pandora::InputUInt      m_nExcessTrackAssociations;     ///< Number of excess track-cluster associations
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ReclusterHelper class
 */
class ReclusterHelper
{
public:
    /**
     *  @brief  Evaluate the compatibility of a cluster with its associated tracks. Reclustering can be used to split up a
     *          cluster and produce more favourable track/cluster matches.
     *
     *  @param  pandora the associated pandora instance
     *  @param  pCluster address of the cluster
     *  @param  trackList address of the list of tracks associated with the cluster
     * 
     *  @return the chi value for the suitability of the track/cluster associations
     */
    static float GetTrackClusterCompatibility(const pandora::Pandora &pandora, const pandora::Cluster *const pCluster,
        const pandora::TrackList &trackList);

    /**
     *  @brief  Evaluate the compatibility of a cluster with its associated tracks. Reclustering can be used to split up a
     *          cluster and produce more favourable track/cluster matches.
     * 
     *  @param  pandora the associated pandora instance
     *  @param  clusterEnergy the cluster energy
     *  @param  trackEnergy the sum of the energies of the associated tracks
     * 
     *  @return the chi value for the suitability of the track/cluster associations
     */
    static float GetTrackClusterCompatibility(const pandora::Pandora &pandora, const float clusterEnergy, const float trackEnergy);

    /**
     *  @brief  Extract recluster results, indicating suitability of recluster candidates as a replacement for the original clusters
     * 
     *  @param  pandora the associated pandora instance
     *  @param  pReclusterCandidatesList address of the recluster candidates list
     *  @param  reclusterResult to receive the recluster results
     */
    static pandora::StatusCode ExtractReclusterResults(const pandora::Pandora &pandora, const pandora::ClusterList *const pReclusterCandidatesList,
        ReclusterResult &reclusterResult);
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetChi() const
{
    return m_chi.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetChi2() const
{
    return m_chi2.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetChiPerDof() const
{
    return m_chiPerDof.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetChi2PerDof() const
{
    return m_chi2PerDof.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetUnassociatedEnergy() const
{
    return m_unassociatedEnergy.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterResult::GetMinTrackAssociationEnergy() const
{
    return m_minTrackAssociationEnergy.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int ReclusterResult::GetNExcessTrackAssociations() const
{
    return m_nExcessTrackAssociations.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetChi(float chi)
{
    if (!(m_chi = chi))
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetChi2(float chi2)
{
    if (!(m_chi2 = chi2))
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetChiPerDof(float chiPerDof)
{
    if (!(m_chiPerDof = chiPerDof))
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetChi2PerDof(float chi2PerDof)
{
    if (!(m_chi2PerDof = chi2PerDof))
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetUnassociatedEnergy(float unassociatedEnergy)
{
    if (!(m_unassociatedEnergy = unassociatedEnergy))
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetMinTrackAssociationEnergy(float minTrackAssociationEnergy)
{
    if (!(m_minTrackAssociationEnergy = minTrackAssociationEnergy))
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterResult::SetNExcessTrackAssociations(unsigned int nExcessTrackAssociations)
{
    if (!(m_nExcessTrackAssociations = nExcessTrackAssociations))
        throw pandora::StatusCodeException(pandora::STATUS_CODE_INVALID_PARAMETER);
}

} // namespace lc_content

#endif // #ifndef LC_RECLUSTER_HELPER_H
