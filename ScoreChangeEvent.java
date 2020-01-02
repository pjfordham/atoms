package forwardtechnologies.atoms.event;

import java.util.*;

public class ScoreChangeEvent extends EventObject
{
	private static final long serialVersionUID = 0;	 
	private int[] scores;
	public ScoreChangeEvent(Object source, int[] $scores)
	{
		super(source);
		scores = $scores;
	}

	public int[] getScores() { return scores; }
} 