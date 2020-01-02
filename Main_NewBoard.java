
//Title:        Atoms
//Version:
//Copyright:    Copyright (c) 1998
//Author:       Peter Fordham
//Company:      Forward Technologies
//Description:  A port of a dodgy Amiga games that Me a a few freinds found quite addictive.

package forwardtechnologies.atoms.application;

import java.awt.*;
import java.awt.event.*;

import forwardtechnologies.atoms.core.*;

public class Main_NewBoard extends Dialog
{
	private static final long serialVersionUID = 0;	 
	private Panel newGame;

	Panel getNewGame()
	{
		return newGame;
	}

	Frame owner;
	Panel panel1            = new Panel();
	Label newYSizeLabel     = new Label("Board Height:",2);
	Label newXSizeLabel     = new Label("Board Width:",2);
	GridLayout gridLayout1  = new GridLayout(3,2,5,5);
	Button newOK            = new Button("OK");
	Button newCancel        = new Button("Cancel");
	TextField newYSize      = new TextField("20");
	TextField newXSize      = new TextField("20");


	public Main_NewBoard(Frame frame, String title, boolean modal) {
		super(frame, title, modal);
		owner = (Frame)frame;
		enableEvents(AWTEvent.WINDOW_EVENT_MASK);
		try  {
			jbInit();
			add(panel1);
			pack();
		}
		catch (Exception ex) {
			ex.printStackTrace();
		}
	}


	public Main_NewBoard(Frame frame) {
		this(frame, "", false);
	}


	public Main_NewBoard(Frame frame, boolean modal) {
		this(frame, "", modal);
	}


	public Main_NewBoard(Frame frame, String title) {
		this(frame, title, false);
	}

	void jbInit() throws Exception {

		panel1.setLayout(gridLayout1);
		newCancel.addMouseListener(new java.awt.event.MouseAdapter() {
			public void mouseClicked(MouseEvent e) {
				newCancel_mouseClicked(e);
			}
		});
		newOK.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(ActionEvent e) {
				newOK_actionPerformed(e);
			}
		});
		panel1.add(newXSizeLabel);
		panel1.add(newXSize);
		panel1.add(newYSizeLabel);
		panel1.add(newYSize);
		panel1.add(newOK);
		panel1.add(newCancel);
	}

	protected void processWindowEvent(WindowEvent e) 
	{
		if (e.getID() == WindowEvent.WINDOW_CLOSING) {
			cancel();
		}
		super.processWindowEvent(e);
	}

	void cancel()
	{
		dispose();
	}

	void newCancel_mouseClicked(MouseEvent e) 
	{
		cancel();
	}

	void newOK_actionPerformed(ActionEvent e)
	{
		newGame = new Board( (new Integer(this.newXSize.getText())).intValue() + 2 ,
				(new Integer(this.newYSize.getText())).intValue() + 2);

		cancel();
	}
}

