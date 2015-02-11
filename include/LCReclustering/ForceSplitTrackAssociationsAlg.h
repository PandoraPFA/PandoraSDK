/**
 *  @file   LCContent/include/LCReclustering/ForceSplitTrackAssociationsAlg.h
 * 
 *  @brief  Header file for the force split track associations algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_FORCE_SPLIT_TRACK_ASSOCIATIONS_ALGORITHM_H
#define LC_FORCE_SPLIT_TRACK_ASSOCIATIONS_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace lc_content
{

/**
 *  @brief  ForceSplitTrackAssociationsAlg class
 */
class ForceSplitTrackAssociationsAlg : public pandora::Algorithm
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
    ForceSplitTrackAssociationsAlg();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    typedef std::map<const pandora::Track *, const pandora::Cluster *> TrackToClusterMap;

    unsigned int    m_minTrackAssociations;         ///< The minimum number of track associations to forcibly resolve
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ForceSplitTrackAssociationsAlg::Factory::CreateAlgorithm() const
{
    return new ForceSplitTrackAssociationsAlg();
}

} // namespace lc_content

#endif // #ifndef LC_FORCE_SPLIT_TRACK_ASSOCIATIONS_ALGORITHM_H
