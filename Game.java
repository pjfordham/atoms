package forwardtechnologies.atoms.core;

import java.io.*;
import java.awt.*;

import forwardtechnologies.messageboxes.*;
import forwardtechnologies.atoms.event.*;

public class Game extends Panel implements Serializable, GameOverListener
{
	static final long serialVersionUID = 3235325945414467919L;

	private Board  currentBoard;
	private Scores scoreBoard;
	//private Label  statusBar;

	private boolean gameOver;
	private int currentPlayer;

	public boolean getGameOver() { return gameOver; }
	public int getCurrentPlayer() { return currentPlayer;  }

	public Game(boolean $highlight, int $density, Board $currentBoard, int $noOfPlayers)
	{
		super();

		currentBoard  = $currentBoard;
		scoreBoard    = new Scores($noOfPlayers);
		//statusBar     = new Label("Welcome To Atoms!");
		gameOver      = false;
		currentPlayer = 1;

		// Convert Board object from editable board to gamplay board
		currentBoard.makePlayable($noOfPlayers + 1,$density,$highlight);

		scoreBoard.addGameOverListener(this);
		currentBoard.addScoreChangeListener(scoreBoard);

		BorderLayout MyBorder = new BorderLayout();
		this.setLayout(MyBorder);
		this.add(scoreBoard,   BorderLayout.EAST);
		//this.add(statusBar,    BorderLayout.SOUTH);
		this.add(currentBoard, BorderLayout.CENTER);
	}

	public void gameOverPerformed(GameOverEvent e)
	{
		((GameOverGenerator)e.getSource()).removeGameOverListener(this);
		currentBoard.setGameOver(true);
		(new MessageBox(this.getParent(),"Game Over","Congratulations Player " + e.getCurrentPlayer() + " you win!")).setVisible(true);
	}

	public int getNextPlayer()
	{
		return scoreBoard.getNextPlayer();
	}

	public void restart()
	{
		currentBoard.setCurrentPlayer(1);
		scoreBoard.setCurrentPlayer(1);
		currentBoard.reset();
	}

}
