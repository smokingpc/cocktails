using System;
using System.Runtime;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using SimpleWinFormsApp;

namespace SimpleWinFormsApp
{
    internal static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            OSPlatform platform = OSPlatform.Windows;
            string[] args = Environment.GetCommandLineArgs();

            DebugPrint(args);
            if (!RuntimeInformation.IsOSPlatform(platform) || OSPlatform.Windows != platform)
            {
                ConsoleMain(args);
            }

            //else
            //{
            //    // To customize application configuration such as set high DPI settings or default font,
            //    // see https://aka.ms/applicationconfiguration.
            //    ApplicationConfiguration.Initialize();
            //    Application.Run(new frmMain());
            //}
        }
        internal static void ConsoleMain(string[] args)
        {
            for (Int32 i = 0; i < args.Length; i++)
            {
                Console.WriteLine("[Console] Args[{0}]={0}", args[i]);
            }
        }

        internal static void DebugPrint(string[] args)
        {
            for (Int32 i = 0; i < args.Length; i++)
            {
                Console.WriteLine("[Debug] Args[{0}]={0}", args[i]);
            }
        }
    }
}