package forwardtechnologies.atoms.event;

public interface GameOverGenerator
{
	public void addGameOverListener(GameOverListener l);
	public void removeGameOverListener(GameOverListener l);
	public void fireGameOverEvent();
}
