/**
 *  @file   LCContent/include/LCUtility/CaloHitPreparationAlgorithm.h
 * 
 *  @brief  Header file for the calo hit preparation algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_CALO_HIT_PREPARATION_ALGORITHM_H
#define LC_CALO_HIT_PREPARATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  CaloHitPreparationAlgorithm class
 */
class CaloHitPreparationAlgorithm : public pandora::Algorithm
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
    CaloHitPreparationAlgorithm();

private:
    pandora::StatusCode Run();

    /**
     *  @brief  Calculate calo hit properties for a particular calo hit, through comparison with an ordered list of other hits.
     * 
     *  @param  pCaloHit the calo hit
     *  @param  pOrderedCaloHitList the ordered calo hit list
     */
    void CalculateCaloHitProperties(const pandora::CaloHit *const pCaloHit, const pandora::OrderedCaloHitList &orderedCaloHitList) const;

    /**
     *  @brief  Count number of "nearby" hits using the isolation scheme
     * 
     *  @param  pCaloHit the calo hit
     *  @param  pCaloHitList the calo hit list
     * 
     *  @return the number of nearby hits
     */
    unsigned int IsolationCountNearbyHits(const pandora::CaloHit *const pCaloHit, const pandora::CaloHitList *const pCaloHitList) const;

    /**
     *  @brief  Count number of "nearby" hits using the mip identification scheme
     * 
     *  @param  pCaloHit the calo hit
     *  @param  pCaloHitList the calo hit list
     * 
     *  @return the number of nearby hits
     */
    unsigned int MipCountNearbyHits(const pandora::CaloHit *const pCaloHit, const pandora::CaloHitList *const pCaloHitList) const;

    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    float           m_caloHitMaxSeparation2;            ///< Max separation to consider associations between hits, units mm (used squared)
    float           m_isolationCaloHitMaxSeparation2;   ///< Max separation considered when identifying isolated hits, units mm (used squared)

    unsigned int    m_isolationNLayers;                 ///< Number of adjacent layers to use in isolation calculation
    float           m_isolationCutDistanceFine2;        ///< Fine granularity isolation cut distance, units mm (used squared)
    float           m_isolationCutDistanceCoarse2;      ///< Coarse granularity isolation cut distance, units mm (used squared)
    unsigned int    m_isolationMaxNearbyHits;           ///< Max number of "nearby" hits for a hit to be considered isolated

    float           m_mipLikeMipCut;                    ///< Mip equivalent energy cut for hit to be flagged as possible mip
    unsigned int    m_mipNCellsForNearbyHit;            ///< Separation (in calo cells) for hits to be declared "nearby"
    unsigned int    m_mipMaxNearbyHits;                 ///< Max number of "nearby" hits for hit to be flagged as possible mip
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CaloHitPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new CaloHitPreparationAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_CALO_HIT_PREPARATION_ALGORITHM_H
