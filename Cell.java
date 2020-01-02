package forwardtechnologies.atoms.core;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

/**
 * A <em>Cell</em> is an object having a graphical representation
 * that can be displayed on the screen and that can interact with the
 * user. A Cell must be wiithin an object that implements the <em>CellContainer</em>
 * interafce. <p>
 *
 * @version  1.0, 22/09/99
 * @author 	 Peter Fordham
 * @see      forwardtechnologies.atoms.core.CellContainer
 */
public class Cell extends Component implements MouseListener, Serializable
{
	static final long serialVersionUID = -7099964165391162481L;
	public static final int HIDDEN = 0, GAMEPLAY = 1, BOARDEDIT = 2;

	private int mode, explodeNumber, currentNumber, colour, density;
	private boolean highlight;
	private Cell north, east, south, west;
	private Image stone, wood;

	/**
	 * Constructs a new cell with in Cell.BOARDEDIT mode
	 */
	public Cell(int $explodeNumber, Cell North, Cell West)
	{
		super();
		this.addMouseListener(this);
		this.mode           = Cell.BOARDEDIT;
		this.explodeNumber  = $explodeNumber;
		this.highlight      = false;
		this.currentNumber  = 0;
		this.colour         = 0;
		this.density        = 0;
		this.north          = North;
		this.east           = null;
		this.south          = null;
		this.west           = West;

		if (north != null) north.setSouth(this);
		if (west  != null)  west.setEast(this);

		stone = getToolkit().getImage("forwardtechnologies/atoms/core/stone.gif");
		wood  = getToolkit().getImage("forwardtechnologies/atoms/core/wood.gif");

	}

	/**
	 * Default getter for north
	 */
	public Cell getNorth() { return north; }

	/**
	 * Default getter for south
	 */
	public Cell getSouth() { return south; }

	/**
	 * Default getter for east
	 */
	public Cell getEast()  { return east; }

	/**
	 * Default getter for west
	 */
	public Cell getWest()  { return west; }

	/**
	 * Default setter for north
	 */
	public void setNorth(Cell value) { north = value; }

	/**
	 * Default setter for south
	 */
	public void setSouth(Cell value) { south = value; }

	/**
	 * Default setter for west
	 */
	public void  setWest(Cell value) {  west = value; }

	/**
	 * Default setter for east
	 */
	public void  setEast(Cell value) {  east = value; }

	/**
	 * Default getter for explodeNumber
	 */
	public int getExplodeNumber() {return explodeNumber;}

	/**
	 * Default getter for mode
	 */
	public int getMode() {return mode;}

	/**
	 * Default setter for mode
	 */
	public void setMode(int value) {mode = value;}

	/**
	 * Default setter for density
	 */
	public void setDensity(int value) {density = value;}

	/**
	 * Default getter for currentNumber
	 */
	public int getCurrentNumber() {return currentNumber;}

	/**
	 * Default getter for cellNumber
	 */
	public int getColour() {return colour;}

	/**
	 * Default getter for highlight
	 */
	public boolean getHighlight() {return highlight;}

	/**
	 * Default setter for highlight
	 */
	public void setHighlight(boolean value)
	{
		if (highlight != value)
		{
			highlight = value;
			if (unstable() && (mode == Cell.GAMEPLAY)) repaint();
		}
	}

	/**
	 * Set explodeNumber and update graphics if required
	 */
	public void setExplodeNumber(int value)
	{
		if (explodeNumber != value)
		{
			explodeNumber = value;
			if (mode == Cell.BOARDEDIT)
			{
				repaint();
				if (north != null) north.calculateExplodeNumber();
				if ( east != null)  east.calculateExplodeNumber();
				if (south != null) south.calculateExplodeNumber();
				if ( west != null)  west.calculateExplodeNumber();
			}
		}
	}

	/**
	 * Set currentNumber and update graphics if required
	 */
	public void setCurrentNumber(int value)
	{
		if (currentNumber != value)
		{
			currentNumber = value;
			if (currentNumber == 0) setColour(0);
			//      if (mode == Cell.GAMEPLAY) repaint();
		}
	}

