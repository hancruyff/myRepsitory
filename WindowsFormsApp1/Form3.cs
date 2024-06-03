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

namespace WindowsFormsApp1
{
    public partial class Form3 : Form
    {
        public Form3()
        {
            InitializeComponent();
            dataGridView1.Rows.Clear();
        }
        
        private void button_approve_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();
            button_trainadd.Enabled = false;
            button_TTadd.Enabled = false;
            string connectionString = "Provider=MSDAORA;Password=han1111;User ID=hjh4119";
            OleDbConnection conn;
            conn = new OleDbConnection(connectionString);
            conn.Open(); //데이터베이스 연결
            approve.Enabled = true;
            try
            {
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 회원 where 등급='none'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 2;
                dataGridView1.Columns[0].Name = read.GetName(0);
                while (read.Read())
                {
                    Object[] obj = new object[2];
                    obj[0] = new object();
                    obj[0] = read.GetValue(6);
                    obj[1] = new object();
                    obj[1] = read.GetValue(0);
                    dataGridView1.Rows.Add(obj);
                }
                read.Close();
            }
            catch(Exception ex)
            {
                MessageBox.Show("Error: " + ex.Message);
            }
        }

        private void approve_Click(object sender, EventArgs e)
        {
            
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1111;User ID=hjh4119";
            dataGridView1.Rows.Clear();
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "update 회원 set 등급='Silver' where 회원번호 =" + textBox2.Text;
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();
                cmd = new OleDbCommand();
                cmd.CommandText = "select * from 회원 where 등급='none'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 2;
                dataGridView1.Columns[0].Name = read.GetName(0);
                dataGridView1.Columns[1].Name = read.GetName(1);
                while (read.Read())
                {
                    Object[] obj = new object[2];
                    obj[0] = new object();
                    obj[0] = read.GetValue(6);
                    obj[1] = new object();
                    obj[1] = read.GetValue(0);
                    dataGridView1.Rows.Add(obj);
                }
                read.Close();
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

        

        private void dataGridView1_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            textBox1.Text = dataGridView1.Rows[e.RowIndex].Cells[0].Value.ToString();
            textBox2.Text = dataGridView1.Rows[e.RowIndex].Cells[1].Value.ToString();
        }

        private void button_addtrain_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();
            approve.Enabled = false;
            button_TTadd.Enabled = false;
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1111;User ID=hjh4119";
            dataGridView1.Rows.Clear();
            label3.Text = "열차번호";
            label4.Text = "열차등급";
            label5.Text = "";
            label6.Text = "";
            button_trainadd.Enabled = true;
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 열차";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 2;
                dataGridView1.Columns[0].Name = read.GetName(0);
                while (read.Read())
                {
                    Object[] obj = new object[2];
                    obj[0] = new object();
                    obj[0] = read.GetValue(0);
                    obj[1] = new object();
                    obj[1] = read.GetValue(1);
                    dataGridView1.Rows.Add(obj);
                }
                read.Close();
                /*cmd.CommandText = "insert into 열차 values(" + textBox1.Text + ",'" + textBox2.Text + "')";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();*/
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

        private void button_trainadd_Click(object sender, EventArgs e)
        {
            
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1111;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "insert into 열차 values(" + textBox1.Text + ",'" + textBox2.Text + "')";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();
                int tnum;
                int seatnum;
                if (textBox2.Text == "SM")
                {
                    for(int i = 1; i < 3; i++)
                    {
                        for(int j = 1; j < 5; j++)
                        {
                            cmd.CommandText = "insert into 열차좌석 values(" + textBox1.Text + "," +i+","+j +")";
                            cmd.CommandType = CommandType.Text;
                            cmd.Connection = conn;
                            cmd.ExecuteNonQuery();
                        }
                    }
                }
                else
                {
                    for (int i = 1; i < 3; i++)
                    {
                        for (int j = 1; j < 7; j++)
                        {
                            cmd.CommandText = "insert into 열차좌석 values(" + textBox1.Text + "," + i + "," + j + ")";
                            cmd.CommandType = CommandType.Text;
                            cmd.Connection = conn;
                            cmd.ExecuteNonQuery();
                        }
                    }
                }
                dataGridView1.Rows.Clear();

                cmd.CommandText = "select * from 열차";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 2;
                dataGridView1.Columns[0].Name = read.GetName(0);
                while (read.Read())
                {
                    Object[] obj = new object[2];
                    obj[0] = new object();
                    obj[0] = read.GetValue(0);
                    obj[1] = new object();
                    obj[1] = read.GetValue(1);
                    dataGridView1.Rows.Add(obj);
                }
                textBox1.Clear();
                textBox2.Clear();
                read.Close();
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

        private void button_TT_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();
            button_TTadd.Enabled = true;
            button_trainadd.Enabled = false;
            approve.Enabled = false;
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1111;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            dataGridView1.Rows.Clear();
            label3.Text = "역순번";
            label4.Text = "시간";
            label5.Text = "열차번호";
            label6.Text = "방향";
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 운행시간표";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 5;
                for(int i = 0; i < 5; i++)
                {
                    dataGridView1.Columns[i].Name = read.GetName(i);
                }
                while (read.Read())
                {
                    Object[] obj = new object[5];
                    for (int i = 0; i < 5; i++)
                    {
                        
                        obj[i] = read.GetValue(i);
                    }
                    
                    
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

        private void button_TTadd_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1111;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "insert into 운행시간표 (역순번,열차번호,방향,시간,역이름) values(" + textBox1.Text + "," +comboBox1.Text + ",'"+comboBox2.Text+"','"+textBox2.Text+"',(select 역이름 from 기차역 where 역순번="+textBox1.Text+"))";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();
                cmd.CommandText = "select * from 운행시간표";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 5;
                for (int i = 0; i < 5; i++)
                {
                    dataGridView1.Columns[i].Name = read.GetName(i);
                }
                while (read.Read())
                {
                    Object[] obj = new object[5];
                    for (int i = 0; i < 5; i++)
                    {

                        obj[i] = read.GetValue(i);
                    }


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

        private void Form3_Load(object sender, EventArgs e)
        {
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1111;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 열차";
                cmd.CommandType= CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                while (read.Read())
                {
                    Object obj = read.GetValue(0);
                    comboBox1.Items.Add(obj.ToString());
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

        private void button1_Click(object sender, EventArgs e)
        {
            dataGridView1.Rows.Clear();
            label7.Text = "등급";
            button2.Enabled = true;
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1111;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 회원 where 등급 !='none'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 2;
                dataGridView1.Columns[0].Name = read.GetName(3);
                dataGridView1.Columns[1].Name = read.GetName(0);
                while (read.Read())
                {
                    Object[] obj = new object[2];
                    obj[0] = read.GetValue(3);
                    obj[1] = read.GetValue(0);
                    dataGridView1.Rows.Add(obj);
                }
                read.Close();
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

        private void button2_Click(object sender, EventArgs e)
        {
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1111;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "update 회원 set 등급='"+comboBox3.SelectedItem.ToString()+"' where 회원번호 =" + textBox2.Text;
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();
                cmd = new OleDbCommand();
                dataGridView1.Rows.Clear();
                cmd.CommandText = "select * from 회원 where 등급 !='none'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                dataGridView1.ColumnCount = 2;
                dataGridView1.Columns[0].Name = read.GetName(3);
                dataGridView1.Columns[1].Name = read.GetName(0);
                while (read.Read())
                {
                    Object[] obj = new object[2];
                    obj[0] = read.GetValue(3);
                    obj[1] = read.GetValue(0);
                    dataGridView1.Rows.Add(obj);
                }
                read.Close();
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
