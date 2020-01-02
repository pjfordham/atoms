
//Title:      Atoms
//Version:    
//Copyright:  Copyright (c) 1998
//Author:     Peter Fordham
//Company:    Forward Technologies
//Description:A port of a dodgy Amiga games that Me a a few freinds found quite addictive.
package forwardtechnologies.atoms.application;

import java.awt.*;
import java.awt.event.*;

public class Main_AboutBox extends Dialog implements ActionListener {

	private static final long serialVersionUID = 0;	 
	Panel panel1 = new Panel();
	Panel panel2 = new Panel();
	Panel insetsPanel1 = new Panel();
	Panel insetsPanel2 = new Panel();
	Panel insetsPanel3 = new Panel();
	Button button1 = new Button();
	Label imageControl1 = new Label();
	Label label1 = new Label();
	Label label2 = new Label();
	Label label3 = new Label();
	Label label4 = new Label();
	BorderLayout borderLayout1 = new BorderLayout();
	BorderLayout borderLayout2 = new BorderLayout();
	FlowLayout flowLayout1 = new FlowLayout();
	FlowLayout flowLayout2 = new FlowLayout();
	GridLayout gridLayout1 = new GridLayout();
	String product = "Atoms";
	String version = "1.0J";
	String copyright = "Copyright (c) 1999-2009";
	String comments = "A port of a dodgy Amiga game that me a a few friends found quite addictive.";

	public Main_AboutBox(Frame parent) {
		super(parent);
		enableEvents(AWTEvent.WINDOW_EVENT_MASK);
		try  {
			jbInit();
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		pack();
	}

	private void jbInit() throws Exception  {
		this.setTitle("About");
		setResizable(false);
		panel1.setLayout(borderLayout1);
		panel2.setLayout(borderLayout2);
		insetsPanel1.setLayout(flowLayout1);
		insetsPanel2.setLayout(flowLayout1);
		gridLayout1.setRows(4);
		gridLayout1.setColumns(1);
		label1.setText(product);
		label2.setText(version);
		label3.setText(copyright);
		label4.setText(comments);
		insetsPanel3.setLayout(gridLayout1);
		button1.setLabel("OK");
		button1.addActionListener(this);
		insetsPanel2.add(imageControl1, null);
		panel2.add(insetsPanel2, BorderLayout.WEST);
		this.add(panel1, null);
		insetsPanel3.add(label1, null);
		insetsPanel3.add(label2, null);
		insetsPanel3.add(label3, null);
		insetsPanel3.add(label4, null);
		panel2.add(insetsPanel3, BorderLayout.CENTER);
		insetsPanel1.add(button1, null);
		panel1.add(insetsPanel1, BorderLayout.SOUTH);
		panel1.add(panel2, BorderLayout.NORTH);
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

	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == button1) {
			cancel();
		}
	}
}