	/**
	 * Set Colour and update graphics if required
	 */
	public void setColour(int value)
	{
		if (colour != value)
		{
			colour    = value;
			//      if (mode == Cell.GAMEPLAY) repaint();;
		}
	}

	/**
	 * One step incrementer for explodeNumber.
	 */
	public void incExplodeNumber()
	{
		setExplodeNumber( getExplodeNumber() + 1 );
	}

	/**
	 * One step incrementer for currentNumber.
	 */
	public void incCurrentNumber()
	{
		setCurrentNumber(getCurrentNumber() + 1);
	}

	/**
	 * Increment currentNumber and change Colour to Colour
	 */
	public void incAndClr(int Colour)
	{
		setColour(Colour);
		incCurrentNumber();
		paint(this.getGraphics());
		//	repaint();
	}

	/**
	 * Returns true if Cell is one increment away from exploding
	 */
	public boolean unstable()
	{
		return (getCurrentNumber() == (getExplodeNumber() - 1));
	}

	/**
	 * Returns true if Cell is exploding
	 */
	public boolean exploding()
	{
		return ((getCurrentNumber() >= getExplodeNumber()) && ( getExplodeNumber() != 0 ));
	}

	/**
	 * Calculate Cells explodeNumber using it neighbours explodeNumbers
	 */
	public void calculateExplodeNumber()
	{
		int newExplodeNumber = 0;
		if (this.getExplodeNumber() != 0)
		{
			if ((north != null) && (north.getExplodeNumber() != 0)) newExplodeNumber++;
			if (( east != null) && ( east.getExplodeNumber() != 0)) newExplodeNumber++;
			if ((south != null) && (south.getExplodeNumber() != 0)) newExplodeNumber++;
			if (( west != null) && ( west.getExplodeNumber() != 0)) newExplodeNumber++;
			this.setExplodeNumber(newExplodeNumber);
		}
	}

	/**
	 * Manages explosion and spreads excess atoms to it neighbours
	 */
	public boolean explode()
	{
		CellContainer owner = (CellContainer)(this.getParent());
		boolean NotFinished = false;
		if (!owner.getGameOver())
		{
			if (exploding())
			{
				NotFinished = true;
				int Colour = this.getColour();
				setCurrentNumber(getCurrentNumber() - getExplodeNumber());
				paint(this.getGraphics());
				if ((north != null) && (north.getExplodeNumber() != 0)) north.incAndClr(Colour);
				if ((east  != null) && ( east.getExplodeNumber() != 0))  east.incAndClr(Colour);
				if ((south != null) && (south.getExplodeNumber() != 0)) south.incAndClr(Colour);
				if ((west  != null) && ( west.getExplodeNumber() != 0))  west.incAndClr(Colour);
			}
		}
		return NotFinished;
	}

	/**
	 * Resets all the variable members of this Cell to their initial state
	 */
	public void reset()
	{
		setCurrentNumber(0);
		setColour(0);

		if (density != 10)
			if ((Utils.random(density)==0) && (getExplodeNumber() >2))
				setCurrentNumber(Utils.random(getExplodeNumber()-1));
	}

	/**
	 * Sets all the members of this Cell to the member of the given cell.
	 */
	public void set(Cell copy)
	{
		setExplodeNumber(copy.explodeNumber);
		setCurrentNumber(copy.currentNumber);
		setColour(copy.colour);
		setHighlight(copy.highlight);
		setMode(copy.mode);
		setDensity(copy.density);
	}

	/**
	 * React to a mouseClicked event.
	 */
	public void mouseClicked(MouseEvent e)
	{
		// We need to work here to remove dependency on CellContainer interface
		CellContainer owner = (CellContainer)(this.getParent());
		switch (mode)
		{
		case Cell.GAMEPLAY :
			if ( ( ( currentNumber == 0 ) || ( colour == owner.getCurrentPlayer() ) ) && (explodeNumber != 0) && (!owner.getGameOver()))
			{
				this.incAndClr(owner.getCurrentPlayer());
				owner.recalculateBoard();
				owner.nextPlayer();
			}
			break;

		case Cell.BOARDEDIT :
			if (explodeNumber == 0) setExplodeNumber(4);
			else                    setExplodeNumber(0);
			if (north != null) north.calculateExplodeNumber();
			if (south != null) south.calculateExplodeNumber();
			if (east  != null)  east.calculateExplodeNumber();
			if (west  != null)  west.calculateExplodeNumber();
			repaint();
			break;
		}
	}

