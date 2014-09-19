/**
 *  @file   LCContent/include/LCFragmentRemoval/MergeSplitPhotonsAlgorithm.h
 * 
 *  @brief  Header file for the merge split photons algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_MERGE_SPLIT_PHOTONS_ALGORITHM_H
#define LC_MERGE_SPLIT_PHOTONS_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  MergeSplitPhotonsAlgorithm class
 */
class MergeSplitPhotonsAlgorithm : public pandora::Algorithm
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
    MergeSplitPhotonsAlgorithm();

private:
    /**
     *  @brief  Get the cluster layer in which the maximum electromagnetic energy has been deposited
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return The shower max layer
     */
    unsigned int GetShowerMaxLayer(const pandora::Cluster *const pCluster) const;

    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    float           m_minShowerMaxCosAngle;                 ///< Max cosine of angle between parent and daughter shower max layer centroids
    float           m_contactDistanceThreshold;             ///< Distance threshold to declare cluster layers as "in contact"
    unsigned int    m_minContactLayers;                     ///< Minimum number of parent/daughter layers in contact
    float           m_minContactFraction;                   ///< Minimum contact fraction between parent/daughter clusters
    unsigned int    m_transProfileMaxLayer;                 ///< Maximum layer to consider in calculation of shower transverse profiles
    float           m_acceptMaxSmallFragmentEnergy;         ///< Max value of small fragment cluster energy to accept cluster merge
    float           m_acceptMaxSubsidiaryPeakEnergy;        ///< Max value of subsidiary peak energy to accept cluster merge
    float           m_acceptFragmentEnergyRatio;            ///< Max value of small/large fragment energy ratio to accept cluster merge
    float           m_acceptSubsidiaryPeakEnergyRatio;      ///< Max value of subsidiary peak/small fragment energy ratio to accept cluster merge
    unsigned int    m_earlyTransProfileMaxLayer;            ///< Maximum layer to consider in calculation of early shower transverse profiles
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MergeSplitPhotonsAlgorithm::Factory::CreateAlgorithm() const
{
    return new MergeSplitPhotonsAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_MERGE_SPLIT_PHOTONS_ALGORITHM_H
