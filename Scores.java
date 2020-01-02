package forwardtechnologies.atoms.core;

import java.awt.*;
import java.io.*;
import java.util.*;

import forwardtechnologies.atoms.event.*;

/**
 * A <em>Scores</em> object manages the current scores for a
 * Board object.
 *
 * @version 	1.0, 22/09/99
 * @author 	  Peter Fordham
 */
public class Scores extends Panel implements Serializable, GameOverGenerator, ScoreChangeListener
{
	static final long serialVersionUID = -8472335815307340082L;

	private GridLayout gridLayout;
	private BorderLayout borderLayout = new BorderLayout();
	private Panel panel1 = new Panel();

	int[] totals;
	private int currentPlayer = 1;
	private int noOfPlayers;
	private int tally = 0;
	Label scoresPlayerTitle = new Label("Player",Label.CENTER);
	Label scoresScoreTitle  = new Label("Score",Label.CENTER);
	Label scoresPlayers[], scoresScores[];

	public Scores(int _noOfPlayers)
	{
		super();
		noOfPlayers = _noOfPlayers;
		gridLayout = new GridLayout(noOfPlayers+1,2,0,5);
		this.setLayout(borderLayout);
		this.add(panel1,BorderLayout.NORTH);
		panel1.setLayout(gridLayout);
		panel1.setBackground(Color.lightGray);
		panel1.setForeground(Color.white);

		scoresPlayerTitle.setFont(new Font("Serif",Font.PLAIN,18));
		scoresScoreTitle.setFont(new Font("Serif",Font.PLAIN,18));
		panel1.add(scoresPlayerTitle);
		panel1.add(scoresScoreTitle);

		scoresPlayers = new Label[noOfPlayers];
		scoresScores  = new Label[noOfPlayers];

		for (int i=0;i<noOfPlayers;i++)
		{
			scoresPlayers[i] = new Label("Player " + (i+1),Label.CENTER);
			scoresScores[i]  = new Label("0",Label.CENTER);
			scoresPlayers[i].setForeground(Utils.intToColor(i+1));
			scoresScores[i].setForeground(Utils.intToColor(i+1));

			panel1.add(scoresPlayers[i]);
			panel1.add(scoresScores[i]);
		}
		scoresPlayers[0].setBackground(Utils.intToColor(1));
		scoresPlayers[0].setForeground(Color.lightGray);

	}

	public void scoreChangePerformed(ScoreChangeEvent e)
	{
		totals = e.getScores();
		int playersLeft = 0;
		for (int i=0;i<noOfPlayers;i++)
		{
			scoresScores[i].setText( (new Integer(totals[i+1])).toString() );
			scoresScores[i].setForeground(Utils.intToColor(i+1));
			playersLeft = playersLeft + ((totals[i+1] != 0 )? 1 : 0 );
		}
		if ((playersLeft == 1) && (totals[1] != 1)) fireGameOverEvent();
	}

	public int getNextPlayer()
	{
		int temp = currentPlayer;
		do
		{
			temp++;
			if (temp == (noOfPlayers+1) )
				temp = 1;
		}  while ((totals[temp] == 0) && (tally >= (noOfPlayers)) );

		setCurrentPlayer(temp);

		if (tally < (noOfPlayers)) tally++;
		return currentPlayer;
	}

	public void setCurrentPlayer(int value)
	{
		scoresPlayers[currentPlayer-1].setBackground(Color.lightGray);
		scoresPlayers[currentPlayer-1].setForeground(Utils.intToColor(currentPlayer));
		currentPlayer = value;
		scoresPlayers[currentPlayer-1].setBackground(Utils.intToColor(currentPlayer));
		scoresPlayers[currentPlayer-1].setForeground(Color.lightGray);
	}

	private Vector<GameOverListener> gameOverListeners = new Vector<GameOverListener>();

	public synchronized void addGameOverListener(GameOverListener l)
	{
		gameOverListeners.addElement(l);

	}
	public synchronized void removeGameOverListener(GameOverListener l)
	{
		gameOverListeners.removeElement(l);
	}
	public synchronized void fireGameOverEvent()
	{
		Enumeration<GameOverListener> listeners = gameOverListeners.elements();
		GameOverEvent e = new GameOverEvent(this,currentPlayer);
		while (listeners.hasMoreElements())
		{
			listeners.nextElement().gameOverPerformed(e);
		}
	}
}
