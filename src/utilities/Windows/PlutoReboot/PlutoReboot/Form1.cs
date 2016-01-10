using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Management;
using Microsoft.Win32;
using Org.Mentalis.Utilities;

namespace PlutoReboot
{
    public partial class FrmMain : Form
    {
        public string m_sRegIP = "";
        public FrmMain()
        {
            InitializeComponent();
        }

        private void BtnRestNo_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }


        private void BtnRestYes_Click(object sender, EventArgs e)
        {            
            //Finding DCERouter….
            try
            {
                if (m_sRegIP == "")
                {
                    FrmFindRouter FrmRouter = new FrmFindRouter();
                    FrmRouter.ShowDialog();
                    m_sRegIP = FrmRouter.DCERouterIP;
                }
                string sMACAddress = "";
                PlutoManager PlutoMan = new PlutoManager();
                if (!PlutoMan.GetMAC(m_sRegIP, out sMACAddress))
                {
                    System.Windows.Forms.MessageBox.Show("Error retrieving MAC address!", "Error...");
                }
                else
                {
                    ShellCommands shCmds = new ShellCommands();
                    if (shCmds.Exec_HaltDevice(m_sRegIP, sMACAddress))
                    {
                        WindowsController.ExitWindows(RestartOptions.Reboot, false);
                    }
                    else
                    {
                        System.Windows.Forms.MessageBox.Show("Error sending HALT command to router!", "Error...");
                    }
                }
            }
            finally
            {
                Application.Exit();
            }
            
        }
        private bool LoadRegistrySettings()
        {
            try
            {
                RegistryKey reg = Registry.LocalMachine.OpenSubKey(@"Software\Pluto", false);
                if (reg == null)
                    return false;

                try
                {
                    m_sRegIP = reg.GetValue("DCERouter", "").ToString();
                    return true;
                }
                catch( Exception )
                {
                    return false;
                }
                finally
                {
                    reg.Close();
                }
            }
            catch (Exception)
            {
                return false;
            }

        }


        private void FrmMain_Shown(object sender, EventArgs e)
        {
            LoadRegistrySettings();
            if (m_sRegIP != "")
            {
                this.Text = "DCERouter: " + m_sRegIP;
            }
            else this.Text = "DCERouter unknown ";
        }
    }
}