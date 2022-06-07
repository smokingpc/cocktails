using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ETL_Splitter
{
    public partial class frmMain : Form
    {
        public frmMain()
        {
            InitializeComponent();
        }

        private void ClearAll() 
        {
            txtLog.Text = txtSrcFile.Text = "";
            txtBeginTime.Text = txtEndTime.Text = "0";
        }
        private void InitUI() 
        {
            ClearAll();
            cbCondition.Checked = false;
            txtBeginTime.Enabled = txtEndTime.Enabled = cbCondition.Checked;
        }
        private void frmMain_Load(object sender, EventArgs e)
        {
            InitUI();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            txtSrcFile.Text = SelectSrcFile();
            if (txtSrcFile.Text == "")
            {
                LogOutput("Error", "Please select a ETL file first!");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            CutFileRules rule = ParseRules();
            if(null != rule)
                DoCutFile(txtSrcFile.Text, rule);
        }

        private void cbCondition_CheckedChanged(object sender, EventArgs e)
        {
            txtBeginTime.Enabled = txtEndTime.Enabled = cbCondition.Checked;
        }
    }
}
