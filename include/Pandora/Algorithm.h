/**
 *  @file   PandoraSDK/include/Pandora/Algorithm.h
 * 
 *  @brief  Header file for the algorithm class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_ALGORITHM_H
#define PANDORA_ALGORITHM_H 1

#include "Pandora/Process.h"

namespace pandora
{

/**
 *  @brief  Algorithm class. Algorithm addresses are held only by the algorithm manager. They have a fully defined interface
 *          and can only be run via the PandoraContent Apis.
 */
class Algorithm : public Process
{
protected:
    /**
     *  @brief  Run the algorithm
     */
    virtual StatusCode Run() = 0;

    friend class AlgorithmManager;
    friend class PandoraContentApiImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Factory class for instantiating algorithms
 */
class AlgorithmFactory
{
public:
    /**
     *  @brief  Create an instance of an algorithm
     * 
     *  @return the address of the algorithm instance
     */
    virtual Algorithm *CreateAlgorithm() const = 0;

    /**
     *  @brief  Destructor
     */
    virtual ~AlgorithmFactory();
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline AlgorithmFactory::~AlgorithmFactory()
{
}

} // namespace pandora

#endif // #ifndef PANDORA_ALGORITHM_H
