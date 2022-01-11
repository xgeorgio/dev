//Title:        Java Graphsearch System - class: GameNode
//Version:      1.0 (BETA)
//Copyright:    Copyright (c) 2000
//Author:       Harris Georgiou
//Company:
//Description:

package jgs;

import java.util.*;

public class GameNode extends GameBoard
{
  public static final int PLAYER_NONE=0, PLAYER_COMPUTER=100, PLAYER_HUMAN=1000;
  public static final int NODE_ROOT=0, NODE_NULL=-1;
  protected static Integer tagCounter=new Integer(GameNode.NODE_ROOT);

  public int  id, player, parent, depth;
  public double cost;
  public HashSet next;

  public GameNode()
  {
    super();
    id=nextTag(); player=GameNode.PLAYER_COMPUTER;
    parent=GameNode.NODE_NULL; depth=0;
    next=new HashSet();
  }

  public GameNode( int cols, int rows )
  {
    super(cols,rows);
    id=nextTag(); player=GameNode.PLAYER_COMPUTER;
    parent=GameNode.NODE_NULL; depth=0;
    next=new HashSet();
    init(cols,rows);
  }

  public GameNode( GameNode other )
  {
    super();
    this.copy(other);
  }

  public synchronized int nextTag()
  {
    int ret=tagCounter.intValue();
    tagCounter=new Integer(tagCounter.intValue()+1);
    return(ret);
  }

  public void copy( Object obj )
  {
    GameNode other=(GameNode)obj;
    super.copy(other);
    id=nextTag();
    player=other.player;  parent=other.parent;
    depth=other.depth;  next=new HashSet(other.next);
  }

  public String toString()
  {
    StringBuffer s=new StringBuffer();

    s.append("id:"+id+"\n");
    s.append("board:\n"+super.toString());
    s.append("player:"+player+"\n");
    s.append("depth:"+depth+"\n");
    s.append("parent:"+parent+"\n");
    s.append("next:"+next.toString()+"\n");
    s.append("cost:"+cost);

    return(s.toString()+"\n");
  }

  public int hashCode()
  {
    return(id);
  }

  public boolean equals( Object obj )
  {
    GameNode other=(GameNode)obj;
    return(this.id==other.id);
  }

  public int nextPlayer()
  {
    return(player);
  }

  //... node.expand() - Game-specific routine ...
  public HashSet expand()
  {
    HashSet nextSet=new HashSet();
    GameNode newNode;
    int   col, row, emptyRow=0, emptyCol=0;

    // find empty tile
    for ( row=0; row<getRows(); row++ )
      for ( col=0; col<getCols(); col++ )
        if (board[row][col]==GameBoard.BV_EMPTY)
          { emptyRow=row; emptyCol=col; col=getCols(); row=getRows(); }

    // move empty tile UP
    if (emptyRow>0)
    {
      newNode=new GameNode(this);
      newNode.parent=id;  //newNode.id=nextTag();
      newNode.depth=depth+1;  newNode.player=nextPlayer();  //newNode.cost=0;
      newNode.board[emptyRow][emptyCol]=newNode.board[emptyRow-1][emptyCol];
      newNode.board[emptyRow-1][emptyCol]=GameBoard.BV_EMPTY;
      nextSet.add(newNode);
    }

    // move empty tile DOWN
    if (emptyRow<getRows()-1)
    {
      newNode=new GameNode(this);
      newNode.parent=id;  //newNode.id=nextTag();
      newNode.depth=depth+1;  newNode.player=nextPlayer();  //newNode.cost=0;
      newNode.board[emptyRow][emptyCol]=newNode.board[emptyRow+1][emptyCol];
      newNode.board[emptyRow+1][emptyCol]=GameBoard.BV_EMPTY;
      nextSet.add(newNode);
    }

    // move empty tile LEFT
    if (emptyCol>0)
    {
      newNode=new GameNode(this);
      newNode.parent=id;  //newNode.id=nextTag();
      newNode.depth=depth+1;  newNode.player=nextPlayer();  //newNode.cost=0;
      newNode.board[emptyRow][emptyCol]=newNode.board[emptyRow][emptyCol-1];
      newNode.board[emptyRow][emptyCol-1]=GameBoard.BV_EMPTY;
      nextSet.add(newNode);
    }

    // move empty tile RIGHT
    if (emptyRow<getCols()-1)
    {
      newNode=new GameNode(this);
      newNode.parent=id;  //newNode.id=nextTag();
      newNode.depth=depth+1;  newNode.player=nextPlayer();  //newNode.cost=0;
      newNode.board[emptyRow][emptyCol]=newNode.board[emptyRow][emptyCol+1];
      newNode.board[emptyRow][emptyCol+1]=GameBoard.BV_EMPTY;
      nextSet.add(newNode);
    }

    next=nextSet;
    return(nextSet);
  }

  //... node.evaluate() - Game-specific routine ...
  public double evaluate()
  {
    int   sum=depth;
    if (board[0][0]!='1')  sum++;
    if (board[0][1]!='2')  sum++;
    if (board[0][2]!='3')  sum++;
    if (board[1][0]!='8')  sum++;
    if (board[1][1]!=GameBoard.BV_EMPTY)  sum++;
    if (board[1][2]!='4')  sum++;
    if (board[2][0]!='7')  sum++;
    if (board[2][1]!='6')  sum++;
    if (board[2][2]!='5')  sum++;
    cost=(double)sum;
    return(cost);
  }

  //... node.isFinal() - Game-specific routine ...
  public boolean isFinal()
  {
    int   sum=0;
    if (board[0][0]!='1')  sum++;
    if (board[0][1]!='2')  sum++;
    if (board[0][2]!='3')  sum++;
    if (board[1][0]!='8')  sum++;
    if (board[1][1]!=GameBoard.BV_EMPTY)  sum++;
    if (board[1][2]!='4')  sum++;
    if (board[2][0]!='7')  sum++;
    if (board[2][1]!='6')  sum++;
    if (board[2][2]!='5')  sum++;
    return(sum==0);
  }

  //... node.isSame() - Game-specific routine ...
  public boolean isSame( GameNode goalNode )
  {
    return(sameBoard((GameBoard)goalNode));
  }

  public static void main(String[] args)
  {
    GameNode g1 = new GameNode(3,1), g2;
    g1.invokedStandalone = true;
    HashSet nextSet=g1.expand();
    Iterator iter=nextSet.iterator();

    while (iter.hasNext())
    {
      g2=(GameNode)iter.next();
      g2.evaluate();
    }
    System.out.println(nextSet);
  }

  private boolean invokedStandalone = false;
}
