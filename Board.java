package forwardtechnologies.atoms.core;

import java.io.*;
import java.util.*;
import java.awt.*;

import forwardtechnologies.atoms.event.*;

/**
 * A <em>Board</em> is an object encapsulating a two dinesional array
 * of Cell obejects.
 *
 * @version  1.0, 22/09/99
 * @author 	 Peter Fordham
 * @see      forwardtechnologies.atoms.core.Cell
 */
public class Board extends Panel implements Serializable, CellContainer, ScoreChangeGenerator
{
	static final long serialVersionUID = 3235325945414479919L;

	private Cell data[][];
	private int xSize, ySize, noOfPlayers, currentPlayer;
	private boolean gameOver;

	/**
	 * Constructs a new Board object for Board Editing xSize x ySize.
	 * In order to use this for Gameplay makePlayable must be called.
	 */
	public Board(int $xSize, int $ySize)
	{
		Cell one, two;

		this.noOfPlayers   = 0;
		this.xSize         = $xSize;
		this.ySize         = $ySize;
		this.data          = new Cell [this.xSize][this.ySize];
		this.currentPlayer = 0;
		this.gameOver      = false;

		this.setLayout(new GridLayout(ySize,xSize,0,0));

		for (int b=0;b<ySize;b++)
			for (int a=0;a<xSize;a++)
			{
				try {one = this.data[a-1][b];} catch (ArrayIndexOutOfBoundsException e) { one = null; }
				try {two = this.data[a][b-1];} catch (ArrayIndexOutOfBoundsException e) { two = null; }
				this.data[a][b] = new Cell( (( (a == 0) || (b == 0) || (a == (xSize-1)) || (b == (ySize-1)) ) ? 0 : 1),one,two);
				this.add(this.data[a][b]);
			}

		this.calculateBoard();
	}

	/**
	 * Default getter for gameOver
	 */
	public boolean getGameOver() { return gameOver;  }

	/**
	 * Default setter for gameOver
	 */
	public void setGameOver(boolean value) { gameOver = value; }

	/**
	 * Default getter for currentPlayer
	 */
	public int getCurrentPlayer() { return currentPlayer;  }

	/**
	 * Default setter for currentPlayer
	 */
	public void setCurrentPlayer(int value) { currentPlayer = value; }

	/**
	 * Default getter for noOfPlayer
	 */
	public int getNoOfPlayer() { return noOfPlayers;   }

	/**
	 * Default setter for noOfPlayer
	 */
	public void setNoOfPlayers(int value) { noOfPlayers = value;  }

	/**
	 * Default getter for mode
	 */
	public int getMode() { return data[0][0].getMode(); }

	/**
	 * Forces each Cell in the Board to recalculate its explodeNumber
	 */
	public void calculateBoard()
	{
		for (int a=0;a<xSize;a++)
			for (int b=0;b<ySize;b++)
				data[a][b].calculateExplodeNumber();
	}

	/**
	 * Forces each Cell in the Board to update its gameplay state.
	 * i.e. if currentNumber > explodeNumber explode the cell and
	 * update the surrounding ones.
	 */
	public void recalculateBoard()
	{
		boolean NOTFINISHED = true;

		while (NOTFINISHED)
		{
			NOTFINISHED = false;

			for (int a=0;a<xSize;a++)
				for (int b=0;b<ySize;b++)
					if (data[a][b].explode())
						NOTFINISHED = true;

			fireScoreChangeEvent();
		}
	}

	/**
	 * Forces each Cell in the Board to reset to default values, also update scores
	 * and reset currentPlayer to 1.
	 */
	public void reset()
	{
		for (int a=0;a<xSize;a++)
			for (int b=0;b<ySize;b++)
				data[a][b].reset();

		setCurrentPlayer(1);
		fireScoreChangeEvent();
	}

	/**
	 * Make an editable Board object playable
	 */
	public void makePlayable(int $noOfPlayers, int $density, boolean $highlight)
	{
		for (int a=0;a<xSize;a++)
			for (int b=0;b<ySize;b++)
			{
				data[a][b].setMode(Cell.GAMEPLAY);
				data[a][b].setDensity($density);
				data[a][b].setHighlight($highlight);
				data[a][b].reset();
			}

		setCurrentPlayer(1);
		setNoOfPlayers($noOfPlayers);
		fireScoreChangeEvent();
	}

	/**
	 * Get next player
	 */
	public void nextPlayer()
	{
		currentPlayer = ((Game)this.getParent()).getNextPlayer();
	}

	private Vector<ScoreChangeListener> scoreChangeListeners = new Vector<ScoreChangeListener>();

	/**
	 * ScoreChangeEvent handling method
	 */
	public void addScoreChangeListener(ScoreChangeListener l)
	{
		scoreChangeListeners.addElement(l);
	}

	/**
	 * ScoreChangeEvent handling method
	 */
	public void removeScoreChangeListener(ScoreChangeListener l)
	{
		scoreChangeListeners.removeElement(l);
	}

	/**
	 * ScoreChangeEvent handling method
	 */
	public void fireScoreChangeEvent()
	{
		Enumeration<ScoreChangeListener> listeners = scoreChangeListeners.elements();
		int[] scores = new int[noOfPlayers];
		for (int a=0;a<noOfPlayers;a++)
			scores[a] = 0;

		for (int a=0;a<xSize;a++)
			for (int b=0;b<ySize;b++)
				scores[data[a][b].getColour()]+=data[a][b].getCurrentNumber();

		ScoreChangeEvent e = new ScoreChangeEvent(this,scores);
		while (listeners.hasMoreElements())
		{
			listeners.nextElement().scoreChangePerformed(e);
		}
	}



}