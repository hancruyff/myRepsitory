using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.OleDb;
using System.Drawing;
using System.Linq;
using System.Linq.Expressions;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        OleDbConnection conn;
        string connectionString = "Provider=MSDAORA;Password=han1111;User ID=hjh4119";
        private void button_login_Click(object sender, EventArgs e)
        {
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 회원 where ID='" + textBox_ID.Text+"'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                string ID;
                ID = textBox_ID.Text;
                string pw;
                SHA256 hash = new SHA256Managed();
                byte[] bytes = hash.ComputeHash(Encoding.ASCII.GetBytes(textBox_PW.Text));
                StringBuilder sb = new StringBuilder();
                foreach (byte b in bytes)
                    sb.AppendFormat("{0:x2}", b);
                pw = sb.ToString();
                string sha_length = Convert.ToString(pw.Length);
                OleDbDataReader read = cmd.ExecuteReader();
                
                if (textBox_ID.Text == "admin" && textBox_PW.Text == "1111")
                {
                    Form3 frm = new Form3();
                    frm.ShowDialog();
                }
                if (!read.Read())
                    label1.Text = "아이디없음";
                else
                {
                    if (read.GetValue(3).ToString() == "none")
                    {
                        label1.Text = "승인되지않은 사용자";
                    }
                    else if (read.GetValue(5).ToString() != pw)
                    {
                        label1.Text = "비밀번호 틀림";
                    }
                    else
                    {
                        label1.Text = read.GetValue(1).ToString() + "님 환영합니다";
                        Form4.usernum = read.GetValue(0).ToString();
                        Form4 frm = new Form4();
                        frm.ShowDialog();
                    }
                }
                read.Close();
                conn.Close();
                
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: " + ex.Message);
            }
        }

        private void button_sign_Click(object sender, EventArgs e)
        {
            Form2 frm = new Form2();
            frm.ShowDialog();
        }
    }
}
