using System;
using System.Windows.Forms;

namespace ETL_Splitter
{
    static class Utils
    {
        public static void SetText(this TextBox tb, string msg, bool append = true)
        {
            if (tb.InvokeRequired)
            {
                tb.Invoke((Action)(() => { SetText(tb, msg, append); }));
            }
            else
            {
                if (append)
                    tb.AppendText(msg);
                else
                    tb.Text = msg;
            }
        }
    }
}
