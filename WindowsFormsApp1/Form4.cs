using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.OleDb;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Security.Cryptography;
namespace WindowsFormsApp1
{
    public partial class Form4 : Form
    {
        public Form4()
        {
            InitializeComponent();
        }
        public static string usernum;
        private void button1_Click(object sender, EventArgs e)
        {
            Form5.usernum = usernum;
            Form5 frm = new Form5();
            frm.ShowDialog();
        }
        string RN;
        string start;
        string dest;
        private void button2_Click(object sender, EventArgs e)
        {
            button6.Enabled = false;
            button7.Enabled = true;
            dataGridView1.Rows.Clear();
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 회원 where 회원번호=" + usernum;
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 5;
                dataGridView1.Columns[0].Name = read.GetName(1);
                dataGridView1.Columns[1].Name = read.GetName(2);
                dataGridView1.Columns[2].Name = read.GetName(3);
                dataGridView1.Columns[3].Name = read.GetName(4);
                dataGridView1.Columns[4].Name = read.GetName(6);
                while (read.Read())
                {
                    Object[] obj = new object[5];
                    obj[0] = read.GetValue(1);
                    obj[1] = read.GetValue(2);
                    obj[2] = read.GetValue(3);
                    obj[3] = read.GetValue(4);
                    obj[4] = read.GetValue(6);
                    dataGridView1.Rows.Add(obj);
                }
                textBox2.Text = dataGridView1.Rows[0].Cells[1].Value.ToString();
                textBox3.Text = dataGridView1.Rows[0].Cells[3].Value.ToString();
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

        private void button8_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 운행시간표 where 역이름='" + comboBox1.SelectedItem.ToString()+"'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 2;
                dataGridView1.Columns[0].Name = read.GetName(1);
                dataGridView1.Columns[1].Name = read.GetName(4);
                while (read.Read())
                {
                    Object[] obj = new object[2];
                    obj[0] = read.GetValue(1);
                    obj[1] = read.GetValue(4);
                    dataGridView1.Rows.Add(obj);
                }
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

        private void Form4_Load(object sender, EventArgs e)
        {

            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 기차역 order by 역순번";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                while (read.Read())
                {
                    Object obj = read.GetValue(1);
                    comboBox1.Items.Add(obj.ToString());
                }
                read.Close();
                cmd.CommandText = "select 열차번호 from 열차 order by 열차번호";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                read = cmd.ExecuteReader();
                while (read.Read())
                {
                    Object obj = read.GetValue(0);
                    comboBox2.Items.Add(obj.ToString());
                }
                label2.Text = "비밀번호";
                label3.Text = "전화번호";
                label4.Text = "카드번호";
            }
            catch (Exception ex)
            {
                MessageBox.Show("1Error: " + ex.Message); //에러 메세지 
            }
            finally
            {
                if (conn != null)
                {
                    conn.Close(); //데이터베이스 연결 해제

                }
            }
        }

