/**
 *  @file   LCContent/include/LCTrackClusterAssociation/LoopingTrackAssociationAlgorithm.h
 * 
 *  @brief  Header file for the looping track association algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_LOOPING_TRACK_ASSOCIATION_ALGORITHM_H
#define LC_LOOPING_TRACK_ASSOCIATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  LoopingTrackAssociationAlgorithm class
 */
class LoopingTrackAssociationAlgorithm : public pandora::Algorithm
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
    LoopingTrackAssociationAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Get mean distance of hits in first m_nClusterDeltaRLayers layers of cluster from centre of specified helix
     * 
     *  @param  pCluster address of the cluster
     *  @param  helixXCentre x coordinate of helix centre
     *  @param  helixYCentre y coordinate of helix centre
     *  @param  helixRadius radius of the helix
     */
    float GetMeanDeltaR(pandora::Cluster *const pCluster, const float helixXCentre, const float helixYCentre, const float helixRadius) const;

    float           m_maxTrackClusterDeltaZ;            ///< Max z separation between track calorimeter projection and cluster start

    unsigned int    m_minHitsInCluster;                 ///< Min number of calo hits in cluster
    unsigned int    m_minOccupiedLayersInCluster;       ///< Min number of occupied layers in cluster
    unsigned int    m_maxClusterInnerLayer;             ///< Max cluster inner pseudo layer to allow association
    float           m_maxAbsoluteTrackClusterChi;       ///< Max absolute track-cluster consistency chi value to allow association

    float           m_maxDeltaR;                        ///< Max separation between cluster and centre of track helix
    float           m_minDeltaR;                        ///< Min separation between cluster and centre of track helix

    unsigned int    m_nClusterFitLayers;                ///< Number of cluster layers to use in fit to initial cluster direction
    unsigned int    m_nClusterDeltaRLayers;             ///< Number of cluster layers to use in calculation of cluster meanDeltaR

    float           m_directionCosineCut;               ///< Pair of cuts: min direction cosine between track and cluster directions
    float           m_clusterMipFractionCut;            ///< Pair of cuts: cluster min mip fraction

    float           m_directionCosineCut1;              ///< Selection parameter: min direction cosine between track and cluster directions

    float           m_directionCosineCut2;              ///< Pair of selection params: min direction cosine between track/cluster directions
    float           m_deltaRCut2;                       ///< Pair of selection params: max absolute distance between cluster and helix centre

    float           m_directionCosineCut3;              ///< Pair of selection params: min direction cosine between track/cluster directions
    float           m_deltaRCut3;                       ///< Pair of selection params: max absolute distance between cluster and helix centre

    float           m_directionCosineCut4;              ///< Pair of selection params: min direction cosine between track/cluster directions
    float           m_deltaRCut4;                       ///< Pair of selection params: max absolute distance between cluster and helix centre
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *LoopingTrackAssociationAlgorithm::Factory::CreateAlgorithm() const
{
    return new LoopingTrackAssociationAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_LOOPING_TRACK_ASSOCIATION_ALGORITHM_H
