package forwardtechnologies.atoms.event;

import java.util.*;

public class GameOverEvent extends EventObject
{
	private static final long serialVersionUID = 0;	 
	private int currentPlayer;	
	public GameOverEvent(Object source, int $currentPlayer)
	{
		super(source);
		currentPlayer = $currentPlayer;
	}

	public int getCurrentPlayer() { return currentPlayer; }
} 