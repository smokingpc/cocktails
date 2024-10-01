using System;
using System.IO;

internal class Program
{
    private static void Main(string[] args)
    {
        Console.WriteLine("This is SimpleConsole C# 10.0 example.");
        Console.WriteLine($"Total Args={args.Length}");

        try 
        {
            Console.WriteLine("Args[0]={0:D}, {0:X}", Convert.ToInt32(args[0]));
        }
        catch 
        {
            Console.WriteLine("Args[0]={0:D}, {0:X}", Convert.ToInt32(args[0], 16));
        }

        Console.WriteLine("Args[1]={0}", args[1]);
        Console.WriteLine($"Args[2]={args[2]}");
        Console.WriteLine("");
    }
}