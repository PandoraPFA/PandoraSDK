/**
 *  @file   LCContent/include/LCV0PfoCreationAlgorithm.h
 * 
 *  @brief  Header file for the v0 pfo creation algorithm class.
 * 
 *  $Log: $
 */
#ifndef LC_V0_PFO_CREATION_ALGORITHM_H
#define LC_V0_PFO_CREATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "Objects/CartesianVector.h"

namespace lc_content
{

/**
 *  @brief  V0PfoCreationAlgorithm class
 */
class V0PfoCreationAlgorithm : public pandora::Algorithm
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
    V0PfoCreationAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Calculate the invariant mass for a decaying charged particle
     * 
     *  @param  momentum1 is cartesian vector of track1 at start
     *  @param  momentum2 is cartesian vector of track2 at start
     *  @param  mass1 is assumed mass of track1
     *  @param  mass2 is assumed mass of track2
     *  @param  v0energy to receive resulting V0 energy
     *  @param  v0mass to receive resulting V0 mass
     */
    pandora::StatusCode GetV0Mass(const pandora::CartesianVector &momentum1, const pandora::CartesianVector &momentum2, float mass1,
        float mass2, float &v0energy, float &v0mass) const;

    bool    m_mcMonitoring;         ///< Whether to use mc monitoring functionality
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *V0PfoCreationAlgorithm::Factory::CreateAlgorithm() const
{
    return new V0PfoCreationAlgorithm();
}

} // namespace lc_content

#endif // #ifndef LC_V0_PFO_ALGORITHM_H
