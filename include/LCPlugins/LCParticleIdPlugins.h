/**
 *  @file   LCContent/include/LCPlugins/LCParticleIdPlugins.h
 * 
 *  @brief  Header file for the lc particle id plugins class.
 * 
 *  $Log: $
 */
#ifndef LC_PARTICLE_ID_PLUGINS_H
#define LC_PARTICLE_ID_PLUGINS_H 1

#include "Plugins/ParticleIdPlugin.h"

namespace lc_content
{

/**
 *  @brief  LCParticleIdPlugins class
 */
class LCParticleIdPlugins
{
public:
    /**
     *   @brief  LCEmShowerId class
     */
    class LCEmShowerId : public pandora::ParticleIdPlugin
    {
    public:
        /**
         *  @brief  Default constructor
         */
        LCEmShowerId();

        bool IsMatch(const pandora::Cluster *const pCluster) const;

    private:
        typedef std::pair<float, float> HitEnergyDistance;
        typedef std::vector<HitEnergyDistance> HitEnergyDistanceVector;

        /**
         *  @brief  Sort HitEnergyDistance objects by increasing distance
         * 
         *  @param  lhs the first hit energy distance pair
         *  @param  rhs the second hit energy distance pair
         */
        static bool SortHitsByDistance(const HitEnergyDistance &lhs, const HitEnergyDistance &rhs);

        pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

        float       m_mipCut_0;                          ///< Default cluster mip fraction cut for emshw id
        float       m_mipCutEnergy_1;                    ///< Energy above which mip fraction cut value 1 is applied
        float       m_mipCut_1;                          ///< Cluster mip fraction cut value 1
        float       m_mipCutEnergy_2;                    ///< Energy above which mip fraction cut value 2 is applied
        float       m_mipCut_2;                          ///< Cluster mip fraction cut value 2
        float       m_mipCutEnergy_3;                    ///< Energy above which mip fraction cut value 3 is applied
        float       m_mipCut_3;                          ///< Cluster mip fraction cut value 3
        float       m_mipCutEnergy_4;                    ///< Energy above which mip fraction cut value 4 is applied
        float       m_mipCut_4;                          ///< Cluster mip fraction cut value 4
        float       m_dCosRCutEnergy;                    ///< Energy at which emshw id cut (on cluster fit result dCosR) changes
        float       m_dCosRLowECut;                      ///< Low energy cut on cluster fit result dCosR
        float       m_dCosRHighECut;                     ///< High energy cut on cluster fit result dCosR
        float       m_rmsCutEnergy;                      ///< Energy at which emshw id cut (on cluster fit result rms) changes
        float       m_rmsLowECut;                        ///< Low energy cut on cluster fit result rms
        float       m_rmsHighECut;                       ///< High energy cut on cluster fit result rms
        float       m_minCosAngle;                       ///< Min angular correction used to adjust radiation length measures
        float       m_maxInnerLayerRadLengths;           ///< Max number of radiation lengths before cluster inner layer
        float       m_minLayer90RadLengths;              ///< Min number of radiation lengths before cluster layer90
        float       m_maxLayer90RadLengths;              ///< Max number of radiation lengths before cluster layer90
        float       m_minShowerMaxRadLengths;            ///< Min number of radiation lengths before cluster shower max layer
        float       m_maxShowerMaxRadLengths;            ///< Max number of radiation lengths before cluster shower max layer
        float       m_highRadLengths;                    ///< Max number of radiation lengths expected to be spanned by em shower
        float       m_maxHighRadLengthEnergyFraction;    ///< Max fraction of cluster energy above max expected radiation lengths
        float       m_maxRadial90;                       ///< Max value of transverse profile radial90
    };

    /**
     *   @brief  LCPhotonId class
     */
    class LCPhotonId : public pandora::ParticleIdPlugin
    {
    public:
        /**
         *  @brief  Default constructor
         */
        LCPhotonId();

        bool IsMatch(const pandora::Cluster *const pCluster) const;

    private:
        pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);
    };

    /**
     *   @brief  LCElectronId class
     */
    class LCElectronId : public pandora::ParticleIdPlugin
    {
    public:
        /**
         *  @brief  Default constructor
         */
        LCElectronId();

        bool IsMatch(const pandora::Cluster *const pCluster) const;

    private:
        pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

        unsigned int m_maxInnerLayer;                   ///< Max inner psuedo layer for fast electron id
        float        m_maxEnergy;                       ///< Max electromagnetic energy for fast electron id
        float        m_maxProfileStart;                 ///< Max shower profile start for fast electron id
        float        m_maxProfileDiscrepancy;           ///< Max shower profile discrepancy for fast electron id
        float        m_profileDiscrepancyForAutoId;     ///< Shower profile discrepancy for automatic fast electron selection
        float        m_maxResidualEOverP;               ///< Max absolute difference between unity and ratio em energy / track momentum
    };

    /**
     *   @brief  LCMuonId class
     */
    class LCMuonId : public pandora::ParticleIdPlugin
    {
    public:
        /**
         *  @brief  Default constructor
         */
        LCMuonId();

        bool IsMatch(const pandora::Cluster *const pCluster) const;

    private:
        pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

