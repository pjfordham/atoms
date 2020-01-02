
//Title:      Atoms
//Version:
//Copyright:  Copyright (c) 1998
//Author:     Peter Fordham
//Company:    Forward Technologies
//Description:A port of a dodgy Amiga games that Me a a few freinds found quite addictive.
package forwardtechnologies.atoms.application;

import java.io.*;
import java.awt.*;
import java.awt.event.*;

import forwardtechnologies.atoms.core.*;

public class Main extends Frame
{
	private static final long serialVersionUID = 0;	 
	// Construct the frame
	MenuBar menuBar1 = new MenuBar();

	Menu menuFile = new Menu("File");
	MenuItem menuFileNewGame  = new MenuItem("New Game",  new MenuShortcut(78));
	MenuItem menuFileNewBoard = new MenuItem("New Board", new MenuShortcut(66));
	MenuItem menuFileLoad     = new MenuItem("Load",      new MenuShortcut(76));
	MenuItem menuFileSave     = new MenuItem("Save",      new MenuShortcut(83));
	MenuItem menuFileExit     = new MenuItem("Exit");

	Menu menuOptions = new Menu("Options");
	MenuItem menuOptionsRestart   = new MenuItem("Restart Game");

	Menu menuHelp = new Menu("Help");
	MenuItem menuHelpAbout = new MenuItem("About");

	Panel playAtoms;

