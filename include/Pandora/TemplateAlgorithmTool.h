/**
 *  @file   PandoraSDK/include/Pandora/TemplateAlgorithmTool.h
 * 
 *  @brief  Header file for the template algorithm tool class.
 * 
 *  $Log: $
 */
#ifndef TEMPLATE_ALGORITHM_TOOL_H
#define TEMPLATE_ALGORITHM_TOOL_H 1

#include "Pandora/AlgorithmTool.h"

/**
 *  @brief  TemplateAlgorithmTool class
 */
class TemplateAlgorithmTool : public pandora::AlgorithmTool
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm tool
     */
    class Factory : public pandora::AlgorithmToolFactory
    {
    public:
        pandora::AlgorithmTool *CreateAlgorithmTool() const;
    };

    /**
     *  @brief  Run the algorithm tool
     */
    pandora::StatusCode Run();

private:
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    // Member variables here
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::AlgorithmTool *TemplateAlgorithmTool::Factory::CreateAlgorithmTool() const
{
    return new TemplateAlgorithmTool();
}

#endif // #ifndef TEMPLATE_ALGORITHM_TOOL_H
