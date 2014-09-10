/**
 *  @file   LCContent/include/LCPlugins/LCEnergyCorrectionPlugins.h
 * 
 *  @brief  Header file for the lc energy correction plugins class.
 * 
 *  $Log: $
 */
#ifndef LC_ENERGY_CORRECTION_PLUGINS_H
#define LC_ENERGY_CORRECTION_PLUGINS_H 1

#include "Plugins/EnergyCorrectionsPlugin.h"

namespace lc_content
{

/**
 *  @brief  LCEnergyCorrectionPlugins class
 */
class LCEnergyCorrectionPlugins
{
public:
    /**
     *   @brief  Correct cluster energy to account for non-linearities in calibration
     */
    class NonLinearityCorrection : public pandora::EnergyCorrectionPlugin
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  inputEnergyCorrectionPoints the input energy points for energy correction
         *  @param  outputEnergyCorrectionPoints the output energy points for energy correction
         */
        NonLinearityCorrection(const pandora::FloatVector &inputEnergyCorrectionPoints, const pandora::FloatVector &outputEnergyCorrectionPoints);

        pandora::StatusCode MakeEnergyCorrections(const pandora::Cluster *const pCluster, float &correctedEnergy) const;

    private:
        pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

        pandora::FloatVector m_inputEnergyCorrectionPoints; ///< The input energy points for energy correction
        pandora::FloatVector m_energyCorrections;           ///< The energy correction factors
    };

    /**
     *   @brief  CleanCluster class. Correct cluster energy by searching for constituent calo hits with anomalously high energy.
     *           Corrections are made by examining the energy in adjacent layers of the cluster.
     */
    class CleanCluster : public pandora::EnergyCorrectionPlugin
    {
    public:
        /**
         *  @brief  Default constructor
         */
        CleanCluster();

        pandora::StatusCode MakeEnergyCorrections(const pandora::Cluster *const pCluster, float &correctedEnergy) const;

    private:
        /**
         *  @brief  Get the sum of the hadronic energies of all calo hits in a specified layer of an ordered calo hit list
         * 
         *  @param  orderedCaloHitList the ordered calo hit list
         *  @param  pseudoLayer the specified pseudolayer
         */
        float GetHadronicEnergyInLayer(const pandora::OrderedCaloHitList &orderedCaloHitList, const unsigned int pseudoLayer) const;

        pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

        float           m_minCleanHitEnergy;                ///< Min calo hit hadronic energy to consider cleaning hit/cluster
        float           m_minCleanHitEnergyFraction;        ///< Min fraction of cluster energy represented by hit to consider cleaning
        float           m_minCleanCorrectedHitEnergy;       ///< Min value of new hit hadronic energy estimate after cleaning
    };

    /**
     *   @brief  ScaleHotHadrons class. Correct cluster energy by searching for clusters with anomalously high mip energies per
     *           constituent calo hit. Corrections are made by scaling back the mean number of mips per calo hit.
     */
    class ScaleHotHadrons : public pandora::EnergyCorrectionPlugin
    {
    public:
        /**
         *  @brief  Default constructor
         */
        ScaleHotHadrons();

        pandora::StatusCode MakeEnergyCorrections(const pandora::Cluster *const pCluster, float &correctedEnergy) const;

    private:
        pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

        unsigned int    m_minHitsForHotHadron;             ///< Min number of hits in a hot hadron candidate cluster
        unsigned int    m_maxHitsForHotHadron;             ///< Max number of hits in a hot hadron candidate cluster
        unsigned int    m_hotHadronInnerLayerCut;          ///< Cut 1 of 3 (must fail all for rejection): Min inner layer for hot hadron
        float           m_hotHadronMipFractionCut;         ///< Cut 2 of 3 (must fail all for rejection): Min mip fraction for hot hadron
        unsigned int    m_hotHadronNHitsCut;               ///< Cut 3 of 3 (must fail all for rejection): Max number of hits for hot hadron
        float           m_hotHadronMipsPerHit;             ///< Min number of mips per hit for a hot hadron cluster
        float           m_scaledHotHadronMipsPerHit;       ///< Scale factor (new mips per hit value) to correct hot hadron energies
    };

    /**
     *   @brief  MuonCoilCorrection class. Addresses issue of energy loss in uninstrumented coil region.
     */
    class MuonCoilCorrection : public pandora::EnergyCorrectionPlugin
    {
    public:
        /**
         *  @brief  Default constructor
         */
        MuonCoilCorrection();

        pandora::StatusCode MakeEnergyCorrections(const pandora::Cluster *const pCluster, float &correctedEnergy) const;

    private:
        pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

        float           m_muonHitEnergy;                    ///< The energy for a digital muon calorimeter hit, units GeV
        float           m_coilEnergyLossCorrection;         ///< Energy correction due to missing energy deposited in coil, units GeV
        unsigned int    m_minMuonHitsInInnerLayer;          ///< Min muon hits in muon inner layer to correct charged cluster energy
        float           m_coilEnergyCorrectionChi;          ///< Track-cluster chi value used to assess need for coil energy correction
    };
};

} // namespace lc_content

#endif // #ifndef LC_ENERGY_CORRECTION_PLUGINS_H
