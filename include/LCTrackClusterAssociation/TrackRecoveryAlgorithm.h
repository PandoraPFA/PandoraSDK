/**
 *  @file   LCContent/include/LCTrackClusterAssociation/TrackRecoveryAlgorithm.h
 * 
 *  @brief  Header file for the track recovery algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_TRACK_RECOVERY_ALGORITHM_H
#define LC_TRACK_RECOVERY_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  TrackRecoveryAlgorithm class
 */
class TrackRecoveryAlgorithm : public pandora::Algorithm
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
    TrackRecoveryAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    float           m_maxTrackZStart;                   ///< Max track start z coordinate for track without parent to be considered
    float           m_maxAbsoluteTrackClusterChi;       ///< Max absolute value of track-cluster consistency chi for non-leaving cluster

    float           m_endCapMaxTrackClusterDistance1;   ///< Max track-cluster distance to allow association of endcap-reaching track
    float           m_endCapMaxTrackClusterDistance2;   ///< Max distance for association of endcap-reaching track with a cluster of lower energy
    float           m_barrelMaxTrackClusterDistance;    ///< Max track-cluster distance to allow association of barrel-reaching track

    unsigned int    m_maxSearchLayer;                   ///< Max pseudo layer to examine when calculating track-cluster distance
    float           m_parallelDistanceCut;              ///< Max allowed projection of track-hit separation along track direction
    float           m_minTrackClusterCosAngle;          ///< Min cos(angle) between track and cluster initial direction
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TrackRecoveryAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackRecoveryAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_TRACK_RECOVERY_ALGORITHM_H
