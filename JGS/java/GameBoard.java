//Title:        Java Graphsearch System - class: GameBoard
//Version:      1.0 (BETA)
//Copyright:    Copyright (c) 2000
//Author:       Harris Georgiou
//Company:
//Description:

package jgs;

public class GameBoard extends Object
{
  public static char  BV_EMPTY='.', BV_FILLED='o';

  public char board[][];
  protected int nCols, nRows;

  public GameBoard()
  {
    init(0,0);
  }

  public GameBoard( int cols, int rows )
  {
    init(cols,rows);
  }

  public GameBoard( GameBoard other )
  {
    this.copy(other);
  }

  public void fill( char fillchar )
  {
    int i, j;

    for ( i=0; i<nRows; i++ )
      for ( j=0; j<nCols; j++ )
        board[i][j]=fillchar;
  }

  public int getRows()
  {
    return(nRows);
  }

  public int getCols()
  {
    return(nCols);
  }

  public char getXY( int col, int row )
  {
    return(board[row][col]);
  }

  public void setXY( int col, int row, char ch )
  {
    board[row][col]=ch;
  }

  protected Object clone() throws CloneNotSupportedException
  {
    return(new GameBoard(this));
  }

  public String toString()
  {
    StringBuffer s=new StringBuffer();
    int i, j;

    for ( i=0; i<nRows; i++ )
    {
      for ( j=0; j<nCols; j++ )
        s.append(board[i][j]);
      s.append("\n");
    }

    return(s.toString());
  }

  public int hashCode()
  {
    return(this.toString().hashCode());
  }

  public boolean sameBoard( GameBoard other )
  {
    int   row, col;

    for ( row=0; row<getRows(); row++ )
      for ( col=0; col<getCols(); col++ )
        if (board[row][col]!=other.board[row][col])  return(false);

    return(true);
  }

  public boolean equals( Object obj )
  {
    GameBoard other=(GameBoard)obj;
    return(sameBoard(other));
  }

  public void init( int cols, int rows )
  {
    int i, j;

    if ((cols>0)&&(rows>0))
    {
      nRows=rows;  nCols=cols;
      board=new char[nRows][nCols];
      fill(BV_EMPTY);
    }
  }

  public void copy( Object obj )
  {
    GameBoard other=(GameBoard)obj;
    int i, j, cols, rows;

    cols=other.getCols();  rows=other.getRows();
    if ((cols>0)&&(cols>0))
    {
      nRows=rows;  nCols=cols;
      board=new char[nRows][nCols];
      for ( i=0; i<nRows; i++ )
        for ( j=0; j<nCols; j++ )
          board[i][j]=other.board[i][j];
    }
  }

  public static void main(String[] args)
  {
    GameBoard gameBoard = new GameBoard(3,3);
    gameBoard.fill('x');
    gameBoard.invokedStandalone = true;

    System.out.println(gameBoard);
    System.out.println(gameBoard.hashCode());

    GameBoard g2=new GameBoard(gameBoard);
    System.out.println(g2);
    System.out.println(gameBoard.equals(g2));

    g2.setXY(1,1,'o');
    System.out.println(g2);
    System.out.println(gameBoard.equals(g2));
  }

  private boolean invokedStandalone = false;
}