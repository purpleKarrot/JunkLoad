#ifndef __STREAM_PROCESS__REEB_LINK_ELEMENT_SA__HPP__
#define __STREAM_PROCESS__REEB_LINK_ELEMENT_SA__HPP__

#include <stream_process/stream_process_types.hpp>


namespace stream_process
{
template< typename sp_types_t >
class reeb_node_sa;

template< typename sp_types_t >
class reeb_link_sa;

template< typename sp_types_t >
class reeb_link_element_sa
{
public:
    STREAM_PROCESS_TYPES
    
    typedef reeb_link_sa< sp_types_t >             reeb_link;
    typedef reeb_node_sa< sp_types_t >             reeb_node;
    typedef reeb_link_element_sa< sp_types_t >     reeb_link_element;
    typedef std::list< reeb_link_element >         reeb_link_element_iterator;

	reeb_link_element_sa(
        reeb_link* this_link_,
        reeb_node* this_node_,
        reeb_link_element* upper_element_,
        reeb_link_element* lower_element_)
        : this_link( this_link_ )
        , this_node( this_node_ )
        , upper_element( upper_element_ )
        , lower_element( lower_element_ ) 
    {}

	void set_this_link(reeb_link *this_link) { this->this_link = this_link; }
	void set_node_ptr(reeb_node * this_node) { this->this_node = this_node; } 
	void set_upper_element(reeb_link_element *upper_element) { this->upper_element = upper_element; }
	void set_lower_element(reeb_link_element *lower_element) { this->lower_element = lower_element; }
	void set_this_it (typename std::list <reeb_link_element> :: iterator this_it) { this->this_it = this_it; }
	
	reeb_link* get_this_link() { return this_link; }
	reeb_node* get_node_ptr() { return this_node; }
	reeb_link_element* get_upper_element() { return upper_element; }
	reeb_link_element* get_lower_element() { return lower_element; }
	typename std::list <reeb_link_element> :: iterator get_this_it() { return this_it; }

protected:
	reeb_link*              this_link;
	reeb_node*              this_node;
	reeb_link_element*      upper_element;
	reeb_link_element*      lower_element;

	typename std::list< reeb_link_element >::iterator this_it;

}; // class reeb_link_element_sa

} // namespace stream_process

#endif

