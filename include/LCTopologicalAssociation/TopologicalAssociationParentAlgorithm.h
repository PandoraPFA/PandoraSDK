/**
 *  @file   LCContent/include/LCTopologicalAssociation/TopologicalAssociationParentAlgorithm.h
 * 
 *  @brief  Header file for the topological association parent algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_TOPOLOGICAL_ASSOCIATION_PARENT_ALGORITHM_H
#define LC_TOPOLOGICAL_ASSOCIATION_PARENT_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  TemplateAlgorithm class
 */
class TopologicalAssociationParentAlgorithm : public pandora::Algorithm
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

    pandora::StringVector   m_associationAlgorithms;    ///< The ordered list of topological association algorithms to be used
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TopologicalAssociationParentAlgorithm::Factory::CreateAlgorithm() const
{
    return new TopologicalAssociationParentAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_TOPOLOGICAL_ASSOCIATION_PARENT_ALGORITHM_H
