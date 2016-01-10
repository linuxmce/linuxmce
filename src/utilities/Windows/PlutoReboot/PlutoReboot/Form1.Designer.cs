namespace PlutoReboot
{
    partial class FrmMain
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
            this.label1 = new System.Windows.Forms.Label();
            this.BtnRestYes = new System.Windows.Forms.Button();
            this.BtnRestNo = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(380, 15);
            this.label1.TabIndex = 0;
            this.label1.Text = "Shutdown and restart computer as a Pluto media director?";
            // 
            // BtnRestYes
            // 
            this.BtnRestYes.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.BtnRestYes.Location = new System.Drawing.Point(91, 50);
            this.BtnRestYes.Name = "BtnRestYes";
            this.BtnRestYes.Size = new System.Drawing.Size(82, 30);
            this.BtnRestYes.TabIndex = 1;
            this.BtnRestYes.Text = "&Yes";
            this.BtnRestYes.UseVisualStyleBackColor = true;
            this.BtnRestYes.Click += new System.EventHandler(this.BtnRestYes_Click);
            // 
            // BtnRestNo
            // 
            this.BtnRestNo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.BtnRestNo.Location = new System.Drawing.Point(202, 50);
            this.BtnRestNo.Name = "BtnRestNo";
            this.BtnRestNo.Size = new System.Drawing.Size(80, 30);
            this.BtnRestNo.TabIndex = 2;
            this.BtnRestNo.Text = "&No";
            this.BtnRestNo.UseVisualStyleBackColor = true;
            this.BtnRestNo.Click += new System.EventHandler(this.BtnRestNo_Click);
            // 
            // FrmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(404, 92);
            this.Controls.Add(this.BtnRestNo);
            this.Controls.Add(this.BtnRestYes);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "FrmMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Shown += new System.EventHandler(this.FrmMain_Shown);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button BtnRestYes;
        private System.Windows.Forms.Button BtnRestNo;
    }
}

