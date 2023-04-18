using System;
using System.Collections.Generic;
using System.Linq;
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
using ExtendedSerialPort;
using System.Windows.Threading;

namespace RobotInterface_MunigliaLieutier
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>



    public partial class MainWindow : Window
    {
        ReliableSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        Robot robot = new Robot();

        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM8", 115200, System.IO.Ports.Parity.None, 8, System.IO.Ports.StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick; ;
            timerAffichage.Start();
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            while (robot.byteListReceived.Count > 0)
            {
                var c = robot.byteListReceived.Dequeue();
                DecodeMessage(c);
                //TextBoxReception.Text += "0x" + robot.byteListReceived.Dequeue().ToString("X2") + " ";
            }
        }


        private void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            for (int i = 0; i < e.Data.Length; i++)
            {
                robot.byteListReceived.Enqueue(e.Data[i]);
            }
        }


        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            if (buttonEnvoyer.Background == Brushes.RoyalBlue)
            {
                buttonEnvoyer.Background = Brushes.Beige;
            }
            else
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
            }

            SendMessage();
        }

        private void TextBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }

        private void SendMessage()
        {
            byte[] tests = Encoding.ASCII.GetBytes(TextBoxEmission.Text);
            UartEncodeAndSendMessage(0x0080, tests.Length, tests);
            TextBoxEmission.Text = null;
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            TextBoxReception.Text = null;
        }

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            string s = "Bonjour";
            byte[] tests = Encoding.ASCII.GetBytes(s);

            UartEncodeAndSendMessage(0x0080, tests.Length, tests);

            
        }

        byte CalculateChecksum(int msgFunction,int msgPayloadLength, byte[] msgPayload)
        {
            byte checksum = 0;
            checksum ^= 0xFE;
            checksum ^= (byte)(msgFunction >> 8);
            checksum ^= (byte)(msgFunction);
            checksum ^= (byte)(msgPayloadLength >> 8);
            checksum ^= (byte)(msgPayloadLength);
            for (int i = 0; i < msgPayloadLength; i++)
            {
                checksum ^= msgPayload[i];
            }
            return checksum;
        }

        void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte[] Frame = new byte[msgPayloadLength + 6];
            int pos = 0;

            Frame[pos++] = (byte)0xFE;
            Frame[pos++] = (byte)(msgFunction >> 8);
            Frame[pos++] = (byte)(msgFunction);
            Frame[pos++] = (byte)(msgPayloadLength >> 8);
            Frame[pos++] = (byte)(msgPayloadLength);

            for (int i = 0; i < msgPayloadLength; i++)
            {
                Frame[pos++] = msgPayload[i];
            }
            Frame[pos++] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

            serialPort1.Write(Frame, 0, pos);
        }

        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }

        StateReception rcvState = StateReception.Waiting;
        int msgDecodeFunction = 0;
        int msgDecodePayloadLength = 0;
        byte[] msgDecodePayload;
        int msgDecodePayloadIndex = 0;

        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                        rcvState = StateReception.FunctionMSB;
                    break;
                case StateReception.FunctionMSB:
                    msgDecodeFunction = c << 8;
                    rcvState = StateReception.FunctionLSB;
                    break;
                case StateReception.FunctionLSB:
                    msgDecodeFunction += c << 0;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;
                case StateReception.PayloadLengthMSB:
                    msgDecodePayloadLength = c << 8;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;
                case StateReception.PayloadLengthLSB:
                    msgDecodePayloadLength += c << 0;
                    msgDecodePayload = new byte[msgDecodePayloadLength]; 
                    msgDecodePayloadIndex = 0;
                    rcvState = StateReception.Payload;
                    if (msgDecodePayloadLength <= 0 && msgDecodePayloadLength >= 50000)
                    {
                        rcvState = StateReception.Waiting;
                    }
                    break;
                case StateReception.Payload:
                    msgDecodePayload[msgDecodePayloadIndex++] = c;
                    if (msgDecodePayloadIndex >= msgDecodePayloadLength)
                    {
                        rcvState = StateReception.CheckSum;
                    }
                    break;
                case StateReception.CheckSum:
                    if (CalculateChecksum(msgDecodeFunction, msgDecodePayloadLength, msgDecodePayload) == c)
                    {
                        //Success, on a un message valide
                        TextBoxReception.Text += "Ok";
                    }
                    rcvState = StateReception.Waiting;
                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }

        public enum TypeMessage
        {
            Leds=0x0020,
            TelemetresIR=0x0030,
            VitesseMoteur=0x0040,
            Texte=0x0080
        }

        void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            switch ((TypeMessage)msgFunction)
            {
                case TypeMessage.Texte:
                    // Texte
                    string str = Encoding.Default.GetString(this.msgDecodePayload);
                    TextBoxReception.Text = str;
                    break;

                case TypeMessage.Leds:
                    // Réglage LED
                    if (this.msgDecodePayload[0] == 1)
                    {
                        LED_Orange.IsChecked = Convert.ToBoolean(this.msgDecodePayload[1]);
                    }
                    if (this.msgDecodePayload[0] == 2)
                    {
                        LED_Bleu.IsChecked = Convert.ToBoolean(this.msgDecodePayload[1]);
                    }
                    if (this.msgDecodePayload[0] == 3)
                    {
                        LED_Blanche.IsChecked = Convert.ToBoolean(this.msgDecodePayload[1]);
                    }
                    break;

                case TypeMessage.TelemetresIR:
                    //Télémètres
                    Telemetres.Text = "IR Ext Gauche :" + this.msgDecodePayload[0] + "\nIR Gauche : " + this.msgDecodePayload[1] + "\nIR Centre : " + this.msgDecodePayload[2] + "\nIR Droit : " + this.msgDecodePayload[3] + "\nIR Ext Droit: " + this.msgDecodePayload[4];
                    break;

                case TypeMessage.VitesseMoteur:
                    Moteurs.Text = "Vitesse  Gauche: " + this.msgDecodePayload[0] + "\nVitesse Droit : " + this.msgDecodePayload[1];
                    break;
            }
        }
    }
}