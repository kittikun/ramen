#include "ioUtility.h"

namespace boost
{
    namespace filesystem
    {
        template<>
        path& path::append<typename path::iterator>(typename path::iterator begin, typename path::iterator end, const path::codecvt_type& cvt)
        {
            for (; begin != end; ++begin)
                *this /= *begin;
            return *this;
        }
    } // namespace filesystem
} // namespace boost

namespace ramen
{
    namespace ioUtility
    {
        // Return path when appended to from will resolve to same as to
        boost::filesystem::path makeRelativePath(boost::filesystem::path from, boost::filesystem::path to)
        {
            boost::filesystem::path ret;
            boost::filesystem::path::const_iterator itrFrom(from.begin()), itrTo(to.begin());

            from = boost::filesystem::absolute(from);
            to = boost::filesystem::absolute(to);

            // Find common base
            for (boost::filesystem::path::const_iterator toEnd(to.end()), fromEnd(from.end()); itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo; ++itrFrom, ++itrTo) ;

            // Navigate backwards in directory to reach previously found base
            for (boost::filesystem::path::const_iterator fromEnd(from.end()); itrFrom != fromEnd; ++itrFrom) {
                if ((*itrFrom) != ".")
                    ret /= "..";
            }
            // Now navigate down the directory branch
            ret.append(itrTo, to.end());
            return ret;
        }
    } // namespace ioUtility
} // namespace ramen