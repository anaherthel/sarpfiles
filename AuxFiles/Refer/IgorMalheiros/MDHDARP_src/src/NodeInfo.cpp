#include "NodeInfo.h"

//metodos da classe DepthData.
//Um objeto desta classe é associado a cada nó do B&C duranto o branching. Esse objeto é recuperado no cut callback.
NodeInfo::NodeInfo(unsigned  int depth) : depth(depth)
{} 

unsigned int NodeInfo::getDepth() const 
{ 
   return this->depth; 
}

