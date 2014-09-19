/**
 *  @file   LCContent/include/LCTopologicalAssociation/BackscatteredTracks2Algorithm.h
 * 
 *  @brief  Header file for the backscattered tracks 2 algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_BACKSCATTERED_TRACKS_2_ALGORITHM_H
#define LC_BACKSCATTERED_TRACKS_2_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  BackscatteredTracksAlgorithm class
 */
class BackscatteredTracks2Algorithm : public pandora::Algorithm
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
    BackscatteredTracks2Algorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    float           m_canMergeMinMipFraction;           ///< The min mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The max all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_minCaloHitsPerCluster;            ///< Min number of calo hits for cluster to be used as a parent cluster
    float           m_maxFitRms;                        ///< Max rms of fit (from inner to shower layer) for cluster to be used as a parent

    unsigned int    m_nFitProjectionLayers;             ///< The number of layers to project parent fit for comparison with daughter cluster
    float           m_maxFitDistanceToClosestHit;       ///< The max distance between projected parent fit and hits in daughter cluster

    float           m_maxCentroidDistance;              ///< Max value of closest layer centroid distance between parent/daughter clusters
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *BackscatteredTracks2Algorithm::Factory::CreateAlgorithm() const
{
    return new BackscatteredTracks2Algorithm();
}

} // namespace lc_content

#endif // #ifndef LC_BACKSCATTERED_TRACKS_2_ALGORITHM_H
