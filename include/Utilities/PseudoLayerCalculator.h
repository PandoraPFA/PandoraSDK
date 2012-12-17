/**
 *  @file   PandoraPFANew/Framework/include/Utilities/PseudoLayerCalculator.h
 * 
 *  @brief  Header file for the pseudo layer calculator interface class.
 * 
 *  $Log: $
 */
#ifndef PSEUDO_LAYER_CALCULATOR_H
#define PSEUDO_LAYER_CALCULATOR_H 1

#include "Helpers/GeometryHelper.h"
#include "Helpers/XmlHelper.h"

namespace pandora
{

/**
 *  @brief  PseudoLayerCalculator class
 */
class PseudoLayerCalculator
{
protected:
    /**
     *  @brief  Destructor
     */
    virtual ~PseudoLayerCalculator();

    /**
     *  @brief  Initialize pseudo layer calculator geometry information. This function will receive a callback from the pandora
     *          GeometryHelper when the geometry has been initialized.
     */
    virtual void InitializeGeometry();

    /**
     *  @brief  Read pseudo layer calculator settings from xml. This function will receive a callback if the top level xml tags
     *          <PseudoLayerCalculator></PseudoLayerCalculator> are present.
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    virtual void ReadSettings(const TiXmlHandle *const pXmlHandle);

    /**
     *  @brief  Get the appropriate pseudolayer for a specified position vector
     * 
     *  @param  positionVector the specified position
     * 
     *  @return the appropriate pseudolayer
     */
    virtual PseudoLayer GetPseudoLayer(const CartesianVector &positionVector) const = 0;

    /**
     *  @brief  Get the pseudolayer assigned to a point at the ip, i.e. the initial offset for pseudolayer values
     *          and the start of the pseudolayer scale
     * 
     *  @return the pseudolayer assigned to a point at the ip
     */
    virtual PseudoLayer GetPseudoLayerAtIp() const = 0;

    friend class GeometryHelper;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline PseudoLayerCalculator::~PseudoLayerCalculator()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void PseudoLayerCalculator::InitializeGeometry()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void PseudoLayerCalculator::ReadSettings(const TiXmlHandle *const /*pXmlHandle*/)
{
}

} // namespace pandora

#endif // #ifndef PSEUDO_LAYER_CALCULATOR_H
