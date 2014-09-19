/**
 *  @file   LCContent/include/LCTopologicalAssociation/ShowerMipMerging2Algorithm.h
 * 
 *  @brief  Header file for the shower mip merging 2 algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_SHOWER_MIP_MERGING_2_ALGORITHM_H
#define LC_SHOWER_MIP_MERGING_2_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  ShowerMipMerging2Algorithm class
 */
class ShowerMipMerging2Algorithm : public pandora::Algorithm
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
    ShowerMipMerging2Algorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    float           m_canMergeMinMipFraction;           ///< The min mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The max all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_minHitsInCluster;                 ///< Min number of calo hits in cluster
    unsigned int    m_minOccupiedLayersInCluster;       ///< Min number of occupied layers in cluster

    float           m_fitToAllHitsChi2Cut;              ///< The max all hit fit chi2 for cluster to be used as a parent cluster

    unsigned int    m_nPointsToFit;                     ///< The number of occupied pseudolayers to use in fit to the end of the cluster

    unsigned int    m_maxLayerDifference;               ///< The max difference (for merging) between cluster outer and inner pseudo layers
    float           m_maxCentroidDifference;            ///< The max difference (for merging) between cluster outer and inner centroids

    float           m_maxFitDirectionDotProduct;        ///< Max dot product between fit direction and centroid difference unit vectors

    float           m_perpendicularDistanceCutFine;     ///< Fine granularity cut on perp. distance between fit direction and centroid difference
    float           m_perpendicularDistanceCutCoarse;   ///< Coarse granularity cut on perp. distance between fit direction and centroid difference
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ShowerMipMerging2Algorithm::Factory::CreateAlgorithm() const
{
    return new ShowerMipMerging2Algorithm();
}

} // namespace lc_content

#endif // #ifndef LC_SHOWER_MIP_MERGING_2_ALGORITHM_H
