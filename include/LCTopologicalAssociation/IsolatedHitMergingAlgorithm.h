/**
 *  @file   LCContent/include/LCTopologicalAssociation/IsolatedHitMergingAlgorithm.h
 * 
 *  @brief  Header file for the isolated hit merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_ISOLATED_HIT_MERGING_ALGORITHM_H
#define LC_ISOLATED_HIT_MERGING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  IsolatedHitMergingAlgorithm class
 */
class IsolatedHitMergingAlgorithm : public pandora::Algorithm
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
    IsolatedHitMergingAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Get closest distance between a specified calo hit and a non-isolated hit in a specified cluster
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     * 
     *  @return The closest distance between the calo hit and a non-isolated hit in the cluster
     */
    float GetDistanceToHit(const pandora::Cluster *const pCluster, const pandora::CaloHit *const pCaloHit) const;

    bool                    m_shouldUseCurrentClusterList;  ///< Whether to use clusters from the current list in the algorithm
    pandora::StringVector   m_additionalClusterListNames;   ///< Additional cluster lists from which to consider clusters

    unsigned int            m_minHitsInCluster;             ///< Min number of hits allowed in a cluster - smaller clusters will be split up
    float                   m_maxRecombinationDistance;     ///< Max distance between calo hit and cluster to allow addition of hit
    float                   m_minCosOpeningAngle;           ///< Min cos(angle) between hit and cluster directions to allow addition of hit
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *IsolatedHitMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new IsolatedHitMergingAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_ISOLATED_HIT_MERGING_ALGORITHM_H
