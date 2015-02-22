/**
 *  @file   LCContent/include/LCContentFast.h
 * 
 *  @brief  Header file detailing faster versions of algorithms in the LCContent library, using e.g. KD-trees and *relying on c++11*
 * 
 *  $Log: $
 */
#ifndef LINEAR_COLLIDER_CONTENT_FAST_H
#define LINEAR_COLLIDER_CONTENT_FAST_H 1

#include "LCContentFast/CaloHitPreparationAlgorithmFast.h"
#include "LCContentFast/ConeClusteringAlgorithmFast.h"
#include "LCContentFast/MainFragmentRemovalAlgorithmFast.h"
#include "LCContentFast/SoftClusterMergingAlgorithmFast.h"
#include "LCContentFast/TrackClusterAssociationAlgorithmFast.h"

/**
 *  @brief  LCContentFast class
 */
class LCContentFast
{
public:
    #define LC_ALGORITHM_FAST_LIST(d)                                                                                           \
        d("CaloHitPreparationFast",                 lc_content_fast::CaloHitPreparationAlgorithm::Factory)                      \
        d("ConeClusteringFast",                     lc_content_fast::ConeClusteringAlgorithm::Factory)                          \
        d("MainFragmentRemovalFast",                lc_content_fast::MainFragmentRemovalAlgorithm::Factory)                     \
        d("SoftClusterMergingFast",                 lc_content_fast::SoftClusterMergingAlgorithm::Factory)                      \
        d("TrackClusterAssociationFast",            lc_content_fast::TrackClusterAssociationAlgorithm::Factory)

    /**
     *  @brief  Register all the linear collider algorithms with pandora
     * 
     *  @param  pandora the pandora instance with which to register content
     */
    static pandora::StatusCode RegisterAlgorithms(const pandora::Pandora &pandora);
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode LCContentFast::RegisterAlgorithms(const pandora::Pandora &pandora)
{
    LC_ALGORITHM_FAST_LIST(PANDORA_REGISTER_ALGORITHM);

    return pandora::STATUS_CODE_SUCCESS;
}

#endif // #ifndef LINEAR_COLLIDER_CONTENT_FAST_H
