/**
 *  @file   LCContent/include/LCCheating/CheatingClusterCleaningAlgorithm.h
 * 
 *  @brief  Header file for the cheating cluster cleaning algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_CHEATING_CLUSTER_CLEANING_ALGORITHM_H
#define LC_CHEATING_CLUSTER_CLEANING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "Pandora/PandoraInternal.h"

namespace lc_content
{

/**
 *  @brief CheatingClusterCleaningAlgorithm class
 */
class CheatingClusterCleaningAlgorithm : public pandora::Algorithm
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

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CheatingClusterCleaningAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingClusterCleaningAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_CHEATING_CLUSTER_CLEANING_ALGORITHM_H
