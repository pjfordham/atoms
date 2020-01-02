package forwardtechnologies.atoms.event;

public interface ScoreChangeGenerator
{
	public void addScoreChangeListener(ScoreChangeListener l);
	public void removeScoreChangeListener(ScoreChangeListener l);
	public void fireScoreChangeEvent();
}
