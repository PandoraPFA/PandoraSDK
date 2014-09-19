/**
 *  @file   LCContent/include/LCTopologicalAssociation/ShowerMipMerging3Algorithm.h
 * 
 *  @brief  Header file for the shower mip merging 3 algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_SHOWER_MIP_MERGING_3_ALGORITHM_H
#define LC_SHOWER_MIP_MERGING_3_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  ShowerMipMerging3Algorithm class
 */
class ShowerMipMerging3Algorithm : public pandora::Algorithm
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
    ShowerMipMerging3Algorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    float           m_canMergeMinMipFraction;           ///< The min mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The max all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_minCaloHitsInDaughter;            ///< The min number of calo hits for cluster to be used as a daughter cluster
    unsigned int    m_minOccupiedLayersInDaughter;      ///< The min number of occupied layers for cluster to be used as a daughter cluster
    unsigned int    m_minCaloHitsInParent;              ///< The min number of calo hits for cluster to be used as a parent cluster

    unsigned int    m_nPointsToFit;                     ///< The number of occupied pseudolayers to use in fit to end of daughter cluster
    float           m_maxFitChi2;                       ///< The max chi2 value for cluster to be used as a daughter cluster

    unsigned int    m_nFitProjectionLayers;             ///< The number of layers to project cluster fit for comparison with second cluster
    float           m_maxFitDistanceToClosestHit;       ///< The max distance between projected cluster fit and hits in second cluster

    float           m_maxClusterApproach;               ///< The max value of smallest hit separation between parent and daughter clusters
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ShowerMipMerging3Algorithm::Factory::CreateAlgorithm() const
{
    return new ShowerMipMerging3Algorithm();
}

} // namespace lc_content

#endif // #ifndef LC_SHOWER_MIP_MERGING_3_ALGORITHM_H
