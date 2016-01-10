using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using Microsoft.Win32;


namespace PlutoReboot
{
    public partial class FrmFindRouter : Form
    {
        DCERouterFind m_DCERouterFinder = null;
        int m_nDotCount = 0;
        private string m_sDCERouterIP;
        private bool m_bCloseAppOnExit = true;

        public FrmFindRouter()
        {
            InitializeComponent();
        }

        public string DCERouterIP
        {
            get { return m_sDCERouterIP; }
        }

        private void FrmFindRouter_FormClosed(object sender, FormClosedEventArgs e)
        {
            if ( m_bCloseAppOnExit )
                Application.Exit();
        }

        bool m_bManFind = false;
        private void BtnManIP_Click(object sender, EventArgs e)
        {            
            if (!m_bManFind)
            {
                this.Text = "Manually locate DCERouter";
                FindingAnimTimer.Enabled = false;
                if (null != m_DCERouterFinder)
                {
                    m_DCERouterFinder.Stop();
                    m_DCERouterFinder = null;
                }
                TBRouterIP.ReadOnly = false;
                TBRouterIP.Enabled = true;
                BtnManIP.Text = "Check IP";
                BtnAccept.Enabled = false;
                m_bManFind = true;
                TBRouterIP.Focus();
            }
            else
            {
                BtnManIP.Enabled = false;
                try
                {
                    PlutoManager plutoman = new PlutoManager();
                    if (plutoman.CheckHostByIP(TBRouterIP.Text))
                    {
                        m_sDCERouterIP = TBRouterIP.Text;
                        BtnAccept.Enabled = true;
                        this.Text = "Router found!";
                        TBRouterIP.ReadOnly = true;
                        TBRouterIP.Enabled = false;
                        BtnManIP.Enabled = false;
                    }
                    else
                    {
                        throw new Exception( "Router could not be found on: " + TBRouterIP.Text );
                    }
                }
                catch (Exception except)
                {
                    BtnManIP.Enabled = true;
                    System.Windows.Forms.MessageBox.Show(except.Message, "Error...");
                }
            }
        }

        private void FrmFindRouter_Shown(object sender, EventArgs e)
        {
            m_DCERouterFinder = new DCERouterFind();
            m_DCERouterFinder.Start();
            m_nDotCount = 0;
            FindingAnimTimer.Enabled = true;
        }

        private void FindingAnimTimer_Tick(object sender, EventArgs e)
        {
            if (null != m_DCERouterFinder)
            {
                if (m_DCERouterFinder.Searching)
                {
                    ++m_nDotCount;
                    if (m_nDotCount > 5)
                    {
                        m_nDotCount = 1;
                    }
                    string sDots = new string('.', m_nDotCount);
                    this.Text = "Finding DCERouter" + sDots;
                    this.Refresh();
                }
                else
                {
                    FindingAnimTimer.Enabled = false;
                    if (m_DCERouterFinder.DCERouterIP != "")
                    {
                        m_sDCERouterIP = m_DCERouterFinder.DCERouterIP;
                        TBRouterIP.Text = m_DCERouterFinder.DCERouterIP;
                        this.Text = "DCERouter found.";
                        BtnAccept.Enabled = true;
                        System.Windows.Forms.MessageBox.Show("DCERouter found on: " + TBRouterIP.Text);
                    }
                    else
                    {
                        this.Text = "DCERouter not found.";
                    }
                    m_DCERouterFinder.Stop();
                    m_DCERouterFinder = null;
                }
            }
            else
            {
                FindingAnimTimer.Enabled = false;
            }
        }

        private void TBRouterIP_KeyPress(object sender, KeyPressEventArgs e)
        {
            if ( e.KeyChar == (char)Keys.Return)
            {
                BtnManIP.PerformClick();
            }
        }

        private bool SaveSettingsToRegistry()
        {
            try
            {
                RegistryKey reg = Registry.LocalMachine.OpenSubKey(@"Software\Pluto", true);
                try
                {
                    if (reg == null)
                    {
                        reg = Registry.LocalMachine.CreateSubKey(@"Software\Pluto");
                        if (reg == null)
                            return false;

                    }
                    reg.SetValue("DCERouter", m_sDCERouterIP);
                    Registry.LocalMachine.Flush();
                    return true;
                }
                catch (Exception)
                {
                    return false;
                }
                finally
                {
                    reg.Close();
                }
            }
            catch( Exception )
            {
                return false;
            }

        }

        private void BtnAccept_Click(object sender, EventArgs e)
        {
            m_bCloseAppOnExit = false;
            if (!SaveSettingsToRegistry())
            {
                System.Windows.Forms.MessageBox.Show( "Error saving setting to registry!", "Error..." );
            }            
            this.DialogResult = System.Windows.Forms.DialogResult.OK;
        }
    }

    class DCERouterFind
    {
        private Thread m_WorkerThread;
        private string m_sDCERouterIP;
        private bool m_bSearching = false;
        public DCERouterFind()
        {
            m_WorkerThread = null;
            m_sDCERouterIP = "";
        }
        public void Start()
        {
            if (null == m_WorkerThread)
            {
                m_WorkerThread = new Thread(new ThreadStart(this.WorkerThreadFunction));
                m_WorkerThread.Name = "FindingPluto";
                m_WorkerThread.Start();
            }
        }
        public void Stop()
        {
            if (null != m_WorkerThread)
            {
                m_WorkerThread.Abort();
                m_WorkerThread = null;
            }
        }

        public string DCERouterIP
        {
            get { return m_sDCERouterIP; }
            set { m_sDCERouterIP = value; }
        }
        public bool Searching
        {
            get { return m_bSearching; }
        }

        private void WorkerThreadFunction()
        {
            PlutoManager plutoman = new PlutoManager();
            DCERouterIP = "";
            m_bSearching = true;
            try
            {
                if (plutoman.CheckDefault())
                {
                    DCERouterIP = plutoman.ConnectedIP;
                    return;
                }
                if (plutoman.UDPFind())
                {
                    DCERouterIP = plutoman.ConnectedIP;
                    return;
                }
                DCERouterIP = "";
            }
            finally
            {
                plutoman = null;
                m_WorkerThread = null;
                m_bSearching = false;
            }                        
        }
    }
}