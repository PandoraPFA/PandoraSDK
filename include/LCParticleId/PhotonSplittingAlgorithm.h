/**
 *  @file   LCContent/include/LCParticleId/PhotonSplittingAlgorithm.h
 * 
 *  @brief  Header file for the photon splitting algorithm class.
 * 
 *  $Log: $
 */
#ifndef PHOTON_SPLITTING_ALGORITHM_H
#define PHOTON_SPLITTING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content{

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PhotonSplitter class
 */
class PhotonSplittingAlgorithm : public pandora::Algorithm
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
     *  @brief  Default constructor
     */
    PhotonSplittingAlgorithm();
    /**
     *  @brief  Destructor
     */
    ~PhotonSplittingAlgorithm();
    
    
    
private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    unsigned int            m_maxSearchLayer;                        ///< Max pseudo layer to examine when calculating track-cluster distance
    float                   m_parallelDistanceCut;                   ///< Max allowed projection of track-hit separation along track direction
    float                   m_minTrackClusterCosAngle;               ///< Min cos(angle) between track and cluster initial direction
    float                   m_maxDistanceToTrackCut;                 ///< Minimum distance to track to separate clusters close to track or not
    bool                    m_transProfileEcalOnly;                  ///< Transverse profile shower calculator uses EcalOnly. Can be overridden by the m_transProfileMaxLayer
    unsigned int            m_transProfileMaxLayer;                  ///< Maximum layer to consider in calculation of shower transverse profiles
    float                   m_minClusterEnergy1;                     ///< Minimum cluster energy for cluster far from tracks
    float                   m_minDaughterEnergy1;                    ///< Minimum daughter cluster energy for cluster far from tracks
    float                   m_minClusterEnergy2;                     ///< Minimum cluster energy for cluster close to 1 track
    float                   m_minDaughterEnergy2;                    ///< Minimum daughter cluster energy for cluster close to 1 track
    float                   m_minClusterEnergy3;                     ///< Minimum cluster energy for cluster close to more than 1 track
    float                   m_minDaughterEnergy3;                    ///< Minimum daughter cluster energy for cluster to more than 1 track
    
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonSplittingAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonSplittingAlgorithm();
}

#endif // #ifndef PHOTON_SPLITTING_ALGORITHM_H
}