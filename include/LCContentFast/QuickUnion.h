/**
 *  @file   LCContent/include/LCContentFast/QuickUnion.h
 * 
 *  @brief  Header file for the quick union class
 * 
 *  $Log: $
 */
#ifndef LC_QUICK_UNION_H
#define LC_QUICK_UNION_H 1

#include <vector>

namespace lc_content_fast
{

/**
 *  @brief  QuickUnion class
 */
class QuickUnion
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  nBranches the number of original indices
     */
    QuickUnion(const unsigned nBranches);

    /**
     *  @brief  Get the current number of target indices
     * 
     *  @return the current number of target indices
     */
    int Count() const;

    /**
     *  @brief  Find the current target index for provided index p
     * 
     *  @param  p index p
     */
    unsigned int Find(unsigned p);

    /**
     *  @brief  Whether two original indices are now connected
     * 
     *  @param  p index p
     *  @param  q index q
     * 
     *  @return boolean
     */
    bool Connected(unsigned p, unsigned q);

    /**
     *  @brief  Unite two indices
     * 
     *  @param  p index p
     *  @param  q index q
     */
    void Unite(unsigned p, unsigned q);

private:
    std::vector<unsigned>   m_id;       ///< Stores target index for each original index
    //std::vector<unsigned> m_size;     ///< Stores number of connected indices, used for book-keeping
    int                     m_count;    ///< The current number of target indices
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline QuickUnion::QuickUnion(const unsigned int nBranches)
{
    m_count = nBranches;
    m_id.resize(nBranches);
    //m_size.resize(nBranches);

    for (unsigned int i = 0; i < nBranches; ++i)
    {
        m_id[i] = i;
        //m_size[i] = 1;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int QuickUnion::Count() const
{
    return m_count;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int QuickUnion::Find(unsigned int p)
{
    while (p != m_id[p])
    {
        m_id[p] = m_id[m_id[p]];
        p = m_id[p];
    }

    return p;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool QuickUnion::Connected(const unsigned int p, const unsigned int q)
{
    return (this->Find(p) == this->Find(q));
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void QuickUnion::Unite(const unsigned int p, const unsigned int q)
{
    const unsigned int rootP(this->Find(p));
    const unsigned int rootQ(this->Find(q));

    m_id[p] = q;

    // TODO finalise implementation of Unite function

    //if (m_size[rootP] < m_size[rootQ])
    //{
    m_id[rootP] = rootQ;
    //    m_size[rootQ] += m_size[rootP];
    //}
    //else
    //{
    //    m_id[rootQ] = rootP;
    //    m_size[rootP] += m_size[rootQ];
    //}

    --m_count;
}

} // namespace lc_content_fast

#endif // LC_QUICK_UNION_H
