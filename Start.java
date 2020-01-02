// Title:       Atoms
// Version:     1.0J
// Copyright:   Copyright (c) 1999
// Author:      Peter Fordham
// Company:     Forward Technologies
// Description: A port of a dodgy Amiga game that me
//              and few freinds found quite addictive.

package forwardtechnologies.atoms.application;

import java.awt.*;

/**
 * <em>Start</em> is a stub class which
 * constructs an instance of the Main Frame
 * and displays it.
 *
 * @version 	1.0, 22/09/99
 * @author 	Peter Fordham
 */
public class Start
{

	/**
	 * The main method to allow the class to
	 * be run be any JVM.
	 */
	public static void main(String[] args)
	{
		Frame frame = new Main();
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		Dimension frameSize  = frame.getSize();

		// Center the window
		if (frameSize.height > screenSize.height)
			frameSize.height = screenSize.height;
		if (frameSize.width > screenSize.width)
			frameSize.width = screenSize.width;
		frame.setLocation((screenSize.width - frameSize.width) / 2, (screenSize.height - frameSize.height) / 2);
		frame.setVisible(true);
		frame.repaint();
	}

}