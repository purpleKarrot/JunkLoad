#ifndef __STREAM_PROCESS__BUILT_IN_OPERATORS__HPP__
#define __STREAM_PROCESS__BUILT_IN_OPERATORS__HPP__

namespace stream_process
{
/**
*   @brief this class sets up the factories for all the built-in operators.
*   it exists to prevent chain_manager from having to know all operators
*/
class chain_manager;
class built_in_operators
{
public:
    built_in_operators( chain_manager& chain_manager_ );

}; // class built_in_operators

} // namespace stream_process

#endif

