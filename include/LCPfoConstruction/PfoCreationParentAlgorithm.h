/**
 *  @file   LCContent/include/LCPfoConstruction/PfoCreationParentAlgorithm.h
 * 
 *  @brief  Header file for the pfo creation parent algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_PFO_CREATION_PARENT_ALGORITHM_H
#define LC_PFO_CREATION_PARENT_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  PfoCreationParentAlgorithm class
 */
class PfoCreationParentAlgorithm : public pandora::Algorithm
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
    PfoCreationParentAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    std::string     m_pfoCreationAlgorithmName;     ///< The name of the pfo creation algorithm to run

    std::string     m_inputClusterListName;         ///< The name of the input cluster list, containing the clusters for pfo creation
    bool            m_restoreOriginalClusterList;   ///< Whether to restore the original cluster list as the "current" list upon completion

    std::string     m_inputTrackListName;           ///< The name of the input track list, containing the tracks for pfo creation
    bool            m_restoreOriginalTrackList;     ///< Whether to restore the original track list as the "current" list upon completion

    std::string     m_pfoListName;                  ///< The name under which to save the new pfo list
    bool            m_replaceCurrentPfoList;        ///< Whether to subsequently use the new pfo list as the "current" list

    std::string     m_vertexListName;               ///< The name under which to save the new vertex list
    bool            m_replaceCurrentVertexList;     ///< Whether to subsequently use the new vertex list as the "current" list
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PfoCreationParentAlgorithm::Factory::CreateAlgorithm() const
{
    return new PfoCreationParentAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_PFO_CREATION_PARENT_ALGORITHM_H
