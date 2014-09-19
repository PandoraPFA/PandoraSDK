/**
 *  @file   LCContent/include/LCTopologicalAssociation/BackscatteredTracksAlgorithm.h
 * 
 *  @brief  Header file for the backscattered tracks algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_BACKSCATTERED_TRACKS_ALGORITHM_H
#define LC_BACKSCATTERED_TRACKS_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  BackscatteredTracksAlgorithm class
 */
class BackscatteredTracksAlgorithm : public pandora::Algorithm
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
    BackscatteredTracksAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    float           m_canMergeMinMipFraction;           ///< The min mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The max all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_minCaloHitsPerCluster;            ///< The min number of calo hits for cluster to be used as a daughter cluster
    float           m_fitToAllHitsRmsCut;               ///< The max rms value (for the fit to all hits) to use cluster as a daughter

    unsigned int    m_nOuterFitExclusionLayers;         ///< The number of outer layers to exclude from the daughter cluster fit
    unsigned int    m_nFitProjectionLayers;             ///< The number of layers to project daughter fit for comparison with parent clusters

    float           m_maxFitDistanceToClosestHit;       ///< Max distance between daughter cluster fit and hits in parent cluster
    float           m_maxCentroidDistance;              ///< Max value of closest layer centroid distance between parent/daughter clusters
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *BackscatteredTracksAlgorithm::Factory::CreateAlgorithm() const
{
    return new BackscatteredTracksAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_BACKSCATTERED_TRACKS_ALGORITHM_H
