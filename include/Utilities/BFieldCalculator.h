/**
 *  @file   PandoraPFANew/Framework/include/Utilities/BFieldCalculator.h
 * 
 *  @brief  Header file for the bfield calculator interface class.
 * 
 *  $Log: $
 */
#ifndef BFIELD_CALCULATOR_H
#define BFIELD_CALCULATOR_H 1

#include "Helpers/GeometryHelper.h"
#include "Helpers/XmlHelper.h"

namespace pandora
{

/**
 *  @brief  BFieldCalculator class
 */
class BFieldCalculator
{
protected:
    /**
     *  @brief  Destructor
     */
    virtual ~BFieldCalculator();

    /**
     *  @brief  Initialize bfield calculator geometry information. This function will receive a callback from the pandora
     *          GeometryHelper when the geometry has been initialized.
     */
    virtual void InitializeGeometry();

    /**
     *  @brief  Read bfield calculator settings from xml.This function will receive a callback if the top level xml tags
     *          <BFieldCalculator></BFieldCalculator> are present.
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    virtual void ReadSettings(const TiXmlHandle *const pXmlHandle);

    /**
     *  @brief  Get the bfield value for a specified position vector
     * 
     *  @param  positionVector the specified position
     * 
     *  @return the bfield, units Tesla
     */
    virtual float GetBField(const CartesianVector &positionVector) const = 0;

    friend class GeometryHelper;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline BFieldCalculator::~BFieldCalculator()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void BFieldCalculator::InitializeGeometry()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void BFieldCalculator::ReadSettings(const TiXmlHandle *const /*pXmlHandle*/)
{
}

} // namespace pandora

#endif // #ifndef BFIELD_CALCULATOR_H
