/**
 *  @file   LCContent/include/LCTopologicalAssociation/MipPhotonSeparationAlgorithm.h
 * 
 *  @brief  Header file for the mip-photon separation algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_MUON_PHOTON_SEPARATION_ALGORITHM_H
#define LC_MUON_PHOTON_SEPARATION_ALGORITHM_H 1

#include "LCTopologicalAssociation/MipPhotonSeparationAlgorithm.h"

namespace lc_content
{

/**
 *  @brief  MuonPhotonSeparationAlgorithm class
 */
class MuonPhotonSeparationAlgorithm : public MipPhotonSeparationAlgorithm
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
    MuonPhotonSeparationAlgorithm();

private:
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    pandora::StatusCode PerformFragmentation(pandora::Cluster *const pOriginalCluster, pandora::Track *const pTrack,
        unsigned int showerStartLayer, unsigned int showerEndLayer) const;

    pandora::StatusCode MakeClusterFragments(const unsigned int showerStartLayer, const unsigned int showerEndLayer,
        pandora::Cluster *const pOriginalCluster, pandora::Cluster *&pMipCluster, pandora::Cluster *&pPhotonCluster) const;

    float           m_highEnergyMuonCut;            ///< Cut for muon to be considered high energy
    unsigned int    m_nTransitionLayers;            ///< Number of transition layers, treated more flexibly, between shower and mip-region
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MuonPhotonSeparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new MuonPhotonSeparationAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_MUON_PHOTON_SEPARATION_ALGORITHM_H
