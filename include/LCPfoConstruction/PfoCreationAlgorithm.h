/**
 *  @file   LCContent/include/LCPfoConstruction/PfoCreationAlgorithm.h
 * 
 *  @brief  Header file for the pfo creation algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_PFO_CREATION_ALGORITHM_H
#define LC_PFO_CREATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "Api/PandoraContentApi.h"

namespace lc_content
{

/**
 *  @brief  PfoCreationAlgorithm class
 */
class PfoCreationAlgorithm : public pandora::Algorithm
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
    PfoCreationAlgorithm();

private:
    typedef PandoraContentApi::ParticleFlowObject::Parameters PfoParameters;

    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Create particle flow objects starting from tracks in the main tracker. The pfos will account for associated
     *          daughter/sibling tracks and associated calorimeter clusters.
     */
    pandora::StatusCode CreateTrackBasedPfos() const;

    /**
     *  @brief  Add relevant tracks and clusters to a track-based pfo
     * 
     *  @param  pTrack address of the track to consider
     *  @param  pfoParameters the pfo parameters to populate
     *  @param  readSiblingInfo whether to read sibling track information (set to false to avoid multiple counting)
     */
    pandora::StatusCode PopulateTrackBasedPfo(const pandora::Track *const pTrack, PfoParameters &pfoParameters, const bool readSiblingInfo = true) const;

    /**
     *  @brief  Set the basic parameters for a track-based pfo
     * 
     *  @param  pTrack address of the track to consider
     *  @param  pfoParameters the pfo parameters to populate
     */
    pandora::StatusCode SetTrackBasedPfoParameters(const pandora::Track *const pTrack, PfoParameters &pfoParameters) const;

    /**
     *  @brief  Set the parameters for a track-based pfo, where pfo target consists of two or more sibling tracks
     * 
     *  @param  pTrack address of the track to consider
     *  @param  pfoParameters the pfo parameters to populate
     */
    pandora::StatusCode SetSiblingTrackBasedPfoParameters(const pandora::Track *const pTrack, PfoParameters &pfoParameters) const;

    /**
     *  @brief  Set the parameters for a track-based pfo, where the pfo target has one or more daughter tracks
     * 
     *  @param  pTrack address of the track to consider
     *  @param  pfoParameters the pfo parameters to populate
     */
    pandora::StatusCode SetDaughterTrackBasedPfoParameters(const pandora::Track *const pTrack, PfoParameters &pfoParameters) const;

    /**
     *  @brief  Set the parameters for a simple track-based pfo, where the track has no associations with other tracks
     * 
     *  @param  pTrack address of the track to consider
     *  @param  pfoParameters the pfo parameters to populate
     */
    pandora::StatusCode SetSimpleTrackBasedPfoParameters(const pandora::Track *const pTrack, PfoParameters &pfoParameters) const;

    /**
     *  @brief  Create particle flow objects corresponding to neutral particles, These pfos consist only of clusters that have no
     *          associated tracks.
     */
    pandora::StatusCode CreateNeutralPfos() const;

    /**
     *  @brief  Get the energy-weighted centroid for a specified cluster, calculated over a particular pseudo layer range
     * 
     *  @param  pCluster address of the cluster to consider
     *  @param  pseudoLayer the inner pseudo layer of interest
     *  @param  pseudoLayer the outer pseudo layer of interest
     * 
     *  @return The energy-weighted centroid, returned by value
     */
    const pandora::CartesianVector GetEnergyWeightedCentroid(const pandora::Cluster *const pCluster, const unsigned int innerPseudoLayer,
        const unsigned int outerPseudoLayer) const;

    std::string     m_outputPfoListName;                    ///< The output pfo list name
    bool            m_shouldCreateTrackBasedPfos;           ///< Whether to create track-based (charged) pfos
    bool            m_shouldCreateNeutralPfos;              ///< Whether to create neutral pfos
    float           m_minClusterHadronicEnergy;             ///< Min hadronic energy for neutral (non-photon) clusters to be added to pfos
    float           m_minClusterElectromagneticEnergy;      ///< Min electromagnetic energy for neutral (photon) clusters to be added to pfos
    unsigned int    m_minHitsInCluster;                     ///< Min number of calo hits for neutral cluster to be added to pfos
    bool            m_allowSingleLayerClusters;             ///< Whether to allow neutral clusters spanning only a single pseudolayer
    unsigned int    m_photonPositionAlgorithm;              ///< Identifies the algorithm used to calculate photon pfo position vectors
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PfoCreationAlgorithm::Factory::CreateAlgorithm() const
{
    return new PfoCreationAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_PFO_CREATION_ALGORITHM_H