        private void button7_Click(object sender, EventArgs e)
        {
            string pw;
            SHA256 hash = new SHA256Managed();
            byte[] bytes = hash.ComputeHash(Encoding.ASCII.GetBytes(textBox1.Text));
            StringBuilder sb = new StringBuilder();
            foreach (byte b in bytes)
                sb.AppendFormat("{0:x2}", b);
            pw = sb.ToString();
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "update 회원 set 비밀번호='" + pw + "',휴대전화='" + textBox2.Text + "',카드번호='" + textBox3.Text + "' where 회원번호="+usernum;
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();
                dataGridView1.Rows.Clear();
                cmd = new OleDbCommand();
                cmd.CommandText = "select * from 회원 where 회원번호="+usernum;
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 5;
                dataGridView1.Columns[0].Name = read.GetName(1);
                dataGridView1.Columns[1].Name = read.GetName(2);
                dataGridView1.Columns[2].Name = read.GetName(3);
                dataGridView1.Columns[3].Name = read.GetName(4);
                dataGridView1.Columns[4].Name = read.GetName(6);
                while (read.Read())
                {
                    Object[] obj = new object[5];
                    obj[0] = read.GetValue(1);
                    obj[1] = read.GetValue(2);
                    obj[2] = read.GetValue(3);
                    obj[3] = read.GetValue(4);
                    obj[4] = read.GetValue(6);
                    dataGridView1.Rows.Add(obj);
                }
                textBox1.Clear();
                textBox2.Text = dataGridView1.Rows[0].Cells[1].Value.ToString();
                textBox3.Text = dataGridView1.Rows[0].Cells[3].Value.ToString();
            }
            catch (Exception ex)
            {
                MessageBox.Show("1Error: " + ex.Message); //에러 메세지 
            }
            finally
            {
                if (conn != null)
                {
                    conn.Close(); //데이터베이스 연결 해제

                }
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            button6.Enabled = false;
            button7.Enabled = false;
            dataGridView1.Rows.Clear();
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select 출발역,도착역,예약번호 from 예약현황 where 회원번호=" + usernum.ToString();
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 3;
                dataGridView1.Columns[0].Name = read.GetName(0);
                dataGridView1.Columns[1].Name = read.GetName(1);
                dataGridView1.Columns[2].Name = read.GetName(2);
                /*dataGridView1.Columns[3].Name = read.GetName(3);
                dataGridView1.Columns[4].Name = read.GetName(4);
                dataGridView1.Columns[5].Name = read.GetName(5);
                dataGridView1.Columns[6].Name = read.GetName(6);*/
                while (read.Read())
                {
                    Object[] obj = new Object[3];
                    obj[0] = read.GetValue(0);
                    obj[1] = read.GetValue(1);
                    obj[2] = read.GetValue(2);
                    /*obj[3] = read.GetValue(3);
                    obj[4] = read.GetValue(4);
                    obj[5] = read.GetValue(5);
                    obj[6] = read.GetValue(6);*/
                    dataGridView1.Rows.Add(obj);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("1Error: " + ex.Message); //에러 메세지 
            }
            finally
            {
                if (conn != null)
                {
                    conn.Close(); //데이터베이스 연결 해제

                }
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            button6.Enabled = true;
            button7.Enabled = false;
            dataGridView1.Rows.Clear();
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select 출발역,도착역,예약번호 from 예약현황 where 회원번호=" + usernum.ToString();
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 3;
                dataGridView1.Columns[0].Name = read.GetName(0);
                dataGridView1.Columns[1].Name = read.GetName(1);
                dataGridView1.Columns[2].Name = read.GetName(2);
                /*dataGridView1.Columns[3].Name = read.GetName(3);
                dataGridView1.Columns[4].Name = read.GetName(4);
                dataGridView1.Columns[5].Name = read.GetName(5);
                dataGridView1.Columns[6].Name = read.GetName(6);
                dataGridView1.Columns[7].Name = read.GetName(7);*/
                while (read.Read())
                {
                    Object[] obj = new Object[3];
                    obj[0] = read.GetValue(0);
                    obj[1] = read.GetValue(1);
                    obj[2] = read.GetValue(2);
                    /*obj[3] = read.GetValue(3);
                    obj[4] = read.GetValue(4);
                    obj[5] = read.GetValue(5);
                    obj[6] = read.GetValue(6);
                    obj[7] = read.GetValue(7);*/
                    dataGridView1.Rows.Add(obj);
                }
                read.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("1Error: " + ex.Message); //에러 메세지 
            }
            finally
            {
                if (conn != null)
                {
                    conn.Close(); //데이터베이스 연결 해제

                }
            }
        }

        private void dataGridView1_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            RN= dataGridView1.Rows[e.RowIndex].Cells[2].Value.ToString();
            start= dataGridView1.Rows[e.RowIndex].Cells[0].Value.ToString();
            dest= dataGridView1.Rows[e.RowIndex].Cells[1].Value.ToString();
        }

