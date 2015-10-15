using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;

namespace Determined
{
    public class DeterminedRobot
    {
        
        #region Variables

        /// <summary>
        /// 
        /// </summary>
        private int timeOut;

        /// <summary>
        /// Comunication port.
        /// </summary>
        private SerialPort port;

        /// <summary>
        /// Comunication lock object.
        /// </summary>
        private Object requestLock = new Object();

        /// <summary>
        /// TODO: MAke cooment.
        /// </summary>
        private bool isConnected = false;

        private string portName = String.Empty;

        /// <summary>
        /// Delimiting characters.
        /// </summary>
        private char[] delimiterChars = { ' ', ',', '.', ':', '\t' };

        #endregion

        #region Properties

        /// <summary>
        /// Maximum timeout.
        /// </summary>
        public int MaxTimeout { get; set; }

        /// <summary>
        /// If the board is correctly connected.
        /// </summary>
        public bool IsConnected
        {
            get
            {
                return this.isConnected;
            }
        }

        #endregion

        #region Events

        /// <summary>
        /// Rise when error occurred beteween PLC and PC.
        /// </summary>
        public event EventHandler<ErrorEventArgs> Message;

        #endregion

        #region Constructor / Destructor

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="port">Comunication port.</param>
        public DeterminedRobot(string portName)
        {
            // max tiem 5 minutes
            this.MaxTimeout = 30000;

            // Save the port name.
            this.portName = portName;
        }

        /// <summary>
        /// Destructor
        /// </summary>
        ~DeterminedRobot()
        {
            this.Dispose();
        }

        /// <summary>
        /// Dispose
        /// </summary>
        public void Dispose()
        {
            if (this.isConnected)
            {
                this.port.Close();
            }
        }

        #endregion

        #region Public

        /// <summary>
        /// Connetc to the serial port.
        /// </summary>
        public void Connect()
        {
            while (!this.isConnected)
            {
                try
                {
                    if (!this.isConnected)
                    {
                        this.port = new SerialPort(this.portName);
                        this.port.BaudRate = 9600;
                        this.port.DataBits = 8;
                        this.port.StopBits = StopBits.One;
                        this.port.Parity = Parity.None;
                        this.port.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
                        this.port.Open();

                        this.timeOut = 0;
                        this.isConnected = true;
                    }
                }
                catch (Exception exception)
                {
                    this.timeOut += 1000;
                    Thread.Sleep(timeOut);
                    this.isConnected = false;
                }
                finally
                {
                    if (timeOut > this.MaxTimeout)
                    {
                        throw new InvalidOperationException("Could not connect to the device.");
                    }
                }
            }
        }

        /// <summary>
        /// Disconnect
        /// </summary>
        public void Disconnect()
        {
            this.port.Close();
            this.isConnected = false;
        }

        /// <summary>
        /// Reset the MCU
        /// </summary>
        public void Reset()
        {
            lock (this.requestLock)
            {
                if (this.isConnected)
                {
                    this.port.DtrEnable = true;
                    Thread.Sleep(200);
                    this.port.DtrEnable = false;
                }
            }
        }

        /// <summary>
        /// Move the robots.
        /// </summary>
        /// <param name="value">Value of the movment tenth of the mm.</param>
        public void Move(int value)
        {
            string direction = "";

            lock (this.requestLock)
            {
                if (value > 0)
                {
                    direction = "+";
                }

                if (value <= 0)
                {
                    direction = "-";
                }

                if (port.IsOpen)
                {
                    string command = String.Format("?M{0}{1:D3}", direction, value);
                    this.SendRequest(command);
                }
            }
        }

        /// <summary>
        /// Rotate the robots.
        /// </summary>
        /// <param name="value">Value of the rotation tenth of the degree.</param>
        public void Rotate(int value)
        {
            string direction = "";

            lock (this.requestLock)
            {
                if (value > 0)
                {
                    direction = "+";
                }

                if (value <= 0)
                {
                    direction = "-";
                }

                if (port.IsOpen)
                {
                    string command = String.Format("?R{0}{1:D3}", direction, value);
                    this.SendRequest(command);
                }
            }
        }

        /// <summary>
        /// Get sensors.
        /// </summary>
        public void GetSensors()
        {
            lock (this.requestLock)
            {
                if (port.IsOpen)
                {
                    string command = String.Format("?SENSORS");
                    this.SendRequest(command);
                }
            }
        }

        #endregion

        #region Private

        /// <summary>
        /// Data recievce event.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            // Wait ...
            Thread.Sleep(150);

            if (sender != null)
            {
                // Make serial port to get data from.
                SerialPort sp = (SerialPort)sender;

                //string indata = sp.ReadLine();
                string inData = sp.ReadExisting();

                //TODO: Parse the incommning string from the serial port.
                // POS will be the index of the array.
                // CM will be the data in the cell.
                //string[] tokens = inData.Split(this.delimiterChars);

                //Console.WriteLine("Data: {0};\r\nTokens: {1}", indata, tokens.Length);

                //Console.WriteLine("Test: {0}", indata);

                if (this.Message != null)
                {
                    this.Message(this, new ErrorEventArgs(new Exception(inData)));
                }

                // Discart the duffer.
                sp.DiscardInBuffer();
            }
        }

        /// <summary>
        /// Send the request string to the robot.
        /// </summary>
        /// <param name="request"></param>
        private void SendRequest(string request)
        {
            try
            {
                if (this.isConnected)
                {
                    this.port.WriteLine(request);
                }
            }
            catch (Exception exception)
            {
                this.isConnected = false;
                this.timeOut = 0;
                // Reconnect.
                this.Connect();
            }
        }

        #endregion
    }
}
