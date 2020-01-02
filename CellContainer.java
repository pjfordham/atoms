package forwardtechnologies.atoms.core;

/**
 * The <em>CellConatins</em> interface specifies some methods that
 * the Cell object must be able to call on its awt parent.
 *
 * @version  1.0, 22/09/99
 * @author 	Peter Fordham
 * @see      forwardtechnologies.atoms.core.Cell
 */
public interface CellContainer
{

	/**
	 * Ascertain whether the game involving the Cells is finished
	 */
	public boolean getGameOver();

	/**
	 * Get the currentPlayer for the game involving the Cells
	 */
	public int     getCurrentPlayer();

	/**
	 * Update all the Cells in the current game
	 */
	public void    recalculateBoard();

	/**
	 * Advance the game to the next player
	 */
	public void    nextPlayer();

}