using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;
using System.Threading;
using System.IO;

namespace Revsoft.Wabbitcode
{
    public class Server
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern SafeFileHandle CreateNamedPipe(
           String pipeName,
           uint dwOpenMode,
           uint dwPipeMode,
           uint nMaxInstances,
           uint nOutBufferSize,
           uint nInBufferSize,
           uint nDefaultTimeOut,
           IntPtr lpSecurityAttributes);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern int ConnectNamedPipe(
           SafeFileHandle hNamedPipe,
           IntPtr lpOverlapped);

        public const uint DUPLEX = (0x00000003);
        public const uint FILE_FLAG_OVERLAPPED = (0x40000000);

        public class Client
        {
            public SafeFileHandle handle;
            public FileStream stream;
        }

        public delegate void MessageEventHandler(Client client, string message);
        public event MessageEventHandler MessageReceived;
        public const int BUFFER_SIZE = 4096;

        string pipeName;
        Thread listenThread, readThread;
        bool running;
        List<Client> clients;

        public string PipeName
        {
            get { return this.pipeName; }
            set { this.pipeName = value; }
        }

        public bool Running
        {
            get { return this.running; }
        }

        public Server()
        {
            this.clients = new List<Client>();
        }

        /// <summary>
        /// Starts the pipe server
        /// </summary>
        public void Start()
        {
            //start the listening thread
            this.listenThread = new Thread(new ThreadStart(ListenForClients));
            this.listenThread.IsBackground = true;
            this.listenThread.Start();

            this.running = true;
        }

        public void Stop()
        {
            this.listenThread.Abort();
            if (readThread != null)
                this.readThread.Abort();
            this.running = false;
        }

        /// <summary>
        /// Listens for client connections
        /// </summary>
        private void ListenForClients()
        {
            while (true)
            {
                SafeFileHandle clientHandle =
                CreateNamedPipe(
                     this.pipeName,
                     DUPLEX | FILE_FLAG_OVERLAPPED,
                     0,
                     255,
                     BUFFER_SIZE,
                     BUFFER_SIZE,
                     50,
                     IntPtr.Zero);

                //could not create named pipe
                if (clientHandle.IsInvalid)
                    return;

                int success = 0;
                try
                {
                    success = ConnectNamedPipe(clientHandle, IntPtr.Zero);
                }
                catch (Exception)
                { }

                //could not connect client
                if (success == 0)
                    return;

                Client client = new Client();
                client.handle = clientHandle;

                lock (clients)
                    this.clients.Add(client);

                readThread = new Thread(new ParameterizedThreadStart(Read));
                readThread.Start(client);
                return;
            }
        }

        /// <summary>
        /// Reads incoming data from connected clients
        /// </summary>
        /// <param name="clientObj"></param>
        private void Read(object clientObj)
        {
            Client client = (Client)clientObj;
            client.stream = new FileStream(client.handle, FileAccess.ReadWrite, BUFFER_SIZE, true);
            byte[] buffer = new byte[BUFFER_SIZE];
            ASCIIEncoding encoder = new ASCIIEncoding();

            while (true)
            {
                int bytesRead = 0;

                try
                {
                    if (client.handle.IsClosed)
                        return;
                    bytesRead = client.stream.Read(buffer, 0, BUFFER_SIZE);
                }
                catch
                {
                    //read error has occurred
                    break;
                }

                //client has disconnected
                if (bytesRead == 0)
                    break;

                //fire message received event
                if(this.MessageReceived != null)
                    this.MessageReceived(client, encoder.GetString(buffer, 0, bytesRead));
            }
            //clean up resources
            client.stream.Close();
            client.handle.Close();
            lock (this.clients)
                this.clients.Remove(client);
        }

        /// <summary>
        /// Sends a message to all connected clients
        /// </summary>
        /// <param name="message">the message to send</param>
        public void SendMessage(string message)
        {
            lock (this.clients)
            {
                ASCIIEncoding encoder = new ASCIIEncoding();
                byte[] messageBuffer = encoder.GetBytes(message);
                foreach (Client client in this.clients)
                {
                    client.stream.Write(messageBuffer, 0, messageBuffer.Length);
                    client.stream.Flush();
                }
            }
        }
    }
}
