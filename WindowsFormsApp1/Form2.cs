using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Data.OleDb;
using System.Security.Cryptography;
namespace WindowsFormsApp1
{
    public partial class Form2 : Form
    {
        public Form2()
        {
            InitializeComponent();
        }
        bool idcheck = false;
        OleDbConnection conn;
        string connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119"; //oracle 서버 연결
        int usernum;

        private void button_sign_Click(object sender, EventArgs e)
        {
            conn = new OleDbConnection(connectionString);
            
            
            if (textBox_PW.Text.Length < 2)
            {
                MessageBox.Show("Pass Word는 3자 이상이어야 합니다");
                return;

            }
            
            conn = new OleDbConnection(connectionString);
            try
            {
                string pw;
                SHA256 hash = new SHA256Managed();
                byte[] bytes = hash.ComputeHash(Encoding.ASCII.GetBytes(textBox_PW.Text));
                StringBuilder sb = new StringBuilder();
                foreach (byte b in bytes)
                    sb.AppendFormat("{0:x2}", b);
                pw = sb.ToString();
                string sha_length = Convert.ToString(pw.Length);
                conn.Open(); //데이터베이스 연결
                OleDbCommand cmd = new OleDbCommand();
                
                
                
                cmd.CommandText = "INSERT INTO 회원 VALUES(" + " (SELECT NVL(MAX(회원번호), 0) + 1 FROM 회원)" + ",'" + textBox_name.Text + "','" + textBox4_phone.Text + "','" + "none" + "','" + textBox_card.Text + "','" + pw + "','" + textBox_ID.Text + "')";

                cmd.CommandType = CommandType.Text; //검색명령을 쿼리 형태로
                cmd.Connection = conn;

                cmd.ExecuteNonQuery(); //쿼리문을 실행하고 영향받는 행의 수를 반환.
                MessageBox.Show("가입이 완료되었습니다");
                this.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: " + ex.Message); //에러 메세지 
            }
            finally
            {
                if (conn != null)
                {
                    conn.Close(); //데이터베이스 연결 해제
                }
            }

        }
    }
    
}
