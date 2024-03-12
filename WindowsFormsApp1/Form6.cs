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
    public partial class Form6 : Form
    {
        public Form6()
        {
            
            InitializeComponent();
            for (int i = 0; i < 8; i++)
            {
                seat[i] = 0;
            }
        }
        public static string usernum;
        public static string Resrv;
        int Sstation;
        int Dstation;
        int S;
        int D;
        string Direction;
        string grade;
        int[] seat = new int[8];
        int P = 0;
        int charge = 0;
        private void Form6_Load(object sender, EventArgs e)
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
                    Object obj = new Object();
                    obj = read.GetValue(1);
                    comboBox1.Items.Add(obj);
                    comboBox2.Items.Add(obj);
                }
                read.Close();
                cmd.CommandText = "select * from 회원 where 회원번호=" + usernum;
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                read = cmd.ExecuteReader();
                while (read.Read())
                {
                    Object obj = new Object();
                    obj = read.GetValue(4);
                    comboBox6.Items.Add(obj);
                }
                comboBox6.Items.Add("무통장입금");
                comboBox6.Items.Add("카카오페이");
                comboBox6.Items.Add("토스");
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

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 기차역 where 역이름='" + comboBox1.SelectedItem.ToString() + "'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                while (read.Read())
                {
                    Object obj = new Object();
                    obj = read.GetValue(0);
                    Sstation = Convert.ToInt32(obj);
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

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            comboBox3.Items.Clear();
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 기차역 where 역이름='" + comboBox2.SelectedItem.ToString() + "'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                while (read.Read())
                {
                    Object obj = new Object();
                    obj = read.GetValue(0);
                    Dstation = Convert.ToInt32(obj);
                }
                if (Sstation > Dstation)
                {
                    Direction = "상행";
                    S = Dstation;
                    D = Sstation;
                }
                else
                {
                    Direction = "하행";
                    S = Sstation;
                    D = Dstation;
                }
                cmd = new OleDbCommand();
                cmd.CommandText = "select * from 운행시간표 where 역순번=" + Sstation.ToString() + " and 방향='" + Direction + "'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read1 = cmd.ExecuteReader();
                while (read1.Read())
                {
                    Object obj = new Object();
                    obj = read1.GetValue(2);
                    comboBox3.Items.Add(obj);
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

        private void comboBox3_SelectedIndexChanged(object sender, EventArgs e)
        {
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 열차 where 열차번호=" + comboBox3.SelectedItem.ToString();
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                while (read.Read())
                {
                    Object obj = new Object();
                    obj = read.GetValue(1);
                    grade = obj.ToString();
                    if (obj.ToString() == "SM")
                    {
                        button5.Enabled = false;
                        button6.Enabled = false;
                    }
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

        private void comboBox5_SelectedIndexChanged(object sender, EventArgs e)
        {
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select * from 예약좌석 where 운행날짜='" + dateTimePicker1.Value.ToString("yyyyMMdd") + "' and 역순번=" + Sstation.ToString() + " and 열차번호=" + comboBox3.SelectedItem.ToString() + " and 방향='" + Direction + "' and 차량번호=" + comboBox5.SelectedItem.ToString();
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                while (read.Read())
                {
                    Object obj = new Object();
                    obj = read.GetValue(7);
                    if (Convert.ToInt32(obj) == 1)
                    {
                        button1.Enabled = false;
                    }
                    if (Convert.ToInt32(obj) == 2)
                    {
                        button2.Enabled = false;
                    }
                    if (Convert.ToInt32(obj) == 3)
                    {
                        button3.Enabled = false;
                    }
                    if (Convert.ToInt32(obj) == 4)
                    {
                        button4.Enabled = false;
                    }
                    if (Convert.ToInt32(obj) == 5)
                    {
                        button5.Enabled = false;
                    }
                    if (Convert.ToInt32(obj) == 6)
                    {
                        button6.Enabled = false;
                    }
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

        private void comboBox6_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void button7_Click(object sender, EventArgs e)
        {
            int GN = 0;
            int DN = 0;
            OleDbConnection conn;
            string connectionString;
            connectionString = "Provider=MSDAORA;Password=han1120;User ID=hjh4119";
            conn = new OleDbConnection(connectionString);
            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand();
                cmd.CommandText = "select count(*) from 예약현황 where 예약번호 like '" + grade + "%'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                OleDbDataReader read = cmd.ExecuteReader();
                while (read.Read())
                {
                    Object obj = new Object();
                    obj = read.GetValue(0);
                    GN = Convert.ToInt32(obj);
                }
                read.Close();
                cmd.CommandText = "select count(*) from 예약현황 where 예매일시 like '%" + dateTimePicker1.Value.ToString("yyyyMMdd") + "%'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                read = cmd.ExecuteReader();
                while (read.Read())
                {
                    Object obj1 = new Object();
                    obj1 = read.GetValue(0);
                    DN = Convert.ToInt32(obj1);
                }
                read.Close();
                GN = GN + 1;
                DN = DN + 1;
                string RN;
                string T;
                T = "";
                cmd.CommandText = "delete from 예약좌석 where 예약번호='" + Resrv + "'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();
                cmd.CommandText = "delete from 예약현황 where 예약번호='" + Resrv + "'";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();
                RN = grade + dateTimePicker1.Value.ToString("yyyyMMdd") + GN.ToString() + DN.ToString();
                cmd.CommandText = "insert into 예약현황 values('" + RN + "'," + usernum.ToString() + ",'" + comboBox1.SelectedItem.ToString() + "','" + comboBox2.SelectedItem.ToString() + "','" + dateTimePicker1.Value.ToString("yyyyMMdd") + "'," + charge.ToString() + ",'" + comboBox6.SelectedItem.ToString() + "')";
                cmd.CommandType = CommandType.Text;
                cmd.Connection = conn;
                cmd.ExecuteNonQuery();
                for (int i = S; i <= D; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        if (seat[j] == 1)
                        {
                            cmd.CommandText = "select 시간 from 운행시간표 where 역순번=" + j.ToString();
                            cmd.CommandType = CommandType.Text;
                            cmd.Connection = conn;
                            read = cmd.ExecuteReader();
                            while (read.Read())
                            {
                                T = read.GetValue(0).ToString();
                            }
                            read.Close();
                            cmd.CommandText = "insert into 예약좌석 (역순번,역이름,열차번호,방향,시간,운행날짜,차량번호,좌석번호,예약번호) values(" + i.ToString() + ",(select 역이름 from 기차역 where 역순번=" + i.ToString() + ")," + comboBox3.SelectedItem.ToString() + ",'" + Direction + "','" + T + "'," + "'" + dateTimePicker1.Value.ToString("yyyyMMdd") + "'," + comboBox5.SelectedItem.ToString() + "," + j.ToString() + ",'" + RN + "')";
                            cmd.CommandType = CommandType.Text;
                            cmd.Connection = conn;
                            cmd.ExecuteNonQuery();
                        }
                    }
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

        private void button1_Click(object sender, EventArgs e)
        {
            seat[1] = 1;

            for (int i = 0; i < 8; i++)
            {
                if (seat[i] == 1)
                {
                    P = P + 1;
                }
            }

            if (grade == "SM")
            {
                charge = P * ((D - S) * 5000);
            }
            if (grade == "MG")
            {
                charge = P * ((D - S) * 4000);
            }
            label9.Text = "결제금액" + charge + "원";
        }

        private void button2_Click(object sender, EventArgs e)
        {
            seat[2] = 1;

            for (int i = 0; i < 8; i++)
            {
                if (seat[i] == 1)
                {
                    P = P + 1;
                }
            }

            if (grade == "SM")
            {
                charge = P * ((D - S) * 5000);
            }
            if (grade == "MG")
            {
                charge = P * ((D - S) * 4000);
            }
            label9.Text = "결제금액" + charge + "원";
        }

        private void button3_Click(object sender, EventArgs e)
        {
            seat[3] = 1;

            for (int i = 0; i < 8; i++)
            {
                if (seat[i] == 1)
                {
                    P = P + 1;
                }
            }

            if (grade == "SM")
            {
                charge = P * ((D - S) * 5000);
            }
            if (grade == "MG")
            {
                charge = P * ((D - S) * 4000);
            }
            label9.Text = "결제금액" + charge + "원";
        }

        private void button4_Click(object sender, EventArgs e)
        {
            seat[4] = 1;

            for (int i = 0; i < 8; i++)
            {
                if (seat[i] == 1)
                {
                    P = P + 1;
                }
            }

            if (grade == "SM")
            {
                charge = P * ((D - S) * 5000);
            }
            if (grade == "MG")
            {
                charge = P * ((D - S) * 4000);
            }
            label9.Text = "결제금액" + charge + "원";
        }

        private void button6_Click(object sender, EventArgs e)
        {
            seat[5] = 1;

            for (int i = 0; i < 8; i++)
            {
                if (seat[i] == 1)
                {
                    P = P + 1;
                }
            }

            if (grade == "SM")
            {
                charge = P * ((D - S) * 5000);
            }
            if (grade == "MG")
            {
                charge = P * ((D - S) * 4000);
            }
            label9.Text = "결제금액" + charge + "원";
        }

        private void button5_Click(object sender, EventArgs e)
        {
            seat[6] = 1;

            for (int i = 0; i < 8; i++)
            {
                if (seat[i] == 1)
                {
                    P = P + 1;
                }
            }

            if (grade == "SM")
            {
                charge = P * ((D - S) * 5000);
            }
            if (grade == "MG")
            {
                charge = P * ((D - S) * 4000);
            }
            label9.Text = "결제금액" + charge + "원";
        }
    }
}
