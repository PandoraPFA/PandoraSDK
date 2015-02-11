/**
 *  @file   LCContent/include/LCTopologicalAssociation/MipPhotonSeparationAlgorithm.h
 * 
 *  @brief  Header file for the mip-photon separation algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_MIP_PHOTON_SEPARATION_ALGORITHM_H
#define LC_MIP_PHOTON_SEPARATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  MipPhotonSeparationAlgorithm class
 */
class MipPhotonSeparationAlgorithm : public pandora::Algorithm
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
    MipPhotonSeparationAlgorithm();

protected:
    virtual pandora::StatusCode Run();
    virtual pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Whether to attempt to fragment a cluster into a mip-like part and a photon-like part
     * 
     *  @param  pCluster address of the cluster
     *  @param  pTrack address of the associated track
     *  @param  showerStartLayer to receive the shower start layer for the cluster
     *  @param  showerEndLayer to receive the shower end layer for the cluster
     * 
     *  @return boolean
     */
    virtual bool ShouldFragmentCluster(const pandora::Cluster *const pCluster, const pandora::Track *const pTrack,
        unsigned int &showerStartLayer, unsigned int &showerEndLayer) const;

    /**
     *  @brief  Perform cluster fragmentation operations to separate cluster into mip-like and photon-like sections
     * 
     *  @param  pOriginalCluster address of the cluster to fragment
     *  @param  pTrack address of the associated track
     *  @param  showerStartLayer the shower start layer
     *  @param  showerEndLayer the shower end layer
     */
    virtual pandora::StatusCode PerformFragmentation(const pandora::Cluster *const pOriginalCluster, const pandora::Track *const pTrack,
        unsigned int showerStartLayer, unsigned int showerEndLayer) const;

    /**
     *  @brief  Make mip-like and photon-like fragments from a cluster
     * 
     *  @param  showerStartLayer the shower start layer
     *  @param  showerEndLayer the shower end layer
     *  @param  pOriginalCluster address of the original cluster
     *  @param  pMipCluster to receive the address of the mip-like cluster fragment
     *  @param  pPhotonCluster to receive the address of the photon-like cluster fragment
     */
    virtual pandora::StatusCode MakeClusterFragments(const unsigned int showerStartLayer, const unsigned int showerEndLayer,
        const pandora::Cluster *const pOriginalCluster, const pandora::Cluster *&pMipCluster, const pandora::Cluster *&pPhotonCluster) const;

    /**
     *  @brief  Get the distance between a calo hit and the track seed (projected) position at the calorimeter surface
     * 
     *  @param  pCluster address of the cluster
     *  @param  pTrack address of the track
     *  @param  pCaloHit address of the calo hit
     *  @param  distance to receive the distance
     */
    virtual pandora::StatusCode GetDistanceToTrack(const pandora::Cluster *const pCluster, const pandora::Track *const pTrack,
        const pandora::CaloHit *const pCaloHit, float &distance) const;

    std::string     m_trackClusterAssociationAlgName;///< The name of the track-cluster association algorithm to run

    unsigned int    m_nLayersForMipRegion;          ///< To identify mip region: number of layers with mip hit, but no shower hit
    unsigned int    m_nLayersForShowerRegion;       ///< To identify shower region: number of layers with shower hit, but no mip hit
    unsigned int    m_maxLayersMissed;              ///< Maximum number of successive layers in which no track hit is found

    unsigned int    m_minMipRegion2Span;            ///< Minimum number of layers spanned by mip 2 region to allow fragmentation
    unsigned int    m_maxShowerStartLayer;          ///< 1st pair of cuts: Maximum shower start layer to allow fragmentation
    unsigned int    m_minShowerRegionSpan;          ///< 1st pair of cuts: Minimum layers spanned by shower region to allow fragmentation
    unsigned int    m_maxShowerStartLayer2;         ///< 2nd pair of cuts: Maximum shower start layer to allow fragmentation
    unsigned int    m_minShowerRegionSpan2;         ///< 2nd pair of cuts: Minimum layers spanned by shower region to allow fragmentation

    float           m_nonPhotonDeltaChi2Cut;        ///< Delta chi2 cut for case when photon cluster fragment fails photon id
    float           m_photonDeltaChi2Cut;           ///< Delta chi2 cut for case when photon cluster fragment passes photon id
    unsigned int    m_minHitsInPhotonCluster;       ///< Minimum number of hit in photon cluster fragment

    float           m_genericDistanceCut;           ///< Generic distance cut to apply throughout algorithm
    float           m_trackPathWidth;               ///< Track path width, used to determine whether hits are associated with seed track
    float           m_maxTrackSeparation2;          ///< Maximum distance between a calo hit and track seed (squared)
    float           m_additionalPadWidthsFine;      ///< Fine granularity adjacent pad widths used to calculate cone approach distance
    float           m_additionalPadWidthsCoarse;    ///< Coarse granularity adjacent pad widths used to calculate cone approach distance
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MipPhotonSeparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new MipPhotonSeparationAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_MIP_PHOTON_SEPARATION_ALGORITHM_H