	public Main() {
		
		enableEvents(AWTEvent.WINDOW_EVENT_MASK);
		try  {
			this.setSize(new Dimension(500, 350));
			this.setTitle("Atoms");
			this.setBackground(Color.lightGray);

			this.setMenuBar(menuBar1);
			menuBar1.add(menuFile);
			menuFile.add(menuFileNewGame);
			menuFile.add(menuFileNewBoard);
			menuFile.add(menuFileLoad);
			menuFile.add(menuFileSave);
			menuFile.addSeparator();
			menuFile.add(menuFileExit);
			menuBar1.add(menuOptions);
			menuOptions.add(menuOptionsRestart);
			menuBar1.add(menuHelp);
			menuHelp.add(menuHelpAbout);


			menuFileExit.addActionListener(new ActionListener()  {
				public void actionPerformed(ActionEvent e) {
					filemenuFileExit_actionPerformed(e);
				}
			});

			menuHelpAbout.addActionListener(new ActionListener()  {
				public void actionPerformed(ActionEvent e) {
					helpAbout_actionPerformed(e);
				}
			});

			menuOptionsRestart.addActionListener(new ActionListener()  {
				public void actionPerformed(ActionEvent e) {
					optionsRestart_actionPerformed(e);
				}
			});

			menuFileNewGame.addActionListener(new ActionListener()  {
				public void actionPerformed(ActionEvent e) {
					filemenuFileNewGame_actionPerformed(e);
				}
			});

			menuFileNewBoard.addActionListener(new ActionListener()  {
				public void actionPerformed(ActionEvent e) {
					filemenuFileNewBoard_actionPerformed(e);
				}
			});

			menuFileLoad.addActionListener(new ActionListener()  {
				public void actionPerformed(ActionEvent e) {
					filemenuFileLoad_actionPerformed(e);
				}
			});

			menuFileSave.addActionListener(new ActionListener()  {
				public void actionPerformed(ActionEvent e) {
					filemenuFileSave_actionPerformed(e);
				}
			});


			// Create a new game on a 10x10 board with 2
			// players with an initial density of 1 and
			// volatile highlighting on.

			playAtoms = new Game(true,1,new Board(10,10),2);
			this.removeAll();
			this.add(playAtoms);
			menuOptionsRestart.setEnabled(true);

		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}

	// File | Exit action performed
	public void filemenuFileExit_actionPerformed(ActionEvent e)
	{
		System.exit(0);
	}

	public void optionsRestart_actionPerformed(ActionEvent e)
	{
		((Game)playAtoms).restart();
	}

	public void filemenuFileSave_actionPerformed(ActionEvent e)
	{
		FileDialog saveDialog = new FileDialog(this, "Save Game", FileDialog.SAVE);
		Dimension dlgSize = saveDialog.getPreferredSize();
		Dimension frmSize = this.getSize();
		Point loc = this.getLocation();
		saveDialog.setLocation((frmSize.width - dlgSize.width) / 2 + loc.x, (frmSize.height - dlgSize.height) / 2 + loc.y);
		saveDialog.setModal(true);
		//    saveDialog.setFilenameFilter();
		saveDialog.setVisible(true);
		if (saveDialog.getFile() != null)
		{
			File outFile = new File(saveDialog.getDirectory() + saveDialog.getFile());
			try
			{
				ObjectOutputStream out = new ObjectOutputStream( new FileOutputStream(outFile));
				out.writeObject(playAtoms);
				out.close();
			}
			catch (Exception excp)
			{
				excp.printStackTrace();
			}
		}
		saveDialog.dispose();
	}

	public void filemenuFileLoad_actionPerformed(ActionEvent e)
	{
		FileDialog loadDialog = new FileDialog(this, "Load Game", FileDialog.LOAD);
		Dimension dlgSize = loadDialog.getPreferredSize();
		Dimension frmSize = this.getSize();
		Point loc = this.getLocation();
		loadDialog.setLocation((frmSize.width - dlgSize.width) / 2 + loc.x, (frmSize.height - dlgSize.height) / 2 + loc.y);
		loadDialog.setModal(true);
		loadDialog.setVisible(true);
		if (loadDialog.getFile() != null)
		{
			File inFile = new File(loadDialog.getDirectory() + loadDialog.getFile());
			try
			{
				ObjectInputStream in = new ObjectInputStream( new FileInputStream(inFile));
				playAtoms = (Panel) in.readObject();
				this.removeAll();
				this.add(playAtoms);
				paintAll(getGraphics());
				in.close();
			}
			catch (Exception excp)
			{
				excp.printStackTrace();
			}
		}
		loadDialog.dispose();
	}

	//Help | About action performed
	public void helpAbout_actionPerformed(ActionEvent e)
	{
		Main_AboutBox dlg = new Main_AboutBox(this);
		Dimension dlgSize = dlg.getPreferredSize();
		Dimension frmSize = getSize();
		Point loc = getLocation();
		dlg.setLocation((frmSize.width - dlgSize.width) / 2 + loc.x, (frmSize.height - dlgSize.height) / 2 + loc.y);
		dlg.setModal(true);
		dlg.setVisible(true);
		dlg.dispose();
	}
	//Overriden so we can exit on System Close

	public void filemenuFileNewGame_actionPerformed(ActionEvent e) {
		Main_NewGame dlg = new Main_NewGame(this, "New Game", true );
		Dimension dlgSize = dlg.getPreferredSize();
		Dimension frmSize = getSize();
		Point loc = getLocation();
		dlg.setLocation((frmSize.width - dlgSize.width) / 2 + loc.x, (frmSize.height - dlgSize.height) / 2 + loc.y);
		dlg.setVisible(true);
		if (dlg.getNewGame() != null)
		{
			playAtoms = dlg.getNewGame();
			this.removeAll();
			this.add(playAtoms);
			paintAll(getGraphics());
			menuOptionsRestart.setEnabled(true);
		}
		dlg.dispose();
	}

	public void filemenuFileNewBoard_actionPerformed(ActionEvent e) {
		Main_NewBoard dlg = new Main_NewBoard(this, "New Baord", true );
		Dimension dlgSize = dlg.getPreferredSize();
		Dimension frmSize = getSize();
		Point loc = getLocation();
		dlg.setLocation((frmSize.width - dlgSize.width) / 2 + loc.x, (frmSize.height - dlgSize.height) / 2 + loc.y);
		dlg.setVisible(true);
		if (dlg.getNewGame() != null)
		{
			playAtoms = dlg.getNewGame();
			this.removeAll();
			this.add(playAtoms);
			paintAll(getGraphics());
			menuOptionsRestart.setEnabled(false);
		}
		dlg.dispose();
	}

	protected void processWindowEvent(WindowEvent e) {
		super.processWindowEvent(e);
		if (e.getID() == WindowEvent.WINDOW_CLOSING) {
			filemenuFileExit_actionPerformed(null);
		}
	}

}

