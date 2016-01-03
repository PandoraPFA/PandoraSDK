/**
 *  @file   LCContent/include/LCTopologicalAssociation/HighEnergyPhotonRecoveryAlgorithm.h
 * 
 *  @brief  Header file for the high energy photon recovery algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_HIGH_ENERGY_PHOTON_RECOVERY_ALGORITHM_H
#define LC_HIGH_ENERGY_PHOTON_RECOVERY_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace pandora { class ClusterFitResult; }

//------------------------------------------------------------------------------------------------------------------------------------------

namespace lc_content
{

/**
 *  @brief  HighEnergyPhotonRecoveryAlgorithm class
 */
class HighEnergyPhotonRecoveryAlgorithm : public pandora::Algorithm
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
    HighEnergyPhotonRecoveryAlgorithm();

private:

    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);
    
    typedef std::map<const pandora::ClusterList *, std::string> ClusterListToNameMap;
    typedef std::multimap<const pandora::Cluster *, const pandora::Cluster *> ClusterClusterMultiMap;
    typedef std::map<const pandora::Cluster *, const pandora::Cluster *> ClusterClusterMap;
    
    /**
     *  @brief  Get cluster list and cluster list to name map
     * 
     *  @param  clusterList to receive the cluster list
     *  @param  clusterListToNameMap to receive the cluster list to name map
     */
    pandora::StatusCode GetClusterListAndNameMap(pandora::ClusterList &clusterList, ClusterListToNameMap &clusterListToNameMap) const;
    
    /**
     *  @brief  Prepare clusters for the high energy photon recovery algorithm, applying pre-selection cuts 
     * 
     *  @param  daughterVector to receive the daughter cluster vector
     *  @param  parentVector to receive the parent cluster vector
     */
    pandora::StatusCode PrepareClusters(const pandora::ClusterList &clusterList, pandora::ClusterVector &daughterVector, pandora::ClusterVector &parentVector) const;

    /**
     *  @brief  Pre select clusters for the high energy photon recovery algorithm, applying pre-selection cuts 
     * 
     *  @param  daughterVector daughter cluster vector
     *  @param  parentVector the parent cluster vector
     *  @param  parentCandidateMultiMap to receive the parent candidate cluster multi map
     */
    pandora::StatusCode PreSelectClusters(const pandora::ClusterVector &daughterVector, const pandora::ClusterVector &parentVector, ClusterClusterMultiMap &parentCandidateMultiMap) const;

    /**
     *  @brief  Merge daughters to the best parent
     * 
     *  @param  daughterBestParentMap daughter to best parent map
     *  @param  clusterListToNameMap the cluster list to name map
     */
    pandora::StatusCode MergeClusters(const ClusterClusterMap &daughterBestParentMap, const ClusterListToNameMap &clusterListToNameMap) const;

    /**
     *  @brief  Get the fraction of hits in a daughter candidate cluster that are contained in a cluster defined by a fit
     *          to the parent candidate cluster
     * 
     *  @param  pParentCluster address of the parent candidate cluster
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  parentMipFitResult the mip fit result for the parent candidate cluster
     * 
     *  @return the fraction of the daughter cluster hits contained in the cone
     */
    float GetFractionInCone(const pandora::Cluster *const pParentCluster, const pandora::Cluster *const pDaughterCluster,
        const pandora::ClusterFitResult &parentMipFitResult) const;
        
    /**
     *  @brief  Get the name of the cluster list in which a specified cluster can be found
     * 
     *  @param  pCluster address of the cluster
     *  @param  clusterListToNameMap the cluster list to name map
     *  @param  listName to receive the list name
     */
    pandora::StatusCode GetClusterListName(const pandora::Cluster *const pCluster, const ClusterListToNameMap &clusterListToNameMap,
        std::string &listName) const;
        
    /**
     *  @brief  Get Hadronic energy weighted distance squared of all calo hits to a line of the the fit of 2nd cluster
     * 
     *  @param  pCluster address of the cluster
     *  @param  clusterFitResult 2nd cluster fit result
     * 
     *  @return Hadronic energy weighted distance squared of all calo hits to a line
     */
    float GetHadEnergyWeightedDistance2ToLine(const pandora::Cluster *const pCluster, const pandora::ClusterFitResult &clusterFitResult) const;
    
    /**
     *  @brief  Get Hadronic energy weighted distance squared of all calo hits in a layer to a line of the the fit of 2nd cluster
     * 
     *  @param  pCluster address of the cluster
     *  @param  clusterFitResult 2nd cluster fit result
     *  @param  pseudoLayer pseudoLayer of the cluster to consider
     * 
     *  @return Hadronic energy weighted distance squared of all calo hits to a line
     */
    float GetHadEnergyWeightedDistance2ToLine(const pandora::Cluster *const pCluster, const pandora::ClusterFitResult &clusterFitResult,
        unsigned int pseudoLayer) const;

    /**
     *  @brief  Get distance squared from a point to a line
     * 
     *  @param  point position of the point
     *  @param  direction direction of the line
     *  @param  intercept intercept of the line
     * 
     *  @return the distance squared from a point to a line
     */
    float GetDistance2ToLine(const pandora::CartesianVector &point, const pandora::CartesianVector &direction,
        const pandora::CartesianVector &intercept ) const;
    
    std::string     m_trackClusterAssociationAlgName;               ///< The name of the track-cluster association algorithm to run
    bool            m_shouldUseCurrentClusterList;          ///< Whether to use clusters from the current list in the algorithm
    bool            m_updateCurrentTrackClusterAssociations;///< Whether to update track-cluster associations for current list
    pandora::StringVector   m_additionalClusterListNames;           ///< Additional cluster lists from which to consider clusters
    
    int             m_numberContactLayers;                          ///< Number of layers to define contact layers
    float           m_centroidDistance2Cut;                         ///< Centroid distance squared cut for the distance between centroid of first layer of daughter and last layer of parent
    float           m_fullClosestApproachCut;                       ///< The cut for small closest approach for full fit of daughter and parent
    float           m_contactClosestApproachCut;                    ///< The cut for small closest approach for fit using 2 layers of daughter and parent
    float           m_minRmsRatioCut;                               ///< The min ratio of rms of fit using 2 layers of daughter and parent
    float           m_maxRmsRatioCut;                               ///< The max ratio of rms of fit using 2 layers of daughter and parent
    float           m_daughterDistance2ToParentFitCut;              ///< The cut for Hadronic energy weighted calo hit distance to the parent fit result
    float           m_energyRatioCut;                               ///< The hadronic energy of daughter to the EM energy of parent ratio cut
    float           m_fractionInConeCut;                            ///< Min faction of daughter in cone of the parent

    float           m_coneCosineHalfAngle;                          ///< Cosine of cone half angle
    float           m_minCosConeAngleWrtRadial;                     ///< Min cosine of angle between cone and radial direction
    float           m_cosConeAngleWrtRadialCut1;                    ///< 1st pair of cuts: Min cosine of angle between cone and radial direction
    float           m_minHitSeparationCut1;                         ///< 1st pair of cuts: Max separation between cone vertex and daughter cluster hit
    float           m_cosConeAngleWrtRadialCut2;                    ///< 2nd pair of cuts: Min cosine of angle between cone and radial direction
    float           m_minHitSeparationCut2;                         ///< 2nd pair of cuts: Max separation between cone vertex and daughter cluster hit
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *HighEnergyPhotonRecoveryAlgorithm::Factory::CreateAlgorithm() const
{
    return new HighEnergyPhotonRecoveryAlgorithm();
}

//------------------------------------------------------------------------------------------------------------------------------------------

} // namespace lc_content

#endif // #ifndef LC_HIGH_ENERGY_PHOTON_RECOVERY_ALGORITHM_H
