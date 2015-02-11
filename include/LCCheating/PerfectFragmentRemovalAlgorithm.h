/**
 *  @file   LCContent/include/LCCheating/PerfectFragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the perfect fragment removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_PERFECT_FRAGMENT_REMOVAL_ALGORITHM_H
#define LC_PERFECT_FRAGMENT_REMOVAL_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief PerfectFragmentRemovalAlgorithm class
 */
class PerfectFragmentRemovalAlgorithm : public pandora::Algorithm
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
    PerfectFragmentRemovalAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    typedef std::map<const pandora::MCParticle*, const pandora::Cluster*> MCParticleToClusterMap;

    bool    m_shouldMergeChargedClusters;   ///< Whether to merge charged clusters sharing same mc particle (otherwise use only highest E charged cluster)
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PerfectFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new PerfectFragmentRemovalAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_PERFECT_FRAGMENT_REMOVAL_ALGORITHM_H
