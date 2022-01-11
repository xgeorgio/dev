//Title:        Java Graphsearch System - class: CostNode
//Version:      1.0 (BETA)
//Copyright:    Copyright (c) 2000
//Author:       Harris Georgiou
//Company:
//Description:

package jgs;

public class CostNode extends Object implements Comparable
{
  public int  id;
  public double cost;

  public CostNode()
  {
    id=0;  cost=0;
  }

  public CostNode( int nodeid, double nodecost )
  {
    id=nodeid; cost=nodecost;
  }

  public CostNode( CostNode other )
  {
    id=other.id; cost=other.cost;
  }

  public int hashCode()
  {
    return(id);
  }

  public boolean equals( Object obj )
  {
    CostNode other=(CostNode)obj;
    return((cost==other.cost)&&(id==other.id));
  }

  public int compareTo( Object obj )
  {
    CostNode other=(CostNode)obj;

    // implement normal ordering (ascending)
    if (cost<other.cost)  return(-1);
    else if (cost==other.cost)
    {
      if (id<other.id)  return(-1);
      else if (id==other.id)  return(0);
      else /*if (id>other.id)*/  return(1);
    }
    else /*if (cost>other.cost)*/  return(1);
  }
}
