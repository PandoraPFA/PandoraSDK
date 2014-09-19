/**
 *  @file   LCContent/include/LCReclustering/ResolveTrackAssociationsAlg.h
 * 
 *  @brief  Header file for the resolve track associations algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_RESOLVE_TRACK_ASSOCIATIONS_ALGORITHM_H
#define LC_RESOLVE_TRACK_ASSOCIATIONS_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  ResolveTrackAssociationsAlg class
 */
class ResolveTrackAssociationsAlg : public pandora::Algorithm
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
    ResolveTrackAssociationsAlg();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    typedef std::vector<unsigned int> UIntVector;

    pandora::StringVector   m_clusteringAlgorithms;                 ///< The ordered list of clustering algorithms to be used
    std::string             m_associationAlgorithmName;             ///< The name of the topological association algorithm to run
    std::string             m_trackClusterAssociationAlgName;       ///< The name of the track-cluster association algorithm to run

    unsigned int            m_minTrackAssociations;                 ///< The min number of associated tracks required to start reclustering
    unsigned int            m_maxTrackAssociations;                 ///< The max number of associated tracks required to start reclustering
    float                   m_chiToAttemptReclustering;             ///< The min track/cluster chi value required to start reclustering
    float                   m_minChi2Improvement;                   ///< The min improvement in chi2 required to use reclustering results

    float                   m_coneCosineHalfAngle;                  ///< Angle of cone used to identify parent-daughter relationships
    float                   m_minConeFraction;                      ///< Min fraction of daughter hits found in cone along parent direction

    float                   m_minClusterEnergyForTrackAssociation;  ///< Energy threshold for recluster candidates with track associations
    float                   m_chi2ForAutomaticClusterSelection;     ///< Chi2 below which recluster candidates are automatically selected

    bool                    m_usingOrderedAlgorithms;               ///< Whether the clustering algorithms are used in a specified order
    float                   m_bestChi2ForReclusterHalt;             ///< If using ordered algorithms, halt if best chi2 below this value
    float                   m_currentChi2ForReclusterHalt;          ///< If using ordered algorithms, halt if current chi2 above this value

    bool                    m_shouldUseBestGuessCandidates;         ///< Whether to use "best guess" clusters if no ideal replacement found

    bool                    m_shouldUseForcedClustering;            ///< Whether to use a forced clustering algorithm if all else fails
    std::string             m_forcedClusteringAlgorithmName;        ///< The name of the forced clustering algorithm to run

    float                   m_minChiForForcedClustering;            ///< The min chi value required to use forced clustering
    float                   m_minForcedChi2Improvement;             ///< The min improvement in chi2 required to use forced clustering
    float                   m_maxForcedChi2;                        ///< The max allowed chi2 value to use forced clustering
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ResolveTrackAssociationsAlg::Factory::CreateAlgorithm() const
{
    return new ResolveTrackAssociationsAlg();
}

} // namespace lc_content

#endif // #ifndef LC_RESOLVE_TRACK_ASSOCIATIONS_ALGORITHM_H
