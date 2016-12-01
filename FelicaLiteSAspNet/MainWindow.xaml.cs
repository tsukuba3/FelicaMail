using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace FelicaLiteSAspNet
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        [DllImport("MyFelicaLiteS.dll")]
        public extern static IntPtr FelicaRW_main(int block, Byte[] wData, bool read);
        [DllImport("MyFelicaLiteS.dll")]
        public extern static IntPtr GetName();
        [DllImport("MyFelicaLiteS.dll")]
        public extern static IntPtr GetData();

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern int MultiByteToWideChar(int codepage, int flags, IntPtr utf8, int utf8len, StringBuilder buffer, int buflen);

        int bankNum = 0;

        public MainWindow()
        {
            InitializeComponent();

            for(int i=0;i<16;i++)
                Bank.Items.Add(i);

            Bank.SelectedIndex = 0;
        }

        unsafe private void Write_Click(object sender, RoutedEventArgs e)
        {
            bankNum = Bank.SelectedIndex;

            Byte[] wData = Encoding.ASCII.GetBytes(Line.Text);

            FelicaRW_main(bankNum, wData, false);

            MessageBox.Show("書き込みが完了しました");
        }

        unsafe private void Read_Click(object sender, RoutedEventArgs e)
        {
            FelicaRW_main(0, null, true);

            IntPtr po = GetData();
            String str = Marshal.PtrToStringAnsi(po);

            String[] args = str.Split(',');

            po = GetName();

            int len = MultiByteToWideChar(65001, 0, po, -1, null, 0);

            if (len == 0)
                throw new System.ComponentModel.Win32Exception();

            var buf = new StringBuilder(len);

            len = MultiByteToWideChar(65001, 0, po, -1, buf, len);

            String name = buf.ToString();

            LName.Content = name;
            Userid.Text = args[2];
            Password.Text = args[3];
            Account.Text = args[0];
            Domain.Text = args[1];

        }

        private void MailSend_Click(object sender, RoutedEventArgs e)
        {
            String url = @"http://konekophoto.azurewebsites.net/FelicaMail?user_id=" + Userid.Text + "&password=" + Password.Text + "&mail_address=" + Account.Text + "@" + Domain.Text;

            var client = new WebClient();
            client.OpenRead(url);

        }
    }
}
