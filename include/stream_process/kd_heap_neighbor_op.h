#ifndef _KD_Heap_H_
#define _KD_Heap_H_

#include <list> 

#include "stream_op.h"
#include "Priority2DTree.h"

#include "neighbor.h"

namespace stream_process
{
/**
*
* @brief neighbourhood detection operator
*
* NeighbourOp reads the following inputs from the stream:
*   "position"          - point position                    - vec3f          
*   "point_index"       - point index in stream             - uint32_t          
*
* NeighbourOp writes the following outputs into the stream:
*   "list"             - list of neighbours                 - stream_point*[n]   
*   "dist"             - list of neighbour distances        - float[n]          
*   "cnt"              - nb count, number of neighbours     - uint32_t
*   "min_ref_index"    - neighbour with smallest index      - uint32_t
*   "max_ref_index"    - neighbour with largest index       - uint32_t
*
* @author renato pajarola
* @author jonas boesch
*
*/
    
class kd_heap_neighbor_op 
    : public stream_op 
{
public:
    kd_heap_neighbor_op();
    ~kd_heap_neighbor_op();

    virtual void pull_push();
    virtual stream_point* front();
    virtual void pop_front();
    virtual void clear_stage();

    virtual size_t smallest_element();
    virtual size_t smallest_reference();

    virtual size_t size() const;
    
    virtual void setup_stage_0();
    virtual void setup_stage_1();
    virtual void setup_stage_2();

protected:  
    
    Priority2DTree* _heap;
    std::list< Priority2DTree* > L;
    IndexQueue ZQ;
    ExtraQueue Buffer;
    float max_element;
    
    size_t _max_neighbors;
    size_t _pointSizeInBytes;
    
    Priority2DTree::PTreePool _pool;

    rt_struct_member< vec3f >           _position;
    rt_struct_member< uint32_t >        _minReference;
    rt_struct_member< uint32_t >        _maxReference;
    rt_struct_member< uint32_t >        _pointIndex;
    rt_struct_member< stream_point* >   _neighbours;
    rt_struct_member< uint32_t >        _nbCount;
    rt_struct_member< float >           _nbDistances;
    
    rt_struct_member< neighbor >        _new_neighbors;
};


// kd heap nb op options 
struct kd_heap_neighbor_op_options : public op_options
{
    kd_heap_neighbor_op_options();
};


} // namespace stream_process

#endif