        private void button6_Click(object sender, EventArgs e)
        {
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "delete from 예약좌석 where 예약번호='" + RN + "'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();
                cmd.CommandText = "delete from 예약현황 where 예약번호='" + RN + "'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();
                dataGridView1.Rows.Clear();
                cmd.CommandText = "select 출발역,도착역,예약번호 from 예약좌석,예약현황 where 회원번호=" + usernum.ToString();
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 3;
                dataGridView1.Columns[0].Name = read.GetName(0);
                dataGridView1.Columns[1].Name = read.GetName(1);
                dataGridView1.Columns[2].Name = read.GetName(2);
                /*dataGridView1.Columns[3].Name = read.GetName(3);
                dataGridView1.Columns[4].Name = read.GetName(4);
                dataGridView1.Columns[5].Name = read.GetName(5);
                dataGridView1.Columns[6].Name = read.GetName(6);
                dataGridView1.Columns[7].Name = read.GetName(7);*/
                while (read.Read())
                {
                    Object[] obj = new Object[3];
                    obj[0] = read.GetValue(0);
                    obj[1] = read.GetValue(1);
                    obj[2] = read.GetValue(2);
                    /*obj[3] = read.GetValue(3);
                    obj[4] = read.GetValue(4);
                    obj[5] = read.GetValue(5);
                    obj[6] = read.GetValue(6);
                    obj[6] = read.GetValue(7);*/
                    dataGridView1.Rows.Add(obj);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("1Error: " + ex.Message); //에러 메세지 
            }
            finally
            {
                if (conn != null)
                {
                    conn.Close(); //데이터베이스 연결 해제

                }
            }
        }

        private void button9_Click(object sender, EventArgs e)
        {
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "delete from 회원 where 회원번호=" + usernum.ToString();
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();
                this.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("1Error: " + ex.Message); //에러 메세지 
            }
            finally
            {
                if (conn != null)
                {
                    conn.Close(); //데이터베이스 연결 해제

                }
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            button6.Enabled = false;
            button7.Enabled = false;
            dataGridView1.Rows.Clear();
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select 출발역,도착역,예약번호 from 예약현황 where 회원번호=" + usernum.ToString();
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 8;
                dataGridView1.Columns[0].Name = read.GetName(0);
                dataGridView1.Columns[1].Name = read.GetName(1);
                dataGridView1.Columns[2].Name = read.GetName(2);
                /*dataGridView1.Columns[3].Name = read.GetName(3);
                dataGridView1.Columns[4].Name = read.GetName(4);
                dataGridView1.Columns[5].Name = read.GetName(5);
                dataGridView1.Columns[6].Name = read.GetName(6);*/
                while (read.Read())
                {
                    Object[] obj = new Object[3];
                    obj[0] = read.GetValue(0);
                    obj[1] = read.GetValue(1);
                    obj[2] = read.GetValue(2);
                    /*obj[3] = read.GetValue(3);
                    obj[4] = read.GetValue(4);
                    obj[5] = read.GetValue(5);
                    obj[6] = read.GetValue(6);*/
                    dataGridView1.Rows.Add(obj);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("1Error: " + ex.Message); //에러 메세지 
            }
            finally
            {
                if (conn != null)
                {
                    conn.Close(); //데이터베이스 연결 해제

                }
            }
        }

        private void button10_Click(object sender, EventArgs e)
        {
            Form6.usernum = usernum;
            Form6.Resrv = RN;
            Form6 frm = new Form6();
            frm.ShowDialog();
        }

        private void button11_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select 역이름, 시간 from 운행시간표 where 열차번호=" + comboBox2.SelectedItem.ToString();
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 2;
                dataGridView1.Columns[0].Name = read.GetName(0);
                dataGridView1.Columns[1].Name = read.GetName(1);
                while (read.Read())
                {
                    Object[] obj = new Object[2];
                    obj[0] = read.GetValue(0);
                    obj[1] = read.GetValue(1);
                    dataGridView1.Rows.Add(obj);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("1Error: " + ex.Message); //에러 메세지 
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
