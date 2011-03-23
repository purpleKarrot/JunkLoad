#ifndef __STREAM_PROCESS__NORMAL_TEST__HPP__
#define __STREAM_PROCESS__NORMAL_TEST__HPP__

namespace stream_process
{

template< typename T >
class normal_test
{
public:
    bool operator()( const T& normal_, std::string& problem_ ) const
    {
        typedef typename T::value_type  value_type;
        bool ok = true;
        
        // component test...
        typename T::const_iterator
            it      = normal_.begin(),
            it_end  = normal_.end();
        for( ; ok && it != it_end; ++it )
        {
            const value_type& nc = *it;
            if ( std::isnan( nc ) || std::isinf( nc ) )
            {
                ok          = false;
                problem_    = "normal contains nan or inf components.";
            }
        }
        
        if ( normal_ == static_cast< value_type >( 0 ) )
        {
            ok          = false;
            problem_    = "normal is zero.";
        }
        
        const value_type s = normal_.length();
        if ( fabs( s - 1.0 ) > 0.0001 )
        {
            ok          = false;
            problem_    = "normal is not unit length.";
            std::cout << "normal length = " << s << "." << std::endl;
            
        }
        return ok;
    }

}; // class vector_test

} // namespace stream_process

#endif

