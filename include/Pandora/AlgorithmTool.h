/**
 *  @file   PandoraSDK/include/Pandora/AlgorithmTool.h
 * 
 *  @brief  Header file for the algorithm tool class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_ALGORITHM_TOOL_H
#define PANDORA_ALGORITHM_TOOL_H 1

#include "Pandora/Process.h"

namespace pandora
{

/**
 *  @brief  AlgorithmTool class. Algorithm tools will tend to be tailored for specific parent algorithms, which will define
 *          their interface. Unlike standard daughter algorithms, algorithm tools can be accessed directly via parent algorithms
 *          and there is no change in the pandora list-management when a parent algorithm runs its daughter algorithm tools.
 */
class AlgorithmTool : public Process
{
protected:
    friend class AlgorithmManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Factory class for instantiating algorithm tools
 */
class AlgorithmToolFactory
{
public:
    /**
     *  @brief  Create an instance of an algorithm
     * 
     *  @return the address of the algorithm instance
     */
    virtual AlgorithmTool *CreateAlgorithmTool() const = 0;

    /**
     *  @brief  Destructor
     */
    virtual ~AlgorithmToolFactory();
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline AlgorithmToolFactory::~AlgorithmToolFactory()
{
}

} // namespace pandora

#endif // #ifndef PANDORA_ALGORITHM_TOOL_H
