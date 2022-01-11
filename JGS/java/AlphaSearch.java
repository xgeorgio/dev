//Title:        Java Graphsearch System - class: AlphaSearch
//Version:      1.0 (BETA)
//Copyright:    Copyright (c) 2000
//Author:       Harris Georgiou
//Company:
//Description:

package jgs;

import java.util.*;

public class AlphaSearch extends Object
{
  public GameNode root, goal;
  public TreeSet openSet, openCostSet, closedSet;
  public TreeMap nodeSet;

  public AlphaSearch()
  {
    init();
  }

  public AlphaSearch( GameNode rootNode )
  {
    init(); setStart(rootNode);
  }

  public AlphaSearch( AlphaSearch other )
  {
    this.copy(other);
  }

  protected Object clone() throws CloneNotSupportedException
  {
    return(new AlphaSearch(this));
  }

  public String toString()
  {
    return("root:"+root+"\ngoal:"+goal+"\nnodeSet:"+nodeSet+"\nopenCosts:"+openCostSet);
  }

  public int hashCode()
  {
    return(nodeSet.hashCode());
  }

  public boolean equals( Object obj )
  {
    AlphaSearch other=(AlphaSearch)obj;
    return(this.toString().equals(other.toString()));
  }

  public void copy( AlphaSearch other )
  {
    root=new GameNode(other.root);
    goal=new GameNode(other.goal);
    openSet=(TreeSet)other.openSet.clone();
    closedSet=(TreeSet)other.closedSet.clone();
    openCostSet=(TreeSet)other.openCostSet.clone();
    nodeSet=(TreeMap)other.nodeSet.clone();
  }

  public void init()
  {
    root=null;
    goal=null;
    openSet=new TreeSet();
    closedSet=new TreeSet();
    openCostSet=new TreeSet();
    nodeSet=new TreeMap();
  }

  public boolean assertNode( GameNode newNode )
  {
    boolean res=true;
    newNode.evaluate();
    nodeSet.put(new Integer(newNode.id),newNode);
    openSet.add(new Integer(newNode.id));
    openCostSet.add(new CostNode(newNode.id,newNode.cost));
    return(res);
  }

  public boolean closeNode( GameNode remNode )
  {
    boolean res=true;
    openSet.remove(new Integer(remNode.id));
    openCostSet.remove(new CostNode(remNode.id,remNode.cost));
    closedSet.add(new Integer(remNode.id));
    return(res);
  }

  public void setStart( GameNode rootNode )
  {
    root=rootNode;
    if (root!=null)  assertNode(root);
  }

  public void setGoal( GameNode goalNode )
  {
    goal=goalNode;
  }

  public boolean hasParent( GameNode nCurrent, GameNode nToCheck )
          throws NullPointerException, ClassCastException
  {
    // use nodeSet to check parents backwards to the root
    if ((nCurrent==null)||(nToCheck==null))  throw(new NullPointerException());
    GameNode  ncurr=nCurrent;

    while (ncurr!=null)
    {
      if (ncurr.isSame(nToCheck))  return(true);
      else  ncurr=(GameNode)nodeSet.get(new Integer(ncurr.parent));
    }

    return(false);
  }

  public int selectNext()
  {
      CostNode bestCNode=(CostNode)openCostSet.first();
      return(bestCNode.id);
  }

  public boolean searchFinal()
  {
    GameNode  currNode, nextNode;
    HashSet   nextSet;
    Iterator  iter, nextIter;
    boolean   finished=false;
    Integer   nodeID;
    CostNode  cnodeID;

    //if (goal==null)  { System.out.println("ERROR: goal is empty"); return(false); }

    while (!(openSet.isEmpty()||openCostSet.isEmpty()))
    {
      currNode=(GameNode)nodeSet.get(new Integer(selectNext()));
      //System.out.println("Current cost = "+currNode.cost);
      //System.out.println("sets (T/O/Oc/C): "+nodeSet.size()+" "+openSet.size()+" "+openCostSet.size()+" "+closedSet.size());
      //try { System.in.read(); } catch(Exception e) {}

      if (currNode.isFinal())   //-> terminate search when on solution
      {
        goal=currNode;
        System.out.println("FINISHED:\n"+currNode);
        return(true);
      }

      closeNode(currNode);
      nextSet=currNode.expand();
      nextIter=nextSet.iterator();
      while (nextIter.hasNext())
      {
        try
        {
          nextNode=(GameNode)nextIter.next();
          if (!hasParent(currNode,nextNode))  assertNode(nextNode);
        }
        catch (NullPointerException e)  { return(false); }
        catch (ClassCastException e)  { return(false); }
      }
    }

    System.out.println("FAILED: openSet is empty");
    return(false);    // openSet empty
  }

  public boolean searchGoal()
  {
    GameNode  currNode, nextNode;
    HashSet   nextSet;
    Iterator  iter, nextIter;
    boolean   finished=false;

    if (goal==null)  { System.out.println("ERROR: goal is empty"); return(false); }

    while (!(openSet.isEmpty()||openCostSet.isEmpty()))
    {
      currNode=(GameNode)nodeSet.get(new Integer(selectNext()));
      //System.out.println("Current cost = "+currNode.cost);

      if (goal.isSame(currNode))    //-> terminate search when on specific node
      {
        goal=currNode;
        System.out.println("GOAL REACHED:\n"+currNode);
        return(true);
      }

      nextSet=currNode.expand();
      closeNode(currNode);
      nextIter=nextSet.iterator();
      while (nextIter.hasNext())
      {
        try
        {
          nextNode=(GameNode)nextIter.next();
          if (!hasParent(currNode,nextNode))  assertNode(nextNode);
        }
        catch (NullPointerException e)  { return(false); }
        catch (ClassCastException e)  { return(false); }
      }
    }

    System.out.println("FAILED: openSet is empty");
    return(false);    // openSet empty
  }

  public void solution()  throws NullPointerException, ClassCastException
  {
    // use nodeSet to check parents backwards to the root
    GameNode  currNode=goal;
    GameBoard currBoard;
    int       searchDepth=0;

    System.out.println("SOLUTION:");
    while (currNode!=null)   // go up to the root
    {
      currBoard=new GameBoard((GameBoard)currNode);
      System.out.println(currBoard);
      currNode=(GameNode)nodeSet.get(new Integer(currNode.parent));
      if (currNode!=null)
      {
        searchDepth++;
        try { System.in.read(); } catch(Exception e) {}
        System.out.println("from:");
      }
    }
    System.out.println("Search depth: "+searchDepth+" moves");
  }


  public static void main(String[] args)
  {
    System.out.println("JGS-AlphaSearch, v1.0B - Harris Georgiou (c) 2000.");
    System.out.println("--------------------------------------------------\n");

    AlphaSearch AlphaSearch = new AlphaSearch();
    AlphaSearch.invokedStandalone = true;
    GameNode root=new GameNode(3,3);

    root.setXY(0,0,'2'); root.setXY(1,0,'8'); root.setXY(2,0,'3');
    root.setXY(0,1,'1'); root.setXY(1,1,'6'); root.setXY(2,1,'4');
    root.setXY(0,2,'7'); root.setXY(1,2,GameBoard.BV_EMPTY); root.setXY(2,2,'5');

    AlphaSearch.setStart(root);

    System.out.println("STARTING NODE IS:\n"+root);
    System.out.println("...<press ENTER to begin>...");
    try { System.in.read(); } catch (Exception e) {}

    if (AlphaSearch.searchFinal())  AlphaSearch.solution();
  }

  private boolean invokedStandalone = false;
}