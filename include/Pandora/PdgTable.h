/**
 *  @file   PandoraPFANew/Framework/include/PdgTable.h
 * 
 *  @brief  Header file defining masses, widths, and mc id numbers from 2002 edition of rpp
 * 
 *  $Log: $
 */
#ifndef PDG_TABLE_H
#define PDG_TABLE_H 1

namespace pandora
{

// Specify (name, pdg code, mass in GeV, width in GeV, charge)
#define PARTICLE_DATA_TABLE(d)                                                          \
    d(PHOTON,               22,             0.E+00f,             0.E+00f,       0)      \
    d(E_MINUS,              11,     5.10998902E-04f,             0.E+00f,      -1)      \
    d(E_PLUS,              -11,     5.10998902E-04f,             0.E+00f,      +1)      \
    d(MU_MINUS,             13,     1.05658357E-01f,        2.99591E-19f,      -1)      \
    d(MU_PLUS,             -13,     1.05658357E-01f,        2.99591E-19f,      +1)      \
    d(TAU_MINUS,            15,        1.77699E+00f,          2.265E-12f,      -1)      \
    d(TAU_PLUS,            -15,        1.77699E+00f,          2.265E-12f,      +1)      \
    d(NU_E,                 12,             0.E+00f,             0.E+00f,       0)      \
    d(NU_E_BAR,            -12,             0.E+00f,             0.E+00f,       0)      \
    d(NU_MU,                14,             0.E+00f,             0.E+00f,       0)      \
    d(NU_MU_BAR,           -14,             0.E+00f,             0.E+00f,       0)      \
    d(NU_TAU,               16,             0.E+00f,             0.E+00f,       0)      \
    d(NU_TAU_BAR,          -16,             0.E+00f,             0.E+00f,       0)      \
    d(PI_PLUS,             211,      1.3957018E-01f,         2.5284E-17f,      +1)      \
    d(PI_MINUS,           -211,      1.3957018E-01f,         2.5284E-17f,      -1)      \
    d(PI_ZERO,             111,       1.349766E-01f,            7.8E-09f,       0)      \
    d(LAMBDA,             3122,       1.115683E+00f,          2.501E-15f,       0)      \
    d(LAMBDA_BAR,        -3122,       1.115683E+00f,          2.501E-15f,       0)      \
    d(K_PLUS,              321,        4.93677E-01f,          5.315E-17f,      +1)      \
    d(K_MINUS,            -321,        4.93677E-01f,          5.315E-17f,      -1)      \
    d(K_SHORT,             310,        4.97672E-01f,          7.367E-15f,       0)      \
    d(K_LONG,              130,        4.97672E-01f,          1.272E-17f,       0)      \
    d(SIGMA_MINUS,        3112,         1.1975E+00f,           8.28E-15f,      -1)      \
    d(SIGMA_PLUS,         3222,         1.1975E+00f,           8.28E-15f,      +1)      \
    d(SIGMA_MINUS_BAR,   -3112,         1.1975E+00f,           8.28E-15f,      +1)      \
    d(SIGMA_PLUS_BAR,    -3222,         1.1975E+00f,           8.28E-15f,      -1)      \
    d(HYPERON_ZERO ,      3322,        1.31483E+00f,           2.28E-15f,       0)      \
    d(HYPERON_ZERO_BAR,  -3322,        1.31483E+00f,           2.28E-15f,       0)      \
    d(HYPERON_MINUS,      3312,        1.32131E+00f,           4.04E-15f,      -1)      \
    d(HYPERON_MINUS_BAR, -3312,        1.32131E+00f,           4.04E-15f,      +1)      \
    d(PROTON,             2212,      9.3827200E-01f,             0.E+00f,      +1)      \
    d(PROTON_BAR,        -2212,      9.3827200E-01f,             0.E+00f,      -1)      \
    d(NEUTRON,            2112,      9.3956533E-01f,          7.432E-28f,       0)      \
    d(NEUTRON_BAR,       -2112,      9.3956533E-01f,          7.432E-28f,       0)

/**
 *  @brief  The particle type enum macro
 */
#define GET_PARTICLE_TYPE_ENTRY(a, b, c, d, e)                                          \
    a = b,

/**
 *  @brief  The particle type switch statement macro
 */
#define GET_PARTICLE_TYPE_SWITCH(a, b, c, d, e)                                         \
    case b : return a;

/**
 *  @brief  The name switch statement macro
 */
#define GET_PARTICLE_NAME_SWITCH(a, b, c, d, e)                                         \
    case a : return std::string(#a);

/**
 *  @brief  The pdg code switch statement macro
 */
#define GET_PARTICLE_PDG_CODE_SWITCH(a, b, c, d, e)                                     \
    case a : return b;

/**
 *  @brief  The mass switch statement macro
 */
#define GET_PARTICLE_MASS_SWITCH(a, b, c, d, e)                                         \
    case a : return c;

/**
 *  @brief  The width switch statement macro
 */
#define GET_PARTICLE_WIDTH_SWITCH(a, b, c, d, e)                                        \
    case a : return d;

/**
 *  @brief  The charge switch statement macro
 */
#define GET_PARTICLE_CHARGE_SWITCH(a, b, c, d, e)                                       \
    case a : return e;

/**
 *  @brief  The particle type enum
 */
enum ParticleType
{
    PARTICLE_DATA_TABLE(GET_PARTICLE_TYPE_ENTRY)
    UNKNOWN_PARTICLE_TYPE
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PdgTable class
 */
class PdgTable
{
public:
    /**
     *  @brief  Get the particle type for a given pdg code
     * 
     *  @param  pdgCode the pdg code
     * 
     *  @return the particle type
     */
    static ParticleType GetParticleType(const int pdgCode);

    /**
     *  @brief  Get the name of a particle type as a string
     * 
     *  @param  pdgCode the pdg code (or particle type)
     * 
     *  @return the name
     */
    static std::string GetParticleName(const int pdgCode);

    /**
     *  @brief  Get the pdg code of a particle type
     * 
     *  @param  pdgCode the pdg code (or particle type)
     * 
     *  @return the name
     */
    static int GetParticlePdgCode(const int pdgCode);

    /**
     *  @brief  Get the mass of a particle type
     * 
     *  @param  pdgCode the pdg code (or particle type)
     * 
     *  @return the mass
     */
    static float GetParticleMass(const int pdgCode);

    /**
     *  @brief  Get the width of a particle type
     * 
     *  @param  pdgCode the pdg code (or particle type)
     * 
     *  @return the width
     */
    static float GetParticleWidth(const int pdgCode);

    /**
     *  @brief  Get the charge of a particle type
     * 
     *  @param  pdgCode the pdg code (or particle type)
     * 
     *  @return the charge
     */
    static int GetParticleCharge(const int pdgCode);
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline ParticleType PdgTable::GetParticleType(const int pdgCode)
{
    switch (pdgCode)
    {
        PARTICLE_DATA_TABLE(GET_PARTICLE_TYPE_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline std::string PdgTable::GetParticleName(const int pdgCode)
{
    switch (pdgCode)
    {
        PARTICLE_DATA_TABLE(GET_PARTICLE_NAME_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int PdgTable::GetParticlePdgCode(const int pdgCode)
{
    switch (pdgCode)
    {
        PARTICLE_DATA_TABLE(GET_PARTICLE_PDG_CODE_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PdgTable::GetParticleMass(const int pdgCode)
{
    switch (pdgCode)
    {
        PARTICLE_DATA_TABLE(GET_PARTICLE_MASS_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PdgTable::GetParticleWidth(const int pdgCode)
{
    switch (pdgCode)
    {
        PARTICLE_DATA_TABLE(GET_PARTICLE_WIDTH_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int PdgTable::GetParticleCharge(const int pdgCode)
{
    switch (pdgCode)
    {
        PARTICLE_DATA_TABLE(GET_PARTICLE_CHARGE_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

} // namespace pandora

#endif // #ifndef PDG_TABLE_H
