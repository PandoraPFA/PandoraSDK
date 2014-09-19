/**
 *  @file   LCContent/include/LCPfoConstruction/CLICPfoSelectionAlgorithm.h
 * 
 *  @brief  Header file for an algorithm to select PFOs for CLIC physics studies
 * 
 *  $Log: $
 */
#ifndef LC_CLIC_PFO_SELECTION_ALGORITHM_H
#define LC_CLIC_PFO_SELECTION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  CLICPfoSelectionAlgorithm class
 */
class CLICPfoSelectionAlgorithm : public pandora::Algorithm
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
    CLICPfoSelectionAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Extract energy weighted mean times from the hits in a cluster. Separate times are also extracted for ecal and
     *          hcal endcap regions, alongside the number of hits in these regions.
     * 
     *  @param  pCluster address pf the cluster
     *  @param  meanTime to receive the energy weighted mean time
     *  @param  meanTimeECal to receive the energy weighted mean time for ecal hits
     *  @param  nECalHits to receive the number of ecal hits
     *  @param  meanHCalEndCapTime to receive the energy weighted mean time for hcal endcap hits
     *  @param  nHCalEndCapHits to receive the number of hcal endcap hits
     */
    void GetClusterTimes(const pandora::Cluster *const pCluster, float &meanTime, float &meanTimeECal, unsigned int &nECalHits,
        float &meanHCalEndCapTime, unsigned int &nHCalEndCapHits) const;

    bool            m_monitoring;                                   ///< Whether to display monitoring information
    bool            m_displaySelectedPfos;                          ///< Whether to display monitoring information concerning selected pfos
    bool            m_displayRejectedPfos;                          ///< Whether to display monitoring information concerning rejected pfos
    float           m_monitoringPfoEnergyToDisplay;                 ///< Minimum pfo energy in order to display monitoring information

    float           m_farForwardCosTheta;                           ///< Value of cos theta identifying the detector forward region
    float           m_ptCutForTightTiming;                          ///< The pt value below which tight timing cuts are used

    float           m_photonPtCut;                                  ///< The basic pt cut for a photon pfo
    float           m_photonPtCutForLooseTiming;                    ///< The photon pt value below which tight timing cuts are used
    float           m_photonLooseTimingCut;                         ///< The photon loose high timing cut
    float           m_photonTightTimingCut;                         ///< The photon tight high timing cut

    float           m_chargedPfoPtCut;                              ///< The basic pt cut for a charged hadron pfo
    float           m_chargedPfoPtCutForLooseTiming;                ///< The charged hadron pt value below which tight timing cuts are used
    float           m_chargedPfoLooseTimingCut;                     ///< The charged hadron loose high timing cut
    float           m_chargedPfoTightTimingCut;                     ///< The charged hadron tight high timing cut
    float           m_chargedPfoNegativeLooseTimingCut;             ///< The charged hadron loose low timing cut
    float           m_chargedPfoNegativeTightTimingCut;             ///< The charged hadron tight low timing cut

    float           m_neutralHadronPtCut;                           ///< The basic pt cut for a neutral hadron pfo
    float           m_neutralHadronPtCutForLooseTiming;             ///< The neutral hadron pt value below which tight timing cuts are used
    float           m_neutralHadronLooseTimingCut;                  ///< The neutral hadron loose high timing cut
    float           m_neutralHadronTightTimingCut;                  ///< The neutral hadron tight high timing cut
    float           m_neutralFarForwardLooseTimingCut;              ///< The neutral hadron loose high timing cut for the forward region
    float           m_neutralFarForwardTightTimingCut;              ///< The neutral hadron tight high timing cut for the forward region

    float           m_hCalBarrelLooseTimingCut;                     ///< The loose timing cut for hits predominantly in hcal barrel
    float           m_hCalBarrelTightTimingCut;                     ///< The tight timing cut for hits predominantly in hcal barrel
    float           m_hCalEndCapTimingFactor;                       ///< Factor by which high timing cut is multiplied for hcal barrel hits
    float           m_neutralHadronBarrelPtCutForLooseTiming;       ///< pt above which loose timing cuts are applied to neutral hadrons in barrel

    unsigned int    m_minECalHitsForTiming;                         ///< Minimum ecal hits in order to use ecal timing info
    unsigned int    m_minHCalEndCapHitsForTiming;                   ///< Minimum hcal endcap hits in order to use hcal endcap timing info

    bool            m_useClusterLessPfos;                           ///< Whether to accept any cluster-less pfos
    float           m_minMomentumForClusterLessPfos;                ///< Minimum momentum for a cluster-less pfo
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CLICPfoSelectionAlgorithm::Factory::CreateAlgorithm() const
{
    return new CLICPfoSelectionAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_CLIC_PFO_SELECTION_ALGORITHM_H
