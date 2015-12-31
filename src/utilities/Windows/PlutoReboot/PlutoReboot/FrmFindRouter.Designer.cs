namespace PlutoReboot
{
    partial class FrmFindRouter
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.label1 = new System.Windows.Forms.Label();
            this.TBRouterIP = new System.Windows.Forms.TextBox();
            this.BtnManIP = new System.Windows.Forms.Button();
            this.FindingAnimTimer = new System.Windows.Forms.Timer(this.components);
            this.BtnAccept = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 18);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "DCERouter IP:";
            // 
            // TBRouterIP
            // 
            this.TBRouterIP.Enabled = false;
            this.TBRouterIP.Location = new System.Drawing.Point(95, 15);
            this.TBRouterIP.Name = "TBRouterIP";
            this.TBRouterIP.ReadOnly = true;
            this.TBRouterIP.Size = new System.Drawing.Size(128, 20);
            this.TBRouterIP.TabIndex = 1;
            this.TBRouterIP.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TBRouterIP_KeyPress);
            // 
            // BtnManIP
            // 
            this.BtnManIP.Location = new System.Drawing.Point(229, 12);
            this.BtnManIP.Name = "BtnManIP";
            this.BtnManIP.Size = new System.Drawing.Size(150, 23);
            this.BtnManIP.TabIndex = 3;
            this.BtnManIP.Text = "Manually locate dcerouter";
            this.BtnManIP.UseVisualStyleBackColor = true;
            this.BtnManIP.Click += new System.EventHandler(this.BtnManIP_Click);
            // 
            // FindingAnimTimer
            // 
            this.FindingAnimTimer.Tick += new System.EventHandler(this.FindingAnimTimer_Tick);
            // 
            // BtnAccept
            // 
            this.BtnAccept.Enabled = false;
            this.BtnAccept.Location = new System.Drawing.Point(120, 55);
            this.BtnAccept.Name = "BtnAccept";
            this.BtnAccept.Size = new System.Drawing.Size(128, 29);
            this.BtnAccept.TabIndex = 4;
            this.BtnAccept.Text = "Accept settings";
            this.BtnAccept.UseVisualStyleBackColor = true;
            this.BtnAccept.Click += new System.EventHandler(this.BtnAccept_Click);
            // 
            // FrmFindRouter
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(391, 87);
            this.Controls.Add(this.BtnAccept);
            this.Controls.Add(this.BtnManIP);
            this.Controls.Add(this.TBRouterIP);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FrmFindRouter";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Finding DCERouter";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.FrmFindRouter_FormClosed);
            this.Shown += new System.EventHandler(this.FrmFindRouter_Shown);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox TBRouterIP;
        private System.Windows.Forms.Button BtnManIP;
        private System.Windows.Forms.Timer FindingAnimTimer;
        private System.Windows.Forms.Button BtnAccept;
    }
}