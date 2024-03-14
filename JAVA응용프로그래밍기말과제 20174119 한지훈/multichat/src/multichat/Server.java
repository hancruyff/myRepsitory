package multichat;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;

public class Server {

	
	private ServerSocket ss = null;
	private Socket s = null;
	ArrayList <ChatThread> chatlist = new ArrayList <ChatThread>();
	public void start() {
		try {			
			ss = new ServerSocket(8888);			
			System.out.println("서버 실행");					
			while(true) {												
				s = ss.accept();							
				ChatThread chat = new ChatThread();
				chatlist.add(chat);
				chat.start();
			}
		} catch(Exception e) {			
			System.out.println("예외 발생!!");			
		}   
	} 	
	public static void main(String[] args) {
		Server server = new Server();
		server.start();
	}	
	void msgSendAll(String msg) { 
		for(ChatThread ct : chatlist) {
			ct.outMsg.println(msg); 
		}
	}	
	class ChatThread extends Thread {		
		String msg;
		String[] Rmsg;				
		private BufferedReader inMsg = null;
		private PrintWriter outMsg = null;
		private BufferedReader id=null,pw=null;
		public void run() {		
			boolean status = true; 
			System.out.println("##ChatThread start...");
			try {
				inMsg = new BufferedReader(new InputStreamReader(s.getInputStream()));
				outMsg = new PrintWriter(s.getOutputStream(), true);				
				while(status) {					
					msg = inMsg.readLine();					
					Rmsg = msg.split("/");										
					
							
					
						msgSendAll(msg);
					
				} 				
				this.interrupt();
				System.out.println("##"+this.getName()+"stop!!");
			} catch(IOException e) {
				chatlist.remove(this);
				System.out.println("예외 발생!!");
			}
		}
	}
	
}