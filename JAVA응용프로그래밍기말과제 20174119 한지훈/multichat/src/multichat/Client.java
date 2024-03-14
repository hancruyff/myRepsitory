package multichat;
import java.awt.BorderLayout;
import java.awt.CardLayout;
import java.awt.Container;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

public class Client implements ActionListener, Runnable {
    private String ip;
    private String id;
    private Socket socket;
    private BufferedReader inMsg = null;
    private PrintWriter outMsg = null;

    private JPanel loginPanel;
    private JButton loginButton;
    private JLabel label1;
    private JTextField idInput;
    private JTextField pwInput;
    private JPanel logoutPanel;
    private JLabel label2;
    private JButton logoutButton;
    private JLabel label3;

    private JPanel msgPanel;
    private JTextField msgInput;
    

    private JFrame jframe;
    private JTextArea msgOut;

    private Container tab;
    private CardLayout clayout;
    private Thread thread;

    boolean status;

    public Client(String ip) {
        this.ip = ip; 

        loginPanel = new JPanel();
        loginPanel.setLayout(new BorderLayout());
        idInput = new JTextField("ID",15);
        pwInput = new JTextField("password",15);
        loginButton = new JButton("로그인");
        loginButton.addActionListener(this);
        loginPanel.add(idInput, BorderLayout.CENTER);
        loginPanel.add(pwInput,BorderLayout.SOUTH);
        loginPanel.add(loginButton, BorderLayout.EAST);  
        logoutPanel = new JPanel();
        logoutPanel.setLayout(new BorderLayout());
        label2 = new JLabel();
        logoutButton = new JButton("로그아웃");
        logoutButton.addActionListener(this); 
        logoutPanel.add(label2, BorderLayout.CENTER);
        logoutPanel.add(logoutButton, BorderLayout.EAST); 
        msgPanel = new JPanel(); 
        msgPanel.setLayout(new BorderLayout());
        msgInput = new JTextField(30); 
        msgInput.addActionListener(this);         
        msgPanel.add(msgInput, BorderLayout.CENTER);
        
        tab = new JPanel();
        clayout = new CardLayout();
        tab.setLayout(clayout);
        tab.add(loginPanel, "login");
        tab.add(logoutPanel, "logout");
        jframe = new JFrame("JAVA응용프로그래밍 기말과제");
        msgOut = new JTextArea("", 10, 30);      
        msgOut.setEditable(false); 
        
        JScrollPane jsp = new JScrollPane(msgOut, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
        jframe.add(tab, BorderLayout.NORTH);
        jframe.add(jsp, BorderLayout.CENTER);
        jframe.add(msgPanel, BorderLayout.SOUTH);
       
        clayout.show(tab, "login");
        
        jframe.pack();       
        jframe.setResizable(false);        
        jframe.setVisible(true);
        jframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }
    public void connectServer() {
        try {          
            socket = new Socket(ip, 8888);
            System.out.println("[Client]Server 연결 성공");
            inMsg = new BufferedReader(new InputStreamReader(socket.getInputStream()));           
            outMsg = new PrintWriter(socket.getOutputStream(), true);                
            thread = new Thread(this);
            thread.start();
       }catch(Exception e) {            
            System.out.println("예외발생");
        }
    }    
    	public static Connection makeConnection() {
    		 try {
 				Class.forName("com.mysql.jdbc.Driver");
 			} catch (ClassNotFoundException e) {
 				// TODO Auto-generated catch block
 				e.printStackTrace();
 			}
             String password ="1234";
             String DBID="root";
             String url="jdbc:mysql://localhost:3306/idpw?characterEncoding=UTF-8 &serverTimezone=UTC";
             Connection con =null;
             try {
				Class.forName("com.mysql.cj.jdbc.Driver");
				try {
					con=DriverManager.getConnection(url,DBID,password);
				} catch (SQLException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} catch (ClassNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return con;
    	}       
    public void actionPerformed(ActionEvent arg0) {
        Object obj = arg0.getSource();
        String ID,PW;
        int ipw; 
        if(obj == loginButton) {//로그인한 경우
            id = idInput.getText(); //id입력칸에 입력값을 id에 저장
            ID=idInput.getText();
            PW=pwInput.getText();
            ipw=Integer.parseInt(PW);
            Connection conn=null;
    		Statement stmt=null;
    		String user="root";
    		String password="han1120";
    		String url="jdbc:mysql://localhost:3306/idpw?serverTimezone=UTC";
    		try {
    			Class.forName("com.mysql.cj.jdbc.Driver");
    			conn=DriverManager.getConnection(url,user,password);
    			System.out.println("연결 성공");
    			stmt=conn.createStatement();
    			ResultSet rs=stmt.executeQuery("SELECT * FROM users");
    			while(rs.next()) {
    				String Did=rs.getString("id");
    				int Dpw=rs.getInt("pw");
    				if(ID.equals(Did)&&ipw==Dpw) {
    					if(ID.equals(Did)==false) {
    						System.out.println("아이디 틀림");
    						
    					}
    					if(ipw!=Dpw) {
    						System.out.println("비번틀림");
    						
    					}
    					label2.setText("대화명 : " + id);
    		            clayout.show(tab, "logout"); 
    		            connectServer(); 
    					break;
    				}
    			}
    			
    		} catch(Exception e) {
    			System.out.println("연걸 실패");
    		}
            
        } else if(obj == logoutButton) {
            msgOut.setText(""); 
            clayout.show(tab, "login"); 
            outMsg.close();
            try {
                inMsg.close(); 
                socket.close(); 
            } catch(IOException e) {
                e.printStackTrace();
            }

            status = false; 
        } else if(obj == msgInput) {       
            outMsg.println(id + "/" + msgInput.getText());    
            msgInput.setText("");
        }
    }
    public void run() {
        String msg; 
        String[] rmsg; 
        status = true; 
        while(status) {
            try {          
                msg = inMsg.readLine();
                rmsg = msg.split("/");
                msgOut.append(rmsg[0] + ": "+rmsg[1] + "\n");      
                msgOut.setCaretPosition(msgOut.getDocument().getLength());
            } catch(IOException e) {                
                status = false; 
           }
        }
        System.out.println("[multiChatClient]" + thread.getName() + "종료됨"); 
    }
    public static void main(String[] args) {
        Client C = new Client("127.0.0.1");
    }
}