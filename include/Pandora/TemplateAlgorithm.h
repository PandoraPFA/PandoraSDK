/**
 *  @file   PandoraPFANew/Framework/include/Pandora/TemplateAlgorithm.h
 * 
 *  @brief  Header file for the template algorithm class.
 * 
 *  $Log: $
 */
#ifndef TEMPLATE_ALGORITHM_H
#define TEMPLATE_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  TemplateAlgorithm class
 */
class TemplateAlgorithm : public pandora::Algorithm
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

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    // Member variables here
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TemplateAlgorithm::Factory::CreateAlgorithm() const
{
    return new TemplateAlgorithm();
}

#endif // #ifndef TEMPLATE_ALGORITHM_H
