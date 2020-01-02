package forwardtechnologies.messageboxes;

import java.awt.*;
import java.awt.event.*;

public class MessageBox extends Frame implements ActionListener
{
	private static final long serialVersionUID = 0;	 

	private Container parent;
	private Panel panel1 = new Panel();
	private Panel panel2 = new Panel();
	private Panel insetsPanel1 = new Panel();
	private Label label1 = new Label();
	private Button button1 = new Button();

	private BorderLayout borderLayout1 = new BorderLayout();
	private BorderLayout borderLayout2 = new BorderLayout();

	public MessageBox(Container $parent, String caption, String message)
	{
		super();
		parent = $parent;
		enableEvents(AWTEvent.WINDOW_EVENT_MASK);
		try
		{
			parent.setEnabled(false);
			this.setTitle(caption);
			setResizable(false);
			setBackground(Color.lightGray);
			panel1.setLayout(borderLayout1);
			panel2.setLayout(borderLayout2);
			label1.setText(message);
			label1.setAlignment(1);
			button1.setLabel("OK");
			button1.addActionListener(this);
			this.add(panel1);
			panel2.add(label1, BorderLayout.NORTH);
			insetsPanel1.add(button1, null);
			panel1.add(insetsPanel1, BorderLayout.SOUTH);
			panel1.add(panel2, BorderLayout.NORTH);

			Dimension dlgSize = getPreferredSize();
			Dimension frmSize = parent.getSize();
			Point loc         = parent.getLocation();
			this.setLocation((frmSize.width - dlgSize.width) / 2 + loc.x, (frmSize.height - dlgSize.height) / 2 + loc.y);
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
		pack();
	}

	protected void processWindowEvent(WindowEvent e)
	{
		if (e.getID() == WindowEvent.WINDOW_CLOSING)
		{
			ok();
		}
		super.processWindowEvent(e);
	}

	private void ok()
	{
		dispose();
		parent.setEnabled(true);
	}

	public void actionPerformed(ActionEvent e)
	{
		if (e.getSource() == button1)
		{
			ok();
		}
	}
}

