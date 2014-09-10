/**
 *  @file   LCContent/include/LCReclustering/ReclusteringParentAlgorithm.h
 * 
 *  @brief  Header file for the reclustering parent algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_RECLUSTERING_PARENT_ALGORITHM_H
#define LC_RECLUSTERING_PARENT_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  ReclusteringParentAlgorithm class
 */
class ReclusteringParentAlgorithm : public pandora::Algorithm
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

    pandora::StringVector   m_ReclusteringParentAlgorithms;       ///< The ordered list of reclustering algorithms to be used
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ReclusteringParentAlgorithm::Factory::CreateAlgorithm() const
{
    return new ReclusteringParentAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_RECLUSTERING_PARENT_ALGORITHM_H