        unsigned int m_maxInnerLayer;                   ///< Max inner psuedo layer for fast muon id
        float        m_minTrackEnergy;                  ///< Min energy of associated track for fast muon id
        float        m_maxHCalHitEnergy;                ///< Max hadronic energy for an individual hcal hit (suppress fluctuations)
        unsigned int m_minECalLayers;                   ///< Min number of ecal layers for fast muon id
        unsigned int m_minHCalLayers;                   ///< Min number of hcal layers for fast muon id
        bool         m_shouldPerformGapCheck;           ///< Whether to perform muon recovery gap checks
        unsigned int m_minHCalLayersForGapCheck;        ///< Min number of hcal layers to perform muon recovery gap check
        unsigned int m_minMuonHitsForGapCheck;          ///< Min number of muon hits to perform muon recovery gap check
        float        m_eCalEnergyCut0;                  ///< Parameter 0 for ecal energy cut: cut = par0 + (par1 * trackEnergy)
        float        m_eCalEnergyCut1;                  ///< Parameter 1 for ecal energy cut: cut = par0 + (par1 * trackEnergy)
        float        m_hCalEnergyCut0;                  ///< Parameter 0 for hcal energy cut: cut = par0 + (par1 * trackEnergy)
        float        m_hCalEnergyCut1;                  ///< Parameter 1 for hcal energy cut: cut = par0 + (par1 * trackEnergy)
        unsigned int m_minECalLayersForFit;             ///< Min number of ecal layers to perform fit to ecal section of cluster
        unsigned int m_minHCalLayersForFit;             ///< Min number of hcal layers to perform fit to hcal section of cluster
        unsigned int m_minMuonLayersForFit;             ///< Min number of muon layers to perform fit to muon section of cluster
        unsigned int m_eCalFitInnerLayer;               ///< Inner layer used for fit to ecal section of cluster
        unsigned int m_eCalFitOuterLayer;               ///< Outer layer used for fit to ecal section of cluster
        unsigned int m_hCalFitInnerLayer;               ///< Inner layer used for fit to hcal section of cluster
        unsigned int m_hCalFitOuterLayer;               ///< Outer layer used for fit to hcal section of cluster
        float        m_eCalRmsCut0;                     ///< Parameter 0 for ecal rms cut: cut = par0 + (par1 * trackEnergy)
        float        m_eCalRmsCut1;                     ///< Parameter 1 for ecal rms cut: cut = par0 + (par1 * trackEnergy)
        float        m_eCalMaxRmsCut;                   ///< Max value of ecal rms cut
        float        m_hCalRmsCut0;                     ///< Parameter 0 for hcal rms cut: cut = par0 + (par1 * trackEnergy)
        float        m_hCalRmsCut1;                     ///< Parameter 0 for hcal rms cut: cut = par0 + (par1 * trackEnergy)
        float        m_hCalMaxRmsCut;                   ///< Max value of hcal rms cut
        float        m_eCalMipFractionCut0;             ///< Parameter 0 for ecal mip fraction cut: cut = par0 - (par1 * trackEnergy)
        float        m_eCalMipFractionCut1;             ///< Parameter 1 for ecal mip fraction cut: cut = par0 - (par1 * trackEnergy)
        float        m_eCalMaxMipFractionCut;           ///< Max value of ecal mip fraction cut
        float        m_hCalMipFractionCut0;             ///< Parameter 0 for hcal mip fraction cut: cut = par0 - (par1 * trackEnergy)
        float        m_hCalMipFractionCut1;             ///< Parameter 1 for hcal mip fraction cut: cut = par0 - (par1 * trackEnergy)
        float        m_hCalMaxMipFractionCut;           ///< Max value of hcal mip fraction cut
        float        m_eCalHitsPerLayerCut0;            ///< Parameter 0 for ecal hits per layer cut: cut = par0 + (par1 * trackEnergy)
        float        m_eCalHitsPerLayerCut1;            ///< Parameter 1 for ecal hits per layer cut: cut = par0 + (par1 * trackEnergy)
        float        m_eCalMaxHitsPerLayerCut;          ///< Max value of ecal hits per layer cut
        float        m_hCalHitsPerLayerCut0;            ///< Parameter 0 for hcal hits per layer cut: cut = par0 + (par1 * trackEnergy)
        float        m_hCalHitsPerLayerCut1;            ///< Parameter 1 for hcal hits per layer cut: cut = par0 + (par1 * trackEnergy)
        float        m_hCalMaxHitsPerLayerCut;          ///< Max value of hcal hits per layer cut
        float        m_curlingTrackEnergy;              ///< Max energy for associated track to be considered as curling
        float        m_inBarrelHitFraction;             ///< Min fraction of hcal hits in barrel region to identify "barrel cluster"
        float        m_tightMipFractionCut;             ///< Tight mip fraction cut
        float        m_tightMipFractionECalCut;         ///< Tight ecal mip fraction cut
        float        m_tightMipFractionHCalCut;         ///< Tight hcal mip fraction cut
        unsigned int m_minMuonHitsCut;                  ///< Min number of hits in muon region
        unsigned int m_minMuonTrackSegmentHitsCut;      ///< Min number of muon track segment hits
        float        m_muonRmsCut;                      ///< Muon rms cut
        float        m_maxMuonHitsCut0;                 ///< Parameter 0 for max muon hits cut: cut = par0 + (par1 * trackEnergy)
        float        m_maxMuonHitsCut1;                 ///< Parameter 1 for max muon hits cut: cut = par0 + (par1 * trackEnergy)
        float        m_maxMuonHitsCutMinValue;          ///< Min value of max muon hits cut
    };
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool LCParticleIdPlugins::LCEmShowerId::SortHitsByDistance(const HitEnergyDistance &lhs, const HitEnergyDistance &rhs)
{
    return (lhs.second < rhs.second);
}

} // namespace lc_content

#endif // #ifndef LC_PARTICLE_ID_PLUGINS_H