	public void mousePressed(MouseEvent e)  {;}
	public void mouseReleased(MouseEvent e) {;}
	public void mouseEntered(MouseEvent e)  {;}
	public void mouseExited(MouseEvent e)   {;}

	/**
	 * Draw a graphical representation Of Cell's current state.
	 */
	public void paint (Graphics g)
	{
		int width, height;

		width  = this.getSize().width  -1;
		height = this.getSize().height -1;

		if (explodeNumber == 0)
			// Draw the default gray square
		{
			g.drawImage(stone,0,0,width+1,height+1,null);
		}
		else if (mode == Cell.BOARDEDIT)
			// Draw any empty sqaure
		{
			g.setColor(Utils.intToColor(explodeNumber));
			g.fillRect(0,0,width,height);

			g.setColor(Color.black);
			g.drawRect(0,0,width,height);
		}
		else if (currentNumber>=explodeNumber)
			// Draw eplosion graphic in Cell
		{
			g.drawImage(wood,1,1,width,height,null);

			g.setColor(Color.black);
			g.drawRect(0,0,width,height);

			g.setColor(Color.yellow);
			g.drawLine(0,0,width,height);           // /
			g.drawLine(0,height,width,0);           // \
			g.drawLine(width/2,0,width/2,height);   // |
			g.drawLine(0,height/2,width,height/2);  // -
		}
		else if (currentNumber == 0)
			// Draw any empty sqaure
		{
			g.drawImage(wood,1,1,width,height,null);

			g.setColor(Color.black);
			g.drawRect(0,0,width,height);
		}
		else if (currentNumber == 1)
			// Draw one atom
		{
			g.drawImage(wood,1,1,width,height,null);

			g.setColor(Color.black);
			g.drawRect(0,0,width,height);

			g.setColor(Utils.intToColor(colour));
			g.fillOval(width/2 - width/8,height/2 - height/8,width/4,height/4);

			if (highlight && (explodeNumber == 2))
			{
				g.setColor(Color.yellow);
				g.drawOval(width/2 - width/8,height/2 - height/8,width/4,height/4);
			}
		}
		else if (currentNumber == 2)
			// Draw two atoms
		{
			g.drawImage(wood,1,1,width,height,null);

			g.setColor(Color.black);
			g.drawRect(0,0,width,height);

			g.setColor(Utils.intToColor(colour));
			g.fillOval(width/4 - width/8,height/2 - height/8,width/4,height/4);
			g.fillOval(width*3/4 - width/8,height/2 - height/8,width/4,height/4);

			if (highlight && (explodeNumber == 3))
			{
				g.setColor(Color.yellow);
				g.drawOval(width/4 - width/8,height/2 - height/8,width/4,height/4);
				g.drawOval(width*3/4 - width/8,height/2 - height/8,width/4,height/4);
			}
		}
		else if (currentNumber == 3)
			// Draw three atoms
		{
			g.drawImage(wood,1,1,width,height,null);

			g.setColor(Color.black);
			g.drawRect(0,0,width,height);

			g.setColor(Utils.intToColor(colour));
			g.fillOval(width/2 - width/8,height/4 - height/8,width/4,height/4);
			g.fillOval(width/4 - width/8,height*3/4 - height/8,width/4,height/4);
			g.fillOval(width*3/4 - width/8,height*3/4 - height/8,width/4,height/4);

			if (highlight && (explodeNumber == 4))
			{
				g.setColor(Color.yellow);
				g.drawOval(width/2 - width/8,height/4 - height/8,width/4,height/4);
				g.drawOval(width/4 - width/8,height*3/4 - height/8,width/4,height/4);
				g.drawOval(width*3/4 - width/8,height*3/4 - height/8,width/4,height/4);
			}
		}
	}

}