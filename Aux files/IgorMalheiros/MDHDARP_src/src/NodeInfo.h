#ifndef NODE_INFO_H_
#define NODE_INFO_H_

#include <ilcplex/ilocplex.h>

/** A subclass of IloCplex::MIPCallbackI::NodeData that stores data related to the node **/
class NodeInfo : public IloCplex::MIPCallbackI::NodeData 
{

   //Depth of node at which this data is stored. 
   unsigned int const depth;

  public: 

      NodeInfo(unsigned int idepth); 

      unsigned int getDepth() const;
};

#endif
