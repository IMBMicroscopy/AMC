using System;
using System.Management;
using System.IO.Ports;

namespace AllOff
{
    class Program
    {

        public static int Main(string[] args)
        {
            ManagementScope connectionScope = new ManagementScope();
            SelectQuery serialQuery = new SelectQuery("SELECT * FROM Win32_SerialPort");
            ManagementObjectSearcher searcher = new ManagementObjectSearcher(connectionScope, serialQuery);
            try
            {
                foreach (ManagementObject item in searcher.Get())
                {
                    string desc = item["Description"].ToString();
                    string deviceId = item["DeviceID"].ToString();
                    if (desc.Contains("Arduino"))
                    {
                        //Console.WriteLine(deviceId);  //write com port to console
                        try
                        {
                            SerialPort port = new SerialPort(deviceId, 9600);
                            port.ReadTimeout = 200;
                            port.WriteTimeout = 200;
                            port.Parity = Parity.None;
                            port.StopBits = StopBits.One;
                            port.DataBits = 8;
                            port.Handshake = Handshake.None;
                            port.DtrEnable = true;  //required for Arduino leonardo boards
                            port.Open();
                            System.Threading.Thread.Sleep(200);
                            port.DiscardInBuffer();
                            port.DiscardOutBuffer();
                            System.Threading.Thread.Sleep(20);
                            try
                            {
                                port.Write("allOff" + "\r");
                                System.Threading.Thread.Sleep(100);
                                port.Close();
                                break;
                            }
                            catch (SystemException)
                            {
                                Console.Write(deviceId);
                                Console.WriteLine(" - Write timeout");
                            }
                            port.Close();
                            System.Threading.Thread.Sleep(20);
                        }
                        catch (SystemException)
                        {
                            Console.Write(deviceId);
                            Console.WriteLine(" - port unavailable");
                        }

                    }
                }
            }
            catch (SystemException) { }
            //Console.ReadLine();  //used for debugging to keep console open
            return 0;
        }

    }
}
