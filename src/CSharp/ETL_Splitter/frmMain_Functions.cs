using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Microsoft.Diagnostics.Tracing;

namespace ETL_Splitter
{
    public partial class frmMain
    {
        CutFileRules ParseRules() 
        {
            CutFileRules ret = new CutFileRules();
            try 
            {
                ret.BeginTime = Int64.Parse(txtBeginTime.Text);
                ret.EndTime = Int64.Parse(txtEndTime.Text);

                if (ret.BeginTime < 0)
                    ret.BeginTime = 0;

                if (ret.EndTime < 0)
                    ret.EndTime = 0;
            }
            catch (Exception ex)
            {
                ret = null;
                LogOutput("Error", "ParseRule failed...");
            }

            return ret;
        }

        void LogOutput(string prefix, string msg, bool msgbox = true)
        {
            txtLog.SetText($"[{prefix}] {msg}\r\n");
            if (msgbox)
                MessageBox.Show(msg);
        }

        string SelectSrcFile() 
        {
            string ret = "";
            OpenFileDialog dlg = new OpenFileDialog()
            {
                InitialDirectory = System.IO.Directory.GetCurrentDirectory(),
                Filter = "ETL log files (*.etl)|*.etl"
            };
            if (dlg.ShowDialog() == DialogResult.OK)
                ret = dlg.FileName;
            return ret;
        }

        bool DoCutFile(string src_file, CutFileRules rule)
        {
            try
            {
                UInt64 processed = 1;
                UInt64 written = 1;

                string out_file = System.IO.Path.Combine(new string[] {
                            System.IO.Path.GetDirectoryName(src_file),
                            "output.etl"
                        });

                using (var relogger = new ETWReloggerTraceEventSource(src_file, out_file))
                {
                    relogger.AllEvents += delegate (TraceEvent data)
                    {
                        if ((rule.BeginTime <= data.TimeStampRelativeMSec) &&
                            (rule.EndTime >= data.TimeStampRelativeMSec))
                        {
                            relogger.WriteEvent(data);

                            if (written % 10000 == 0)
                                LogOutput("Written", $"Wrote {written} events to output file.", false);
                            written++;
                        }

                        if (processed % 10000 == 0)
                            LogOutput("Progress", $"Processed {processed} events.", false);
                        processed++;
                    };
                    relogger.Process();
                }

                LogOutput("Finished", $"Cut ETL file done. Processed {processed} events, written {written} events.");
            }
            catch (Exception ex)
            {
                txtLog.Text = ex.Message;
                return false;
            }

            return true;
        }
    }
}
