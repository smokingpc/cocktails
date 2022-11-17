using System.ComponentModel;
using System.ServiceProcess;
using System.Diagnostics;

namespace ServiceDependentTool;

public partial class frmMain : Form
{
    ServiceController PrimarySvc = null;
    List<ServiceController> ChildSvcList = new List<ServiceController>();

    public frmMain()
    {
        InitializeComponent();
    }

    private void frmMain_Load(object sender, EventArgs e)
    {
        //I only want to show fields which I designed on UI.
        //Don't auto create new column for me....
        dataGridView1.AutoGenerateColumns = false;

        List<ServiceController> result = new List<ServiceController>();
        result.AddRange(ServiceController.GetServices());

        BindingList<ServiceController> binding = new BindingList<ServiceController>(result);
        dataGridView1.DataSource = binding;
    }

    private void dataGridView1_SelectionChanged(object sender, EventArgs e)
    {
        PrimarySvc = null;
        foreach (DataGridViewRow row in dataGridView1.Rows)
        {
            if (dataGridView1.SelectedRows.Count > 0 && dataGridView1.SelectedRows[0] == row)
            {
                row.Cells[0].Value = true;
                PrimarySvc = (ServiceController)row.DataBoundItem;
            }
            else
                row.Cells[0].Value = false;
        }
    }

    private void dataGridView2_SelectionChanged(object sender, EventArgs e)
    {
        ChildSvcList.Clear();
        foreach (DataGridViewRow row in dataGridView1.Rows)
        {
            if (dataGridView1.SelectedRows.Count > 0 && dataGridView1.SelectedRows[0] == row)
            {
                row.Cells[0].Value = true;
                ChildSvcList.Add((ServiceController)row.DataBoundItem);
            }
            else
                row.Cells[0].Value = false;
        }
    }

    private void button1_Click(object sender, EventArgs e)
    {
        if (ChildSvcList.Count > 0)
        { 
            //sc.exe的所有參數等號後面一定要有個空格，否則命令會失敗
            foreach (var svc in ChildSvcList)
            {
                string cmd = $"sc config {svc.ServiceName} depend= {PrimarySvc.ServiceName}";
                Process shell = new Process();
                shell.StartInfo.FileName = "cmd.exe";
                shell.StartInfo.RedirectStandardOutput = true;
                shell.StartInfo.CreateNoWindow = true;
                shell.StartInfo.UseShellExecute = false;
                shell.Start();

                shell.WaitForExit();
                textBox1.AppendText(shell.StandardOutput.ReadToEnd() + "\r\n");
            }
        }
    }
}
