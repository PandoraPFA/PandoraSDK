/**
 *  @file   LCContent/include/LCTrackClusterAssociation/TrackRecoveryInteractionsAlgorithm.h
 * 
 *  @brief  Header file for the track recovery interactions algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_TRACK_RECOVERY_INTERACTIONS_ALGORITHM_H
#define LC_TRACK_RECOVERY_INTERACTIONS_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  TrackRecoveryInteractionsAlgorithm class
 */
class TrackRecoveryInteractionsAlgorithm : public pandora::Algorithm
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
    TrackRecoveryInteractionsAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    float           m_maxTrackClusterDistance;          ///< Max distance between track and cluster to allow track-cluster association

    float           m_trackClusterDistanceCut;          ///< Must pass 1 / 3 cuts: max distance between track and cluster
    float           m_clusterEnergyCut;                 ///< Must pass 1 / 3 cuts: max cluster hadronic energy
    float           m_directionCosineCut;               ///< Must pass 1 / 3 cuts: min cos angle between track to calorimeter proj. and track to cluster

    float           m_maxTrackAssociationChi;           ///< Max value of track-cluster consistency chi to allow track-cluster association

    unsigned int    m_maxSearchLayer;                   ///< Max pseudo layer to examine when calculating track-cluster distance
    float           m_parallelDistanceCut;              ///< Max allowed projection of track-hit separation along track direction
    float           m_minTrackClusterCosAngle;          ///< Min cos(angle) between track and cluster initial direction
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TrackRecoveryInteractionsAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackRecoveryInteractionsAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_TRACK_RECOVERY_INTERACTIONS_ALGORITHM_H
