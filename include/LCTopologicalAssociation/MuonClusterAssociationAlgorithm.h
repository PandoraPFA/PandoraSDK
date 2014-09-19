/**
 *  @file   LCContent/include/LCTopologicalAssociation/MuonClusterAssociationAlgorithm.h
 * 
 *  @brief  Header file for the muon cluster association algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_MUON_CLUSTER_ASSOCIATION_ALGORITHM_H
#define LC_MUON_CLUSTER_ASSOCIATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  MuonClusterAssociationAlgorithm class
 */
class MuonClusterAssociationAlgorithm : public pandora::Algorithm
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
    MuonClusterAssociationAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    std::string     m_muonClusterListName;              ///< The name of the muon cluster list

    float           m_dCosCut;                          ///< The direction cosine cut for association of muon and input (non-muon) clusters
    unsigned int    m_minHitsInMuonCluster;             ///< Minimum number of hits in a muon cluster

    bool            m_shouldEstimateEnergyLostInCoil;   ///< Whether to account for energy loss in coil
    float           m_coilCorrectionMinInnerRadius;     ///< Minimum muon cluster inner radius to apply coil energy loss correction
    unsigned int    m_coilCorrectionMinInnerLayerHits;  ///< Minimum number of inner layer hits to apply coil energy loss correction
    float           m_coilEnergyLossCorrection;         ///< Coil energy loss correction, units GeV

    float           m_minClusterHadronicEnergy;         ///< Minimum hadronic energy for an input (non-muon) cluster
    unsigned int    m_minHitsInCluster;                 ///< Minimum number of hits in an input (non-muon) cluster

    float           m_clusterAssociationChi;            ///< Track-cluster chi value used to identify compatible, track-associated clusters
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MuonClusterAssociationAlgorithm::Factory::CreateAlgorithm() const
{
    return new MuonClusterAssociationAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_MUON_CLUSTER_ASSOCIATION_ALGORITHM_H
