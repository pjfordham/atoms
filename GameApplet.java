
//Title:       Atoms
//Version:     
//Copyright:   Copyright (c) 1998
//Author:      Peter Fordham
//Company:     Forward Technologies
//Description: A port of a dodgy Amiga game that me a a few friends found quite addictive.
package forwardtechnologies.atoms.applet;

import java.awt.*;
import java.applet.*;

import forwardtechnologies.atoms.core.*;

public class GameApplet extends Applet
{
	private static final long serialVersionUID = 0;	 
	boolean isStandalone = false;
	private int xSzie;
	private int ySize;
	private int noOfPlayers;
	private int density;
	private boolean highlight;
	//Get a parameter value

	public String getParameter(String key, String def)
	{
		return isStandalone ? System.getProperty(key, def) :
			(getParameter(key) != null ? getParameter(key) : def);
	}

	// Construct the applet
	public GameApplet()
	{
	}

	// Initialize the applet
	public void init()
	{
		try { xSzie       = Integer.parseInt(this.getParameter("XSZIE", "10")); }       catch (Exception e) { e.printStackTrace(); }
		try { ySize       = Integer.parseInt(this.getParameter("YSIZE", "10")); }       catch (Exception e) { e.printStackTrace(); }
		try { noOfPlayers = Integer.parseInt(this.getParameter("PLAYERS", "2")); }      catch (Exception e) { e.printStackTrace(); }
		try { density     = Integer.parseInt(this.getParameter("DESNITY", "1")); }      catch (Exception e) { e.printStackTrace(); }
		try { highlight   =  Boolean.valueOf(this.getParameter("HIGHLIGHT", "true")).booleanValue(); } catch (Exception e) { e.printStackTrace(); }
		this.setLayout(new BorderLayout());
		this.add( new Game(highlight, density, new Board(xSzie,ySize),noOfPlayers),BorderLayout.CENTER);
	}

	// Start the applet
	public void start()
	{
	}

	// Stop the applet
	public void stop()
	{
	}

	// Destroy the applet
	public void destroy()
	{
	}

	// Get Applet information
	public String getAppletInfo()
	{
		return "Atoms The Applet!";
	}

	// Get parameter info
	public String[][] getParameterInfo()
	{
		String pinfo[][] =
		{
				{"XSZIE", "int", ""},
				{"YSIZE", "int", ""},
				{"PLAYERS", "int", ""},
				{"DESNITY", "int", ""},
				{"HIGHLIGHT", "boolean", ""},
		};
		return pinfo;
	}
}

