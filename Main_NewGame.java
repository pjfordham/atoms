
//Title:        Atoms
//Version:
//Copyright:    Copyright (c) 1998
//Author:       Peter Fordham
//Company:      Forward Technologies
//Description:  A port of a dodgy Amiga games that Me a a few freinds found quite addictive.

package forwardtechnologies.atoms.application;

import java.awt.*;
import java.awt.event.*;
import java.io.*;

import forwardtechnologies.atoms.core.*;

public class Main_NewGame extends Dialog
{
	private static final long serialVersionUID = 0;	 
	private Panel newGame;

	Panel getNewGame()
	{
		return newGame;
	}

	Frame owner;
	Panel panel1            = new Panel();
	Label newHighlightLabel = new Label("Highlight Volatile Cells:",2);
	Label newConstantLabel  = new Label("Proliferation constant:",2);
	Label newCustomLabel    = new Label("Use Custom Board:",2);
	Label newYSizeLabel     = new Label("Board Height:",2);
	Label newBoardFileLabel = new Label("Board Definition File:",2);
	Label newXSizeLabel     = new Label("Board Width:",2);
	Label newPlayersLabel   = new Label("Number Of Players:",2);
	Choice newConstant      = new Choice();
	Choice newPlayers       = new Choice();
	Checkbox newHighlight   = new Checkbox("",true);
	Checkbox newCustom      = new Checkbox("",false);
	TextField newYSize      = new TextField("8");
	TextField newXSize      = new TextField("8");
	Panel newBoardFile     = new Panel();
	TextField newBoardFileText   = new TextField();
	Button    newBoardFileButton = new Button("...");
	GridLayout gridLayout1  = new GridLayout(8,2,5,5);
	Button newOK            = new Button("OK");
	Button newCancel        = new Button("Cancel");


	public Main_NewGame(Frame frame, String title, boolean modal) {
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


	public Main_NewGame(Frame frame) {
		this(frame, "", false);
	}


	public Main_NewGame(Frame frame, boolean modal) {
		this(frame, "", modal);
	}


	public Main_NewGame(Frame frame, String title) {
		this(frame, title, false);
	}

	void jbInit() throws Exception {

		newCustom.addItemListener(new java.awt.event.ItemListener() {
			public void itemStateChanged(ItemEvent e) {
				newCustom_itemStateChanged(e);
			}
		});

		this.newBoardFile.setEnabled(false);
		this.newBoardFileLabel.setEnabled(false);

		for (int i=2;i<=6;i++)
		{
			newPlayers.add((new Integer(i)).toString());
		}

		for (int i=1;i<=10;i++)
		{
			newConstant.add((new Integer(i)).toString());
		}

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
		newBoardFileButton.addActionListener(new java.awt.event.ActionListener()
		{
			public void actionPerformed(ActionEvent e)
			{
				newBoardFileButton_actionPerformed(e);
			}
		});
		panel1.add(newPlayersLabel);
		panel1.add(newPlayers);
		panel1.add(newConstantLabel);
		panel1.add(newConstant);
		panel1.add(newCustomLabel);
		panel1.add(newCustom);
		panel1.add(newBoardFileLabel);
		panel1.add(newBoardFile);
		newBoardFile.setLayout(new BorderLayout());
		newBoardFile.add(newBoardFileText,BorderLayout.CENTER);
		newBoardFile.add(newBoardFileButton,BorderLayout.EAST);
		panel1.add(newXSizeLabel);
		panel1.add(newXSize);
		panel1.add(newYSizeLabel);
		panel1.add(newYSize);
		panel1.add(newHighlightLabel);
		panel1.add(newHighlight);
		panel1.add(newOK);
		panel1.add(newCancel);
	}

	protected void processWindowEvent(WindowEvent e) {
		if (e.getID() == WindowEvent.WINDOW_CLOSING) {
			cancel();
		}
		super.processWindowEvent(e);
	}

	void cancel() {
		dispose();
	}

	void newCustom_itemStateChanged(ItemEvent e)
	{
		if (this.newCustom.getState())
		{
			this.newXSize.setEnabled(false);
			this.newYSize.setEnabled(false);
			this.newBoardFile.setEnabled(true);
			this.newXSizeLabel.setEnabled(false);
			this.newYSizeLabel.setEnabled(false);
			this.newBoardFileLabel.setEnabled(true);
		}
		else
		{
			this.newXSize.setEnabled(true);
			this.newYSize.setEnabled(true);
			this.newBoardFile.setEnabled(false);
			this.newXSizeLabel.setEnabled(true);
			this.newYSizeLabel.setEnabled(true);
			this.newBoardFileLabel.setEnabled(false);
		}
	}

	void newCancel_mouseClicked(MouseEvent e) {
		cancel();
	}

	void newOK_actionPerformed(ActionEvent e)
	{
		if (newCustom.getState())
		{
			// Custom Board
			try
			{
				ObjectInputStream in = new ObjectInputStream( new FileInputStream(newBoardFileText.getText()));
				Board  customBoard = (Board) in.readObject();
				in.close();
				newGame = new Game (  newHighlight.getState(),
						(new Integer(this.newConstant.getSelectedItem())).intValue(),
						customBoard ,
						(new Integer(this.newPlayers.getSelectedItem())).intValue() );
			}
			catch (Exception excep)
			{
				System.out.println("damn!");
			}
		}
		else
		{
			// Rectangular Board
			newGame = new Game (  newHighlight.getState(),
					(new Integer(this.newConstant.getSelectedItem())).intValue(),
					new Board((new Integer(this.newXSize.getText())).intValue() + 2,(new Integer(this.newYSize.getText())).intValue() + 2) ,
					(new Integer(this.newPlayers.getSelectedItem())).intValue() );
		}

		cancel();
	}

	void newBoardFileButton_actionPerformed(ActionEvent e)
	{
		FileDialog loadDialog = new FileDialog((Frame)this.getParent(), "Select Board", FileDialog.LOAD);
		Dimension dlgSize = loadDialog.getPreferredSize();
		Dimension frmSize = this.getSize();
		Point loc = this.getLocation();
		loadDialog.setLocation((frmSize.width - dlgSize.width) / 2 + loc.x, (frmSize.height - dlgSize.height) / 2 + loc.y);
		loadDialog.setModal(true);
		loadDialog.setVisible(true);
		if (loadDialog.getFile() != null)
		{
			this.newBoardFileText.setText(loadDialog.getDirectory() + loadDialog.getFile());
		}
		loadDialog.dispose();

	}
}

