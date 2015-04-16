/**
 *  @file   PandoraSDK/include/Pandora/ObjectFactory.h
 * 
 *  @brief  Header file for the object factory class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_OBJECT_FACTORY_H
#define PANDORA_OBJECT_FACTORY_H 1

#include "Pandora/StatusCodes.h"

namespace pandora
{

/**
 *  @brief  ObjectFactory class responsible for extended pandora object creation
 */
template <typename PARAMETERS, typename OBJECT>
class ObjectFactory
{
public:
    typedef PARAMETERS Parameters;
    typedef OBJECT Object;

    /**
     *  @brief  Create an object with the given parameters
     *
     *  @param  parameters the parameters to pass in constructor
     *  @param  pObject to receive the address of the object created
     */
    virtual StatusCode Create(const Parameters &parameters, const Object *&pObject) const = 0;
};

} // namespace pandora

#endif // #ifndef PANDORA_OBJECT_FACTORY_H
