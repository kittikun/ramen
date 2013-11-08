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
        // Return path when appended to a_From will resolve to same as a_To
        boost::filesystem::path makeRelativePath(boost::filesystem::path a_From, boost::filesystem::path a_To) 
        { 
            boost::filesystem::path ret; 
            boost::filesystem::path::const_iterator itrFrom(a_From.begin()), itrTo(a_To.begin()); 
            
            a_From = boost::filesystem::absolute(a_From); 
            a_To = boost::filesystem::absolute(a_To); 
            
            // Find common base
            for (boost::filesystem::path::const_iterator toEnd(a_To.end()), fromEnd(a_From.end()); itrFrom != fromEnd && itrTo != toEnd && *itrFrom == *itrTo; ++itrFrom, ++itrTo) ; 
            
            // Navigate backwards in directory to reach previously found base
            for (boost::filesystem::path::const_iterator fromEnd(a_From.end()); itrFrom != fromEnd; ++itrFrom) {
                if ((*itrFrom) != ".") 
                    ret /= "..";
            }
            // Now navigate down the directory branch
            ret.append(itrTo, a_To.end()); 
            return ret;
        }
    } // namespace ioUtility
    
} // namespace ramen
